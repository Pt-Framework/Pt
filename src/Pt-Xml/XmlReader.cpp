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
#include "QNameStack.h"
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

void normalizeAttributeValue(String& value)
{
    Pt::String& str = value;

    Pt::String::iterator p1 = str.begin();
    Pt::String::iterator p2 = str.begin();
    int spaces = 1;
    bool normalized = false;
    
    for(; p2 != str.end(); ++p2)
    {
        if( Pt::isspace(*p2) )
        {
            switch(spaces)
            {
                case 0:
                    *p1 = *p2;
                    ++p1;
                    break;

                case 1:
                    normalized = true;
            };

              ++spaces;
        }
        else
        {
            spaces = 0;
                    
            if(normalized)
                *p1 = *p2;
                    
            ++p1;
        }
    }
           
    if(p1 != p2)
    {               
        str.erase( p1, str.end() );
    }

    if( spaces != 0 && ! str.empty() )
    {
        str.erase(str.size() - 1);
    }
}

class XmlReaderImpl
{
    typedef std::char_traits<Char>::int_type int_type;

    typedef void (XmlReaderImpl::*ParseFunc)(int_type);

    private:
        void OnDocType(int_type c)
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

        void AfterDocType(int_type c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _dtd.rootName().name() += ch;
                ++_usedSize;
                _parse = &XmlReaderImpl::OnDtdRootName;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdPublic(int_type c)
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

        void OnDtdBeforePublicId(int_type c)
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

        void OnDtdPublicId(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                popParseState();
                (this->*_parse)(c);
                return;
            }

            if(_usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _token += ch;
            ++_usedSize;
        }

        void OnDtdSystem(int_type c)
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

        void OnDtdBeforeSystemId(int_type c)
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

        void OnDtdSystemId(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                popParseState();
                (this->*_parse)(c);
                return;
            }

            if(_usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _token += ch;
            ++_usedSize;
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
        
        void OnDtdParameterEntityReference(int_type c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _entityRef.name() += ch;
                ++_usedSize;
                return;
            }

            if(ch == ';')
            {
                _usedSize -= _entityRef.name().size();

                resolveParamEntity(_entityRef);
                popParseState();
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void enterParameterReference(ParseFunc from)
        {
            pushParseState(from);
            _entityRef.clear();
            _parse = &XmlReaderImpl::OnDtdParameterEntityReference;
        }

        void OnDtdRootName(int_type c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _dtd.rootName().name() += ch;
                ++_usedSize;
                return;
            }

            if( ch == ':' )
            {
                QName& qn = _dtd.rootName();
                qn.prefix().swap( qn.name() );
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
        void AfterDtdRootName(int_type c)
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

        void OnDtdExternalPublicId(int_type c)
        {
            _docType.setPublicId(_token);
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdAfterExternalPublicId;
        }

        void OnDtdAfterExternalPublicId(int_type c)
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

        void OnDtdExternalSystemId(int_type c)
        {
            _docType.setSystemId(_token);
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdAfterExternalSystemId;
        }

        void OnDtdAfterExternalSystemId(int_type c)
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

        void OnDtdInternal(int_type c)
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
        
        void OnDtdExternal(int_type c)
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

        void OnDtdTag(int_type c)
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

        void OnDtdTagExclam(int_type c)
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

        void OnDtdTagEnd(int_type c)
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

        void OnDtdTagName(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;

                if(_token.size() > 10)
                    throw SyntaxError("XML syntax error", line());
                
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
        
        void OnDtdBeforeIgnoreOrInclude(int_type c)
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

        void OnDtdIgnoreOrInclude(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;

                if(_token.size() > 7)
                    throw SyntaxError("XML syntax error", line());
                
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

        void OnDtdIgnoreBegin(int_type c)
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

        void OnDtdIgnore(int_type c)
        {
            Pt::Char ch = notEof(c);

            if(ch == ']')
            {             
                _parse = &XmlReaderImpl::OnDtdIgnoreEnd; 
                return;
            }
        }

        void OnDtdIgnoreEnd(int_type c)
        {
            Pt::Char ch = notEof(c);

            if(ch == ']')
            {             
                _parse = &XmlReaderImpl::OnDtdIncludeEnd2;
                return;
            }

            _parse = &XmlReaderImpl::OnDtdIgnore; 
        }

        void OnDtdIncludeBegin(int_type c)
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

        void OnDtdIncludeEnd(int_type c)
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

        void OnDtdIncludeEnd2(int_type c)
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

        void OnDtdNotationBegin(int_type c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '%')
            {
                _parse = &XmlReaderImpl::OnDtdNotationBegin;
                return;
            }

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;
                _parse = &XmlReaderImpl::OnDtdNotationName;
                return;
            }

            if( isSpace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
        
        void OnDtdNotationName(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;
                return;
            }

            if( isSpace(ch) )
            {
                assert(_notation == 0);

                // TODO: Is it an error if a notation is declared twice? 
                // nullptr is returned when already declared.
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

        void OnDtdNotationAfterName(int_type c)
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

        void OnDtdNotationPublicId(int_type c)
        {
            // can be NULL if already declared
            if(_notation)
                _notation->setPublicId(_token);
                
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdNotationAfterPublicId;
        }

        void OnDtdNotationAfterPublicId(int_type c)
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

        void OnDtdNotationSystemId(int_type c)
        {
            if(_notation)
                _notation->setSystemId(_token);
                
            _token.clear();
            _notation = 0;
            _parse = &XmlReaderImpl::OnDtdTagEnd;
        }

        void OnDtdEntityBegin(int_type c)
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
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;

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

        void OnDtdParamEntityBegin(int_type c)
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

        void OnDtdBeforeParamEntityName(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;

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

        void OnDtdEntityName(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;
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
                
                // ignored overwritten entity
                if( ! _entity )
                    _usedSize -= _token.size();

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
        
        void OnDtdEntityAfterName(int_type c)
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

        void OnDtdEntityPublicId(int_type c)
        {
            if(_entity)
                _entity->setPublicId(_token);
            else
                _usedSize -= _token.size();
                
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdEntityAfterPublicId;
        }

        void OnDtdEntityAfterPublicId(int_type c)
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

        void OnDtdEntitySystemId(int_type c)
        {
            if(_entity)
                _entity->setSystemId(_token);
            else
                _usedSize -= _token.size();
                
            _token.clear();
            _parse = &XmlReaderImpl::OnDtdEntityAfterSystemId;
        }
        
        void OnDtdEntityAfterSystemId(int_type c)
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

        void OnDtdEntityNDATA(int_type c)
        {
            Pt::Char ch = notEof(c);

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

            if(_token.size() >= 5)
                throw SyntaxError("XML syntax error", line());

            _token += ch;
        }

        void OnDtdEntityAfterNDATA(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _token += ch;
                ++_usedSize;
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

        void OnDtdEntityNotation(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _token += ch;
                ++_usedSize;
                return;
            }
            
            if( ch == '>' )
            {
                if(_entity)
                    _entity->setUnparsed(_token);
                else
                    _usedSize -= _token.size();

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
                else
                    _usedSize -= _token.size();

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
        void OnDtdEntityValue(int_type c)
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

            // _entity is 0 if already declared
            if(_entity)
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _entity->value() += ch;
                ++_usedSize;
            }
        }
        
        // Entity references in entity value literals are left as is except
        // numerical character rerefences, which are resolved immediately
        void OnEntityValueCharacterReference(int_type c)
        {
            Pt::Char ch = notEof(c);

            if(ch == ';')
            {
                _usedSize -= _token.size();

                if( ! _paramEntity)
                {
                    assert(_entity);

                    if(_entity && _entity->name() == _token)
                        throw SyntaxError("self reference", line());
                }

                if( ! resolveCharacterEntity(_token) )
                {
                    _token = '&' + _token + ';';
                }

                if(_entity)
                {
                    _entity->value() += _token;
                    ++_usedSize;

                    if(_usedSize >= _maxSize)
                        throw SyntaxError("XML syntax error", line());
                }

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdEntityValue;
                return;
            }

            if(_usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _token += ch;
            ++_usedSize;
        }

        void OnDtdEntityValueParameterEntityReference(int_type c)
        {
            Pt::Char ch = notEof(c);
            
            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _entityRef.name() += ch;
                ++_usedSize;
                return;
            }

            if(ch == ';')
            {
                _usedSize -= _entity->name().size();

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

        void OnDtdAttListBegin(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _qname.name() += ch;
                ++_usedSize;

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

        void OnDtdAttListName(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _qname.name() += ch;
                ++_usedSize;
                return;
            }

            if(ch == ':')
            {
                if( ! _qname.prefix().empty() )
                    throw SyntaxError("invalid name", line());
                
                _qname.prefix().swap( _qname.name() );
                return;
            }

            if( isSpace(ch) )
            {
                assert( _attlistDecl == 0 );

                _attlistDecl = _dtd.findAttributes(_qname);
                if( ! _attlistDecl )
                    _attlistDecl = &_dtd.declareAttributeList(_qname);
                else
                    _usedSize -= _qname.size();
                
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

        void OnDtdBeforeAttrName(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _qname.name() += ch;
                ++_usedSize;
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

        void OnDtdAttrName(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                _parse = &XmlReaderImpl::OnDtdAfterAttrName;
                return;
            }
            
            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _qname.name() += ch;
                ++_usedSize;
                return;
            }

            if(ch == ':')
            {
                if( ! _qname.prefix().empty() )
                    throw SyntaxError("invalid name", line());

                _qname.prefix().swap( _qname.name() );
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrName);
                return;
            }

            throw SyntaxError("XML syntax error: invalid attribute name", line());
        }

        void OnDtdAfterAttrName(int_type c)
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
                else
                    _usedSize -= _qname.size();

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

        void OnDtdAttrEnum(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                assert( _token.empty() );

                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;
                
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

        void OnDtdAttrEnumValue(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _token += ch;
                ++_usedSize;
                return;
            }

            if( isSpace(ch) )
            {
                if(_attrModel)
                    static_cast<EnumAttributeModel*>(_attrModel)->addValue(_token);
                else
                    _usedSize -= _qname.size();

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrAfterEnumValue;
                return;
            }

            if( ch == '|' )
            {
                if(_attrModel)
                    static_cast<EnumAttributeModel*>(_attrModel)->addValue(_token);
                else
                    _usedSize -= _qname.size();

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrEnumSep;
                return;
            }

            if( ch == ')' )
            {
                if(_attrModel)
                    static_cast<EnumAttributeModel*>(_attrModel)->addValue(_token);
                else
                    _usedSize -= _qname.size();

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

        void OnDtdAttrAfterEnumValue(int_type c)
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

        void OnDtdAttrEnumSep(int_type c)
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

                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _token += ch;
                ++_usedSize;
                
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

        void OnDtdAfterAttrEnumSep(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                assert(_token.empty());

                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _token += ch;
                ++_usedSize;
                
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

        void OnDtdAttrType(int_type c)
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

            if(_token.size() > 12)
                throw SyntaxError("XML syntax error", line());

            _token += ch;
        }

        void OnDtdAfterAttrNotation(int_type c)
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

        void OnDtdAttrNotationBegin(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;
                
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

        void OnDtdAttrNotationId(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _token += ch;
                ++_usedSize;
                return;
            }

            if( isSpace(ch) )
            {
                if(_attrModel) // skip duplicates
                    static_cast<NotationAttributeModel*>(_attrModel)->addNotation(_token);
                else
                    _usedSize -= _token.size();

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrAfterNotationId;
                return;
            }

            if( ch == '|' )
            {
                if(_attrModel) // skip duplicates
                    static_cast<NotationAttributeModel*>(_attrModel)->addNotation(_token);
                else
                    _usedSize -= _token.size();

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrNotationSep;
                return;
            }

            if( ch == ')' )
            {
                if(_attrModel) // skip duplicates
                    static_cast<NotationAttributeModel*>(_attrModel)->addNotation(_token);
                else
                    _usedSize -= _token.size();

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

        void OnDtdAttrAfterNotationId(int_type c)
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

        void OnDtdAttrNotationSep(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;

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

        void OnDtdAfterAttrType(int_type c)
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

        void OnDtdAttrMode(int_type c)
        {
            Pt::Char ch = notEof(c);
            
            if( isAlpha(ch) )
            {
                if(_token.size() > 12)
                    throw SyntaxError("XML syntax error", line());
                
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

        void OnDtdAfterAttrMode(int_type c)
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
                
                _qname.name() += ch;
                ++_usedSize;
                
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

        void OnDtdAfterDtdAttrFixed(int_type c)
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

        void OnDtdAttrDefault(int_type c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '"')
            {
                if(_attrModel) // skip duplicates
                    _attrModel->setDefaultValue(_token);
                else
                    _usedSize -= _token.size();
                
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterAttrMode;
                return;
            }

            if(_usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _token += ch;
            ++_usedSize;
        }

        void OnDtdElementBegin(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _qname.name() +=ch;
                ++_usedSize;
                
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

        void OnDtdElementName(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _qname.name() += ch;
                ++_usedSize;
                return;
            }

            if(ch == ':')
            {
                if( ! _qname.prefix().empty() )
                    throw SyntaxError("invalid name", line());

                _qname.prefix().swap( _qname.name() );
                return;
            }

            if( isSpace(ch) )
            {
                assert(_contentModel == 0);

                ElementModel* elemModel =  _dtd.findElement(_qname);
                if(elemModel)
                {
                    _usedSize -= _qname.size();
                    _contentModel = &elemModel->content();

                    // must only be declared once
                    if( ! _contentModel->isUndeclared() )
                        throw SyntaxError("duplicate element declaration", line());
                }
                else
                    _contentModel = &_dtd.declareContent(_qname);

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

        void OnDtdElementContentBegin(int_type c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'E' || ch == 'A')
            {
                assert( _token.empty() );
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

            if(ch != '(' || _usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _cmBuilder.pushScope();
            ++_usedSize;

            _parse = &XmlReaderImpl::OnDtdElementContent;
        }
        
        void OnDtdEmptyOrAny(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                std::size_t tsize = _token.size();

                if(tsize > 5)
                    throw SyntaxError("invalid token", line());
                
                if(tsize == 5 && _token == L"EMPTY")
                {
                    _token.clear();
                    
                    assert(_contentModel);
                    _contentModel->setEmpty();
                    _contentModel = 0;
                    
                    _parse = &XmlReaderImpl::OnDtdTagEnd;
                }
                else if(tsize == 3 && _token == L"ANY")
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

        void OnDtdElementContent(int_type c)
        {
            Pt::Char ch = notEof(c);

            // TODO: handle #PCDATA differently
            if( isAlpha(ch) || ch == '#')
            {
                assert( _token.empty() );
                
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _token += ch;
                ++_usedSize;
                
                _parse = &XmlReaderImpl::OnDtdIdentifier;
                return;
            }

            if(ch == '(')
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _cmBuilder.pushScope();
                ++_usedSize;
                
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdElementContent);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdIdentifier(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) || ch == ':')
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _token += ch;
                ++_usedSize;

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
                
                --_usedSize;

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
        
        void OnDtdUnrayOp(int_type c)
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
                
                --_usedSize;

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
        
        void OnDtdBinaryOp(int_type c)
        {
            Pt::Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isAlpha(ch) || ch == '#')
            {
                assert( _token.empty() );
                
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _token += ch;
                ++_usedSize;
                
                _parse = &XmlReaderImpl::OnDtdIdentifier;
                return;
            }

            if(ch == '(')
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _cmBuilder.pushScope();
                ++_usedSize;

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

        void OnDtdContentExprEnd(int_type c)
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
                
                --_usedSize;
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

        void OnDtdInternalEnd(int_type c)
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

        void onDocumentBegin(int_type c)
        {
            if( c == std::char_traits<Char>::eof() ) //signed unsigned
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

        void onProcessingInstructionBegin(int_type c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) ) // TODO: XML Name character
            {
                _procInstr.target() += c;
                ++_usedSize;
                _parse = &XmlReaderImpl::onProcessingInstruction;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onProcessingInstruction(int_type c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                _parse = &XmlReaderImpl::onProcessingInstructionData;
                return;
            }

            if( ! isAlpha(ch) || _usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());
            
            _procInstr.target() += c;
            ++_usedSize;
        }

        void onProcessingInstructionData(int_type c)
        {
            Char ch = notEof(c);

            if(isSpace(ch) || isAlpha(ch) || isQuote(ch) || 
               ch == ':' || ch == '/' || ch == '!' || ch == '=')
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _procInstr.data() += c;
                ++_usedSize;
                return;
            }

            if(ch == '?')
            {
                _parse = &XmlReaderImpl::onProcessingInstructionEnd;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onProcessingInstructionEnd(int_type c)
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

        void beforeComment(int_type c)
        {
            if(c == '-')
            {
                _comment.content().clear();
                _parse = &XmlReaderImpl::onComment;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onComment(int_type c)
        {
            Char ch = notEof(c);
            
            if(ch == '-')
            {
                _parse = &XmlReaderImpl::afterComment;
                return;
            }

            if(_usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());
            
            _comment.content() += ch;
            ++_usedSize;
        }

        void afterComment(int_type c)
        {
            Char ch = notEof(c);

            if(ch == '-')
            {
                _parse = &XmlReaderImpl::onCommentEnd;
                return;
            }

            if(_usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _comment.content() += '-';
            _comment.content() += ch;
            _usedSize += 2;
            
            _parse = &XmlReaderImpl::onComment;
        }

        void onCommentEnd(int_type c)
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

        void onTagExclam(int_type c)
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

        void onTag(int_type c)
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
                    _parse = &XmlReaderImpl::onEndElement;
                    return;
            }

            if( c == std::char_traits<Char>::eof() || ! isAlpha(c) || _usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _nameStack.pushChar(c);
            ++_usedSize;

            setCharactersEnd();

            _parse = &XmlReaderImpl::onStartElement;
        }

        void afterTag(int_type c)
        {
            assert(depth() != 0);
            
            Char ch = notEof(c);

            switch(c)
            {
                case '\r':
                    //_chunkSize = _chars.content().size();
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

                //case ' ':
                //case '\n':
                //case '\t':
                //    _chars.appendSpace(ch);
                //    _chunkSize = _chars.content().size();
                //    _parse = &XmlReaderImpl::onCharacters;
                //    break;

                default:
                    _chars.append(ch);
                    //_chunkSize = _chars.content().size();
                    ++_chunkSize;
                    _parse = &XmlReaderImpl::onCharacters;
                    break;
            }
        }

        void onStartElement(int_type c)
        {
            switch(c)
            {
                case ' ':
                case '\n':
                case '\r':
                case '\t':
                    _nameStack.pushName();
                    _startElem.setName(_nameStack.top(), _nsctx.emptyNamespace());
                    _parse = &XmlReaderImpl::beforeAttribute;
                    return;

                case '/':
                    _nameStack.pushName();
                    _startElem.setName(_nameStack.top(), _nsctx.emptyNamespace());
                    
                    _chars.clear();               
                    setStartElement();
                    _parse = &XmlReaderImpl::onEmptyElement;
                    return;

                case ':':
                {
                    if( ! _nameStack.pushPrefix() )
                        throw SyntaxError("invalid prefix", line());
                    
                    return;
                }

                case '>':
                    _nameStack.pushName();
                    _startElem.setName(_nameStack.top(), _nsctx.emptyNamespace());
                    
                    _chars.clear();
                    setStartElement();
                    
                    _parse = &XmlReaderImpl::afterStartElement;
                    return;  
            }

            if( c == std::char_traits<Char>::eof() || ! isAlpha(c) || _usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _nameStack.pushChar(c);
            ++_usedSize;
        }

        void afterStartElement(int_type c)
        {
            // remove attribute names from name stack
            std::size_t n = _nameStack.size();
            while(n-- > _depth)
                _nameStack.pop();

            _startElem.attributes().clear();

            afterTag(c);
        }

        void beforeAttribute(int_type c)
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
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());
                
                _nameStack.pushChar(ch);
                ++_usedSize;

                _parse = &XmlReaderImpl::onAttributeName;
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                setStartElement();
                
                _parse = &XmlReaderImpl::afterStartElement;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onAttributeName(int_type c)
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
                if( ! _nameStack.pushPrefix() )
                    throw SyntaxError("invalid attribute prefix", line());

                return;
            }

            if( isAlpha(ch) )
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _nameStack.pushChar(ch);
                ++_usedSize;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void afterAttributeName(int_type c)
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

        void beforeAttributeValue(int_type c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if( isQuote(ch) )
            {
                setQuotedBegin(ch);
                
                _nameStack.pushName();
                const QName& name = _nameStack.top();

                if(name.prefix() == "xmlns")
                {
                    assert(_token.empty() );
                    _parse = &XmlReaderImpl::onNamespace;
                    return;
                }
                else if(name.prefix().empty() && name.name() == "xmlns")
                {
                    assert(_token.empty() );
                    _parse = &XmlReaderImpl::onDefaultNamespace;
                    return;
                }

                const Namespace& ns = _nsctx.emptyNamespace();
                _attr = &_startElem.attributes().append(name, ns);

                _parse = &XmlReaderImpl::onAttributeValue;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onNamespace(int_type c)
        {
            Char ch = notEof(c);

            if( isQuoteEnd(ch) )
            {
                const String& name = _nameStack.top().name();
                _nsctx.pushNamespace(_depth + 1, name, _token);
                _usedSize += name.size();

                _usedSize -= _nameStack.pop();
                _token.clear();
                
                _parse = &XmlReaderImpl::beforeAttribute;
                return;
            }

            if(_usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _token += ch;
            ++_usedSize;
        }

        void onDefaultNamespace(int_type c)
        {
            Char ch = notEof(c);

            if( isQuoteEnd(ch) )
            {
                _nsctx.pushDefaultNamespace(_depth + 1, _token);

                _usedSize -= _nameStack.pop();
                _token.clear();
                
                _parse = &XmlReaderImpl::beforeAttribute;
                return;
            }

            if(_usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());

            _token += ch;
            ++_usedSize;
        }
        
        void onAttributeValue(int_type c)
        {
            Char ch = notEof(c);
            assert(_attr);

            if( isQuoteEnd(ch) )
            {               
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
                assert( _token.empty() );
                _parse = &XmlReaderImpl::onAttributeEntityReference;
                return;
            }

            // For white space characters (#x20, #xD, #xA, #x9), append #x20 to
            // the normalized value, with the exception that a single #x20 is
            // appended for a #xD#xA sequence that is part of an external parsed
            // entity or the literal entity value of an internal parsed entity.
            if( ch == '\r' || ch == '\n' || ch == '\t' )
            {
                ch = ' ';
            }

            if(_usedSize >= _maxSize)
                throw SyntaxError("XML syntax error", line());
            
            _attr->value() += ch;
            ++_usedSize;
        }

        void onAttributeEntityReference(int_type c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;
                return;
            }
            
            if(ch == ';')
            {       
                _usedSize -= _token.size();        
                
                if( resolveDefaultEntity(_token) )
                {
                    _attr->value() += _token;
                    _usedSize += _token.size();

                    if(_usedSize >= _maxSize)
                        throw SyntaxError("XML syntax error", line());
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

        void onEmptyElement(int_type c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
            {
                return;
            }

            if(ch == '>')
            {
                // remove attribute names from name stack
                std::size_t n = _nameStack.size();
                while(n-- > _depth)
                    _nameStack.pop();

                _startElem.attributes().clear();
                
                setEndElement();
                _parse = &XmlReaderImpl::afterEndElement;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onEndElement(int_type c)
        {           
            if( _nameStack.empty() )
                throw SyntaxError("unmatched element", line());

            const QName& name = _nameStack.top();
            _back = name.prefix().empty() ? name.name().c_str() 
                                          : name.prefix().c_str();

            if( c != _back->value() || *_back == '\0' )
                throw SyntaxError("unmatched element", line());

            ++_back;
            _parse = &XmlReaderImpl::onEndElementName;
        }

        void onEndElementName(int_type c)
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
                    
                    _back = _nameStack.top().name().c_str();
                    return;
            }

            if( c != _back->value() || *_back == '\0' )
                throw SyntaxError("unmatched element", line());

            ++_back;
        }
    
        void afterEndElementName(int_type c)
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

        void afterEndElement(int_type c)
        {
            _usedSize -= _nameStack.pop() + _nsctx.popNamespace( _depth + 1 );

            if( depth() == 0 )
            {
                _parse = &XmlReaderImpl::onEpilog;
                onEpilog(c);
            }
            else
            {
                _parse = &XmlReaderImpl::afterTag;
                afterTag(c);
            }
        }

        void onCharacters(int_type c)
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
            
                //case ' ':
                //case '\t':
                //case '\n':
                //    _chars.appendSpace(ch);
                //    ++_chunkSize;

                //    if(_chunkSize >= _maxChunkSize)
                //    {
                //        _parse = &XmlReaderImpl::onCharactersMax;
                //        setCharactersChunk();
                //    }
                //    
                //    break;
            
                default:                   
                    _chars.append(ch);
                    ++_chunkSize;

                    if(_chunkSize >= _maxChunkSize)
                    {
                        _parse = &XmlReaderImpl::onCharactersMax;
                        setCharactersChunk();
                    }
            }
        }

        void onCharactersCR(int_type c)
        {
            if(c != '\n')
            {
                ///_chars.appendSpace('\n');
                _chars.append('\n');
                ++_chunkSize;
            }
            
            _parse = &XmlReaderImpl::onCharacters;
            onCharacters(c);
        }

        void onCharactersMax(int_type c)
        {
            _chars.clear();
            _parse = &XmlReaderImpl::onCharacters;
            onCharacters(c);
        }

        void onEntityReference(int_type c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                if(_usedSize >= _maxSize)
                    throw SyntaxError("XML syntax error", line());

                _token += ch;
                ++_usedSize;
                return;
            }

            if(ch == ';')
            {
                assert( _chunkSize == _chars.content().size() );
                //_chunkSize = _chars.content().size();

                _usedSize -= _token.size();
                _parse = &XmlReaderImpl::onCharacters;

                if( resolveDefaultEntity(_token) )
                {
                    const std::size_t tokenSize = _token.size();
                    _chunkSize += tokenSize;
                    
                    for(std::size_t n = 0; n < tokenSize; ++n)
                    {
                        Pt::Char d = _token[n];
                        //if(c == ' ' || c == '\n' || c == '\r' || c == '\t')
                        //    _chars.appendSpace(c);
                        //else
                            _chars.append(d);
                    }

                    if(_chunkSize >= _maxChunkSize)
                    {
                        _parse = &XmlReaderImpl::onCharactersMax;
                        setCharactersChunk();
                    }
                }
                else
                {
                    _entityRef.setName(_token);
                    resolveEntity(_entityRef);
                }

                _token.clear();
                return;
            }

            throw SyntaxError("XML syntax error", line());
        };

        void beforeCData(int_type c)
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

                assert( _chunkSize == _chars.content().size() );
                //_chunkSize = _chars.content().size();

                if( (_options & ReportCData) && ! _chars.empty() )
                {
                    setCharactersChunk();
                }
                
                _parse = &XmlReaderImpl::onCDataBegin;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        };

        void onCDataBegin(int_type c)
        {
            if(_options & ReportCData)
            {
                _chars.clear();
                _chars.setCData(true);
            }

            _parse = &XmlReaderImpl::onCData;
            onCData(c);
        }

        void onCData(int_type c)
        {
            Char ch = notEof(c);
            if(ch == ']')
            {
                _parse = &XmlReaderImpl::onCDataClose0;
                return;
            }

            _chars.append(ch);

            ++_chunkSize;

            if(_chunkSize >= _maxChunkSize)
            {
                setCharactersChunk();
                _parse = &XmlReaderImpl::onCDataMax;
            }
        }

        void onCDataMax(int_type c)
        {
            _chars.clear();

            if(_options & ReportCData)
                _chars.setCData(true);
            
            _parse = &XmlReaderImpl::onCData;
            onCData(c);
        }

        void onCDataClose0(int_type c)
        {
            Char ch = notEof(c);
            
            if(ch == ']')
            {
                _parse = &XmlReaderImpl::onCDataClose1;
                return;
            }

            _chars.append(']');
            _chars.append(ch);
            _chunkSize += 2;

            _parse = &XmlReaderImpl::onCData;
        }

        void onCDataClose1(int_type c)
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
            _chunkSize += 3;
            
            _parse = &XmlReaderImpl::onCData;
        }

        void afterCData(int_type c)
        {
            if( _options & ReportCData )
                _chars.clear();
            
            _parse = &XmlReaderImpl::afterTag;
            afterTag(c);
        }

        void onEpilog(int_type c)
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

        void onProlog(int_type c)
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
        inline Char notEof(int_type c) const
        {
            if( c == std::char_traits<Char>::eof() )
            {
                throw SyntaxError("XML syntax error", line());
            }

            return Char(c);
        }

        inline bool isQuote(Char ch) const
        {
            return ch == '\'' || ch == '"';
        }

        inline void setQuotedBegin(Char ch)
        {
            _quotChar = static_cast<unsigned char>( ch.value() );
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
            // TODO: push state should add to _usedSize...
            _parseStack.push(parse);
        }

        inline void popParseState()
        {
            // TODO: push state should shrink _usedSize...
            if( _parseStack.empty() )
                throw SyntaxError("XML syntax error", line());

            _parse = _parseStack.top();
            _parseStack.pop();
        }
        
        inline void popParseState(ParseFunc parse)
        {
            // TODO: push state should shrink _usedSize...
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
        }

        void setDocType()
        {
            // NOTE: might want to do _usedSize -= _docType.size();
            if(_options & ReportDtd)
                _current = &_docType;
        }

        void setEndDocType(bool internSubset)
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

            _usedSize -= _comment.content().size();
        }

        void setProcessingInstruction()
        {
            if(_options & ReportProcessingInstructions)
                _current = &_procInstr;

            _usedSize -= _procInstr.target().size() + _procInstr.data().size();
        }

        void setStartElement()
        {
            incDepth();

            const QName& name = _startElem.name();
            const String& prefix = name.prefix();
            
            _startElem.namespaceMapping().clear();

            // TODO: pass StartElement to fill
            const Namespace* ns = _nsctx.startElement(_depth, _startElem.namespaceMapping(), prefix);
            
            //const Namespace* ns = _nsctx.findPrefix( prefix );
            if( ! ns )
                throw SyntaxError("undeclared namespace prefix", line());

            _startElem.setNamespace(*ns);

            AttributeList& attributes = _startElem.attributes();
            AttributeList::Iterator it = attributes.begin();
            
            ElementModel* elemDecl = 0;
            if( it != attributes.end() )
                elemDecl = _dtd.findElement( _startElem.name() );
            
            for( ; it != attributes.end(); ++it)
            {
                const QName& attrName = it->name();
                _usedSize -= attrName.prefix().size() + attrName.name().size() + it->value().size();
                
                const Namespace* nsp = _nsctx.findPrefix( it->name().prefix() );
                if( ! nsp )
                    throw SyntaxError("undeclared namespace prefix", line());

                it->setNamespace(*nsp);

                // If the declared value is not CDATA, then discard any leading and
                // trailing space (#x20) characters and replace sequences of space
                // (#x20) characters by a single space (#x20) character.
                // All attributes for which no declaration has been read SHOULD be 
                // treated by a non-validating processor as if declared CDATA.
                if(elemDecl)
                {
                    AttributeModel* attrDecl = elemDecl->attributes().findAttribute( it->name() );

                    if(attrDecl && attrDecl->isNormalize())
                        normalizeAttributeValue( it->value() );
                }
            }

            // TODO: join this into findPrefix
            //_startElem.namespaceMapping().clear();
            //_nsctx.getMapped(_depth, _startElem.namespaceMapping());

            _current = &_startElem;
        }

        void setEndElement()
        {            
            const QName& name = _nameStack.top();
            const String& prefix = name.prefix();

            _endElem.namespaceMapping().clear();
            const Namespace* ns =_nsctx.endElement(_depth, _endElem.namespaceMapping(), prefix);
            
            //const Namespace* ns = _nsctx.findPrefix( prefix );
            if( ! ns )
                throw SyntaxError("undeclared namespace prefix", line());
                
            _endElem.setName(name, *ns);

            // TODO: join this into findPrefix
            //_endElem.namespaceMapping().clear();
            //_nsctx.getUnmapped(_depth, _endElem.namespaceMapping());

            decDepth();

            _current = &(_endElem);
        }

        inline void setCharactersChunk()
        {
            _chars.setChunk(true);
            _current = &_chars;
            _chunkSize = 0;
        }

        inline void setCharactersEnd()
        {
            if( _chunkSize != 0 )
            {
                _chars.setChunk(false);
                _current = &_chars;
                _chunkSize = 0;
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
        , _maxInputDepth(8)
        , _resolver(resolver)
        , _parse(0)
        , _current(0)
        , _depth(0)
        , _back(0)
        , _attr(0)
        , _options(DefaultOptions)
        , _usedSize(0)
        , _maxSize(2048)
        , _startElem(_nsctx)
        , _maxChunkSize(1024)
        , _chunkSize(0) 
        , _docType(_dtd)
        , _paramEntity(false)
        , _contentModel(0)
        , _attrModel(0)
        , _attlistDecl(0)
        , _notation(0)
        , _entity(0)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;
        }

        XmlReaderImpl(InputSource& is, XmlResolver* resolver = 0)
        : _is(&is)
        , _maxInputDepth(8)
        , _resolver(resolver)
        , _parse(0)
        , _current(0)
        , _depth(0)
        , _back(0)
        , _attr(0)
        , _options(DefaultOptions)
        , _usedSize(0)
        , _maxSize(2048)
        , _startElem(_nsctx)
        , _maxChunkSize(1024)
        , _chunkSize(0)
        , _docType(_dtd)
        , _paramEntity(false)
        , _contentModel(0)
        , _attrModel(0)
        , _attlistDecl(0)
        , _notation(0)
        , _entity(0)
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

        void setMaxSize(std::size_t n)
        {
            _maxSize = n;
        }

        std::size_t maxSize() const
        {
            return _maxSize;
        }

        std::size_t usedSize() const
        {
            return _usedSize;
        }

        void setChunkSize(std::size_t n)
        {
            _maxChunkSize = n;
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
            _usedSize = 0;
            _chunkSize = 0;

            _parse = &XmlReaderImpl::onDocumentBegin;

            while( ! _parseStack.empty() )
                _parseStack.pop();

            _current = 0;
            _back = 0;
            _nameStack.clear();

            _qname.clear();
            _token.clear(); 
            _quotChar = '"';
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

        std::size_t depth() const
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

                //std::cerr << char(c) << " (" << c << ")\n";
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
                else if(n < 0)
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
            while( ! _current);

            return _current;
        }

    private:
        InputSource* _is;
        std::size_t _maxInputDepth;
        XmlResolver* _resolver;
        EntityResolver _entityResolver;
        
        QNameStack _nameStack;
        InputStack _input;
        ParseFunc _parse;
        Node* _current;
        
        std::size_t _depth;
        const Pt::Char* _back;
        Attribute* _attr;
        String _token;
        int _options;

        std::size_t _usedSize;
        std::size_t _maxSize;
        NamespaceContext _nsctx;
        StartElement _startElem;
        EndElement _endElem;

        Characters _chars;
        std::size_t _maxChunkSize;
        std::size_t _chunkSize;

        EntityReference _entityRef;
        ProcessingInstruction _procInstr;
        Comment _comment;

        // TODO: some sort of union?
        StartDocument _startDoc;
        EndDocument _endDoc;

        DocTypeDefinition _dtd;
        DocType _docType;
        EndDocType _endDocType;
        ContentModelBuilder _cmBuilder;

        bool _paramEntity;
        unsigned char _quotChar;
        QName _qname;
        std::stack<ParseFunc> _parseStack;

        ContentModel* _contentModel;
        AttributeModel* _attrModel;
        AttributeListModel* _attlistDecl;
        Notation* _notation;
        Entity* _entity;
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


InputSource* XmlReader::input()
{
    return _impl->input();
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


void XmlReader::setMaxSize(std::size_t n)
{
    return _impl->setMaxSize(n);
}


std::size_t XmlReader::maxSize() const
{
    return _impl->maxSize();
}


std::size_t XmlReader::usedSize() const
{
    return _impl->usedSize();
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


std::size_t XmlReader::depth() const
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

} // namespace Xml

} // namespace Pt
