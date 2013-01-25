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
#include "DocTypeDefinition.h"
#include "DtdValidator.h"

#include "Pt/Xml/XmlReader.h"
#include <Pt/Xml/NamespaceContext.h>
#include <Pt/Xml/EndDocument.h>
#include "Pt/Xml/EntityResolver.h"
#include <Pt/Xml/DocType.h>
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/XmlError.h"
#include "Pt/System/Logger.h"
#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"

#include <iostream>

log_define("Pt.Xml.XmlReader")

namespace Pt {

namespace Xml {

class XmlReaderImpl
{
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
            else
            {
                throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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
                _attr.value() += c;;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void onXmlDeclEnd(int c)
        {
            Char ch = notEof(c);

            if(ch == '>')
            {
                _parse =  &XmlReaderImpl::onProlog;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
        }

        void AfterDtdRootName(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == '[' )
            {
                _parse = &XmlReaderImpl::OnDtdInternal;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
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
                _parse = &XmlReaderImpl::OnDtdInternalEnd;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void OnDtdTag(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch != '!')
                throw SyntaxError("XML syntax error", _line);

            _parse = &XmlReaderImpl::OnDtdTagName;
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

            throw SyntaxError("XML syntax error", _line);
        }

        void OnDtdEntityBegin(int c)
        {
            Pt::Char ch = notEof(c);

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

            throw SyntaxError("XML syntax error", _line);
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
                _entity = _entities.addEntity(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterName;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }
        
        void OnDtdAfterName(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' )
            {
                _parse = &XmlReaderImpl::OnDtdValue;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void OnDtdValue(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '"' )
            {
                assert(_entity);
                _entity->setValue(_token, false);
                _entity = 0;
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdAfterValue;
                return;
            }

            _token += ch;
        }
        
        void OnDtdAfterValue(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '>' )
            {
                _parse = &XmlReaderImpl::OnDtdInternal;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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
                _elemDecl = &_dtd.declareElement(_token);
                _token.clear();
                _parse = &XmlReaderImpl::OnDtdBeforeAttrName;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
        }

        void OnDtdAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::AfterDtdAttrName;
                return;
            }
            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void AfterDtdAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == 'C' )
            {
                _parse = &XmlReaderImpl::OnDtdCDATA0;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void OnDtdCDATA0(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'D' )
                throw SyntaxError("XML syntax error", _line);

            _parse = &XmlReaderImpl::OnDtdCDATA1;
        }

        void OnDtdCDATA1(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'A' )
                throw SyntaxError("XML syntax error", _line);

            _parse = &XmlReaderImpl::OnDtdCDATA2;
        }

        void OnDtdCDATA2(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'T' )
                throw SyntaxError("XML syntax error", _line);

            _parse = &XmlReaderImpl::OnDtdCDATA3;
        }

        void OnDtdCDATA3(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'A' )
                throw SyntaxError("XML syntax error", _line);

            Pt::Xml::CDataAttributeDeclaration* attr = new Pt::Xml::CDataAttributeDeclaration();
            attr->setName(_token);
            _elemDecl->attrListDecl().push(attr);

            _token.clear();
            _parse = &XmlReaderImpl::AfterDtdAttrType;
        }

        void AfterDtdAttrType(int c)
        {
            Pt::Char ch = notEof(c);

            if( Pt::isspace(ch) )
                return;

            if(ch == '"')
            {
                _parse = &XmlReaderImpl::OnDtdAttrDefault;
                return;
            }

            if( ch != '#' )
                throw SyntaxError("XML syntax error", _line);

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

            if(_token == L"REQUIRED")
            {
                _elemDecl->attrListDecl().last().setMode(Pt::Xml::AttributeDeclaration::Required);
                _parse = &XmlReaderImpl::AfterDtdAttrMode;
            }
            else if(_token == L"IMPLIED")
            {
                _elemDecl->attrListDecl().last().setMode(Pt::Xml::AttributeDeclaration::Implied);
                _parse = &XmlReaderImpl::AfterDtdAttrMode;
            }
            else if(_token == L"FIXED")
            {
                _elemDecl->attrListDecl().last().setMode(Pt::Xml::AttributeDeclaration::Fixed);
                _parse = &XmlReaderImpl::AfterDtdAttrFixed;
            }
            else
                throw SyntaxError("XML syntax error", _line);
                
            _token.clear();

            (this->*_parse)(c);
        }

