/*
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
#include "Pt/Xml/XmlReader.h"
#include <Pt/Xml/EndDocument.h>
#include "Pt/Xml/EntityResolver.h"
#include <Pt/Xml/XmlDeclaration.h>
#include <Pt/Xml/DocTypeDeclaration.h>
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/ParseError.h"
#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>
#include <iostream>

#define PT_XML_LOG
// std::cout << PT_SOURCEINFO.func()  << c.narrow('_') << std::endl;

namespace Pt {

namespace Xml {

struct XmlReaderImpl
{
    struct State
    {
        virtual ~State()
        {}

        State* onChar(Pt::Char c, XmlReaderImpl& reader)
        {
            //std::cerr << "onChar: " << c.narrow('_') << std::endl;

            if( c == std::char_traits<Pt::Char>::to_char_type( std::char_traits<Pt::Char>::eof() ) )
            {
                return this->onEof(c, reader);
            }

            switch( c.value() )
            {
                    case '\n':
                    case ' ':
                    case '\t':
                    case '\r':
                        return this->onSpace(c, reader);

                    case '<':
                        return this->onOpenBracket(c, reader);

                    case '>':
                        return this->onCloseBracket(c, reader);

                    case ':':
                        return this->onColon(c, reader);

                    case '/':
                        return this->onSlash(c, reader);

                    case '=':
                        return this->onEqual(c, reader);

                    case '"':
                    case '\'':
                        return this->onQoute(c, reader);

                    case '!':
                        return this->onExclam(c, reader);

                    case '?':
                        return this->onQuest(c, reader);

                    default:
                        return this->onAlpha(c, reader);
            }

            this->syntaxError(reader.line());
            return 0;
        }

        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError( reader.line() );
            return this;
        }

        virtual State* onOpenBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        virtual State* onColon(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        virtual State* onSlash(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        virtual State* onExclam(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        virtual State* onQuest(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        virtual State* onEof(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            this->syntaxError(reader.line());
            return this;
        }

        void syntaxError(unsigned line)
        {
            throw ParseError("syntax error", line);
        }
    };


    struct OnCData : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onOpenBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            const String& content = reader._chars.content();
            unsigned len = content.length();

            if( len > 2 && content[len-2] == ']' &&
                content[len-2] == ']')
            {
                reader._chars.content().resize(len-2);
                return AfterTag::instance();
            }

            reader._chars.content() += c;
            return this;
        }

        virtual State* onColon(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onSlash(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onExclam(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onQuest(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        static State* instance()
        {
            static OnCData _state;
            return &_state;
        }
    };


    struct BeforeCData : public State
    {
        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            String& token = reader._token;
            token += c;

            if(token.length() < 7)
                return this;

            if(token == L"[CDATA[")
            {
                token.clear();
                return OnCData::instance();
            }

            this->syntaxError( reader.line() );
            return this;
        }

        static State* instance()
        {
            static BeforeCData _state;
            return &_state;
        }
    };


    struct OnEntityReference : public State
    {
        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            if(c == ';')
            {
                reader.resolveEntity(reader._token);
                reader._chars.content() += reader._token;
                reader._token.clear();
                return OnCharacters::instance();
            }

            reader._token += c;
            return this;
        }

        static State* instance()
        {
            static OnEntityReference _state;
            return &_state;
        }
    };


    struct OnCharacters : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onColon(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.content() += c;
            return this;
        }

        virtual State* onOpenBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnTag::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            if(c == '&')
            {
                reader._token.clear();
                return OnEntityReference::instance();
            }

            reader._chars.content() += c;
            return this;
        }

        static State* instance()
        {
            static OnCharacters _state;
            return &_state;
        }
    };


    struct AfterEndElementName : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.clear();
            reader._current = &(reader._endElem);
            reader._depth--;

            if(reader.depth() == 0)
                return OnEpilog::instance();

            return AfterTag::instance();
        }

        static State* instance()
        {
            static AfterEndElementName _state;
            return &_state;
        }
    };


    struct OnEndElementName : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return AfterEndElementName::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._endElem.name() += c;
            return this;
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.clear();
            reader._current = &(reader._endElem);
            reader._depth--;

            if(reader.depth() == 0)
                return OnEpilog::instance();

            return AfterTag::instance();
        }

        static State* instance()
        {
            static OnEndElementName _state;
            return &_state;
        }
    };


    struct OnEndElement : public State
    {
        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._endElem.name() += c;
            return OnEndElementName::instance();
        }

        static State* instance()
        {
            static OnEndElement _state;
            return &_state;
        }
    };


    struct OnEmptyElement : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._endElem.name() = reader._startElem.name();
            reader._current = &(reader._endElem);
            reader._depth--;

            if(reader.depth() == 0)
                return OnEpilog::instance();

            return AfterTag::instance();
        }

        static State* instance()
        {
            static OnEmptyElement _state;
            return &_state;
        }
    };


    struct OnAttributeValue : public State
    {
        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._startElem.addAttribute(reader._attr);
            return BeforeAttribute::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._attr.value() += c;;
            return this;
        }

        static State* instance()
        {
            static OnAttributeValue _state;
            return &_state;
        }
    };


    struct BeforeAttributeValue : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnAttributeValue::instance();
        }

        static State* instance()
        {
            static BeforeAttributeValue _state;
            return &_state;
        }
    };


    struct AfterAttributeName : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return BeforeAttributeValue::instance();
        }

        static State* instance()
        {
            static AfterAttributeName _state;
            return &_state;
        }
    };


    struct OnAttributeName : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return AfterAttributeName::instance();
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return BeforeAttributeValue::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._attr.name() += c;
            return this;
        }

        static State* instance()
        {
            static OnAttributeName _state;
            return &_state;
        }
    };


    struct BeforeAttribute : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onSlash(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._current = &(reader._startElem);
            reader._depth++;
            return OnEmptyElement::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._attr.clear();
            reader._attr.name() += c;
            return OnAttributeName::instance();
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._current = &(reader._startElem);
            reader._depth++;
            return AfterTag::instance();
        }

        static State* instance()
        {
            static BeforeAttribute _state;
            return &_state;
        }
    };


    struct OnStartElement : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return BeforeAttribute::instance();
        }

        virtual State* onSlash(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.clear();
            reader._current = &(reader._startElem);
            reader._depth++;
            return OnEmptyElement::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._startElem.name() += c;
            return this;
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._chars.clear();
            reader._current = &(reader._startElem);
            reader._depth++;
            return AfterTag::instance();
        }

        static State* instance()
        {
            static OnStartElement _state;
            return &_state;
        }
    };


    struct OnCommentEnd : public State
    {
        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            if(reader.depth() == 0)
                return OnProlog::instance();

            return AfterTag::instance();
        }

        static State* instance()
        {
            static OnCommentEnd _state;
            return &_state;
        }
    };


    struct AfterComment : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnComment::instance();
        }

        virtual State* onColon(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnComment::instance();
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnComment::instance();
        }

        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnComment::instance();
        }

        virtual State* onExclam(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnComment::instance();
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnComment::instance();
        }

        virtual State* onOpenBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnComment::instance();
        }

        virtual State* onSlash(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnComment::instance();
        }

        virtual State* onQuest(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnComment::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            if(c == '-')
                return OnCommentEnd::instance();

            return OnComment::instance();
        }

        static State* instance()
        {
            static AfterComment _state;
            return &_state;
        }
    };


    struct OnComment : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onColon(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onExclam(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onOpenBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onSlash(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onQuest(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return this;
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            if(c == '-')
                return AfterComment::instance();

            return this;
        }

        static State* instance()
        {
            static OnComment _state;
            return &_state;
        }
    };


    struct BeforeComment: public State
    {
        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            if(c == '-')
                return OnComment::instance();

            this->syntaxError(reader.line()); // TODO DOCTYPE
            return this;
        }

        static State* instance()
        {
            static BeforeComment _state;
            return &_state;
        }
    };


    struct AfterTag : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            if(reader.depth() == 0)
                return OnProlog::instance();

            reader._chars.content() += c;
            return OnCharacters::instance();
        }

        virtual State* onOpenBracket(Pt::Char c, XmlReaderImpl& reader)
        {   PT_XML_LOG
            return OnTag::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            if(c == '&')
            {
                reader._token.clear();
                return OnEntityReference::instance();
            }

            reader._chars.content() += c;
            return OnCharacters::instance();
        }

        virtual State* onEof(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            if(reader.depth() > 0)
                this->syntaxError( reader.line() );

            reader._current = &( reader._endDoc );
            return this;
        }

        static State* instance()
        {
            static AfterTag _state;
            return &_state;
        }
    };


    struct OnDocType : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._docType.content() += c;
            return this;
        }

        virtual State* onQuest(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._docType.content() += c;
            return this;
        }

        virtual State* onColon(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._docType.content() += c;
            return this;
        }

        virtual State* onSlash(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._docType.content() += c;
            return this;
        }

        virtual State* onExclam(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._docType.content() += c;
            return this;
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._docType.content() += c;
            return this;
        }

        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._docType.content() += c;
            return this;
        }

        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._current = &(reader._docType);
            return OnProlog::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._docType.content() += c;
            return this;
        }

        static State* instance()
        {
            static OnDocType _state;
            return &_state;
        }
    };


    struct BeforeDocType : public State
    {
        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            String& token = reader._docType.content();
            token += c;

            if(token.length() < 7)
                return this;

            if(token == L"DOCTYPE")
            {
                return OnDocType::instance();
            }

            token.clear();
            this->syntaxError( reader.line() );
            return this;
        }

        static State* instance()
        {
            static BeforeDocType _state;
            return &_state;
        }
    };


    struct OnTagExclam : public State
    {
        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG

            if(c == '-')
                return BeforeComment::instance();

            if(c == '[' && reader.depth() > 0)
            {
                reader._token.clear();
                reader._token += c;
                return BeforeCData::instance();
            }

            if(c == 'D' && reader.depth() == 0)
            {
                reader._docType.clear();
                reader._docType.content() += c;
                return BeforeDocType::instance();
            }

            this->syntaxError(reader.line());
            return this;
        }

        static State* instance()
        {
            static OnTagExclam _state;
            return &_state;
        }
    };


    struct OnTag : public State
    {
        virtual State* onQuest(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._procInstr.clear();
            return OnProcessingInstruction::instance();
        }

        virtual State* onExclam(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnTagExclam::instance();
        }

        virtual State* onSlash(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            if( reader._chars.content().length() )
            {
                reader._current = &(reader._chars);
            }

            reader._endElem.clear();
            return OnEndElement::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            if( reader._chars.content().length() )
            {
                reader._current = &(reader._chars);
            }

            reader._startElem.clear();
            reader._startElem.name() += c;
            return OnStartElement::instance();
        }

        static State* instance()
        {
            static OnTag _state;
            return &_state;
        }
    };


    struct OnEpilog : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {   PT_XML_LOG
            return this;
        }

        virtual State* onOpenBracket(Pt::Char c, XmlReaderImpl& reader)
        {   PT_XML_LOG
            return OnTag::instance();
        }

        virtual State* onEof(Pt::Char c, XmlReaderImpl& reader)
        {   PT_XML_LOG
            reader._current = &( reader._endDoc );
            return this;
        }

        static State* instance()
        {
            static OnEpilog _state;
            return &_state;
        }
    };


    struct OnProlog : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {   PT_XML_LOG
            return this;
        }

        virtual State* onOpenBracket(Pt::Char c, XmlReaderImpl& reader)
        {   PT_XML_LOG
            return OnTag::instance();
        }

        virtual State* onEof(Pt::Char c, XmlReaderImpl& reader)
        {   PT_XML_LOG
            reader._current = &( reader._endDoc );
            return this;
        }

        static State* instance()
        {
            static OnProlog _state;
            return &_state;
        }
    };


    struct OnProcessingInstructionEnd : public State
    {
        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._current = &(reader._procInstr);
            return AfterTag::instance();
        }

        static State* instance()
        {
            static OnProcessingInstructionEnd _state;
            return &_state;
        }
    };


    struct OnProcessingInstructionData : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._procInstr.data() += c;
            return this;
        }

        virtual State* onColon(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._procInstr.data() += c;
            return this;
        }

        virtual State* onSlash(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._procInstr.data() += c;
            return this;
        }

        virtual State* onQuest(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnProcessingInstructionEnd::instance();
        }

        virtual State* onExclam(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._procInstr.data() += c;
            return this;
        }

        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._procInstr.data() += c;
            return this;
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._procInstr.data() += c;
            return this;
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._procInstr.data() += c;
            return this;
        }

        static State* instance()
        {
            static OnProcessingInstructionData _state;
            return &_state;
        }
    };


    struct OnProcessingInstruction : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            return OnProcessingInstructionData::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._procInstr.target() += c;
            return this;
        }

        static State* instance()
        {
            static OnProcessingInstruction _state;
            return &_state;
        }
    };


    struct OnXmlDeclValue : public State
    {
        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            static const Pt::Char version[] = { 'v', 'e', 'r', 's', 'i', 'o', 'n', 0};
            static const Pt::Char encoding[] = { 'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g',  0};
            static const Pt::Char standalone[] = { 's', 't', 'a', 'n', 'd', 'a', 'l', 'o', 'n','e', 0};
            static const Pt::Char trueval[] = { 't', 'r', 'u', 'e', 0};

            if(reader._attr.name() == version)
            {
                reader._version = reader._attr.value();
            }
            else if(reader._attr.name() == encoding)
            {
                reader._encoding = reader._attr.value();
            }
            else if(reader._attr.name() == standalone)
            {
                if(reader._attr.value() == trueval)
                    reader._standalone = true;
            }

            return OnXmlDeclBeforeAttr::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            reader._attr.value() += c;;
            return this;
        }

        static State* instance()
        {
            static OnXmlDeclValue _state;
            return &_state;
        }
    };


    struct OnXmlDeclBeforeValue : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            return this;
        }

        virtual State* onQoute(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnXmlDeclValue::instance();
        }

        static State* instance()
        {
            static OnXmlDeclBeforeValue _state;
            return &_state;
        }
    };


    struct OnXmlDeclAfterName : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            return this;
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnXmlDeclBeforeValue::instance();
        }

        static State* instance()
        {
            static OnXmlDeclAfterName _state;
            return &_state;
        }
    };


    struct OnXmlDeclAttr : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnXmlDeclAfterName::instance();
        }

        virtual State* onEqual(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnXmlDeclBeforeValue::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            reader._attr.name() += c;
            return this;
        }

        static State* instance()
        {
            static OnXmlDeclAttr _state;
            return &_state;
        }
    };


    struct OnXmlDeclEnd : public State
    {
        virtual State* onCloseBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnProlog::instance();
        }

        static State* instance()
        {
            static OnXmlDeclEnd _state;
            return &_state;
        }
    };


    struct OnXmlDeclBeforeAttr : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            return this;
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            reader._attr.clear();
            reader._attr.name() += c;
            return OnXmlDeclAttr::instance();
        }

        virtual State* onQuest(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnXmlDeclEnd::instance();
        }

        static State* instance()
        {
            static OnXmlDeclBeforeAttr _state;
            return &_state;
        }
    };


    struct OnXmlDeclName : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            static const Pt::Char xml[] = { 'x', 'm', 'l', 0};

            if( reader._procInstr.target() == xml )
                return OnXmlDeclBeforeAttr::instance();

            return OnProcessingInstructionData::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            reader._procInstr.target() += c;
            return this;
        }

        static State* instance()
        {
            static OnXmlDeclName _state;
            return &_state;
        }
    };


    struct OnXmlDeclQMark : public State
    {
        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            reader._procInstr.clear();
            reader._procInstr.target() += c;
            return OnXmlDeclName::instance();
        }

        static State* instance()
        {
            static OnXmlDeclQMark _state;
            return &_state;
        }
    };


    struct OnXmlDecl : public State
    {
        virtual State* onQuest(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnXmlDeclQMark::instance();
        }

        virtual State* onExclam(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnTagExclam::instance();
        }

        virtual State* onAlpha(Pt::Char c, XmlReaderImpl& reader)
        {
            PT_XML_LOG
            reader._startElem.clear();
            reader._startElem.name() += c;
            return OnStartElement::instance();
        }

        static State* instance()
        {
            static OnXmlDecl _state;
            return &_state;
        }
    };


    struct OnDocumentBegin : public State
    {
        virtual State* onSpace(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnProlog::instance();
        }

        virtual State* onOpenBracket(Pt::Char c, XmlReaderImpl& reader)
        {
            return OnXmlDecl::instance();
        }

        static State* instance()
        {
            static OnDocumentBegin _state;
            return &_state;
        }
    };


    XmlReaderImpl(std::basic_istream<Char>& is, int flags)
    : _textBuffer( is.rdbuf() )
    , _buffer(0)
    , _flags(flags)
    , _standalone(true)
    , _depth(0)
    , _line(1)
    , _state(0)
    , _current(0)
    {
        _state = XmlReaderImpl::OnDocumentBegin::instance();
    }

    XmlReaderImpl(std::istream& is, int flags)
    : _textBuffer(0)
    , _buffer(0)
    , _flags(flags)
    , _standalone(true)
    , _depth(0)
    , _line(1)
    , _state(0)
    , _current(0)
    {
        _state = XmlReaderImpl::OnDocumentBegin::instance();
        _buffer = new TextBuffer( &is, new Pt::Utf8Codec() );
        _textBuffer = _buffer;
    }

    ~XmlReaderImpl()
    {
        delete _buffer;
    }

    const Pt::String& version() const
    { return _version; }

    const Pt::String& encoding() const
    { return _encoding; }

    bool standalone() const
    { return _standalone; }

    EntityResolver& entityResolver()
    { return _resolver; }

    size_t depth() const
    {
        return _depth;
    }

    std::size_t line() const
    {
        return _line;
    }

    const Node& get()
    {
        if( ! _current )
        {
            this->next();
        }

        return *_current;
    }

    const Node& next()
    {
        const Pt::Char eof = std::char_traits<char>::eof();

        _current = 0;
        Pt::Char ch = 0;
        do
        {
            ch = _textBuffer->sbumpc();
            _state = _state->onChar(ch, *this);

            if(ch == '\n')
            {
                ++_line;
            }
        }
        while ( !_current && ch != eof);

        return *_current;
    }

    bool advance()
    {
        const Pt::Char eof = std::char_traits<char>::eof();

        _current = 0;
        Pt::Char ch = 0;
        while( ! _current && _textBuffer->in_avail() > 0 )
        {
            ch = _textBuffer->sbumpc();
            _state = _state->onChar(ch, *this);

            if(ch == '\n')
            {
                ++_line;
            }
        }

        return _current != 0;
    }

    void resolveEntity(String& str)
    {
        str = _resolver.resolveEntity( str );
    }

    std::basic_streambuf<Char>* _textBuffer;
    std::basic_streambuf<Char>* _buffer;
    int _flags;
    EntityResolver _resolver;

    Pt::String _version;
    Pt::String _encoding;
    bool _standalone;
    size_t _depth;
    std::size_t _line;

    State* _state;
    Node* _current;
    String _token;
    DocTypeDeclaration _docType;
    ProcessingInstruction _procInstr;
    StartElement _startElem;
    EndElement _endElem;
    Characters _chars;
    Attribute _attr;
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


const Pt::String& XmlReader::documentVersion() const
{
    return _impl->version();
}


const Pt::String& XmlReader::documentEncoding() const
{
    return _impl->encoding();
}


bool XmlReader::standaloneDocument() const
{
    return _impl->standalone();
}


EntityResolver& XmlReader::entityResolver()
{
    return _impl->entityResolver();
}


const EntityResolver& XmlReader::entityResolver() const
{
    return _impl->entityResolver();
}


size_t XmlReader::depth() const
{
    return _impl->depth();
}


std::size_t XmlReader::line() const
{
    return _impl->line();
}


const Node& XmlReader::get()
{
    return _impl->get();
}


const Node& XmlReader::next()
{
    return _impl->next();
}


bool XmlReader::advance()
{
    return _impl->advance();
}


const StartElement& XmlReader::nextElement()
{
    bool found = false;
    while( !found )
    {
        const Node& node = this->next();
        switch( node.type() )
        {
            case Node::EndDocument:
                throw std::logic_error("End of document" + PT_SOURCEINFO);

            case Node::StartElement:
                found = true;
                break;

            default:
                break;
        }

    }

    return static_cast<const StartElement&>( this->get() );
}


const Node& XmlReader::nextTag()
{
    bool found = false;
    while( !found )
    {
        const Node& node = this->next();
        switch( node.type() )
        {
            case Node::EndDocument:
                throw std::logic_error("End of document" + PT_SOURCEINFO);

            case Node::StartElement:
            case Node::EndElement:
                found = true;
                break;

            default:
                break;
        }

    }

    return this->get();
}

} // namespace Xml

} // namespace Pt
