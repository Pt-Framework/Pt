/*
 * Copyright (C) 2012 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ElementModel.h"
#include "AttributeModel.h"
#include "InputStack.h"
#include "NamespaceContext.h"

#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/DocTypeValidator.h>
#include <Pt/Xml/Entity.h>
#include <Pt/Xml/Notation.h>
#include <Pt/Xml/XmlResolver.h>
#include <Pt/Xml/DocTypeDefinition.h>
#include <Pt/Xml/StartDocument.h>
#include <Pt/Xml/EndDocument.h>
#include <Pt/Xml/DocType.h>
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/XmlError.h"
#include "Pt/System/Logger.h"

#include <stack>
#include <cassert>

log_define("Pt.Xml.XmlReader")

namespace Pt {

namespace Xml {

class XmlReaderImpl
{
    typedef void (XmlReaderImpl::*ParseFunc)(int);

    private:
        void onDocumentBegin(int c)
        {
            if( c == std::char_traits<Char>::eof() )
            {
                setStartDoc();

                if( ! _current)
                    _current = &_endDoc;
                
                _parse = &XmlReaderImpl::onProlog;
                return;
            }

            Char ch = c;

            if( isSpace(ch) )
            {
                setStartDoc();
                _parse = &XmlReaderImpl::onProlog;
            }
            else if( ch == '<')
            {
                setStartDoc();
                _parse = &XmlReaderImpl::onTag;
            }
            else
            {
                throw SyntaxError("XML syntax error", line());
            }
        }

        void onProcessingInstructionBegin(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) ) // TODO: XML Name character
            {
                _procInstr.target() += c;
                ++_nodeSize;
                _parse = &XmlReaderImpl::onProcessingInstruction;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onProcessingInstruction(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                _parse = &XmlReaderImpl::onProcessingInstructionData;
                return;
            }

            if( ! isAlpha(ch) )
                throw SyntaxError("XML syntax error", line());

            if(_nodeSize == _maxSize)
                throw SyntaxError("node too large", line());

            _procInstr.target() += c;
            ++_nodeSize;
        }

        void onProcessingInstructionData(int c)
        {
            Char ch = notEof(c);

            if(isSpace(ch) || isAlpha(ch) || isQuote(ch) || 
               ch == ':' || ch == '/' || ch == '!' || ch == '=')
            {
                if(_nodeSize == _maxSize)
                    throw SyntaxError("node too long", line());

                _procInstr.data() += c;
                ++_nodeSize;
                return;
            }

            if(ch == '?')
            {
                _parse = &XmlReaderImpl::onProcessingInstructionEnd;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onProcessingInstructionEnd(int c)
        {
            Char ch = notEof(c);

            if(ch == '>')
            {
                setProcessingInstruction();
                
                if(depth() == 0)
                    popParseState(&XmlReaderImpl::onProlog);
                else
                    popParseState(&XmlReaderImpl::afterTag);
                
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onTag(int c)
        {
            switch(c)
            {
                case '?':
                    _procInstr.clear();
                    _parse = &XmlReaderImpl::onProcessingInstructionBegin;
                    return;

                case '!':
                    _parse = &XmlReaderImpl::onTagExclam;
                    return;

                case '/':
                    setCharactersEnd();

                    std::size_t n = _startElem.attributes().size();
                    while(n--)
                        _elements.pop();

                    _startElem.attributes().clear();

                    _parse = &XmlReaderImpl::onEndElement;
                    return;
            }

            if( c == std::char_traits<Char>::eof() || ! isAlpha(c) )
                throw SyntaxError("XML syntax error", line());

            setCharactersEnd();

            std::size_t n = _startElem.attributes().size();
            while(n--)
                _elements.pop();

            _startElem.attributes().clear();

            
            _elements.pushChar(c);
            ++_nodeSize;

            _parse = &XmlReaderImpl::onStartElement;
        }

        void onTagExclam(int c)
        {
            Char ch = notEof(c);

            if(ch == '-')
            {
                _parse = &XmlReaderImpl::beforeComment;
                return;
            }

            if(ch == '[' && depth() > 0)
            {
                _token.clear();
                _token += ch;
                _parse = &XmlReaderImpl::beforeCData;
                return;
            }

            if(ch == 'D' && depth() == 0)
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDocType;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        };
        
        void OnDocType(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += c;

                if(_token.length() < 7)
                    return;

                if(_token == L"DOCTYPE")
                {
                    _token.clear();
                    _parse = &XmlReaderImpl::AfterDocType;
                    return;
                }
            }

            throw SyntaxError("XML syntax error", line());
        }

        void AfterDocType(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _dtd.rootName().addName(ch);
                ++_nodeSize;
                _parse = &XmlReaderImpl::OnDtdRootName;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdPublic(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdPublic);
                return;
            }

            _token += ch;

            if(_token.length() < 6)
                return;

            bool ok = _token == L"PUBLIC";
            _token.clear();
            if( ! ok)
                throw SyntaxError("XML syntax error", line());
                
            _parse = &XmlReaderImpl::OnDtdBeforePublicId;
        }

        void OnDtdBeforePublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _parse = &XmlReaderImpl::OnDtdPublicId;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdBeforePublicId);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdPublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                popParseState();
                (this->*_parse)(c);
                return;
            }

            _token += ch;
        }

        void OnDtdSystem(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdSystem);
                return;
            }

            _token += ch;

            if(_token.length() < 6)
                return;

            bool ok = _token == L"SYSTEM";
            _token.clear();
            if( ! ok)
                throw SyntaxError("XML syntax error", line());
                
            _parse = &XmlReaderImpl::OnDtdBeforeSystemId;
        }

        void OnDtdBeforeSystemId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _parse = &XmlReaderImpl::OnDtdSystemId;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdBeforeSystemId);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdSystemId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                popParseState();
                (this->*_parse)(c);
                return;
            }

            _token += ch;
        }

        // The markup declarations may be made up in whole or in part of the
        // replacement text of parameter entities. The productions later in
        // this specification for individual nonterminals (elementdecl,
        // AttlistDecl, and so on) describe the declarations after all the
        // parameter entities have been included.

        // Parameter entity references are recognized anywhere in the DTD
        // (internal and external subsets and external parameter entities),
        // except in literals, processing instructions, comments, and the
        // contents of ignored conditional sections (see 3.4 Conditional
        // Sections). They are also recognized in entity value literals.
        // The use of parameter entities in the internal subset is restricted
        // furthermore.
        
        void OnDtdParameterEntityReference(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _entityRef.name() += ch;
                //_entityName += ch;
                return;
            }

            if(ch == ';')
            {
                //assert(_beforeEntityReference);
                resolveParamEntity(_entityRef);
                popParseState();
                //_parse = _beforeEntityReference;
                //_beforeEntityReference = 0;
                //_entityName.clear()
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void enterParameterReference(ParseFunc from)
        {
            //assert( ! _beforeEntityReference);
            pushParseState(from);
            //_beforeEntityReference = from;
            _entityRef.clear();
            _parse = &XmlReaderImpl::OnDtdParameterEntityReference;
        }

        void OnDtdRootName(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_nodeSize == _maxSize)
                    throw SyntaxError("node too large", line());

                _dtd.rootName().addName(ch);
                ++_nodeSize;
                return;
            }

            if( ch == ':' )
            {
                QName& qn = _dtd.rootName();
                qn.setPrefix( qn.name() ); // TODO: use swap
                qn.clearName();
                return;
            }

            if( isSpace(ch) )
            {
                _parse = &XmlReaderImpl::AfterDtdRootName;
                return;
            }

            // Note that it is possible to construct a well-formed document
            // containing a doctypedecl that neither points to an external subset
            // nor contains an internal subset.
            if(ch == '>')
            {
                setDocType();
                
                _parse = &XmlReaderImpl::onProlog;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        // If both the external and internal subsets are used, the internal
        // subset MUST be considered to occur before the external subset. This
        // has the effect that entity and attribute-list declarations in the
        // internal subset take precedence over those in the external subset.
        void AfterDtdRootName(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == 'S')
            {
                pushParseState(&XmlReaderImpl::OnDtdExternalSystemId);

                _token += ch;
                _parse = &XmlReaderImpl::OnDtdSystem;
                return;
            }

            if(ch == 'P')
            {
                pushParseState(&XmlReaderImpl::OnDtdExternalPublicId);

                _token += ch;
                _parse = &XmlReaderImpl::OnDtdPublic;
                return;
            }

            if( ch == '[' )
            {
                incDepth();
                _docType.setInternal(true);

                setDocType();
                _parse = &XmlReaderImpl::OnDtdInternal;
                return;
            }

            if(ch == '>')
            {
                setDocType();
                _parse = &XmlReaderImpl::onProlog;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdExternalPublicId(int c)
        {
            _docType.setPublicId(_token);
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdAfterExternalPublicId;
        }

        void OnDtdAfterExternalPublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                pushParseState(&XmlReaderImpl::OnDtdExternalSystemId);
                
                _parse = &XmlReaderImpl::OnDtdSystemId;
                return;
            }

            if( ch == '>' )
            {
                setDocType();

                bool externalDtd = resolveExternalDtd();
                if( externalDtd )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::onProlog;
                
                return;
            }

            if( ch == '[' )
            {
                incDepth();
                _docType.setInternal(true);
                setDocType();
                _parse = &XmlReaderImpl::OnDtdInternal;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdExternalSystemId(int c)
        {
            _docType.setSystemId(_token);
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdAfterExternalSystemId;
        }

        void OnDtdAfterExternalSystemId(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '>' )
            {
                setDocType();

                bool externalDtd = resolveExternalDtd();
                if( externalDtd )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::onProlog;

                return;
            }

            if( ch == '[' )
            {
                incDepth();
                _docType.setInternal(true);
                setDocType();
                _parse = &XmlReaderImpl::OnDtdInternal;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdInternal(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if(ch == '<')
            {
                pushParseState(&XmlReaderImpl::OnDtdInternal);

                _parse = &XmlReaderImpl::OnDtdTag;
                return;
            }

            if( ch == ']' )
            {
                assert( _parseStack.empty() );
                _parse = &XmlReaderImpl::OnDtdInternalEnd;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdInternal);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
        
        void OnDtdExternal(int c)
        {
            if( ! _input.isExternalDtd() )
            {
                _parse = &XmlReaderImpl::onProlog;
                onProlog(c);

                setEndDocType(false);
                return;
            }

            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if(ch == '<')
            {
                pushParseState(&XmlReaderImpl::OnDtdExternal);

                _parse = &XmlReaderImpl::OnDtdTag;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdExternal);
                return;
            }

            if(ch == ']')
            {          
                // end of INCLUDE
                popParseState();
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdTag(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '!')
            {
                _parse = &XmlReaderImpl::OnDtdTagExclam;
                return;
            }

            if(ch == '?')
            {
                _procInstr.clear();
                _parse = &XmlReaderImpl::onProcessingInstructionBegin;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdTag);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdTagExclam(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '-')
            {
                _parse = &XmlReaderImpl::beforeComment;
                return;
            }

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse  = &XmlReaderImpl::OnDtdTagName;
                return;
            }

            // INCLUDE/IGNORE only in external subset allowed
            if(ch == '[' && _input.isExternalDtd())
            {
                _parse = &XmlReaderImpl::OnDtdBeforeIgnoreOrInclude;
                return;
            }
            
            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdTagExclam);
                return;
            }
                
            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdTagEnd(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '>' )
            {
                popParseState();
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdTagEnd);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdTagName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( isSpace(ch) )
            {
                if(_token == L"ELEMENT")
                {
                    _parse = &XmlReaderImpl::OnDtdElementBegin;
                    _token.clear();
                    return;
                }
                else if(_token == L"ATTLIST")
                {
                    _token.clear();
                    _parse = &XmlReaderImpl::OnDtdAttListBegin;
                    return;
                }
                else if(_token == L"ENTITY")
                {
                    _token.clear();
                    _parse = &XmlReaderImpl::OnDtdEntityBegin;
                    return;
                }
                else if(_token == L"NOTATION")
                {
                    _token.clear();
                    _parse = &XmlReaderImpl::OnDtdNotationBegin;
                    return;
                }
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdTagName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
        
        void OnDtdBeforeIgnoreOrInclude(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'I')
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdIgnoreOrInclude;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdBeforeIgnoreOrInclude);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdIgnoreOrInclude(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( isSpace(ch) )
            {
                if(_token == L"INCLUDE")
                {
                    _token.clear();
                    _parse = &XmlReaderImpl::OnDtdIncludeBegin;
                    return;
                }
                else if(_token == L"IGNORE")
                {
                    _token.clear();
                    _parse = &XmlReaderImpl::OnDtdIgnoreBegin;
                    return;
                }
            }

            if(ch == '[')
            {
                if(_token == L"INCLUDE")
                {
                    _token.clear();
                    pushParseState(&XmlReaderImpl::OnDtdIncludeEnd);
                    _parse = &XmlReaderImpl::OnDtdExternal;
                    return; 
                }
                else if(_token == L"IGNORE")
                {
                    _token.clear();
                    _parse = &XmlReaderImpl::OnDtdIgnore; 
                    return;
                }
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIgnoreOrInclude);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdIgnoreBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '[')
            {             
                _parse = &XmlReaderImpl::OnDtdIgnore; 
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIgnoreBegin);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdIgnore(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == ']')
            {             
                _parse = &XmlReaderImpl::OnDtdIgnoreEnd; 
                return;
            }
        }

        void OnDtdIgnoreEnd(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == ']')
            {             
                _parse = &XmlReaderImpl::OnDtdIncludeEnd2;
                return;
            }

            _parse = &XmlReaderImpl::OnDtdIgnore; 
        }

        void OnDtdIncludeBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '[')
            {                                
                pushParseState(&XmlReaderImpl::OnDtdIncludeEnd);
                _parse = &XmlReaderImpl::OnDtdExternal;
                return; 
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIncludeBegin);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdIncludeEnd(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == ']')
            {             
                _parse = &XmlReaderImpl::OnDtdIncludeEnd2; 
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIncludeEnd);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdIncludeEnd2(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '>')
            {      
                popParseState();  
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIncludeEnd2);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdNotationBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '%')
            {
                _parse = &XmlReaderImpl::OnDtdNotationBegin;
                return;
            }

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdNotationName;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
        
        void OnDtdNotationName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( isSpace(ch) )
            {
                assert(_notation == 0);

                // must only be declared once
                _notation = _dtd.declareNotation(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdNotationAfterName;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNotationName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdNotationAfterName(int c)
        {
            Pt::Char ch = notEof(c);
            
            if( ch == 'S')
            {
                pushParseState(&XmlReaderImpl::OnDtdNotationSystemId);
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdSystem;
                return;
            }
            
            if(ch == 'P')
            {
                pushParseState(&XmlReaderImpl::OnDtdNotationPublicId);

                _token += ch;
                _parse = &XmlReaderImpl::OnDtdPublic;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNotationAfterName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdNotationPublicId(int c)
        {
            // can be NULL if already declared
            if(_notation)
                _notation->setPublicId(_token);
                
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdNotationAfterPublicId;
        }

        void OnDtdNotationAfterPublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                pushParseState(&XmlReaderImpl::OnDtdNotationSystemId);
                
                _parse = &XmlReaderImpl::OnDtdSystemId;
                return;
            }

            if( ch == '>' )
            {
                _notation = 0;
                
                popParseState(); // internal / external subset
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNotationAfterPublicId);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdNotationSystemId(int c)
        {
            if(_notation)
                _notation->setSystemId(_token);
                
            _token.clear();
            _notation = 0;
            _parse = &XmlReaderImpl::OnDtdTagEnd;
        }

        void OnDtdEntityBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '%')
            {
                // next state checks if we are on a parameter entity reference
                _parse = &XmlReaderImpl::OnDtdParamEntityBegin;
                return;
            }

            if( isAlpha(ch) )
            {
                _token += ch;
                assert( ! _paramEntity);
                _parse = &XmlReaderImpl::OnDtdEntityName;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdParamEntityBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                // 'name-char' after % is not a parameter entity, so jump back
                enterParameterReference(&XmlReaderImpl::OnDtdEntityBegin);
                return;
            }

            if( isSpace(ch) )
            {
                assert( ! _paramEntity);
                _paramEntity = true;
                _parse = &XmlReaderImpl::OnDtdBeforeParamEntityName;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdBeforeParamEntityName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEntityName;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdBeforeParamEntityName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntityName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( isSpace(ch) )
            {
                assert(_entity == 0);

                // bind to first declaration
                if(_paramEntity)
                    _entity = _dtd.declareParamEntity(_token);
                else
                    _entity = _dtd.declareEntity(_token);
                
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdEntityAfterName;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
        
        void OnDtdEntityAfterName(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'S')
            {
                pushParseState(&XmlReaderImpl::OnDtdEntitySystemId);

                _token += ch;
                _parse = &XmlReaderImpl::OnDtdSystem;
                return;
            }

            if( ch == '"' || ch == '\'' )
            {
                setQuotedBegin(ch);
                _parse = &XmlReaderImpl::OnDtdEntityValue;
                return;
            }
            
            if(ch == 'P')
            {
                pushParseState(&XmlReaderImpl::OnDtdEntityPublicId);

                _token += ch;
                _parse = &XmlReaderImpl::OnDtdPublic;
                return;
            }
            
            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityAfterName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntityPublicId(int c)
        {
            if(_entity)
                _entity->setPublicId(_token);
                
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdEntityAfterPublicId;
        }

        void OnDtdEntityAfterPublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                pushParseState(&XmlReaderImpl::OnDtdEntitySystemId);
                _parse = &XmlReaderImpl::OnDtdSystemId;
                return;
            }

            if( ch == '>' )
            {
                _entity = 0;
                _paramEntity = false;
                
                popParseState(); // internal / external subset
                return;
            }

            if(ch == 'N')
            {
                if(_paramEntity)
                    throw SyntaxError("invalid parameter entity", line());

                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEntityNDATA;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityAfterPublicId);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntitySystemId(int c)
        {
            if(_entity)
                _entity->setSystemId(_token);
                
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdEntityAfterSystemId;
        }
        
        void OnDtdEntityAfterSystemId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == 'N' )
            {
                if(_paramEntity)
                    throw SyntaxError("invalid parameter entity", line());
                
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEntityNDATA;
                return;
            }

            if( ch == '>' )
            {
                _entity = 0;
                _paramEntity = false;

                popParseState(); // internal / external subset
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityAfterSystemId);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntityNDATA(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'D' || ch == 'A' || ch == 'T')
            {
                _token += ch;
                return;
            }

            if( isSpace(ch) )
            {
                bool ok = _token == L"NDATA";
                _token.clear();
                if( ! ok)
                    throw SyntaxError("XML syntax error", line());
                
                _parse = &XmlReaderImpl::OnDtdEntityAfterNDATA;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityNDATA);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntityAfterNDATA(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEntityNotation;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityNDATA);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntityNotation(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }
            
            if( ch == '>' )
            {
                if(_entity)
                    _entity->setUnparsed(_token);

                _token.clear();
                _entity = 0;
                _paramEntity = false;

                popParseState(); // internal / external subset
                return;
            }

            if( isSpace(ch) )
            {
                if(_entity)
                    _entity->setUnparsed(_token);

                _token.clear();
                _entity = 0;
                _paramEntity = false;
                _parse = &XmlReaderImpl::OnDtdTagEnd;
                return;
            }
            
            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityNotation);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        // The entity value literal is preprocessed before it is used in an
        // entity reference. Character references are replaced immediately
        // with the specified character. Parameter entity references must be
        // be resolved recursively.
        void OnDtdEntityValue(int c)
        {
            Pt::Char ch = notEof(c);

            if( isQuoteEnd(ch) )
            {
                _entity = 0;
                _paramEntity = false;
                _parse = &XmlReaderImpl::OnDtdTagEnd;
                return;
            }

            if(ch == '&')
            {
                assert(_token.empty());
                _parse = &XmlReaderImpl::OnEntityValueCharacterReference;
                return;
            }

            if( ch == '%' )
            {
                _entityRef.clear();
                _parse = &XmlReaderImpl::OnDtdEntityValueParameterEntityReference;
                return;
            }

            if(_entity)
            {
                _entity->value() += ch;
            }
        }
        
        // Entity references in entity value literals are left as is except
        // numerical character rerefences, which are resolved immediately
        void OnEntityValueCharacterReference(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == ';')
            {
                if( ! _paramEntity)
                {
                    assert(_entity);

                    if(_entity && _entity->name() == _token)
                        throw SyntaxError("self reference", line());
                }

                if( ! Entity::resolveCharacterEntity(_token) )
                {
                    _token = '&' + _token + ';';
                }

                if(_entity)
                    _entity->addValue(_token);

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdEntityValue;
                return;
            }

            _token += ch;
        }

        void OnDtdEntityValueParameterEntityReference(int c)
        {
            Pt::Char ch = notEof(c);
            
            if( isAlpha(ch) )
            {
                _entityRef.name() += ch;
                return;
            }

            if(ch == ';')
            {
                if(_paramEntity)
                {
                    if(_entity && _entity->name() == _entityRef.name())
                        throw SyntaxError("self reference", line());
                }

                resolveParamEntity(_entityRef);
                _parse = &XmlReaderImpl::OnDtdEntityValue;
                return;
            }

            if(ch == '&')
            {
                // TODO: This is most likely allowed.
                throw SyntaxError("character entity reference in entity reference name", line());
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttListBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _qname.addName(ch);
                _parse = &XmlReaderImpl::OnDtdAttListName;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttListBegin);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttListName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _qname.addName(ch);
                return;
            }

            if(ch == ':')
            {
                _qname.setPrefix(_qname.name() ); // TODO: use swap
                _qname.clearName();
                return;
            }

            if( isSpace(ch) )
            {
                assert( _attlistDecl == 0 );
                _attlistDecl = &_dtd.declareAttributeList(_qname);
                _qname.clear();
                _parse = &XmlReaderImpl::OnDtdBeforeAttrName;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttListName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdBeforeAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _qname.addName(ch);
                _parse = &XmlReaderImpl::OnDtdAttrName;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdBeforeAttrName);
                return;
            }

            throw SyntaxError("XML syntax error: invalid attribute name", line());
        }

        void OnDtdAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                _parse = &XmlReaderImpl::OnDtdAfterAttrName;
                return;
            }
            
            if( isAlpha(ch) )
            {
                _qname.addName(ch);
                return;
            }

            if(ch == ':')
            {
                _qname.setPrefix(_qname.name() ); // TODO: use swap
                _qname.clearName();
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrName);
                return;
            }

            throw SyntaxError("XML syntax error: invalid attribute name", line());
        }

        void OnDtdAfterAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == 'C' || ch == 'N' ||  ch == 'I' || ch == 'E')
            {
                assert( _token.empty() );
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdAttrType;
                return;
            }

            if( ch == '(' )
            {
                assert(_attrModel == 0);
                assert(_attlistDecl);

                if( 0 == _attlistDecl->findAttribute( _qname ) )
                {
                    _attrModel = new EnumAttributeModel();
                    _attrModel->setName(_qname);
                    _attlistDecl->addAttribute(_attrModel);
                }

                _qname.clear();
                _parse = &XmlReaderImpl::OnDtdAttrEnum;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAfterAttrName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrEnum(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                assert( _token.empty() );
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdAttrEnumValue;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrEnum);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrEnumValue(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( isSpace(ch) )
            {
                assert(_attrModel);
                static_cast<EnumAttributeModel*>(_attrModel)->addValue(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrAfterEnumValue;
                return;
            }

            if( ch == '|' )
            {
                assert(_attrModel);
                static_cast<EnumAttributeModel*>(_attrModel)->addValue(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrEnumSep;
                return;
            }

            if( ch == ')' )
            {
                assert(_attrModel);
                static_cast<EnumAttributeModel*>(_attrModel)->addValue(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterAttrType;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrEnumValue);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrAfterEnumValue(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '|' )
            {
                _parse = &XmlReaderImpl::OnDtdAttrEnumSep;
                return;
            }

            if( ch == ')' )
            {
                _parse = &XmlReaderImpl::OnDtdAfterAttrType;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrAfterEnumValue);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrEnumSep(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                _parse = &XmlReaderImpl::OnDtdAfterAttrEnumSep;
                return;
            }

            if( isAlpha(ch) )
            {
                assert(_token.empty());
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdAttrEnumValue;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrEnumSep);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAfterAttrEnumSep(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                assert(_token.empty());
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdAttrEnumValue;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrEnumSep);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrType(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrType);
                return;
            }

            if( isSpace(ch) )
            {
                assert(_attrModel == 0);
                assert(_attlistDecl);

                _parse = &XmlReaderImpl::OnDtdAfterAttrType;

                if( 0 == _attlistDecl->findAttribute(_qname) )
                {
                    if(_token == L"CDATA")
                    {
                        _attrModel = new CDataAttributeModel();
                    }
                    else if(_token == L"NMTOKEN")
                    {
                        _attrModel = new NMTokenAttributeModel();
                    }
                    else if(_token == L"NMTOKENS")
                    {
                        _attrModel = new NMTokensAttributeModel();
                    }
                    else if(_token == L"ID")
                    {
                        _attrModel = new IDAttributeModel();
                    }
                    else if(_token == L"IDREF")
                    {
                        _attrModel = new IDRefAttributeModel();
                    }
                    else if(_token == L"IDREFS")
                    {
                        _attrModel = new IDRefsAttributeModel();
                    }
                    else if(_token == L"ENTITY")
                    {
                        _attrModel = new EntityAttributeModel(_dtd);
                    }
                    else if(_token == L"ENTITIES")
                    {
                        _attrModel = new EntitiesAttributeModel(_dtd);
                    }
                    else if(_token == L"NOTATION")
                    {
                        _attrModel = new NotationAttributeModel(_dtd);
                        _parse = &XmlReaderImpl::OnDtdAfterAttrNotation;
                    }
                    else
                        throw SyntaxError("invalid attribute declaration type", line());
                }

                if(_attrModel)
                {
                    _attrModel->setName(_qname);
                    _attlistDecl->addAttribute(_attrModel);
                }

                _qname.clear();
                _token.clear();
                return;
            }

            _token += ch;
        }

        void OnDtdAfterAttrNotation(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if(ch == '(')
            {
                _parse = &XmlReaderImpl::OnDtdAttrNotationBegin;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAfterAttrNotation);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrNotationBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdAttrNotationId;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrNotationBegin);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrNotationId(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( isSpace(ch) )
            {
                if(_attrModel) // skip duplicates
                    static_cast<NotationAttributeModel*>(_attrModel)->addNotation(_token);

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrAfterNotationId;
                return;
            }

            if( ch == '|' )
            {
                if(_attrModel) // skip duplicates
                    static_cast<NotationAttributeModel*>(_attrModel)->addNotation(_token);

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrNotationSep;
                return;
            }

            if( ch == ')' )
            {
                if(_attrModel) // skip duplicates
                    static_cast<NotationAttributeModel*>(_attrModel)->addNotation(_token);

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterAttrType;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrNotationId);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrAfterNotationId(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
                return;

            if( ch == '|' )
            {
                _parse = &XmlReaderImpl::OnDtdAttrNotationSep;
                return;
            }

            if( ch == ')' )
            {
                _parse = &XmlReaderImpl::OnDtdAfterAttrType;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrAfterNotationId);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrNotationSep(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdAttrNotationId;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrNotationSep);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAfterAttrType(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
                return;

            if(ch == '"')
            {
                _parse = &XmlReaderImpl::OnDtdAttrDefault;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAfterAttrType);
                return;
            }

            if( ch != '#' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdAttrMode;
        }

        void OnDtdAttrMode(int c)
        {
            Pt::Char ch = notEof(c);
            
            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrMode);
                return;
            }

            if(_token == L"REQUIRED")
            {
                if(_attrModel) // skip duplicates
                    _attrModel->setMode(Pt::Xml::AttributeModel::Required);
                
                _parse = &XmlReaderImpl::OnDtdAfterAttrMode;
            }
            else if(_token == L"IMPLIED")
            {
                if(_attrModel) // skip duplicates
                    _attrModel->setMode(Pt::Xml::AttributeModel::Implied);
                
                _parse = &XmlReaderImpl::OnDtdAfterAttrMode;
            }
            else if(_token == L"FIXED")
            {
                if(_attrModel) // skip duplicates
                    _attrModel->setMode(Pt::Xml::AttributeModel::Fixed);
                
                _parse = &XmlReaderImpl::OnDtdAfterDtdAttrFixed;
            }
            else
                throw SyntaxError("XML syntax error", line());
                
            _token.clear();

            (this->*_parse)(c);
        }

        void OnDtdAfterAttrMode(int c)
        {
            Pt::Char ch = notEof(c);

            if(c == '>')
            {
                _attrModel = 0;

                assert(_attlistDecl);
                _attlistDecl = 0;

                popParseState(); // internal / external subset
                return;
            }
            
            if( isSpace(ch) )
                return;

            if( isAlpha(ch) )
            {
                _attrModel = 0;
                
                _qname.addName(ch);
                _parse = &XmlReaderImpl::OnDtdAttrName;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAfterAttrMode);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAfterDtdAttrFixed(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '"')
            {
                _parse = &XmlReaderImpl::OnDtdAttrDefault;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAfterDtdAttrFixed);
                return;
            }

            if( isSpace(ch) )
                return;

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrDefault(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '"')
            {
                //assert(_attrModel);
                if(_attrModel) // skip duplicates
                    _attrModel->setDefaultValue(_token);
                
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterAttrMode;
                return;
            }

            _token += ch;
        }

        void OnDtdElementBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _qname.addName(ch);
                _parse = &XmlReaderImpl::OnDtdElementName;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdElementBegin);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdElementName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _qname.addName(ch);
                return;
            }

            if(ch == ':')
            {
                _qname.setPrefix(_qname.name() ); // TODO: use swap
                _qname.clearName();
                return;
            }

            if( isSpace(ch) )
            {
                assert(_contentModel == 0);
                
                // must only be declared once
                _contentModel = _dtd.declareContent(_qname);
                if( ! _contentModel)
                    throw SyntaxError("duplicate element declaration", line());

                _cmBuilder.reset(*_contentModel);
                _qname.clear();
                _parse = &XmlReaderImpl::OnDtdElementContentBegin;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdElementName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdElementContentBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'E' || ch == 'A')
            {
                assert(_token.empty());
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEmptyOrAny;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdElementContentBegin);
                return;
            }

            if(ch != '(')
                throw SyntaxError("XML syntax error", line());

            _cmBuilder.pushScope();
            _parse = &XmlReaderImpl::OnDtdElementContent;
        }
        
        void OnDtdEmptyOrAny(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                
                if(_token == L"EMPTY")
                {
                    _token.clear();
                    
                    assert(_contentModel);
                    _contentModel->setEmpty();
                    _contentModel = 0;
                    
                    _parse = &XmlReaderImpl::OnDtdTagEnd;
                }
                else if(_token == L"ANY")
                {
                    _token.clear();

                    assert(_contentModel);
                    _contentModel->setAny();
                    _contentModel = 0;
                    
                    _parse = &XmlReaderImpl::OnDtdTagEnd;
                }

                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEmptyOrAny);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void setElementDeclaration()
        {
            assert(_contentModel);
            bool ok = _cmBuilder.finish();
            if( ! ok )
                throw SyntaxError("invalid element declaration", line());
                
            _contentModel = 0;
        }

        void OnDtdBeforeElementEnd(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '>')
            {
                setElementDeclaration();
                popParseState(); // internal / external subset
                return;
            }
            
            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdBeforeElementEnd);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdElementContent(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                assert(_token.empty());
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdIdentifier;
                return;
            }

            if(ch == '(')
            {
                _cmBuilder.pushScope();
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdElementContent);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdIdentifier(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) || ch == ':')
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdIdentifier;
                return;
            }

            if( ch == ',')
            {
                _cmBuilder.pushOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _cmBuilder.pushOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _cmBuilder.pushOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _cmBuilder.pushOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _cmBuilder.pushOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _cmBuilder.pushOperand(_token);
                _token.clear();
                
                bool ok = _cmBuilder.reduceScope();
                if( ! ok )
                    throw SyntaxError("invalid element declaration", line());
                
                _parse = &XmlReaderImpl::OnDtdContentExprEnd;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIdentifier);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
        
        void OnDtdUnrayOp(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '>')
            {
                setElementDeclaration(); 
                popParseState(); // internal / external subset             
                return;
            }

            if( ch == ',')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == ')')
            {
                bool ok = _cmBuilder.reduceScope();
                if( ! ok )
                    throw SyntaxError("invalid element declaration", line());
                
                _parse = &XmlReaderImpl::OnDtdContentExprEnd;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdUnrayOp);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
        
        void OnDtdBinaryOp(int c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) || ch == '#')
            {
                assert(_token.empty());
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdIdentifier;
                return;
            }

            if(ch == '(')
            {
                _cmBuilder.pushScope();
                _parse = &XmlReaderImpl::OnDtdElementContent;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdBinaryOp);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdContentExprEnd(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '>' )
            {
                setElementDeclaration();
                popParseState(); // internal / external subset
                return;
            }

            if( ch == ',')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if( ch == ')')
            {
                bool ok = _cmBuilder.reduceScope();
                if( ! ok )
                    throw SyntaxError("invalid element declaration", line());
                
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdContentExprEnd);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdInternalEnd(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '>' )
            {
                setEndDocType(true);
                decDepth();

                bool externalDtd = resolveExternalDtd();
                if( externalDtd )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::onProlog;
                
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdInternalEnd);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void afterTag(int c)
        {
            assert(depth() != 0);
            
            Char ch = notEof(c);

            switch(c)
            {
                case ' ':
                case '\n':
                case '\t':
                    _chars.appendSpace(ch);
                    _nodeSize = _chars.content().size();
                    _parse = &XmlReaderImpl::onCharacters;
                    break;

                case '\r':
                    _nodeSize = _chars.content().size();
                    _parse = &XmlReaderImpl::onCharactersCR;
                    break;

                case '<':
                    _parse = &XmlReaderImpl::onTag;
                    break;

                case '>':
                    throw SyntaxError("XML syntax error", line());
                    break;

                case '&':
                    assert(_token.empty());
                    _parse = &XmlReaderImpl::onEntityReference;
                    break;

                default:
                    _chars.append(ch);
                    _nodeSize = _chars.content().size();
                    _parse = &XmlReaderImpl::onCharacters;
                    break;
            }
        }

        void beforeComment(int c)
        {
            if(c == '-')
            {
                _comment.content().clear();
                _parse = &XmlReaderImpl::onComment;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onComment(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '-')
            {
                _parse = &XmlReaderImpl::afterComment;
                return;
            }

            if(_nodeSize == _maxSize)
                throw SyntaxError("element name too long", line());

            _comment.content() += ch;
            ++_nodeSize;
        }

        void afterComment(int c)
        {
            Char ch = notEof(c);

            if(ch == '-')
            {
                _parse = &XmlReaderImpl::onCommentEnd;
                return;
            }

            if(_nodeSize == _maxSize)
                throw SyntaxError("element name too long", line());

            _comment.content() += '-';
            _comment.content() += ch;
            _nodeSize += 2;
            
            _parse = &XmlReaderImpl::onComment;
        }

        void onCommentEnd(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '>')
            {
                setComment();
                
                if(depth() == 0)
                    popParseState(&XmlReaderImpl::onProlog);
                else
                    popParseState(&XmlReaderImpl::afterTag);

                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onStartElement(int c)
        {
            switch(c)
            {
                case ' ':
                case '\n':
                case '\r':
                case '\t':
                    _usedSize += _elements.pushName();
                    _startElem.setName(_elements.top(), _nsctx.emptyNamespace());
                    _parse = &XmlReaderImpl::beforeAttribute;
                    return;

                case '/':
                    _usedSize += _elements.pushName();
                    _startElem.setName(_elements.top(), _nsctx.emptyNamespace());
                    
                    _chars.clear();               
                    setStartElement();
                    _parse = &XmlReaderImpl::onEmptyElement;
                    return;

                case ':':
                {
                    if( ! _elements.pushPrefix() )
                        throw SyntaxError("invalid prefix", line());
                    
                    return;
                }

                case '>':
                    _usedSize += _elements.pushName();
                    _startElem.setName(_elements.top(), _nsctx.emptyNamespace());
                    
                    _chars.clear();
                    setStartElement();
                    _parse = &XmlReaderImpl::afterTag;
                    return;  
            }

            if( c == std::char_traits<Char>::eof() || ! isAlpha(c) )
                throw SyntaxError("XML syntax error", line());
            
            if(_nodeSize == _maxSize)
                throw SyntaxError("element name too long", line());

            _elements.pushChar(c);
            ++_nodeSize;
        }

        void beforeAttribute(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if(ch == '/')
            {
                _chars.clear(); 
                setStartElement();
                _parse = &XmlReaderImpl::onEmptyElement;
                return;
            }

            if( isAlpha(ch) )
            {
                _elements.pushChar(ch);
                //_attr = &_startElem.attributes().push();
                //_attr->qname().addName(c);
                ++_nodeSize;

                _parse = &XmlReaderImpl::onAttributeName;
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                setStartElement();
                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onAttributeName(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                _parse = &XmlReaderImpl::afterAttributeName;
                return;
            }

            if(ch == '=')
            {
                _parse = &XmlReaderImpl::beforeAttributeValue;
                return;
            }

            if(ch == ':')
            {
                //assert(_attr);

                if( ! _elements.pushPrefix() )
                    throw SyntaxError("invalid attribute prefix", line());
                
                //QName& qn = _attr->qname();
                //if( ! qn.prefix().empty() )
                //    throw SyntaxError("invalid namespace prefix", line());

                //qn.setPrefix(qn.name() ); // TODO: use swap
                //qn.clearName();
                return;
            }

            if( isAlpha(ch) )
            {
                if(_nodeSize == _maxSize)
                    throw SyntaxError("node too long", line());

                //_attr->qname().addName(c);
                _elements.pushChar(ch);
                ++_nodeSize;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void afterAttributeName(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if(ch == '=')
            {               
                _parse = &XmlReaderImpl::beforeAttributeValue;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void beforeAttributeValue(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isQuote(ch) )
            {
                _usedSize += _elements.pushName();
                
                const QName& name = _elements.top();
                const Namespace& ns = _nsctx.emptyNamespace();
                _attr = &_startElem.attributes().append(name, ns);

                setQuotedBegin(ch);
                _parse = &XmlReaderImpl::onAttributeValue;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onAttributeValue(int c)
        {
            Char ch = notEof(c);
            assert(_attr);

            if( isQuoteEnd(ch) )
            {
                if(_attr->qname().prefix() == "xmlns")
                {
                    _usedSize += _nsctx.setNamespace(_depth+1, _attr->qname().name(), _attr->value());
                    _startElem.attributes().pop();
                    _elements.pop();
                }
                else if(_attr->qname().name() == "xmlns")
                {
                    _usedSize += _nsctx.setDefaultNamespace(_depth+1, _attr->value());
                    _startElem.attributes().pop();
                    _elements.pop();
                }
                
                _attr = 0;
                _parse = &XmlReaderImpl::beforeAttribute;
                return;
            }

            if (ch == '&')
            {
                // For a character reference, append the referenced character to the
                // attribute value.
                // For an entity reference, recursively process the replacement text
                // of the entity.
                assert(_token.empty());
                _parse = &XmlReaderImpl::onAttributeEntityReference;
                return;
            }

            // For white space characters (#x20, #xD, #xA, #x9), append #x20 to
            // the normalized value, with the exception that a single #x20 is
            // appended for a #xD#xA sequence that is part of an external parsed
            // entity or the literal entity value of an internal parsed entity.
            if( ch == '\r' | ch == '\n' | ch == '\t' )
            {
                ch = ' ';
            }

            if(_nodeSize == _maxSize)
                throw SyntaxError("node too long", line());
            
            _attr->value() += ch;
            ++_nodeSize;
        }

        void onAttributeEntityReference(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token += ch;
                ++_nodeSize;
                return;
            }
            
            if(ch == ';')
            {       
                _nodeSize -= _token.size();        
                
                if( Entity::resolveDefaultEntity(_token) )
                {
                    _attr->value() += _token;
                    _nodeSize += _token.size();
                }
                else
                {
                    _entityRef.setName(_token);
                    resolveEntity(_entityRef);
                }

                _token.clear();
                _parse = &XmlReaderImpl::onAttributeValue;
                return;
            }
            
            throw SyntaxError("XML syntax error", line());
        };

        void onEmptyElement(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if(ch == '>')
            {
                std::size_t n = _startElem.attributes().size();
                while(n--)
                    _elements.pop();

                _startElem.attributes().clear();
                
                setEndElement();
                _parse = &XmlReaderImpl::afterEndElement;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onEndElement(int c)
        {           
            if( _elements.empty() )
                throw SyntaxError("unmatched element", line());

            const QName& name = _elements.top();
            _back = name.prefix().empty() ? name.name().c_str() 
                                          : name.prefix().c_str();

            if( c != _back->value() || *_back == '\0' )
                throw SyntaxError("unmatched element", line());

            ++_back;
            ++_nodeSize;
            _parse = &XmlReaderImpl::onEndElementName;
        }

        void onEndElementName(int c)
        {  
            switch(c)
            {
                case '>':
                    if(*_back != '\0')
                        throw SyntaxError("unmatched element", line());
                    
                    _chars.clear();
                    setEndElement();
                    _parse = &XmlReaderImpl::afterEndElement;
                    return;

                case ' ':
                case '\n':
                case '\r':
                case '\t':       
                    if(*_back != '\0')
                        throw SyntaxError("unmatched element", line());
                                   
                    _parse = &XmlReaderImpl::afterEndElementName;
                    return;

                case ':':
                    if(*_back != '\0')
                        throw SyntaxError("unmatched element", line());
                    
                    _back = _elements.top().name().c_str();
                    return;
            }

            if( c != _back->value() || *_back == '\0' )
                throw SyntaxError("unmatched element", line());

            ++_back;
            ++_nodeSize;
        }
    
        void afterEndElementName(int c)
        {          
            switch(c)
            {
                case ' ':
                case '\n':
                case '\r':
                case '\t':
                    break;

                case '>':
                    _chars.clear();
                    setEndElement();
                    _parse = &XmlReaderImpl::afterEndElement;
                    break;

                default:
                    throw SyntaxError("XML syntax error", line());
            }
        }

        void afterEndElement(int c)
        {
            _usedSize -= _elements.pop();

            _parse = depth() == 0 ? &XmlReaderImpl::onEpilog
                                  : &XmlReaderImpl::afterTag;

            _usedSize -= _nsctx.popNamespace( _depth + 1 );
            (this->*_parse)(c);
        }

        void onCharacters(int c)
        {
            Char ch = notEof(c);

            switch(c)
            {                    
                case '<':
                    _parse = &XmlReaderImpl::onTag;
                    break;
                
                case '>':
                    throw SyntaxError("XML syntax error", line());

                case '&':
                    assert(_token.empty());
                    _nodeSize = _usedSize;
                    _parse = &XmlReaderImpl::onEntityReference;
                    break;

                case '\r':
                    _parse = &XmlReaderImpl::onCharactersCR;
                    break;
            
                case ' ':
                case '\t':
                case '\n':
                    _chars.appendSpace(ch);
                    ++_nodeSize;

                    if(_nodeSize >= _chunkSize)
                    {
                        _parse = &XmlReaderImpl::onCharactersMax;
                        setCharactersChunk();
                    }
                    
                    break;
            
                default:                   
                    _chars.append(ch);
                    ++_nodeSize;

                    if(_nodeSize >= _chunkSize)
                    {
                        _parse = &XmlReaderImpl::onCharactersMax;
                        setCharactersChunk();
                    }
            }
        }

        void onCharactersCR(int c)
        {
            _chars.appendSpace('\n');
            _parse = &XmlReaderImpl::onCharacters;
            
            if(c != '\n')
            {
                onCharacters(c);
            }
            else if(_nodeSize >= _chunkSize)
            {
                _parse = &XmlReaderImpl::onCharactersMax;
                setCharactersChunk();
            }
        }

        void onCharactersMax(int c)
        {
            _chars.clear();
            _parse = &XmlReaderImpl::onCharacters;
            onCharacters(c);
        }

        void onEntityReference(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token += ch;
                
                ++_nodeSize;
                if(_nodeSize >= _maxSize)
                    throw SyntaxError("node too large", line());

                return;
            }

            if(ch == ';')
            {
                _nodeSize = _chars.content().size();

                if( Entity::resolveDefaultEntity(_token) )
                {
                    std::size_t tokenSize = _token.size();
                    _nodeSize += tokenSize;
                    for(std::size_t n = 0; n < tokenSize; ++n)
                    {
                        Pt::Char c = _token[n];
                        if(c == ' ' || c == '\n' || c == '\r' || c == '\t')
                            _chars.appendSpace(c);
                        else
                            _chars.append(c);
                    }
                }
                else
                {
                    _entityRef.setName(_token); // use swap
                    resolveEntity(_entityRef);
                }

                _token.clear();
                _parse = &XmlReaderImpl::onCharacters;
                return;
            }

            throw SyntaxError("invalid entity format", line());
        };

        void beforeCData(int c)
        {
            Char ch = notEof(c);

            switch( ch.value() )
            {
                case '[':
                case 'C':
                case 'D':
                case 'A':
                case 'T':
                    _token += ch;
                    break;

                default:
                    throw SyntaxError("XML syntax error", line());
            }
            
            if( _token.length() < 7 )
                return;

            if( _token == L"[CDATA[" )
            {
                _token.clear();

                _nodeSize = _chars.content().size();

                if( (_options & ReportCData) && ! _chars.empty() )
                {
                    setCharactersChunk();
                }
                
                _parse = &XmlReaderImpl::onCDataBegin;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        };

        void onCDataBegin(int c)
        {
            if(_options & ReportCData)
            {
                _chars.clear();
                _chars.setCData(true);
            }

            _parse = &XmlReaderImpl::onCData;
            onCData(c);
        }

        void onCData(int c)
        {
            Char ch = notEof(c);
            if(ch == ']')
            {
                _parse = &XmlReaderImpl::onCDataClose0;
                return;
            }

            if( isSpace(ch) )
                _chars.appendSpace(ch);
            else
                _chars.append(ch);

            ++_nodeSize;

            if(_nodeSize >= _chunkSize)
            {
                setCharactersChunk();
                _parse = &XmlReaderImpl::onCDataMax;
            }
        }

        void onCDataMax(int c)
        {
            _chars.clear();

            if(_options & ReportCData)
                _chars.setCData(true);
            
            _parse = &XmlReaderImpl::onCData;
            onCData(c);
        }

        void onCDataClose0(int c)
        {
            Char ch = notEof(c);
            
            if(ch == ']')
            {
                _parse = &XmlReaderImpl::onCDataClose1;
                return;
            }

            _chars.append(']');
            _chars.append(ch);
            _nodeSize += 2;

            _parse = &XmlReaderImpl::onCData;
        }

        void onCDataClose1(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '>')
            {
                if( _options & ReportCData )
                {
                    setCharactersEnd();
                }

                _parse = &XmlReaderImpl::afterCData;
                return;
            }

            _chars.append(']');
            _chars.append(']');
            _chars.append(ch);
            _nodeSize += 3;
            
            _parse = &XmlReaderImpl::onCData;
        }

        void afterCData(int c)
        {
            if( _options & ReportCData )
                _chars.clear();
            
            _parse = &XmlReaderImpl::afterTag;
            afterTag(c);
        }

        void onEpilog(int c)
        {
            if( c == std::char_traits<Char>::eof() )
            {
                _current = &_endDoc;
                return;
            }

            Char ch(c);
            if( isSpace(ch) )
            {
                return;
            }

            if (ch == '<' )
            {
                // TODO: -> onEpilogTag, allow only Comments and ProcInstr
                _parse = &XmlReaderImpl::onTag;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        };

        void onProlog(int c)
        {           
            if( c == std::char_traits<Char>::eof() )
            {
                _current = &_endDoc;
                return;
            }

            Char ch(c);
            if( isSpace(ch) )
            {
                return;
            }

            if( ch == '<')
            {
                _parse = &XmlReaderImpl::onTag;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

    private:
        inline Char notEof(int c) const
        {
            if( c == std::char_traits<Char>::eof() )
            {
                throw SyntaxError("XML syntax error", line());
            }

            return Char(c);
        }

        inline bool isQuote(Char ch) const
        {
            return ch == '\'' || ch =='"';
        }

        inline void setQuotedBegin(Char ch)
        {
            _quotChar = ch;
        }

        inline bool isQuoteEnd(Char ch) const
        {
            return ch == _quotChar;
        }

        // TODO: isXmlName()
        inline bool isAlpha(Char ch) const
        {
            return ch == '.' || ch == '_' || ch == '-' || Pt::isalnum(ch) != 0;
        }
        
        inline bool isSpace(Char ch) const
        {
            return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
        }

        inline void pushParseState(ParseFunc parse)
        {
            _parseStack.push(parse);
        }

        inline void popParseState()
        {
            if( _parseStack.empty() )
                throw SyntaxError("XML syntax error", line());

            _parse = _parseStack.top();
            _parseStack.pop();
        }
        
        inline void popParseState(ParseFunc parse)
        {
            if( _parseStack.empty() )
            {
                _parse = parse;
            }
            else
            {
                _parse = _parseStack.top();
                _parseStack.pop();
            }
        }

        class EntityResolver : public XmlResolver
        {
            public:
                EntityResolver()
                {}

                InputSource* resolveEntity(const Entity& ent)
                {
                    return new StringInputSource( ent.value() );
                }

            protected:
                virtual InputSource* onResolveInput(const Pt::String& publicId, const Pt::String& systemId)
                {
                    return 0;
                }

                virtual void onReleaseInput(InputSource* is)
                {
                    delete is;
                }
        };

        bool resolveExternalDtd()
        {
            if( _resolver && _docType.isExternal() )
            {
                InputSource* is = _resolver->resolveInput( _docType.publicId(), _docType.systemId() );
                if(is)
                {
                    _input.setExternalDtd(*is, _resolver);
                    return true;
                }
            }

            return false;
        }

        void resolveEntity(EntityReference& entref)
        {
            InputSource* is = 0;
            int reportEntityRefs = _options & ReportEntityReferences;
            const Entity* ent = _dtd.findEntity( entref.name() );

            if( ! reportEntityRefs && ent && ! ent->isUnparsed() )
            {
                if(_input.size() == _maxInputDepth)
                    throw SyntaxError("input depth exceeded", line());

                if( ent->isInternal() )
                {
                    is = _entityResolver.resolveEntity(*ent);
                    _input.addInput(*is, &_entityResolver);
                }
                else if( ent->isExternal() && _resolver)
                {
                    is = _resolver->resolveInput( ent->publicId(), ent->systemId() );
                    if(is)
                        _input.addInput(*is, _resolver);
                }
            }
            
            if( ! is)
            {
                entref.setEntity(ent);
                _current = &entref;
            }
        }

        void resolveParamEntity(EntityReference& entref)
        {
            InputSource* is = 0;
            int reportEntityRefs = _options & ReportEntityReferences;
            const Entity* ent = _dtd.findParamEntity( entref.name() );
            
            if( ! reportEntityRefs && ent )
            {
                if(_input.size() == _maxInputDepth)
                    throw SyntaxError("input depth exceeded", line());

                if( ent->isInternal() )
                {
                    is = _entityResolver.resolveEntity(*ent);
                    _input.addInput(*is, &_entityResolver);
                }
                else if( ent->isExternal() && _resolver)
                {
                    is = _resolver->resolveInput( ent->publicId(), ent->systemId() );
                    if(is)
                        _input.addInput(*is, _resolver);
                }
            }

            if( ! is)
            {
                entref.setEntity(ent);
                _current = &entref;
            }
        }

        void setStartDoc()
        {
            if(_options & ReportStartDocument)
            {
                bool standalone = _is && _is->declaration() && _is->declaration()->isStandalone();
                _startDoc.setStandalone(standalone);
                _current = &_startDoc;
            }

            _nodeSize = _usedSize;
        }

        void setDocType()
        {
            if(_options & ReportDtd)
                _current = &_docType;

            _nodeSize = _usedSize;
        }

        void setEndDocType(bool internSubset)
        {
            if(_options & ReportDtd)
            {
                _endDocType.clear();
                _endDocType.setInternal(internSubset);
                _current = &_endDocType;
            }

            _nodeSize = _usedSize;
        }

        void setComment()
        {
            if(_options & ReportComments)
                _current = &_comment;

            _nodeSize = _usedSize;
        }

        void setProcessingInstruction()
        {
            if(_options & ReportProcessingInstructions)
                _current = &_procInstr;

            _nodeSize = _usedSize;
        }

        void setStartElement()
        {
            _nodeSize = _usedSize;

            const QName& name = _startElem.name();
            const String& prefix = name.prefix();
            
            if( ! prefix.empty() )
            {               
                const Namespace* ns = _nsctx.findPrefix( prefix );
                if( ! ns )
                    throw SyntaxError("undeclared namespace prefix", line());

                _startElem.setName(name, *ns);
            }
            else
            {
                const Namespace& ns = _nsctx.getDefaultNamespace();
                _startElem.setName(name, ns);
            }

            AttributeList& attributes = _startElem.attributes();
            AttributeList::Iterator it = attributes.begin();
            
            ElementModel* elemDecl = 0;
            if( it != attributes.end() )
                elemDecl = _dtd.findElement( _startElem.name() );
            
            for( ; it != attributes.end(); ++it)
            {
                if( it->qname().prefix().empty() )
                {
                    const Namespace& ns = _nsctx.getDefaultNamespace();
                    it->setNamespace(ns);
                }
                else
                {
                    const Namespace* ns = _nsctx.findPrefix( it->qname().prefix() );
                    if( ! ns )
                        throw SyntaxError("undeclared namespace prefix", line());

                    it->setNamespace(*ns);
                }

                // If the declared value is not CDATA, then discard any leading and
                // trailing space (#x20) characters and replace sequences of space
                // (#x20) characters by a single space (#x20) character.
                // All attributes for which no declaration has been read SHOULD be 
                // treated by a non-validating processor as if declared CDATA.
                if(elemDecl)
                {
                    AttributeModel* attrDecl = elemDecl->attributes().findAttribute( it->qname() );

                    if(attrDecl && attrDecl->isNormalize())
                        it->normalize();
                }
            }

            incDepth();

            _current = &_startElem;
        }

        void setEndElement()
        {            
            _nodeSize = _usedSize;

            const QName& name = _elements.top();
            const String& prefix = name.prefix();

            if( prefix.empty() )
            {
                const Namespace& ns = _nsctx.getDefaultNamespace();
                _endElem.setName(name, ns);
            }
            else
            {
                const Namespace* ns = _nsctx.findPrefix( prefix );
                if( ! ns )
                    throw SyntaxError("undeclared namespace prefix", line());
                
                _endElem.setName(name, *ns);
            }

            decDepth();
            
            _current = &(_endElem);
        }

        inline void setCharactersChunk()
        {
            _current = &_chars;
            _nodeSize = 0;
        }

        inline void setCharactersEnd()
        {
            if( ! _chars.content().empty() )
            {
                _current = &_chars;
                _nodeSize = _usedSize;
            }
        }

    public:
        enum Option
        {
            ReportDtd = 1,
            ReportStartDocument = 2,
            ReportProcessingInstructions = 4,
            ReportComments = 8,
            ReportCData = 16,
            ReportEntityReferences = 32
        };

        static const int DefaultOptions = 0;

        XmlReaderImpl(XmlResolver* resolver = 0)
        : _is(0)
        , _resolver(resolver)
        , _options(DefaultOptions)
        , _maxSize(2048)
        , _chunkSize(1024)
        , _maxInputDepth(8)
        , _notation(0)
        , _entity(0)
        , _paramEntity(false)
        , _attr(0)
        , _depth(0)
        , _usedSize(_chunkSize)
        , _nodeSize(_usedSize)
        , _parse(0)                 
        , _current(0)
        , _back(0)
        , _cmBuilder()
        , _contentModel(0)
        , _attrModel(0)
        , _attlistDecl(0)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;
        }

        XmlReaderImpl(InputSource& is, XmlResolver* resolver = 0)
        : _is(&is)
        , _resolver(resolver)
        , _options(DefaultOptions)
        , _maxSize(2048)
        , _chunkSize(1024)
        , _maxInputDepth(8)
        , _notation(0)
        , _entity(0)
        , _paramEntity(false)
        , _attr(0)
        , _depth(0)
        , _usedSize(_chunkSize)
        , _nodeSize(_usedSize)
        , _parse(0)                 
        , _current(0)
        , _back(0)
        , _cmBuilder()
        , _contentModel(0)
        , _attrModel(0)
        , _attlistDecl(0)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;

            _input.addInput(is);
        }

        ~XmlReaderImpl()
        { }
        
        void setOption(Option o)
        {
            _options |= o;
        }

        void unsetOption(Option o)
        {
            _options &= ~o;
        }

        void setMaxInputSize(std::size_t n)
        {
            _maxSize = n;
        }

        void setChunkSize(std::size_t n)
        {
            _chunkSize = n;
        }

        void setMaxInputDepth(std::size_t n)
        {
            _maxInputDepth = n;
        }

        void reset()
        {
            _is = 0;
            _input.clear();
            
            _depth = 0;
            _usedSize = _chunkSize;
            _nodeSize = _usedSize;

            _parse = &XmlReaderImpl::onDocumentBegin;

            while( ! _parseStack.empty() )
                _parseStack.pop();

            _current = 0;
            _back = 0;
            _elements.clear();

            _qname.clear();
            _token.clear(); 
            _quotChar = 0;
            _nsctx.clear();
            
            _dtd.clear();
            _cmBuilder.reset();
            _docType.clear();
            
            _attr = 0;
            _contentModel = 0;
            _attrModel = 0;
            _attlistDecl = 0;
            _entity = 0;
            _paramEntity = false;
            _notation = 0;

            // nodes are cleared before they are parsed
        }

        XmlResolver* resolver() const
        {
            return _resolver;
        }

        InputSource* input()
        {
            return _input.empty() ? _is : _input.source();
        }

        void reset(InputSource& is)
        {
            reset();
            _is = &is;
            _input.addInput(*_is);
        }

        void addInput(InputSource& is)
        { 
            if(_input.size() == _maxInputDepth)
                throw SyntaxError("input recursion depth exceeded", line());

            _input.addInput(is); 
        }

        DocTypeDefinition& dtd()
        { return _dtd; }

        size_t depth() const
        { return _depth; }

        inline void incDepth()
        {           
            _depth++;
        }

        inline void decDepth()
        {
            _depth--;
        }

        std::size_t line() const
        { return _input.line(); }

        Node& get()
        {
            if( ! _current )
            {
                this->next();
            }

            assert(_current != 0);

            return *_current;
        }

        Node& next()
        {
            _current = 0;
            const std::char_traits<Char>::int_type eof = std::char_traits<Char>::eof();
            
            while( ! _current )
            {
                std::char_traits<Char>::int_type c = _input.get();

                if( c == eof)
                {                              
                    _input.removeInput();

                    if( ! _input.empty() )
                        continue;
                }

                //std::cerr << char(c);
                (this->*_parse)(c);

                if(c == '\n')
                    _input.bumpLine();
            }

            return *_current;
        }

        Node* advance()
        {
            _current = 0;

            do
            {
                std::streamsize n = _input.avail();            

                if(n > 0)
                {
                    std::char_traits<Char>::int_type c = _input.get();
                    
                    (this->*_parse)(c);

                    if(c == '\n')
                        _input.bumpLine();
                }
                else
                {                              
                    if(n < 0)
                    {
                        _input.removeInput();

                        if( _input.empty() )
                            (this->*_parse)( std::char_traits<Char>::eof() );
                    }
                    else if (n == 0)
                    {
                        n = _input.import();
                        if(n == 0)
                            break;
                    }
                }
            } 
            while( ! _current);

            return _current;
        }

    private:
        InputSource* _is;
        XmlResolver* _resolver;
        EntityResolver _entityResolver;
        InputStack _input;
        int _options;
        std::size_t _maxSize;
        std::size_t _chunkSize;
        std::size_t _maxInputDepth;

        NamespaceContext _nsctx;
        Notation* _notation;
        Entity* _entity;
        bool _paramEntity;
        QName _qname;
        String _token;
        Pt::Char _quotChar;
        Attribute* _attr;
        std::size_t _depth;
        std::size_t _usedSize;
        std::size_t _nodeSize;
        ParseFunc _parse;
        std::stack<ParseFunc> _parseStack;
        
        Node* _current;
        const Pt::Char* _back;
        QNameStack _elements;

        DocTypeDefinition _dtd;
        ContentModelBuilder _cmBuilder;
        
        ContentModel* _contentModel;
        AttributeModel* _attrModel;
        AttributeListModel* _attlistDecl;
        
        // TODO: some sort of union?
        DocType _docType;
        EndDocType _endDocType;
        StartDocument _startDoc;
        ProcessingInstruction _procInstr;
        Comment _comment;
        StartElement _startElem;
        EntityReference _entityRef;
        EndElement _endElem;
        Characters _chars;
        EndDocument _endDoc;
};


XmlReader::XmlReader()
: _impl(0)
{
    _impl = new XmlReaderImpl();
}


XmlReader::XmlReader(InputSource& is)
: _impl(0)
{
    _impl = new XmlReaderImpl(is);
}


XmlReader::XmlReader(XmlResolver& r, InputSource& is)
: _impl(0)
{
    _impl = new XmlReaderImpl(is, &r);
}


XmlReader::~XmlReader()
{
    delete _impl;
}


void XmlReader::reset()
{
    _impl->reset();
}


void XmlReader::reset(InputSource& is)
{
    _impl->reset(is);
}


void XmlReader::addInput(InputSource& in)
{
    _impl->addInput(in);
}


XmlResolver* XmlReader::resolver() const
{
    return _impl->resolver();
}


void XmlReader::setMaxInputSize(std::size_t n)
{
    return _impl->setMaxInputSize(n);
}


void XmlReader::setChunkSize(std::size_t n)
{
    return _impl->setChunkSize(n);
}


void XmlReader::setMaxInputDepth(std::size_t n)
{
    return _impl->setMaxInputDepth(n);
}


void XmlReader::reportStartDocument(bool value)
{
    if(value)
        _impl->setOption(XmlReaderImpl::ReportStartDocument);
    else
        _impl->unsetOption(XmlReaderImpl::ReportStartDocument);
}


void XmlReader::reportDocType(bool value)
{
    if(value)
        _impl->setOption(XmlReaderImpl::ReportDtd);
    else
        _impl->unsetOption(XmlReaderImpl::ReportDtd);
}


void XmlReader::reportProcessingInstructions(bool value)
{
    if(value)
        _impl->setOption(XmlReaderImpl::ReportProcessingInstructions);
    else
        _impl->unsetOption(XmlReaderImpl::ReportProcessingInstructions);
}


void XmlReader::reportCData(bool value)
{
    if(value)
        _impl->setOption(XmlReaderImpl::ReportCData);
    else
        _impl->unsetOption(XmlReaderImpl::ReportCData);
}


void XmlReader::reportComments(bool value)
{
    if(value)
        _impl->setOption(XmlReaderImpl::ReportComments);
    else
        _impl->unsetOption(XmlReaderImpl::ReportComments);
}


void XmlReader::reportEntityReferences(bool value)
{
    if(value)
        _impl->setOption(XmlReaderImpl::ReportEntityReferences);
    else
        _impl->unsetOption(XmlReaderImpl::ReportCData);
}


DocTypeDefinition& XmlReader::dtd()
{
    return _impl->dtd();
}


const DocTypeDefinition& XmlReader::dtd() const
{
    return _impl->dtd();
}


size_t XmlReader::depth() const
{
    return _impl->depth();
}


std::size_t XmlReader::line() const
{
    return _impl->line();
}


Node& XmlReader::get()
{
    return _impl->get();
}


Node& XmlReader::next()
{
    return _impl->next();
}


Node* XmlReader::advance()
{
    return _impl->advance();
}


InputSource* XmlReader::input()
{
    return _impl->input();
}

} // namespace Xml

} // namespace Pt