        void AfterDtdAttrMode(int c)
        {
            Pt::Char ch = notEof(c);

            if(c == '>')
            {
                _parse = &XmlReaderImpl::OnDtdInternal;
                return;
            }
            
            if( Pt::isspace(ch) )
                return;

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnDtdAttrName;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void AfterDtdAttrFixed(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '"')
            {
                _parse = &XmlReaderImpl::OnDtdAttrDefault;
                return;
            }

            if( Pt::isspace(ch) )
                return;

            throw SyntaxError("XML syntax error", _line);
        }

        void OnDtdAttrDefault(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '"')
            {
                _elemDecl->attrListDecl().last().setDefaultValue(_token);
                _token.clear();
                _parse = &XmlReaderImpl::AfterDtdAttrMode;
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

            throw SyntaxError("XML syntax error", _line);
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
                _elemDecl = &_dtd.declareElement(_token);
                _cmBuilder.clear();
                _token.clear();
                _parse = &XmlReaderImpl::OnElementContentBegin;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void OnElementContentBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'E' || ch == 'A')
            {
                _token.clear();
                _token += ch;
                _parse = &XmlReaderImpl::OnEmptyOrAny;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch != '(')
                throw SyntaxError("XML syntax error", _line);

            _cmBuilder.pushOperator(ch);
            _parse = &XmlReaderImpl::OnElementContent;
        }
        
        void OnEmptyOrAny(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if(_token == L"EMPTY")
            {
                _cmBuilder.setEmpty();
            }
            else
                throw SyntaxError("XML syntax error", _line);
            
            _token.clear();

            _parse = &XmlReaderImpl::OnDtdBeforeElementEnd;
            (this->*_parse)(c);
        }

        void OnDtdBeforeElementEnd(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '>')
            {
                _cmBuilder.finish( _elemDecl->contentModel(), _dtd.getMatch() );
                _parse = &XmlReaderImpl::OnDtdInternal;
                return;
            }
            
            if( Pt::isspace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void OnElementContent(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token.clear();
                _token += ch;
                _parse = &XmlReaderImpl::OnIdentifier;
                return;
            }

            if(ch == '(')
            {
                _cmBuilder.pushOperator(ch);
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void OnIdentifier(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                _parse = &XmlReaderImpl::OnIdentifier;
                return;
            }

            if( ch == ',')
            {
                pushDtdOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                pushDtdOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnBinaryOp;
                return;
            }

            if(ch == '+')
            {
                pushDtdOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnUnrayOp;
                return;
            }

            if(ch == '*')
            {
                pushDtdOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnUnrayOp;
                return;
            }

            if(ch == '?')
            {
                pushDtdOperand(_token);
                _token.clear();
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnUnrayOp;
                return;
            }

            if( ch == ')')
            {
                pushDtdOperand(_token);
                _token.clear();
                _cmBuilder.pushClosingBrace();
                _parse = &XmlReaderImpl::OnDtdContentExprEnd;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }
        
        void OnUnrayOp(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '>' )
            {
                _cmBuilder.finish( _elemDecl->contentModel(), _dtd.getMatch() );
                _parse = &XmlReaderImpl::OnDtdInternal;
                return;
            }

            if( ch == ',')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnBinaryOp;
                return;
            }

            if( ch == ')')
            {
                _cmBuilder.pushClosingBrace();
                _parse = &XmlReaderImpl::OnDtdContentExprEnd;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }
        
        void OnBinaryOp(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token.clear();
                _token += ch;
                _parse = &XmlReaderImpl::OnIdentifier;
                return;
            }

            if(ch == '(')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnElementContent;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void OnDtdContentExprEnd(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '>' )
            {
                _cmBuilder.finish( _elemDecl->contentModel(), _dtd.getMatch() );
                _parse = &XmlReaderImpl::OnDtdInternal;
                return;
            }

            if( ch == ',')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _cmBuilder.pushOperator(ch);
                _parse = &XmlReaderImpl::OnUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _cmBuilder.pushClosingBrace();
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw SyntaxError("XML syntax error", _line);
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
                _parse = &XmlReaderImpl::onProlog;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
        }

        void afterTag(int c)
        {
            if( c == std::char_traits<Char>::eof() )
            {
                if( depth() > 0 )
                    throw SyntaxError("XML syntax error", _line);

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
                throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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
                    throw SyntaxError("XML syntax error (invalid namespace prefix)", _line);

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
                _current = &(_startElem);
                _depth++;

                setNamespace(_startElem);

                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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
                    throw SyntaxError("invalid namespace prefix", _line);

                _attr.prefix() = _attr.name();
                _attr.name().clear();
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.name() += c;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
        }

        void onAttributeValue(int c)
        {
            Char ch = notEof(c);

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
                bool resolved = resolveEntity(_token, 0, &_attr);
                if(resolved)
                {
                    _attr.value() += _token;
                }
                
                _token.clear();
                _parse = &XmlReaderImpl::onAttributeValue;
                return;
            }
            
            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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
                    throw SyntaxError("invalid namespace prefix", _line);

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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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
                throw SyntaxError("XML syntax error", _line);
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
                throw SyntaxError("XML syntax error", _line);
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
                bool resolved = resolveEntity(_token, &_chars, 0);
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

            throw SyntaxError("invalid entity format", _line);
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
                    throw SyntaxError("XML syntax error", _line);
            }
            
