/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                      *
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

                            default:
                                return this->onAlpha(c, parser);
                    }

                    throw std::runtime_error( "Unexpected token" );
                    return 0;
                }

                virtual ~State()
                {}

            protected:
                virtual State* onSpace(Pt::Char c, SettingsParser&) = 0;

                virtual State* onQoute(Pt::Char c, SettingsParser&) = 0;

                virtual State* onComma(Pt::Char c, SettingsParser&) = 0;

                virtual State* onEqual(Pt::Char c, SettingsParser&) = 0;

                virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser&) = 0;

                virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser&) = 0;

                virtual State* onOpenBrace(Pt::Char c, SettingsParser&) = 0;

                virtual State* onCloseBrace(Pt::Char c, SettingsParser&) = 0;

                virtual State* onHash(Pt::Char c, SettingsParser&) = 0;

                virtual State* onAlpha(Pt::Char c, SettingsParser&) = 0;

                virtual State* onEof(Pt::Char c, SettingsParser&) = 0;
        };

        static class BeginStatement : public State
        {
            protected:
                virtual State* onSpace(Pt::Char c, SettingsParser& parser)
                { return this; }

                virtual State* onQoute(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onComma(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onEqual(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onOpenBrace(Pt::Char c, SettingsParser& parser)
                {
                    return &onSection;
                }

                virtual State* onCloseBrace(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onHash(Pt::Char c, SettingsParser& parser)
                {
                    return &onComment;
                }


                virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
                {
                    parser.buildName(c);
                    //return &onName;
                    return this;
                }

                virtual State* onEof(Pt::Char c, SettingsParser& parser)
                {
                    return this;
                }
        } beginStatement;

        static class OnComment : public State
        {
            public:
                virtual State* onChar(Pt::Char c, SettingsParser& parser)
                {
                    if(c == '\n')
                    {
                        return &beginStatement;
                    }

                    return this;
                }

            protected:
                virtual State* onSpace(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onQoute(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onComma(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onEqual(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onOpenBrace(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onCloseBrace(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onHash(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onAlpha(Pt::Char c, SettingsParser& parser) { return this; }
                virtual State* onEof(Pt::Char c, SettingsParser& parser) { return this; }
        } onComment;

        static class OnSection : public State
        {
            protected:
                virtual State* onSpace(Pt::Char c, SettingsParser& parser)
                { return this; }

                virtual State* onQoute(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onComma(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onEqual(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onOpenCurlyBrace(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onCloseCurlyBrace(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onOpenBrace(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onCloseBrace(Pt::Char c, SettingsParser& parser)
                {
                    return &beginStatement;
                }

                virtual State* onHash(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }

                virtual State* onAlpha(Pt::Char c, SettingsParser& parser)
                {
                    parser.buildSection(c);
                    return this;
                }

                virtual State* onEof(Pt::Char c, SettingsParser& parser)
                {
                    throw std::runtime_error("parse error");
                    return this;
                }
        } onSection;

    public:
        SettingsParser(std::basic_istream<Pt::Char>& is);

        void parse(SerializationInfo& si);

        void buildName(Pt::Char c)
        { _name += c; }

        void buildSection(Pt::Char c)
        { _section += c; }

    private:
        //! State of the parser.
        State* state;

        std::basic_istream<Pt::Char>* _is;

        size_t _line;

        Pt::String _name;

        Pt::String _section;
};

} // namespace Pt

#endif
