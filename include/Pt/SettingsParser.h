/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_SettingsParser_h
#define Pt_SettingsParser_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/SerializationInfo.h>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cctype>

namespace Pt {

class SettingsParser
{
    public:
        class State
        {
            public:
                virtual State* onChar(Pt::Char c, SettingsParser& parser)
                {
                    if( c == std::char_traits<Pt::Char>::to_char_type( std::char_traits<Pt::Char>::eof() ) )
                    {
                            return this->onEof(c, parser);
                    }

                    switch( c.value() )
                    {
                            case '\n':
                            case ' ':
                            case '\t':
                            case '\r':
                                return this->onSpace(c, parser);

                            case '"':
                                return this->onQoute(c, parser);

                            case ',':
                                return this->onComma(c, parser);

                            case '=':
                                return this->onEqual(c, parser);

                            case '#':
                            case ';':
                                return this->onHash(c, parser);

                            case '{':
                                return this->onOpenCurlyBrace(c, parser);

                            case '}':
                                return this->onCloseCurlyBrace(c, parser);

                            case '(':
                                return this->onOpenBrace(c, parser);

                            case ')':
                                return this->onCloseBrace(c, parser);

                            case '[':
                                return this->onOpenSquareBrace(c, parser);

                            case ']':
                                return this->onCloseSquareBrace(c, parser);

                            default:
                                return this->onAlpha(c, parser);
                    }

                    throw std::runtime_error( "Unexpected token" );
                    return 0;
                }

                virtual ~State()
                {}

            private:
                virtual State* onSpace(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error space");
                    return this;
                }

                virtual State* onQoute(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error \"");
                    return this;
                }

                virtual State* onComma(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error ,");
                    return this;
                }

                virtual State* onEqual(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error =");
                    return this;
                }

                virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error {");
                    return this;
                }

                virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error }");
                    return this;
                }

                virtual State* onOpenBrace(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error (");
                    return this;
                }

                virtual State* onCloseBrace(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error )");
                    return this;
                }

                virtual State* onOpenSquareBrace(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error [");
                    return this;
                }

                virtual State* onCloseSquareBrace(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error ]");
                    return this;
                }

                virtual State* onHash(Pt::Char c, SettingsParser& parser)
                {
                    parser.beginComment(); // save current state
                    return &onComment;
                }

                virtual State* onAlpha(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error alpha");
                    return this;
                }

                virtual State* onEof(Pt::Char c, SettingsParser&)
                {
                    throw std::runtime_error("parse error EOF");
                    return this;
                }
        };


        static class OnComment : public State
        {
            public:
                State* onChar(Pt::Char c, SettingsParser& parser)
                {
                    if( c == '\n' )
                    {
                        // restore state before comment
                        return parser.endComment();
                    }

                    return this;
                }
        } onComment;


        static class BeginStatement : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }

            virtual State* onQoute(Pt::Char c, SettingsParser& parser)
            {
                if(parser.depth() == 0)
                    throw std::runtime_error("unexpected ','");

                return &onQoutedValue;
            }

            virtual State* onOpenSquareBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.beginSection();
                return &onSection;
            }

            virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.enterMember();
                return &onCurly;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return &beginType;
            }

            virtual State* onEof(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }
        } beginStatement;


        static class OnSection : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }

            virtual State* onCloseSquareBrace(Pt::Char c, SettingsParser& parser)
            {
                return &beginStatement;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildSection(c);
                return this;
            }
        } onSection;


        static class BeginType : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return &afterName;
            }

            virtual State* onEqual(Pt::Char c, SettingsParser& parser)
            {
                if(parser.depth() == 0)
                    parser.enterMember();
                else
                    parser.pushName();

                return &SettingsParser::onEqual;
            }

            virtual State* onComma(Pt::Char c, SettingsParser& parser)
            {
                parser.pushValue();
                parser.leaveMember();
                parser.enterMember();
                return &beginStatement;
            }

            virtual State* onOpenBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.pushTypeName();
                return &beginTypedValue;
            }

            virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                if(parser.depth() == 0)
                    throw std::runtime_error("expected '=' before '{'");

                parser.pushTypeName();
                parser.enterMember();
                return &onCurly;
            }

            virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.pushValue();
                parser.leaveMember();
                parser.leaveMember();
                return &onCloseCurly;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }
        } beginType;


        static class AfterName : public BeginType
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                throw std::runtime_error("parse error after name");
                return this;
            }
        } afterName;


        static class OnEqual : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }

            virtual State* onQoute(Pt::Char c, SettingsParser& parser)
            {
                return &onQoutedValue;
            }

            virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.enterMember();
                return &onCurly;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return &onRValue;
            }
        } onEqual;


        static class OnQoutedValue : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onQoute(Pt::Char c, SettingsParser& parser)
            {
                parser.pushValue();
                return &afterQoutedValue;
            }

            virtual State* onComma(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onEqual(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onHash(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onOpenBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onCloseBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onOpenSquareBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onCloseSquareBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }
        } onQoutedValue;


        static class AfterValue : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }

            virtual State* onComma(Pt::Char c, SettingsParser& parser)
            {
                parser.leaveMember();
                parser.enterMember();
                return &beginStatement;
            }

            virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.leaveMember();
                parser.leaveMember();
                return &onCloseCurly;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.leaveMember();
                parser.buildToken(c);
                return &beginType;
            }

            virtual State* onEof(Pt::Char c, SettingsParser& parser)
            {
                if(parser.depth() > 1)
                    throw std::runtime_error("unexpected EOF");

                return this;
            }
        } afterValue;


        static class AfterQoutedValue : public AfterValue
        {
            virtual State* onQoute(Pt::Char c, SettingsParser& parser)
            {
                /// TODO: multi-line strings
                return this;
            }

        } afterQoutedValue ;


        static class OnRValue : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return &afterRValue;
            }

            virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.pushTypeName();
                parser.enterMember();
                return &onCurly;
            }

            virtual State* onOpenBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.pushTypeName();
                return &beginTypedValue;
            }

            virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.pushValue();
                parser.leaveMember();
                parser.leaveMember();
                return &onCloseCurly;
            }

            virtual State* onComma(Pt::Char c, SettingsParser& parser)
            {
                parser.pushValue();
                parser.leaveMember();
                parser.enterMember();
                return &beginStatement;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }

            virtual State* onEof(Pt::Char c, SettingsParser& parser)
            {
                parser.pushValue();
                parser.leaveMember();
                return &beginStatement;
            }
        } onRValue;


        static class AfterRValue : public OnRValue
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.pushValue();
                parser.leaveMember();
                parser.buildToken(c);
                return &beginType;
            }
        } afterRValue;


        static class OnCurly : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }

            virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.enterMember();
                return &onCurly;
            }

            virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.leaveMember();
                return &onCloseCurly;
            }

            virtual State* onQoute(Pt::Char c, SettingsParser& parser)
            {
                return &onQoutedValue;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return &beginType;
            }
        } onCurly;


        static class OnCloseCurly : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }

            virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.leaveMember();
                return this;
            }

            virtual State* onComma(Pt::Char c, SettingsParser& parser)
            {
                if(parser.depth() == 0)
                {
                    throw std::runtime_error("comma outside braces");
                }

                parser.enterMember();
                return &beginStatement;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return &beginType;
            }

            virtual State* onEof(Pt::Char c, SettingsParser& parser)
            {
                if(parser.depth() != 0)
                    throw std::runtime_error("missing closing curly brace");

                return this;
            }
        } onCloseCurly;


        static class BeginTypedValue : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return this;
            }

            virtual State* onQoute(Pt::Char c, SettingsParser& parser)
            {
                return &onQoutedTypedValue;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return &onTypedValue;
            }
        } beginTypedValue;


        static class OnTypedValue : public State
        {
            virtual State* onSpace(Pt::Char c, SettingsParser& parser)
            {
                return &endTypedValue;
            }

            virtual State* onCloseBrace(Pt::Char c, SettingsParser& parser)
            {
                parser.pushValue();
                return &afterValue;
            }

            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                parser.buildToken(c);
                return this;
            }
        } onTypedValue;


        static class OnQoutedTypedValue : public OnQoutedValue
        {
            virtual State* onQoute(Pt::Char c, SettingsParser& parser)
            {
                return &endTypedValue;
            }
        } onQoutedTypedValue;


        static class EndTypedValue : public OnTypedValue
        {
            virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
            {
                throw std::runtime_error("parse error at end of value");
                return this;
            }
        } endTypedValue;

    public:
        SettingsParser(std::basic_istream<Pt::Char>& is);

        void parse(SerializationInfo& si);

    protected:
        void buildToken(Pt::Char c)
        { _token += c; }

        void beginSection()
        { _section.clear(); }

        void buildSection(Pt::Char c)
        { _section += c; }

        size_t depth() const
        { return _depth; }

        void enterMember();

        void leaveMember();

        void pushValue();

        void pushTypeName();

        void pushName();

        void beginComment()
        { _beforeComment = state; }

        State* endComment() const
        { return _beforeComment; }

    private:
        State* state;

        State* _beforeComment;

        SerializationInfo* _current;

        std::basic_istream<Pt::Char>* _is;

        size_t _line;

        size_t _depth;

        bool _isDotted;

        Pt::String _token;

        Pt::String _section;
};

} // namespace Pt

#endif