            if( _token.length() < 7 )
                return;

            if( _token == L"[CDATA[" )
            {
                _token.clear();
                _parse = &XmlReaderImpl::onCData;
                return;
            }

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
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

            throw SyntaxError("XML syntax error", _line);
        }

    private:
        Char notEof(int c) const
        {
            if( c == std::char_traits<Char>::eof() )
            {
                throw SyntaxError("XML syntax error", _line);
            }

            return Char(c);
        }

        bool isQoute(Char ch) const
        {
            return ch == '\'' || ch =='"';
        }

        // TODO: rename to isXmlName()
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

                ContentModel::PcData& pcdata = _dtd.getPcData();
                _cmBuilder.pushOperand(pcdata);
                
                // TODO: allow #PCDATA only at beginning of first '('
                // TODO: allow only '|' as next token
                return;
            }
                
            ContentModel::Leaf& leaf = _dtd.getLabel(name);
            _cmBuilder.pushOperand(leaf);
        }

        bool resolveEntity(String& token, Characters* chars, Attribute* attr)
        {
            // TODO: handle this in concrete parser states
            if( _entities.resolveDefaultEntity( token ) )
                return true;

            const Entity* ent = _entities.resolveEntity(token);
            if( ! ent )
            {
                if(chars)
                    _entityRef.attach(chars);
                else if(attr)
                    _entityRef.attach(attr);
                
                _current = &_entityRef;
                return false;
            }

            if( ent->isExternal() )
            {
                if(chars)
                    _entityRef.attach(chars);
                else if(attr)
                    _entityRef.attach(attr);
                
                _current = &_entityRef;
                return false;
            }

            if( ent->isUnparsed() )
            {
                token = ent->value();
                return true;
            }

            _external.push( new StringInputSource( ent->value() ) );
            _buffer = _external.top()->rdbuf();
            return false;
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
                    throw SyntaxError("undeclared namespace prefix", _line);

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
                        throw SyntaxError("undeclared namespace prefix", _line);
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
                    throw SyntaxError("undeclared namespace prefix", _line);

                e.setNamespace(*ns);
            }
        }

    public:
        XmlReaderImpl(std::basic_istream<Char>& is, int flags)
        : _textBuffer( is.rdbuf() )
        , _tbuffer(0)
        , _buffer(0)
        , _flags(flags)
        , _standalone(true)
        , _depth(0)
        , _line(1)
        , _entity(0)
        , _parse(0)
        , _current(0)
        , _dtd()
        , _docType(_dtd)
        , _cmBuilder(_dtd)
        , _elemDecl(0)
        , _dtdValidator(_dtd)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;
        }

        XmlReaderImpl(std::istream& is, int flags)
        : _textBuffer(0)
        , _tbuffer(0)
        , _buffer(0)
        , _flags(flags)
        , _standalone(true)
        , _depth(0)
        , _line(1)
        , _entity(0)
        , _parse(0)
        , _current(0)
        , _dtd()
        , _docType(_dtd)
        , _cmBuilder(_dtd)
        , _elemDecl(0)
        , _dtdValidator(_dtd)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;

            _tbuffer = new TextBuffer( &is, new Pt::Utf8Codec() );
            _textBuffer = _tbuffer;
        }

        ~XmlReaderImpl()
        {
            while( ! _external.empty() )
            {
                delete _external.top();
                _external.pop();
            } 

            delete _tbuffer;
        }

        void clear(int flags)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;

            _buffer = 0;
            while( ! _external.empty() )
            {
                delete _external.top();
                _external.pop();
            } 

            _nsctx.clear();
            _cmBuilder.clear();
            _elemDecl = 0;
            _dtdValidator.clear();

            _entities.clear();
            _entity = 0;

            _flags = flags;
            _docType.clear();
            _version.clear();
            _encoding.clear();
            _standalone = true;
            _depth = 0;
            _line = 1;
            _current = 0;
        }

        void attach(std::basic_istream<Char>& is, int flags)
        {
            clear(flags);
            
            delete _tbuffer;
            _tbuffer = 0;
            _textBuffer = is.rdbuf();
        }

        void attach(std::istream& is, int flags)
        {
            clear(flags);
            
            delete _tbuffer;
            _tbuffer = new TextBuffer( &is, new Pt::Utf8Codec() );
            _textBuffer = _tbuffer;
        }

        const Pt::String& version() const
        { return _version; }

        const Pt::String& encoding() const
        { return _encoding; }

        bool isStandalone() const
        { return _standalone; }

        const DocType& docType() const
        { return _docType; }

        size_t depth() const
        {
            return _depth;
        }

        std::size_t line() const
        {
            return _line;
        }

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
            _current = 0;
            int c = 0;

            for(;;)
            {          
                _buffer = _textBuffer;    
                
                if( ! _external.empty() )
                {
                    _buffer = _external.top()->rdbuf();
                }

                if( ! _buffer )
                    throw std::logic_error("no input for XmlReader");

                while( ! _current )
                {
                    c = _buffer->sbumpc();
       
                    if( c == std::char_traits<Char>::eof() )
                    {
                        if( ! _external.empty() )
                        {
                            if( _external.top()->refs() == 0 )
                                delete _external.top();
                            
                            _external.pop();
                            break;
                        }
                    }
                    
                    (this->*_parse)(c); // might change _buffer

                    // TODO: this does not work for external input sources
                    if(c == '\n')
                    {
                        ++_line;
                    }
                }

                if( _current )
                    break;
            }

            if( _docType.isDefined() )
            {
                if( ! _dtdValidator.validate(*_current) )
                    throw SyntaxError("validation failed", _line);
            }

            return *_current;
        }

        bool advance()
        {
            _current = 0;
            int c = 0;

            for(;;)
            {          
                _buffer = _textBuffer;    
                
                if( ! _external.empty() )
                {
                    InputSource* input = _external.top();
 
                    if( ! input->advance() )
                    {
                        if( _external.top()->refs() == 0 )
                            delete _external.top();
                        
                        _external.pop();
                        continue;
                    }

                    _buffer = input->rdbuf();
                }

                if( ! _buffer || _buffer->in_avail() <= 0 )
                    break;

                while( ! _current && _buffer->in_avail() > 0 )
                {
                    c = _buffer->sbumpc();

                    (this->*_parse)(c); // might change _buffer

                    // TODO: this does not work for external input sources
                    if(c == '\n')
                    {
                        ++_line;
                    }
                }

                if( _current )
                {
                    if( _docType.isDefined() )
                    {
                        if( ! _dtdValidator.validate(*_current) )
                            throw SyntaxError("validation failed", _line);
                    }
                    
                    break;
                }
            }

            return _current != 0;
        }

    private:
        std::basic_streambuf<Char>* _textBuffer;
        std::basic_streambuf<Char>* _tbuffer;
        std::basic_streambuf<Char>* _buffer;
        std::stack<InputSource*> _external;
        int _flags;
        
        typedef void (XmlReaderImpl::*ParseFunc)(int);
        ParseFunc _parse;
        Node* _current;

        Pt::String _version;
        Pt::String _encoding;
        bool _standalone;
        std::size_t _depth;
        std::size_t _line;

        NamespaceContext _nsctx;
        EntityMapping _entities;
        Entity* _entity;
        String _token;
        Attribute _attr;

        DocTypeDefinition _dtd;
        DocType _docType;
        ContentModelBuilder _cmBuilder;
        ElementDeclaration* _elemDecl;
        DtdValidator _dtdValidator;

        // TODO: some sort of union?
        ProcessingInstruction _procInstr;
        StartElement _startElem;
        EntityReference _entityRef;
        EndElement _endElem;
        Characters _chars;
        EndDocument _endDoc;
};


XmlReader::XmlReader(std::istream& is, int flags)
: _impl(0)
{
    _impl = new XmlReaderImpl(is, flags);
}


XmlReader::XmlReader(std::basic_istream<Char>& is, int flags)
: _impl(0)
{
    _impl = new XmlReaderImpl(is, flags);
}


XmlReader::~XmlReader()
{
    delete _impl;
}


void XmlReader::attach(std::basic_istream<Char>& is, int flags)
{
    _impl->attach(is, flags);
}


void XmlReader::attach(std::istream& is, int flags)
{
    _impl->attach(is, flags);
}


void XmlReader::addInputSource(InputSource* in)
{
    throw std::logic_error("XmlReader::addInputSource not implemented");
    //_impl->addInputSource(in);
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


const DocType& XmlReader::docType() const
{
    return _impl->docType();
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
