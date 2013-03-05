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
#include "DtdValidator.h"
#include "ElementDeclaration.h"
#include "AttributeDeclaration.h"
#include "InputStack.h"

#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/XmlResolver.h>
#include <Pt/Xml/DocTypeDefinition.h>
#include <Pt/Xml/NamespaceContext.h>
#include <Pt/Xml/EndDocument.h>
#include <Pt/Xml/DocType.h>
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/Xml/XmlError.h"
#include "Pt/System/Logger.h"
#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"

#include <iostream>
#include <memory>
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
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::onProlog;
            }
            else if( ch == '<')
            {
                _parse = &XmlReaderImpl::onXmlDecl;
            }
            else if( ch == 0xfeff)
            {
                return;
            }
            else
            {
                throw SyntaxError("XML syntax error", line());
            }
        }

        void onXmlDecl(int c)
        {
            Char ch = notEof(c);

            if(ch == '?')
            {
                _parse = &XmlReaderImpl::onXmlDeclQMark;
                return;
            }

            if(ch == '!')
            {
                _parse = &XmlReaderImpl::onTagExclam;
                return;
            }

            if( isAlpha(ch) )
            {
                _startElem.clear();
                _startElem.name() += ch;
                _parse = &XmlReaderImpl::onStartElement;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onXmlDeclQMark(int c)
        {
            Char ch = notEof(c);
 
            if( isAlpha(ch) )
            {
                _procInstr.clear();
                _procInstr.target() += ch;
                _parse = &XmlReaderImpl::onXmlDeclName;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onXmlDeclName(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                if( _procInstr.target() == L"xml" )
                    _parse =  &XmlReaderImpl::onXmlDeclBeforeAttr;
                else
                    _parse =  &XmlReaderImpl::onProcessingInstructionData;

                return;
            }

            if( isAlpha(ch) || ch == ':' )
            {
                _procInstr.target() += ch;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onXmlDeclBeforeAttr(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.clear();
                _attr.name() += c;
                _parse =  &XmlReaderImpl::onXmlDeclAttr;
                return;
            }

            if(ch == '?')
            {
                _parse =  &XmlReaderImpl::onXmlDeclEnd;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onXmlDeclAttr(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse =  &XmlReaderImpl::onXmlDeclAfterName;
                return;
            }

            if(ch == '=')
            {
                _parse =  &XmlReaderImpl::onXmlDeclBeforeValue;
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.name() += ch;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onXmlDeclAfterName(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '=')
            {
                _parse =  &XmlReaderImpl::onXmlDeclBeforeValue;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onXmlDeclBeforeValue(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isQoute(ch) )
            {
                _parse =  &XmlReaderImpl::onXmlDeclValue;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onXmlDeclValue(int c)
        {
            Char ch = notEof(c);

            if( isQoute(ch) )
            {
                if(_attr.name() == L"version")
                {
                    _version = _attr.value();
                }
                else if(_attr.name() == L"encoding")
                {
                    _encoding = _attr.value();
                }
                else if(_attr.name() == L"standalone")
                {
                    if(_attr.value() == L"true")
                        _standalone = true;
                }

                _parse =  &XmlReaderImpl::onXmlDeclBeforeAttr;
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.value() += c;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onXmlDeclEnd(int c)
        {
            Char ch = notEof(c);

            if(ch == '>')
            {
                _parse =  &XmlReaderImpl::onProlog;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onProcessingInstruction(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
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

            if(Pt::isspace(ch) || isAlpha(ch) || isQoute(ch) || 
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
                _current = &(_procInstr);
                _parse = &XmlReaderImpl::afterTag;
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
                _parse = &XmlReaderImpl::onProcessingInstruction;
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
                _startElem.name() += ch;
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _docType.rootName() += ch;
                _parse = &XmlReaderImpl::OnDtdRootName;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdRootName(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _docType.rootName() += ch;
                return;
            }

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == 'S')
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdExternalSystem;
                return;
            }

            if( ch == 'P')
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdExternalPublic;
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

        void OnDtdExternalPublic(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'U' || ch == 'B' || ch == 'L' || ch == 'I' || ch == 'C')
            {
                _token += ch;
                return;
            }

            if( Pt::isspace(ch) )
            {
                bool ok = _token == L"PUBLIC";
                _token.clear();
                if( ! ok)
                    throw SyntaxError("XML syntax error", line());
                
                _parse = &XmlReaderImpl::OnDtdExternalBeforePublicId;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdExternalSystem(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'Y' || ch == 'S' || ch == 'T' || ch == 'E' || ch == 'M')
            {
                _token += ch;
                return;
            }

            if( Pt::isspace(ch) )
            {
                bool ok = _token == L"SYSTEM";
                _token.clear();
                if( ! ok)
                    throw SyntaxError("XML syntax error", line());
                
                _parse = &XmlReaderImpl::OnDtdExternalBeforeSystemId;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdExternalBeforePublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _parse = &XmlReaderImpl::OnDtdExternalPublicId;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdExternalPublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _docType.setPublicId(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterExternalPublicId;
                return;
            }

            _token += ch;
        }

        void OnDtdAfterExternalPublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _parse = &XmlReaderImpl::OnDtdExternalSystemId;
                return;
            }

            if( ch == '>' )
            {
                setDocType();

                bool externalDtd = resolveExternalDtd();

                // TODO: revert logic, set a isInternalDtd flag when [ is parsed
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdExternalBeforeSystemId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _parse = &XmlReaderImpl::OnDtdExternalSystemId;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdExternalSystemId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _docType.setSystemId(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterExternalSystemId;
                return;
            }

            _token += ch;
        }

        void OnDtdAfterExternalSystemId(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == '>' )
            {
                setDocType();

                bool externalDtd = resolveExternalDtd();

                // TODO: revert logic, set a isInternalDtd flag when [ is parsed
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '<')
            {
                _parse = &XmlReaderImpl::OnDtdTag;
                return;
            }

            if( ch == ']' )
            {
                setDocumentTypeDefinition();
                --_depth;
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
                return;
            }

            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '<')
            {
                _parse = &XmlReaderImpl::OnDtdTag;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdExternal);
                return;
            }

            throw SyntaxError("XML syntax error", line());
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
                _entityName += ch;
                return;
            }

            if(ch == ';')
            {
                assert(_beforeEntityReference);
                resolveParamEntity(_entityName);
                _parse = _beforeEntityReference;
                _beforeEntityReference = 0;
                _entityName.clear();
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void enterParameterReference(ParseFunc from)
        {
            assert( ! _beforeEntityReference);
            _beforeEntityReference = from;
            _parse = &XmlReaderImpl::OnDtdParameterEntityReference;
        }

        void OnDtdEntityValueParameterEntityReference(int c)
        {
            Pt::Char ch = notEof(c);
            
            if( isAlpha(ch) )
            {
                _entityName += ch;
                return;
            }

            if(ch == ';')
            {
                assert(_beforeEntityReference);
                resolveParamEntity(_entityName);
                _parse = _beforeEntityReference;
                _beforeEntityReference = 0;
                _entityName.clear();
                return;
            }

            if(ch == '&')
            {
                throw SyntaxError("character entity reference in parameter entity reference", line());
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
        
        void enterEntityValueParameterEntityReference(ParseFunc from)
        {
            assert( ! _beforeEntityReference);
            _beforeEntityReference = from;
            _parse = &XmlReaderImpl::OnDtdEntityValueParameterEntityReference;
        }

        void OnDtdTag(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdTag);
                return;
            }

            if(ch == '!')
            {
                _parse = &XmlReaderImpl::OnDtdTagName;
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

            if( Pt::isspace(ch) )
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
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdTagName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntityBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '%')
            {
                _parse = &XmlReaderImpl::OnDtdParamEntityBegin;
                return;
            }

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEntityName;
                return;
            }

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
            {
                assert(_entity == 0);
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
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEntitySystem;
                return;
            }

            if( ch == '"' )
            {
                _parse = &XmlReaderImpl::OnDtdEntityValueQuot;
                return;
            }

            if( ch == '\'' )
            {
                _parse = &XmlReaderImpl::OnDtdEntityValueApos;
                return;
            }
            
            if(ch == 'P')
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEntityPublic;
                return;
            }
            
            if( Pt::isspace(ch) )
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

        void OnDtdEntityPublic(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'U' || ch == 'B' || ch == 'L' || ch == 'I' || ch == 'C')
            {
                _token += ch;
                return;
            }

            if( Pt::isspace(ch) )
            {
                bool ok = _token == L"PUBLIC";
                _token.clear();
                if( ! ok)
                    throw SyntaxError("XML syntax error", line());
                
                _parse = &XmlReaderImpl::OnDtdEntityBeforePublicId;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityPublic);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntityBeforePublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _parse = &XmlReaderImpl::OnDtdEntityPublicId;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityBeforePublicId);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntityPublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                if(_entity)
                    _entity->setPublicId(_token);
                
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdEntityAfterPublicId;
                return;
            }

            _token += ch;
        }

        void OnDtdEntityAfterPublicId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _parse = &XmlReaderImpl::OnDtdEntitySystemId;
                return;
            }

            if( ch == '>' )
            {
                _entity = 0;
                
                if( _input.isExternalDtd() )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }

            if( Pt::isspace(ch) )
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

        void OnDtdEntitySystem(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'Y' || ch == 'S' || ch == 'T' || ch == 'E' || ch == 'M')
            {
                _token += ch;
                return;
            }

            if( Pt::isspace(ch) )
            {
                bool ok = _token == L"SYSTEM";
                _token.clear();
                if( ! ok)
                    throw SyntaxError("XML syntax error", line());
                
                _parse = &XmlReaderImpl::OnDtdEntityBeforeSystemId;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntitySystem);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntityBeforeSystemId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                _parse = &XmlReaderImpl::OnDtdEntitySystemId;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdEntityBeforeSystemId);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdEntitySystemId(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' || ch == '\'' )
            {
                if(_entity)
                    _entity->setSystemId(_token);
                
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterEntityValue;
                return;
            }

            _token += ch;
        }

        // The entity value literal is preprocessed before it is used in an
        // entity reference. Character references are replaced immediately
        // with the specified character. Parameter entity references must be
        // be resolved recursively.
        void OnDtdEntityValueQuot(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' )
            {
                if(_entity)
                    _entity->setValue(_token);
                
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterEntityValue;
                return;
            }

            if(ch == '&')
            {
                enterEntityValueCharacterReference(&XmlReaderImpl::OnDtdEntityValueQuot);
                return;
            }

            if( ch == '%' )
            {
                enterEntityValueParameterEntityReference(&XmlReaderImpl::OnDtdEntityValueQuot);
                return;
            }

            _token += ch;
        }

        void OnDtdEntityValueApos(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '\'' )
            {
                if(_entity)
                    _entity->setValue(_token);
                
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterEntityValue;
                return;
            }

            if(ch == '&')
            {
                enterEntityValueCharacterReference(&XmlReaderImpl::OnDtdEntityValueApos);
                return;
            }

            if( ch == '%' )
            {
                enterEntityValueParameterEntityReference(&XmlReaderImpl::OnDtdEntityValueApos);
                return;
            }

            _token += ch;
        }
        
        // Entity references in entity value literals are left as is except
        // numerical character rerefences, which are resolved immediately
        void OnEntityValueCharacterReference(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == ';')
            {
                if( ! EntityMapping::resolveCharacterEntity(_characterReference) )
                {
                    _token += '&';
                    _token += _characterReference;
                    _token += ';';
                }
                else
                {
                    _token += _characterReference;
                }

                _characterReference.clear();

                assert(_beforeCharacterReference != 0);
                _parse = _beforeCharacterReference;
                _beforeCharacterReference = 0;
                return;
            }

            _characterReference += ch;
        }

        void enterEntityValueCharacterReference(ParseFunc from)
        {
            assert( ! _beforeCharacterReference);
            _beforeCharacterReference = from;
            _parse = &XmlReaderImpl::OnEntityValueCharacterReference;
        }

        void OnDtdAfterEntityValue(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '>' )
            {
                _entity = 0;
                
                if( _input.isExternalDtd() )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::OnDtdInternal;

                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAfterEntityValue);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttListBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdAttListName;
                return;
            }

            if( Pt::isspace(ch) )
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
                _token += ch;
                return;
            }

            if( Pt::isspace(ch) )
            {
                assert( _attlistDecl == 0 );
                _attlistDecl = &_dtd.declareAttributeList(_token);
                _token.clear();
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
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdAttrName;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdBeforeAttrName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::OnDtdAfterAttrName;
                return;
            }
            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAttrName);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAfterAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == 'C' )
            {
                _parse = &XmlReaderImpl::OnDtdCDATA0;
                return;
            }

            if( ch == 'N' )
            {
                _parse = &XmlReaderImpl::OnDtdNMTOKENS0;
                return;
            }

            if( ch == 'I' )
            {
                _parse = &XmlReaderImpl::OnDtdID0;
                return;
            }

            if( ch == '(' )
            {
                assert(_attrDecl == 0);
                assert(_attlistDecl);

                if( 0 == _attlistDecl->findAttribute(_token) )
                {
                    _attrDecl = new Pt::Xml::EnumAttributeDeclaration();
                    _attrDecl->setName(_token);
                    _attlistDecl->addAttribute(_attrDecl);
                }

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrEnum;
                return;
            }

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _token.clear();
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

            if( Pt::isspace(ch) )
            {
                assert(_attrDecl);
                static_cast<EnumAttributeDeclaration*>(_attrDecl)->addValue(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrAfterEnumValue;
                return;
            }

            if( ch == '|' )
            {
                assert(_attrDecl);
                static_cast<EnumAttributeDeclaration*>(_attrDecl)->addValue(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAttrEnumSep;
                return;
            }

            if( ch == ')' )
            {
                assert(_attrDecl);
                static_cast<EnumAttributeDeclaration*>(_attrDecl)->addValue(_token);
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

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::OnDtdAfterAttrEnumSep;
                return;
            }

            if( isAlpha(ch) )
            {
                _token.clear();
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _token.clear();
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

        void OnDtdID0(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdID0);
                return;
            }

            if( ch != 'D' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdID1;
        }
        
        void OnDtdID1(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdID1);
                return;
            }
            
            if( ch == 'R' )
            {
                _parse = &XmlReaderImpl::OnDtdIDREF2;
                return;
            }

            if( ! Pt::isspace(ch) )
                throw SyntaxError("XML syntax error", line());

            assert(_attrDecl == 0);
            assert(_attlistDecl);

            if( 0 == _attlistDecl->findAttribute(_token) )
            {
                _attrDecl = new Pt::Xml::IDAttributeDeclaration(_dtdValidator);
                _attrDecl->setName(_token);
                _attlistDecl->addAttribute(_attrDecl);
            }

            _token.clear();
            _parse = &XmlReaderImpl::OnDtdAfterAttrType;
        }

        void OnDtdIDREF2(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIDREF2);
                return;
            }

            if( ch != 'E' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdIDREF3;
        }

        void OnDtdIDREF3(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIDREF3);
                return;
            }

            if( ch != 'F' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdIDREF4;
        }

        void OnDtdIDREF4(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIDREF4);
                return;
            }

            if( ch == 'S' )
            {
                _parse = &XmlReaderImpl::OnDtdIDREFS;
                return;
            }

            if( ! Pt::isspace(ch) )
                throw SyntaxError("XML syntax error", line());

            assert(_attrDecl == 0);
            assert(_attlistDecl);

            if( 0 == _attlistDecl->findAttribute(_token) )
            {
                _attrDecl = new Pt::Xml::IDRefAttributeDeclaration(_dtdValidator);
                _attrDecl->setName(_token);
                _attlistDecl->addAttribute(_attrDecl);
            }

            _token.clear();
            _parse = &XmlReaderImpl::OnDtdAfterAttrType;
        }

        void OnDtdIDREFS(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdIDREFS);
                return;
            }

            if( ! Pt::isspace(ch) )
                throw SyntaxError("XML syntax error", line());

            assert(_attrDecl == 0);
            assert(_attlistDecl);

            if( 0 == _attlistDecl->findAttribute(_token) )
            {
                _attrDecl = new Pt::Xml::IDRefsAttributeDeclaration(_dtdValidator);
                _attrDecl->setName(_token);
                _attlistDecl->addAttribute(_attrDecl);
            }

            _token.clear();
            _parse = &XmlReaderImpl::OnDtdAfterAttrType;
        }

        void OnDtdCDATA0(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdCDATA0);
                return;
            }

            if( ch != 'D' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdCDATA1;
        }

        void OnDtdCDATA1(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdCDATA1);
                return;
            }

            if( ch != 'A' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdCDATA2;
        }

        void OnDtdCDATA2(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdCDATA2);
                return;
            }

            if( ch != 'T' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdCDATA3;
        }

        void OnDtdCDATA3(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdCDATA3);
                return;
            }

            if( ch != 'A' )
                throw SyntaxError("XML syntax error", line());

            assert(_attrDecl == 0);
            assert(_attlistDecl);

            if( 0 == _attlistDecl->findAttribute(_token) )
            {
                _attrDecl = new Pt::Xml::CDataAttributeDeclaration();
                _attrDecl->setName(_token);
                _attlistDecl->addAttribute(_attrDecl);
            }

            _token.clear();
            _parse = &XmlReaderImpl::OnDtdAfterAttrType;
        }

        void OnDtdNMTOKENS0(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNMTOKENS0);
                return;
            }

            if( ch != 'M' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdNMTOKENS1;
        }

        void OnDtdNMTOKENS1(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNMTOKENS1);
                return;
            }

            if( ch != 'T' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdNMTOKENS2;
        }


        void OnDtdNMTOKENS2(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNMTOKENS2);
                return;
            }

            if( ch != 'O' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdNMTOKENS3;
        }

        void OnDtdNMTOKENS3(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNMTOKENS3);
                return;
            }

            if( ch != 'K' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdNMTOKENS4;
        }

        void OnDtdNMTOKENS4(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNMTOKENS4);
                return;
            }

            if( ch != 'E' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdNMTOKENS5;
        }

        void OnDtdNMTOKENS5(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNMTOKENS5);
                return;
            }

            if( ch != 'N' )
                throw SyntaxError("XML syntax error", line());

            _parse = &XmlReaderImpl::OnDtdNMTOKENS6;
        }

        void OnDtdNMTOKENS6(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdNMTOKENS6);
                return;
            }

            if( ch == 'S' )
            {
                assert(_attrDecl == 0);
                assert(_attlistDecl);

                if( 0 == _attlistDecl->findAttribute(_token) )
                {
                    _attrDecl = new Pt::Xml::NMTokensAttributeDeclaration();
                    _attrDecl->setName(_token);
                    _attlistDecl->addAttribute(_attrDecl);
                }

                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterAttrType;
                return;
            }

            if( Pt::isspace(ch) )
            {
                assert(_attrDecl == 0);
                assert(_attlistDecl);

                if( 0 == _attlistDecl->findAttribute(_token) )
                {
                    _attrDecl = new Pt::Xml::NMTokenAttributeDeclaration();
                    _attrDecl->setName(_token);
                    _attlistDecl->addAttribute(_attrDecl);
                }
                
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterAttrType;
                return;
            }
                
            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAfterAttrType(int c)
        {
            Pt::Char ch = notEof(c);

            if( Pt::isspace(ch) )
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
                //assert(_attrDecl);
                if(_attrDecl) // skip duplicates
                    _attrDecl->setMode(Pt::Xml::AttributeDeclaration::Required);
                
                _parse = &XmlReaderImpl::OnDtdAfterAttrMode;
            }
            else if(_token == L"IMPLIED")
            {
                //assert(_attrDecl);
                if(_attrDecl) // skip duplicates
                    _attrDecl->setMode(Pt::Xml::AttributeDeclaration::Implied);
                
                _parse = &XmlReaderImpl::OnDtdAfterAttrMode;
            }
            else if(_token == L"FIXED")
            {
                //assert(_attrDecl);
                if(_attrDecl) // skip duplicates
                    _attrDecl->setMode(Pt::Xml::AttributeDeclaration::Fixed);
                
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
                //assert(_attrDecl);
                _attrDecl = 0;

                assert(_attlistDecl);
                _attlistDecl = 0;

                if( _input.isExternalDtd() )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }
            
            if( Pt::isspace(ch) )
                return;

            if( isAlpha(ch) )
            {
                //assert(_attrDecl);
                _attrDecl = 0;
                
                _token += ch;
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

            if( Pt::isspace(ch) )
                return;

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdAttrDefault(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '"')
            {
                //assert(_attrDecl);
                if(_attrDecl) // skip duplicates
                    _attrDecl->setDefaultValue(_token);
                
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
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdElementName;
                return;
            }

            if( Pt::isspace(ch) )
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
                _token += ch;
                return;
            }

            if( Pt::isspace(ch) )
            {
                assert(_elemDecl == 0);
                _elemDecl = _dtd.declareElement(_token);
                _dtdContext.resetExpression();
                _token.clear();
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
                _token.clear();
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdEmptyOrAny;
                return;
            }

            if( Pt::isspace(ch) )
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

            _dtdContext.pushOperator(ch);
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
                    
                    if(_elemDecl) // skip duplicates
                        _elemDecl->setEmpty();
                    
                    _elemDecl = 0;
                    _parse = &XmlReaderImpl::OnDtdAfterAnyOrEmpty;
                }
                else if(_token == L"ANY")
                {
                    _token.clear();
                    
                    if(_elemDecl) // skip duplicates
                        _elemDecl->setAny();

                    _elemDecl = 0;
                    _parse = &XmlReaderImpl::OnDtdAfterAnyOrEmpty;
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

        void OnDtdAfterAnyOrEmpty(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '>')
            {
                if( _input.isExternalDtd() )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }
            
            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == '%' )
            {
                enterParameterReference(&XmlReaderImpl::OnDtdAfterAnyOrEmpty);
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void OnDtdBeforeElementEnd(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '>')
            {
                ContentParticle& content = _dtdContext.finishExpression();

                if(_elemDecl) // skip duplicates
                    _elemDecl->setExpression( content, _dtdContext.expressionSize() );
                
                _elemDecl = 0;
                _dtdContext.resetExpression();
                
                if( _input.isExternalDtd() )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }
            
            if( Pt::isspace(ch) )
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
                _token.clear();
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdIdentifier;
                return;
            }

            if(ch == '(')
            {
                _dtdContext.pushOperator(ch);
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
                pushDtdOperand(_token);
                _token.clear();
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == '|')
            {
                pushDtdOperand(_token);
                _token.clear();
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if(ch == '+')
            {
                pushDtdOperand(_token);
                _token.clear();
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '*')
            {
                pushDtdOperand(_token);
                _token.clear();
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '?')
            {
                pushDtdOperand(_token);
                _token.clear();
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if( ch == ')')
            {
                pushDtdOperand(_token);
                _token.clear();
                _dtdContext.pushClosingBrace();
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
                ContentParticle& content = _dtdContext.finishExpression();

                if(_elemDecl) // skip duplicates
                    _elemDecl->setExpression( content, _dtdContext.expressionSize() );

                _elemDecl = 0;
                _dtdContext.resetExpression();
                
                if( _input.isExternalDtd() )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }

            if( ch == ',')
            {
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == ')')
            {
                _dtdContext.pushClosingBrace();
                _parse = &XmlReaderImpl::OnDtdContentExprEnd;
                return;
            }

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isAlpha(ch) || ch == '#')
            {
                _token.clear();
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdIdentifier;
                return;
            }

            if(ch == '(')
            {
                _dtdContext.pushOperator(ch);
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
                ContentParticle& content = _dtdContext.finishExpression();

                if(_elemDecl) // skip duplicates
                    _elemDecl->setExpression( content, _dtdContext.expressionSize() );

                _elemDecl = 0;
                _dtdContext.resetExpression();

                if( _input.isExternalDtd() )
                    _parse = &XmlReaderImpl::OnDtdExternal;
                else
                    _parse = &XmlReaderImpl::OnDtdInternal;
                
                return;
            }

            if( ch == ',')
            {
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _dtdContext.pushOperator(ch);
                _parse = &XmlReaderImpl::OnDtdUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _dtdContext.pushClosingBrace();
                return;
            }

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == '>' )
            {
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

            if( Pt::isspace(ch) )
            {
                if( depth() == 0 )
                {
                    _parse = &XmlReaderImpl::onProlog;
                    return;
                }

                appendContent(c);
                _parse = &XmlReaderImpl::onIgnorableCharacters;
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
                _token.clear();
                _parse = &XmlReaderImpl::onEntityReference;
                return;
            }

            _chars.setIgnorable(false);
            appendContent(c);
            _parse = &XmlReaderImpl::onCharacters;
        }

        void beforeComment(int c)
        {
            if(c == '-')
            {
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
        }

        void afterComment(int c)
        {
            Char ch = notEof(c);

            if(ch == '-')
            {
                _parse = &XmlReaderImpl::onCommentEnd;
                return;
            }

            _parse = &XmlReaderImpl::onComment;
        }

        void onCommentEnd(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '>')
            {
                if(depth() == 0)
                {
                    _parse = &XmlReaderImpl::onProlog;
                    return;
                }

                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onStartElement(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::beforeAttribute;
                return;
            }

            if(ch == '/')
            {
                _chars.clear();
                _current = &(_startElem);
                _depth++;

                setNamespace(_startElem);

                _parse = &XmlReaderImpl::onEmptyElement;
                return;
            }

            if(ch == ':')
            {
                if( ! _startElem.prefix().empty() )
                    throw SyntaxError("XML syntax error (invalid namespace prefix)", line());

                _startElem.prefix() = _startElem.name();
                _startElem.name().clear();
                return;
            }

            if( isAlpha(ch) )
            {
                _startElem.name() += c;
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                _current = &_startElem;
                _depth++;

                setNamespace(_startElem);

                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void beforeAttribute(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '/')
            {
                _current = &(_startElem);
                _depth++;

                setNamespace(_startElem);

                _parse = &XmlReaderImpl::onEmptyElement;
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.clear();
                _attr.name() += c;

                _parse = &XmlReaderImpl::onAttributeName;
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                _current = &(_startElem);
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

            if( Pt::isspace(ch) )
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
                if( ! _attr.prefix().empty() )
                    throw SyntaxError("invalid namespace prefix", line());

                _attr.prefix() = _attr.name();
                _attr.name().clear();
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.name() += c;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void afterAttributeName(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isQoute(ch) )
            {
                _parse = &XmlReaderImpl::onAttributeValue;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        }

        void onAttributeValue(int c)
        {
            Char ch = notEof(c);

            // TODO: normalize WS if the attribute type is defined in the DTD
            //       to be an ID, IDREF, NMTOKEN or an enumerated value.

            if( isQoute(ch) )
            {
                if(_attr.prefix() == "xmlns")
                {
                    _nsctx.setNamespace(_depth+1, _attr.name(), _attr.value());
                }
                else if(_attr.name() == "xmlns")
                {
                    _nsctx.setDefaultNamespace(_depth+1, _attr.value());
                }
                else
                {
                    _startElem.attributes().add(_attr);
                }
                
                _parse = &XmlReaderImpl::beforeAttribute;
                return;
            }

            if (ch == '&')
            {
                _token.clear();
                _parse = &XmlReaderImpl::onAttributeEntityReference;
                return;
            }

            _attr.value() += c;
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
                bool resolved = resolveEntity(_token);
                if(resolved)
                {
                    _attr.value() += _token;
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

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '>')
            {
                _endElem.name() = _startElem.name();
                _current = &(_endElem);
                _depth--;

                setNamespace(_endElem);

                /*if(depth() == 0)
                {
                    _parse = &XmlReaderImpl::onEpilog;
                    return;
                }

                _parse = &XmlReaderImpl::afterTag;*/

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
            
            if( Pt::isspace(ch) )
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

            if(ch == '>')
            {
                _chars.clear();
                _current = &(_endElem);
                _depth--;

                setNamespace(_endElem);

                /*if(depth() == 0)
                {
                    _parse = &XmlReaderImpl::onEpilog;
                    return;
                }

                _parse = &XmlReaderImpl::afterTag;*/

                _parse = &XmlReaderImpl::afterEndElement;

                return;
            }

            throw SyntaxError("XML syntax error", line());
        }
    
        void afterEndElementName(int c)
        {
            Char ch = notEof(c);
            
            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                _current = &(_endElem);
                _depth--;

                setNamespace(_endElem);

                /*if(depth() == 0)
                {
                    _parse = &XmlReaderImpl::onEpilog;
                    return;
                }

                _parse = &XmlReaderImpl::afterTag;*/
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

        void onIgnorableCharacters(int c)
        {
            Char ch = notEof(c);

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
                _token.clear();
                _parse = &XmlReaderImpl::onEntityReference;
                return;
            }

            appendContent(c);

            if( ! Pt::isspace(ch) )
            {
                _chars.setIgnorable(false);
                _parse = &XmlReaderImpl::onCharacters;
                return;
            }
        }

        void onCharacters(int c)
        {
            Char ch = notEof(c);

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
                _token.clear();
                _parse = &XmlReaderImpl::onEntityReference;
                return;
            }

            appendContent(c);
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
                bool resolved = resolveEntity(_token);
                if(resolved)
                {
                    _chars.append(_token);

                    // TODO: do this in Characters::append
                    for(std::size_t n = 0; n < _token.size(); ++n)
                    {
                        Pt::Char c = _token[n];
                        if(c != ' ' && c != '\n' && c != '\r' && c != '\t')
                        {
                            _chars.setIgnorable(false);
                            break;
                        }
                    }

                    _token.clear();
                    _parse = &XmlReaderImpl::onCharacters;
                    return;
                }

                // _chars.setIgnorable(false) is potentially called again when
                // non-space is found in replacement text and non-space text 
                // has been parsed before
                _token.clear();
                _parse = &XmlReaderImpl::onIgnorableCharacters;
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
                _parse = &XmlReaderImpl::onCData;
                return;
            }

            throw SyntaxError("XML syntax error", line());
        };

        void onCData(int c)
        {
            Char ch = notEof(c);

            if(ch == '>')
            {
                const String& content = _chars.content();
                unsigned len = content.length();

                if( len > 2 && content[len-2] == ']' && content[len-2] == ']')
                {
                    _chars.setIgnorable(false);
                    _chars.resize(len-2);

                    _parse = &XmlReaderImpl::afterTag;
                    return;
                }

                appendContent(c);
                return;
            }

            appendContent(c);
        }

        // not neccessary, allow EOF only when depth == 0 in other states

        // join onProlog and onEpilog -> EOF only allowed when root element was found
        void onEpilog(int c)
        {
            if( c == std::char_traits<Char>::eof() )
            {
                _current = &_endDoc;
                return;
            }

            Char ch(c);
            if( Pt::isspace(ch) )
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
            if( Pt::isspace(ch) )
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
        Char notEof(int c) const
        {
            if( c == std::char_traits<Char>::eof() )
            {
                throw SyntaxError("XML syntax error", line());
            }

            return Char(c);
        }

        bool isQoute(Char ch) const
        {
            return ch == '\'' || ch =='"';
        }

        // TODO: isXmlName()
        bool isAlpha(Char ch)
        {
            return ch == '.' || ch == '_' || ch == '-' || Pt::isalnum(ch) != 0;
        }

        void pushDtdOperand(const String& name)
        {
            if(name.at(0) == '#')
            {
                if(name != L"#PCDATA")
                    throw std::logic_error("DTD syntax error: expected PCDATA");

                PcDataParticle& pcdata = _dtdContext.getPcData();
                _dtdContext.pushOperand(pcdata);
                
                // TODO: allow #PCDATA only at beginning of first '('
                // TODO: allow only '|' as next token
                return;
            }
                
            LeafParticle& leaf = _dtdContext.getLabel(name);
            _dtdContext.pushOperand(leaf);
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
                virtual InputSource* onResolve(const Pt::String& publicId, const Pt::String& systemId)
                {
                    return 0;
                }

                virtual void onRelease(InputSource* is)
                {
                    delete is;
                }
        };

        bool resolveExternalDtd()
        {
            if( _resolver && _docType.isExternal() )
            {
                InputSource* is = _resolver->resolve( _docType.publicId(), _docType.systemId() );
                if(is)
                {
                    _input.setExternalDtd(*is, _resolver);
                    return true;
                }
            }

            return false;
        }
        
        bool resolveEntity(String& token)
        {
            // TODO: handle this in concrete parser states
            if( EntityMapping::resolveDefaultEntity( token ) )
                return true;

            InputSource* is = 0;

            const Entity* ent = _dtd.resolveEntity(token);
            if( ent )
            {
                if( ent->isInternal() )
                {
                    is = _entityResolver.resolveEntity(*ent);
                    _input.addInput(*is, &_entityResolver);
                }
                else if( ent->isExternal() && _resolver)
                {
                    is = _resolver->resolve( ent->publicId(), ent->systemId() );
                    if(is)
                    {
                        _input.addInput(*is, _resolver);
                    }
                }
            }
            
            if( ! is)
            {
                _entityRef.set(token, ent);
                _current = &_entityRef;
            }
            
            return false;
        }

        void resolveParamEntity(const String& token)
        {
            InputSource* is = 0;
            
            const Entity* ent = _dtd.resolveParamEntity(token);
            if( ent )
            {
                if( ent->isInternal() )
                {
                    is = _entityResolver.resolveEntity(*ent);
                    _input.addInput(*is, &_entityResolver);
                }
                else if( ent->isExternal() && _resolver)
                {
                    is = _resolver->resolve( ent->publicId(), ent->systemId() );
                    if(is)
                    {
                        _input.addInput(*is, _resolver);
                    }
                }
            }

            if( ! is)
            {
                _entityRef.set(token, ent);
                _current = &_entityRef;
            }
            return;
        }

        void appendContent(Pt::Char c)
        {
            const String& content = _chars.content();
            if (content.capacity() <= content.size() + 20)
            {
                if (content.capacity() < 16)
                    _chars.reserve(16);
                else
                    _chars.reserve(content.capacity() + content.capacity() / 2);
            }
            
            _chars.append(c);
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
                const Namespace* ns = _nsctx.getNamespace( se.prefix() );
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
                    const Namespace* ns = _nsctx.getNamespace( it->prefix() );
                    if( ! ns )
                    {
                        throw SyntaxError("undeclared namespace prefix", line());
                    }

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
                const Namespace* ns = _nsctx.getNamespace( e.prefix() );
                if( ! ns )
                    throw SyntaxError("undeclared namespace prefix", line());

                e.setNamespace(*ns);
            }
        }

        void setDocType()
        {
            if(_flags & XmlReader::ReportDtd)
                  _current = &_docType;
        }

        void setDocumentTypeDefinition()
        {
            if(_flags & XmlReader::ReportDtd)
                  _current = &_dtd;
        }

    public:
        XmlReaderImpl(XmlResolver* resolver = 0)
        : _resolver(resolver)
        , _flags(XmlReader::DefaultParseFlags)
        , _entity(0)
        , _depth(0)
        , _parse(0)                 
        , _beforeCharacterReference(0)
        , _beforeEntityReference(0)
        , _current(0)
        , _standalone(false)
        , _dtdContext()
        , _dtd(_dtdContext)
        , _docType()
        , _dtdValidator(_dtd)
        , _elemDecl(0)
        , _attrDecl(0)
        , _attlistDecl(0)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;
        }

        XmlReaderImpl(InputSource& is, XmlResolver* resolver = 0)
        : _resolver(resolver)
        , _flags(XmlReader::DefaultParseFlags)
        , _entity(0)
        , _depth(0)
        , _parse(0)                 
        , _beforeCharacterReference(0)
        , _beforeEntityReference(0)
        , _current(0)
        , _standalone(false)
        , _dtdContext()
        , _dtd(_dtdContext)
        , _docType()
        , _dtdValidator(_dtd)
        , _elemDecl(0)
        , _attrDecl(0)
        , _attlistDecl(0)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;

            _input.addInput(is);
        }

        ~XmlReaderImpl()
        { }

        int flags() const
        {
            return _flags;
        }
        
        void setFlag(XmlReader::ParseFlag f)
        {
            _flags |= f;
        }

        void unsetFlag(XmlReader::ParseFlag f)
        {
            _flags &= ~f;
        }

        void clear()
        {
            _input.clear();
            _parse = &XmlReaderImpl::onDocumentBegin;
            _beforeEntityReference = 0;
            _beforeCharacterReference = 0;
            _entityName.clear();
            _characterReference.clear();

            //TODO: should clear _token

            _nsctx.clear();
            
            _dtdValidator.clear();
            _dtd.clear();
            _dtdContext.clear();
            
            _elemDecl = 0;
            _attrDecl = 0;
            _attlistDecl = 0;
            _entity = 0;

            _docType.clear();
            _version.clear();
            _encoding.clear();
            _standalone = false;
            _depth = 0;
            _current = 0;

            // other members are cleared before they are parsed
        }

        XmlResolver* resolver() const
        {
            return _resolver;
        }

        void setInput(std::istream& is)
        {
            clear();

            InputSource* source = new ByteInputSource(is);
            _input.addInput(*source, &_entityResolver);
        }

        void setInput(InputSource& is)
        {
            clear();
            _input.addInput(is);
        }

        void addInput(InputSource& is)
        { _input.addInput(is); }

        const Pt::String& version() const
        { return _version; }

        const Pt::String& encoding() const
        { return _encoding; }

        bool isStandalone() const
        { return _standalone; }

        const DocTypeDefinition& dtd() const
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

            return *_current;
        }

        Node& next()
        {
            bool atEnd = false;
            _current = 0;
            std::char_traits<Char>::int_type c = 0;
            std::basic_streambuf<Char>* rdbuf = 0;

            while( ! _current )
            {
                rdbuf = _input.current()->rdbuf();

                if( ! rdbuf || rdbuf->sgetc() == std::char_traits<Char>::eof() )
                {            
                    rdbuf = _input.current()->get();

                    if( ! rdbuf || rdbuf->sgetc() == std::char_traits<Char>::eof() )
                    {
                        _input.removeInput();

                        if( _input.empty() )
                        {
                            (this->*_parse)( std::char_traits<Char>::eof() );
                            
                            if( ! _current)
                                throw SyntaxError("unexpected EOF", line());
                        }

                        continue;
                    }
                }

                c = rdbuf->sbumpc();
                (this->*_parse)(c);

                if(c == '\n')
                {
                    _input.bumpLine();
                }
            }

            if( (_flags & XmlReader::ValidateDtd) && _docType.isDefined() )
            {
                if( ! _dtdValidator.validate(*_current) )
                    throw SyntaxError("validation failed", line());
            }

            return *_current;
        }

        bool advance()
        {
            _current = 0;

            do
            {
                std::basic_streambuf<Char>* rdbuf = _input.current()->rdbuf();
                
                if(rdbuf && rdbuf->in_avail() > 0)
                {
                    std::char_traits<Char>::int_type c = rdbuf->sbumpc();
                    (this->*_parse)(c);

                    // TODO: move this to state functions
                    if(c == '\n')
                    {
                        _input.bumpLine();
                    }
                }
                else
                {                
                    rdbuf = _input.current()->getSome();

                    if( ! rdbuf)
                    {
                        _input.removeInput();
                        
                        if( _input.empty() )
                            (this->*_parse)( std::char_traits<Char>::eof() );
                    }
                    else if(rdbuf->in_avail() <= 0)
                        break;
                }
            } 
            while( ! _current);

            // TODO: use Validator outside of XmlReader
            if( _current )
            {
                if( (_flags & XmlReader::ValidateDtd) && _docType.isDefined() )
                {
                    if( ! _dtdValidator.validate(*_current) )
                        throw SyntaxError("validation failed", line());
                }
                
                return true;
            }

            return false;
        }

    private:
        XmlResolver* _resolver;
        EntityResolver _entityResolver;
        InputStack _input;
        int _flags;
        
        NamespaceContext _nsctx;
        Entity* _entity;
        String _token;
        Attribute _attr;
        std::size_t _depth;
        ParseFunc _parse;
        
        Pt::String _entityName;
        ParseFunc _beforeCharacterReference;
        
        Pt::String _characterReference;
        ParseFunc _beforeEntityReference;
        
        Node* _current;

        Pt::String _version;
        Pt::String _encoding;
        bool _standalone;

        DocTypeContext _dtdContext;
        DocTypeDefinition _dtd;
        DocType _docType;
        DocTypeValidator _dtdValidator;

        ElementDeclaration* _elemDecl;
        AttributeDeclaration* _attrDecl;
        AttributeListDeclaration* _attlistDecl;
        
        // TODO: some sort of union?
        ProcessingInstruction _procInstr;
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


