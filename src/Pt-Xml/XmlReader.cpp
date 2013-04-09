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

#include "DtdContext.h"
#include "ElementDeclaration.h"
#include "AttributeDeclaration.h"
#include "InputStack.h"
#include "NamespaceContext.h"

#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/DocTypeValidator.h>
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
            else if( ch == 0xfeff)
            {
                assert(false);
                return;
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

            if( isAlpha(ch) )
            {
                _procInstr.target() += c;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onProcessingInstructionData(int c)
        {
            Char ch = notEof(c);

            if(isSpace(ch) || isAlpha(ch) || isQuote(ch) || 
               ch == ':' || ch == '/' || ch == '!' || ch == '=')
            {
                _procInstr.data() += c;
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
                //_parse = &XmlReaderImpl::afterTag;
                popParseState(&XmlReaderImpl::afterTag);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onTag(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '?')
            {
                _procInstr.clear();
                _parse = &XmlReaderImpl::onProcessingInstructionBegin;
                return;
            }

            if(ch == '!')
            {
                _parse = &XmlReaderImpl::onTagExclam;
                return;
            }

            if(ch == '/')
            {
                if( _chars.content().length() )
                {
                    _current = &(_chars);
                }

                _endElem.clear();
                _parse = &XmlReaderImpl::onEndElement;
                return;
            }

            if( isAlpha(ch) )
            {
                if( _chars.content().length() )
                {
                    _current = &(_chars);
                }

                _startElem.clear();
                _startElem.qname().name() += ch;
                _parse = &XmlReaderImpl::onStartElement;
                return;
            }

            throw SyntaxError("XML syntax error", line());
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
                _dtd.rootName().name() += ch;
                _parse = &XmlReaderImpl::OnDtdRootName;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdPublic(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'U' || ch == 'B' || ch == 'L' || ch == 'I' || ch == 'C')
            {
                _token += ch;
                return;
            }

            if( isSpace(ch) )
            {
                bool ok = _token == L"PUBLIC";
                _token.clear();
                if( ! ok)
                    throw SyntaxError("XML syntax error", line());
                
                _parse = &XmlReaderImpl::OnDtdBeforePublicId;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdPublic);
                return;
            }

            throw SyntaxError("XML syntax error", line());
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

            if(ch == 'Y' || ch == 'S' || ch == 'T' || ch == 'E' || ch == 'M')
            {
                _token += ch;
                return;
            }

            if( isSpace(ch) )
            {
                bool ok = _token == L"SYSTEM";
                _token.clear();
                if( ! ok)
                    throw SyntaxError("XML syntax error", line());
                
                _parse = &XmlReaderImpl::OnDtdBeforeSystemId;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdSystem);
                return;
            }

            throw SyntaxError("XML syntax error", line());
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
                _dtd.rootName().name() += ch;
                return;
            }

            if( ch == ':' )
            {
                _dtd.rootName().prefix() = _dtd.rootName().name(); // TODO: use swap
                _dtd.rootName().name().clear();
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
                ++_depth;
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
                ++_depth;
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
                ++_depth;
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
                // end of INCLUDE or end of internal DTD

                // TODO: do not allow INCLUDE/IGNORE in internal subset
                popParseState(&XmlReaderImpl::OnDtdInternalEnd);
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

                setDocumentTypeDefinition(false);
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

            if(ch == '[')
            {
                // TODO: INCLUDE/IGNORE only in external subset
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
                
                    if( _input.isExternalDtd() )
                        _parse = &XmlReaderImpl::OnDtdExternal;
                    else
                        _parse = &XmlReaderImpl::OnDtdInternal;
                    
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
                
                if( _input.isExternalDtd() )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::OnDtdInternal;
                    
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
                
                popParseState();

                //if( _input.isExternalDtd() )
                //    _parse = &XmlReaderImpl::OnDtdExternal;
                //else
                //    _parse = &XmlReaderImpl::OnDtdInternal;
                
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
                _parse = &XmlReaderImpl::OnDtdParamEntityName;
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

        void OnDtdParamEntityName(int c)
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
                _entity = _dtd.declareParamEntity(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdEntityAfterName;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdParamEntityName);
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
                
                popParseState();

                //if( _input.isExternalDtd() )
                //    _parse = &XmlReaderImpl::OnDtdExternal;
                //else
                //    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }

            if(ch == 'N')
            {
                // TODO: fail if parameter entity
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
                // TODO: fail if parameter entity
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEntityNDATA;
                return;
            }

            if( ch == '>' )
            {
                _entity = 0;

                popParseState();
                
                //if( _input.isExternalDtd() )
                //    _parse = &XmlReaderImpl::OnDtdExternal;
                //else
                //    _parse = &XmlReaderImpl::OnDtdInternal;
                
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

                popParseState();
                
                //if( _input.isExternalDtd() )
                //    _parse = &XmlReaderImpl::OnDtdExternal;
                //else
                //    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }

            if( isSpace(ch) )
            {
                if(_entity)
                    _entity->setUnparsed(_token);

                _token.clear();
                _entity = 0;
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
                //if(_entity)
                //    _entity->addValue(_token);
                
                //_token.clear();
                _entity = 0;
                _parse = &XmlReaderImpl::OnDtdTagEnd;
                return;
            }

            if(ch == '&')
            {
                assert(_token.empty());
                pushParseState(&XmlReaderImpl::OnDtdEntityValue);
                _parse = &XmlReaderImpl::OnEntityValueCharacterReference;
                return;
            }

            if( ch == '%' )
            {
                pushParseState(&XmlReaderImpl::OnDtdEntityValue);
                _entityRef.clear();
                _parse = &XmlReaderImpl::OnDtdEntityValueParameterEntityReference;
                return;
            }

            if(_entity)
                _entity->value() += ch;

            //_token += ch;
        }
        
        // Entity references in entity value literals are left as is except
        // numerical character rerefences, which are resolved immediately
        void OnEntityValueCharacterReference(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == ';')
            {
                if( ! Entity::resolveCharacterEntity(_token) )
                {
                    _token = '&' + _token + ';';
                }

                //assert(_beforeCharacterReference != 0);
                //_parse = _beforeCharacterReference;
                //_beforeCharacterReference = 0;

                if(_entity)
                    _entity->addValue(_token);

                _token.clear();
                popParseState();
                return;
            }

            _token += ch;
        }

        void OnDtdEntityValueParameterEntityReference(int c)
        {
            Pt::Char ch = notEof(c);
            
            if( isAlpha(ch) )
            {
                //_entityName += ch;
                _entityRef.name() += ch;
                return;
            }

            if(ch == ';')
            {
                //assert(_beforeEntityReference);
                resolveParamEntity(_entityRef);
                popParseState();
                //_parse = _beforeEntityReference;
                //_beforeEntityReference = 0;
                //_entityName.clear();
                return;
            }

            if(ch == '&')
            {
                // TODO: This is most likely allowed.
                throw SyntaxError("character entity reference in parameter entity reference", line());
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttListBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _qname.name() += ch;
                //_token += ch;
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
                _qname.name() += ch;
                //_token += ch;
                return;
            }

            if(ch == ':')
            {
                _qname.prefix() = _qname.name(); // TODO: use swap
                _qname.name().clear();
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
                _qname.name() += ch;
                //_token += ch;
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
                _qname.name() += ch;
                //_token += ch;
                return;
            }

            if(ch == ':')
            {
                _qname.prefix() = _qname.name(); // TODO: use swap
                _qname.name().clear();
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
                //assert(_attrModel);
                _attrModel = 0;

                assert(_attlistDecl);
                _attlistDecl = 0;

                popParseState();

                //if( _input.isExternalDtd() )
                //    _parse = &XmlReaderImpl::OnDtdExternal;
                //else
                //    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }
            
            if( isSpace(ch) )
                return;

            if( isAlpha(ch) )
            {
                //assert(_attrModel);
                _attrModel = 0;
                
                _qname.name() += ch;
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
                _qname.name() += ch;
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
                _qname.name() += ch;
                return;
            }

            if(ch == ':')
            {
                _qname.prefix() = _qname.name(); // TODO: use swap
                _qname.name().clear();
                return;
            }

            if( isSpace(ch) )
            {
                assert(_elemModel == 0);
                
                // must only be declared once
                _elemModel = _dtd.declareElement(_qname);
                if( ! _elemModel)
                    throw SyntaxError("duplicate element declaration", line());

                _elemModelBuilder.reset(*_elemModel);
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

            _elemModelBuilder.pushOperator(ch);
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
                    
                    assert(_elemModel);
                    _elemModel->setEmpty();
                    _elemModel = 0;
                    
                    _parse = &XmlReaderImpl::OnDtdTagEnd;
                }
                else if(_token == L"ANY")
                {
                    _token.clear();

                    assert(_elemModel);
                    _elemModel->setAny();
                    _elemModel = 0;
                    
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

        void OnDtdBeforeElementEnd(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '>')
            {
                assert(_elemModel);
                _elemModelBuilder.finish();
                _elemModel = 0;
                
                popParseState();

                //if( _input.isExternalDtd() )
                //    _parse = &XmlReaderImpl::OnDtdExternal;
                //else
                //    _parse = &XmlReaderImpl::OnDtdInternal;
                
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
                _elemModelBuilder.pushOperator(ch);
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

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdIdentifier;
                return;
            }

            if( ch == ',')
            {
                _elemModelBuilder.pushDtdOperand(_token);
                _token.clear();
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _elemModelBuilder.pushDtdOperand(_token);
                _token.clear();
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _elemModelBuilder.pushDtdOperand(_token);
                _token.clear();
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _elemModelBuilder.pushDtdOperand(_token);
                _token.clear();
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _elemModelBuilder.pushDtdOperand(_token);
                _token.clear();
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _elemModelBuilder.pushDtdOperand(_token);
                _token.clear();
                _elemModelBuilder.pushClosingBrace();
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

            if( ch == '>' )
            {
                assert(_elemModel);
                _elemModelBuilder.finish();
                _elemModel = 0;
                
                popParseState();

                //if( _input.isExternalDtd() )
                //    _parse = &XmlReaderImpl::OnDtdExternal;
                //else
                //    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }

            if( ch == ',')
            {
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == ')')
            {
                _elemModelBuilder.pushClosingBrace();
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
                _elemModelBuilder.pushOperator(ch);
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
                assert(_elemModel);
                _elemModelBuilder.finish();
                _elemModel = 0;

                popParseState();

                //if( _input.isExternalDtd() )
                //    _parse = &XmlReaderImpl::OnDtdExternal;
                //else
                //    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }

            if( ch == ',')
            {
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _elemModelBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _elemModelBuilder.pushClosingBrace();
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
                setDocumentTypeDefinition(true);
                --_depth;

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
            if( c == std::char_traits<Char>::eof() )
            {
                if( depth() > 0 )
                    throw SyntaxError("XML syntax error", line());

                _current = &( _endDoc );
                return;
            }

            Char ch(c);

            if( isSpace(ch) )
            {
                if( depth() == 0 )
                {
                    _parse = &XmlReaderImpl::onProlog;
                    return;
                }

                _chars.appendSpace(ch);
                _parse = &XmlReaderImpl::onCharacters;
                return;
            }

            if(ch == '<')
            {
                _parse = &XmlReaderImpl::onTag;
                return;
            }

            if(ch == '>')
            {
                throw SyntaxError("XML syntax error", line());
            }

            if(ch == '&')
            {
                assert(_token.empty());
                _parse = &XmlReaderImpl::onEntityReference;
                return;
            }

            _chars.append(ch);
            _parse = &XmlReaderImpl::onCharacters;
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

            _comment.content() += ch;
        }

        void afterComment(int c)
        {
            Char ch = notEof(c);

            if(ch == '-')
            {
                _parse = &XmlReaderImpl::onCommentEnd;
                return;
            }

            _comment.content() += '-';
            _comment.content() += ch;
            _parse = &XmlReaderImpl::onComment;
        }

        void onCommentEnd(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '>')
            {
                setComment();
                
                if(depth() == 0)
                {
                    //_parse = &XmlReaderImpl::onProlog;
                    popParseState(&XmlReaderImpl::onProlog);
                }
                else
                {
                    //_parse = &XmlReaderImpl::afterTag;
                    popParseState(&XmlReaderImpl::afterTag);
                }

                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onStartElement(int c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                _parse = &XmlReaderImpl::beforeAttribute;
                return;
            }

            if(ch == '/')
            {
                _chars.clear();
                setStartElement();
                _depth++;

                setNamespace(_startElem);

                _parse = &XmlReaderImpl::onEmptyElement;
                return;
            }

            if(ch == ':')
            {
                if( ! _startElem.prefix().empty() )
                    throw SyntaxError("XML syntax error (invalid namespace prefix)", line());

                _startElem.qname().prefix() = _startElem.name();
                _startElem.qname().name().clear();
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                setStartElement();
                _depth++;

                setNamespace(_startElem);
                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            if( isAlpha(ch) )
            {
                _startElem.qname().name() += c;
                return;
            }

            throw SyntaxError("XML syntax error", line());
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
                setStartElement();
                _depth++;

                setNamespace(_startElem);

                _parse = &XmlReaderImpl::onEmptyElement;
                return;
            }

            if( isAlpha(ch) )
            {
                _attr = &_startElem.attributes().push();
                _attr->qname().name() += c;

                _parse = &XmlReaderImpl::onAttributeName;
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                setStartElement();
                _depth++;

                setNamespace(_startElem);

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
                assert(_attr);
                
                if( ! _attr->prefix().empty() )
                    throw SyntaxError("invalid namespace prefix", line());

                _attr->qname().prefix() = _attr->name();
                _attr->qname().name().clear();
                return;
            }

            if( isAlpha(ch) )
            {
                _attr->qname().name() += c;
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
                if(_attr->prefix() == "xmlns")
                {
                    _nsctx.setNamespace(_depth+1, _attr->name(), _attr->value());
                    _startElem.attributes().pop();
                }
                else if(_attr->name() == "xmlns")
                {
                    _nsctx.setDefaultNamespace(_depth+1, _attr->value());
                    _startElem.attributes().pop();
                }
                else
                {
                    // If the declared value is not CDATA, then discard any leading and
                    // trailing space (#x20) characters and replace sequences of space
                    // (#x20) characters by a single space (#x20) character.
                    // All attributes for which no declaration has been read SHOULD be 
                    // treated by a non-validating processor as if declared CDATA.
                    //
                    // TODO: do this when StartElement is complete so we only
                    //       have to look up the ElementDeclaration once.

                    // TODO: QName ?
                    ElementModel* elemDecl = _dtd.findElement( _startElem.qname() );
                    if(elemDecl)
                    {
                        AttributeModel* attrDecl = elemDecl->attributes().findAttribute( _attr->qname() );

                        if(attrDecl && attrDecl->isNormalize())
                            _attr->normalize();
                    }
                }
                
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
                _attr->value() += ' ';
                return;
            }
            
            _attr->value() += ch;
        }

        void onAttributeEntityReference(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token += ch;
                return;
            }
            
            if(ch == ';')
            {
                if( Entity::resolveDefaultEntity(_token) )
                {
                    _attr->value() += _token;
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
                _endElem.name() = _startElem.name();
                setEndElement();
                _depth--;

                setNamespace(_endElem);
                _parse = &XmlReaderImpl::afterEndElement;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onEndElement(int c)
        {
            Char ch = notEof(c);
            
            if( isAlpha(ch) )
            {
                _endElem.name() += c;
                _parse = &XmlReaderImpl::onEndElementName;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onEndElementName(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '>')
            {
                _chars.clear();
                setEndElement();
                _depth--;

                setNamespace(_endElem);
                _parse = &XmlReaderImpl::afterEndElement;
                return;
            }

            if( isSpace(ch) )
            {
                _parse = &XmlReaderImpl::afterEndElementName;
                return;
            }

            if( isAlpha(ch) )
            {
                _endElem.name() += c;
                return;
            }

            if(ch == ':')
            {
                if( ! _endElem.prefix().empty() )
                    throw SyntaxError("invalid namespace prefix", line());

                _endElem.prefix() = _endElem.name();
                _endElem.name().clear();
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
    
        void afterEndElementName(int c)
        {
            Char ch = notEof(c);
            
            if( isSpace(ch) )
            {
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                setEndElement();
                _depth--;

                setNamespace(_endElem);
                _parse = &XmlReaderImpl::afterEndElement;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void afterEndElement(int c)
        {
            if(depth() == 0)
                _parse = &XmlReaderImpl::onEpilog;
            else
                _parse = &XmlReaderImpl::afterTag;

            _nsctx.popNamespace( _depth + 1 );

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
                    _parse = &XmlReaderImpl::onEntityReference;
                    break;

                case '\r':
                    _parse = &XmlReaderImpl::onCharactersCR;
                    break;
            
                case ' ':
                case '\t':
                case '\n':
                    _chars.appendSpace(ch);
                    break;
            
                default:
                    _chars.append(ch);
            }
        }

        void onCharactersCR(int c)
        {
            _chars.appendSpace('\n');
            _parse = &XmlReaderImpl::onCharacters;
            
            if(c != '\n')
                onCharacters(c);
        }

        void onEntityReference(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token += ch;
                return;
            }

            if(ch == ';')
            {
                if( Entity::resolveDefaultEntity(_token) )
                {
                    for(std::size_t n = 0; n < _token.size(); ++n)
                    {
                        Pt::Char c = _token[n];
                        if(c == ' ' || c == '\n' || c == '\r' || c == '\t')
                            _chars.appendSpace(c);
                        else
                            _chars.append(c);
                    }

                    _token.clear();
                    _parse = &XmlReaderImpl::onCharacters;
                    return;
                }
                else
                {
                    _entityRef.setName(_token);
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

                if( (_options & ReportCData) && ! _chars.empty() )
                    _current = &_chars;
                
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

            _parse = &XmlReaderImpl::onCData;
        }

        void onCDataClose1(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '>')
            {
                if( _options & ReportCData )
                {
                    _current = &_chars;
                }
                    
                _parse = &XmlReaderImpl::afterCData;
                return;
            }

            _chars.append(']');
            _chars.append(']');
            _chars.append(ch);
            
            _parse = &XmlReaderImpl::onCData;
        }

        void afterCData(int c)
        {
            if( _options & ReportCData )
                _chars.clear();
            
            _parse = &XmlReaderImpl::afterTag;
            afterTag(c);
        }

        // onEpilog should differ should not allow StartElements
        // -> onEpilogTag, allow only Comments and ProcInstr
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

        void setNamespace(StartElement& se)
        {
            if( se.prefix().empty() )
            {
                const Namespace* ns = _nsctx.getDefaultNamespace();
                if(ns)
                    se.setNamespace(*ns);
            }
            else
            {
                const Namespace* ns = _nsctx.findPrefix( se.prefix() );
                if( ! ns )
                    throw SyntaxError("undeclared namespace prefix", line());

                se.setNamespace(*ns);
            }
            
            AttributeList& attributes = se.attributes();
            AttributeList::Iterator it;
            for(it = attributes.begin(); it != attributes.end(); ++it)
            {
                if( it->prefix().empty() )
                {
                    const Namespace* ns = _nsctx.getDefaultNamespace();
                    if(ns)
                        it->setNamespace(*ns);
                }
                else
                {
                    const Namespace* ns = _nsctx.findPrefix( it->prefix() );
                    if( ! ns )
                        throw SyntaxError("undeclared namespace prefix", line());

                    it->setNamespace(*ns);
                }    
            }
        }

        void setNamespace(EndElement& e)
        {
            if( e.prefix().empty() )
            {
                const Namespace* ns = _nsctx.getDefaultNamespace();
                if(ns)
                    e.setNamespace(*ns);
            }
            else
            {
                const Namespace* ns = _nsctx.findPrefix( e.prefix() );
                if( ! ns )
                    throw SyntaxError("undeclared namespace prefix", line());

                e.setNamespace(*ns);
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
        }

        void setDocType()
        {
            if(_options & ReportDtd)
                _current = &_docType;
        }

        void setDocumentTypeDefinition(bool internSubset)
        {
            if(_options & ReportDtd)
            {
                _endDocType.clear();
                _endDocType.setInternal(internSubset);
                _current = &_endDocType;
            }
        }

        void setComment()
        {
            if(_options & ReportComments)
                _current = &_comment;
        }

        inline void setStartElement()
        {
            _elements.resize(_elements.size() + _startElem.name().size());
            //std::cerr << _elements.capacity() << " ";
            _current = &(_startElem);
        }

        inline void setEndElement()
        {
            //if(_endElem.name().size() > _elements.size())
            //    throw SyntaxError("unmatched XML element", line());
            
            std::size_t n =  _endElem.name().size();
            _elements.resize(_elements.size() - n);
            _current = &(_endElem);
        }

        void setProcessingInstruction()
        {
            if(_options & ReportProcessingInstructions)
                _current = &_procInstr;
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

        // TODO:
        //     - XmlName object which has non-owning Char* to parse character
        //       buffer. Also for QName, Characters, with a String like API.

        XmlReaderImpl(XmlResolver* resolver = 0)
        : _is(0)
        , _resolver(resolver)
        , _options(DefaultOptions)
        , _notation(0)
        , _entity(0)
        , _attr(0)
        , _depth(0)
        , _parse(0)                 
        , _current(0)
        , _elemModelBuilder()
        , _elemModel(0)
        , _attrModel(0)
        , _attlistDecl(0)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;
            _elements.reserve(64);
        }

        XmlReaderImpl(InputSource& is, XmlResolver* resolver = 0)
        : _is(&is)
        , _resolver(resolver)
        , _options(DefaultOptions)
        , _notation(0)
        , _entity(0)
        , _attr(0)
        , _depth(0)
        , _parse(0)                 
        , _current(0)
        , _elemModelBuilder()
        , _elemModel(0)
        , _attrModel(0)
        , _attlistDecl(0)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;
            _elements.reserve(64);

            _input.addInput(is);
        }

        ~XmlReaderImpl()
        { }

        int flags() const
        {
            return _options;
        }
        
        void setOption(Option o)
        {
            _options |= o;
        }

        void unsetOption(Option o)
        {
            _options &= ~o;
        }

        // TODO: use Char buffer[64] before allocating
        Pt::String _elements;

        void reset()
        {
            _is = 0;
            _input.clear();
            
            _depth = 0;
            _parse = &XmlReaderImpl::onDocumentBegin;
            _current = 0;

            while( ! _parseStack.empty() )
                _parseStack.pop();

            _elements.clear();

            _qname.clear();
            _token.clear(); 
            _quotChar = 0;
            _nsctx.clear();
            
            _dtd.clear();
            _elemModelBuilder.reset();
            _docType.clear();
            
            _attr = 0;
            _elemModel = 0;
            _attrModel = 0;
            _attlistDecl = 0;
            _entity = 0;
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
        { _input.addInput(is); }

        DocTypeDefinition& dtd()
        { return _dtd; }

        size_t depth() const
        { return _depth; }

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

            while( ! _current )
            {
                std::char_traits<Char>::int_type c = _input.get();
                if( c == std::char_traits<Char>::eof() )
                {            
                    _input.removeInput();

                    if( ! _input.empty() )
                        continue;
                }

                (this->*_parse)(c);

                if(c == '\n')
                {
                    _input.bumpLine();
                }
            }

            return *_current;
        }

        Node* advance()
        {
            _current = 0;

            do
            {
                std::char_traits<Char>::int_type c;
                std::streamsize n = _input.avail();                
                
                if(n > 0)
                {
                    c = _input.get();
                }
                else if(n < 0)
                {
                    _input.removeInput();
                    if( _input.empty() )
                        c = std::char_traits<Char>::eof();
                }
                else
                {
                    n = _input.import();
                    if(n == 0)
                        break;

                    continue;
                }

                (this->*_parse)(c);

                // TODO: move this to state functions
                if(c == '\n')
                {
                    _input.bumpLine();
                }

                //if(n > 0)
                //{
                    //std::char_traits<Char>::int_type c = _input.get();
                    //(this->*_parse)(c);

                    //// TODO: move this to state functions
                    //if(c == '\n')
                    //{
                        //_input.bumpLine();
                    //}
                //}
                //else
                //{           
                    //if(n < 0)
                    //{
                        //_input.removeInput();

                        //if( _input.empty() )
                            //(this->*_parse)( std::char_traits<Char>::eof() );
                    //}
                    //else if (n == 0)
                    //{
                        //n = _input.import();
                        //if(n == 0)
                            //break;
                    //}
                //}
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
        
        NamespaceContext _nsctx;
        Notation* _notation;
        Entity* _entity;
        QName _qname;
        String _token;
        Pt::Char _quotChar;
        Attribute* _attr;
        std::size_t _depth;
        ParseFunc _parse;
        std::stack<ParseFunc> _parseStack;
        
        Node* _current;

        DocTypeDefinition _dtd;
        ContentModelBuilder _elemModelBuilder;
        
        ElementModel* _elemModel;
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


void XmlReader::reportStartDocument(bool value)
{
    if(value)
        _impl->setOption(XmlReaderImpl::ReportStartDocument);
    else
        _impl->unsetOption(XmlReaderImpl::ReportStartDocument);
}


void XmlReader::reportDtd(bool value)
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