XmlReader::XmlReader(std::istream& is)
: _impl(0)
{
    _impl = new XmlReaderImpl();
    setInput(is);
}


XmlReader::XmlReader(InputSource& is)
: _impl(0)
{
    _impl = new XmlReaderImpl(is);
}


XmlReader::XmlReader(XmlResolver& r, std::istream& is)
: _impl(0)
{
    _impl = new XmlReaderImpl(&r);
    setInput(is);
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


int XmlReader::flags() const
{
    return _impl->flags();
}


void XmlReader::setFlag(ParseFlag f)
{
    _impl->setFlag(f);
}


void XmlReader::unsetFlag(ParseFlag f)
{
    _impl->unsetFlag(f);
}


void XmlReader::clear()
{
    _impl->clear();
}


XmlResolver* XmlReader::resolver() const
{
    return _impl->resolver();
}


void XmlReader::setInput(std::istream& is)
{
    _impl->setInput(is);
}


void XmlReader::setInput(InputSource& is)
{
    _impl->setInput(is);
}


void XmlReader::addInput(InputSource& in)
{
    _impl->addInput(in);
}


const Pt::String& XmlReader::version() const
{
    return _impl->version();
}


const Pt::String& XmlReader::encoding() const
{
    return _impl->encoding();
}


bool XmlReader::isStandalone() const
{
    return _impl->isStandalone();
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


bool XmlReader::advance()
{
    return _impl->advance();
}

} // namespace Xml

} // namespace Pt
