/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Drner                       *
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
#ifndef Pt_Settings_h
#define Pt_Settings_h

#include <Pt/Api.h>
#include <Pt/SerializationData.h>
#include <Pt/String.h>
#include <Pt/Exception.h>
#include <Pt/Unicode.h>

#include <iostream>
#include <sstream>
#include <cassert>


namespace Pt {

class Settings : public SerializationData
{
    public:
        Settings()
        {}

        void load(std::basic_istream<Pt::Char>& is)
        {
            //SettingsReader reader(is);
            //reader.read(*this);
        }

        void load(const std::string& path)
        {
            //std::ifstream fs( path.c_str() );
            //Pt::Text::TextIStream ts(fs, new Pt::Text::Utf8Codec);
            //SettingsReader reader(ts);
            //reader.read(*this);
        }

        void save(const std::string& path) const
        {
            //std::ifstream fs( path.c_str() );
            //Pt::Text::TextOStream ts(fs, new Pt::Text::Utf8Codec);
            //SettingsWriter writer(ts);
            //writer.write(*this);
        }

        template <typename T>
        const void get(T& type, const Pt::String& name) const
        {
            const SerializationData* data = this->getData(name);
            if(data == 0)
                return;

            *data >> type;
        }
};


class SettingsReader
{
    public:
        Pt::Char eof;

        class ParseError : public std::logic_error
        {
            public:
                ParseError(const std::string& what, unsigned line)
                : std::logic_error("Line " + ParseError::str(line) + ":" + what)
                {}

                static std::string str(unsigned n)
                {
                    std::stringstream ss;
                    ss << n;
                    return ss.str();
                }
        };

        class ParseContext
        {
            public:
                ParseContext(SerializationData& data)
                : _data( &data )
                , _line(1)
                , _depth(0)
                {}

                void reset()
                {
                    if( _section.empty() )
                        _name.clear();
                    else
                        _name = _section + Pt::Char(L'.');

                    _value.clear();
                }

                unsigned line() const
                { return _line; }

                void endl()
                { ++_line; }

                Pt::String& section()
                { return _section; }

                Pt::String& name()
                { return _name; }

                Pt::String& value()
                { return _value; }

                unsigned depth() const
                {
                    return _depth;
                }

                void enterNode()
                {
                    SerializationData* data = _data->getData( _name );
                    if(data == 0)
                        data = &( _data->addData( _name ) );

                    _data = data;

                    _name.clear();
                    ++_depth;
                }

                void leaveNode()
                {
                    assert(_depth > 0);
                    _data = _data->parent();
                    --_depth;
                }

                void addValue()
                {
                    size_t pos  = _name.rfind( Pt::Char(L'.') );

                    if(pos != Pt::String::npos)
                    {
                        SerializationData* data = _data->getData( _name.substr( 0, pos ) );
                        if(data == 0)
                            data = &( _data->addData( _name.substr( 0, pos ) ) );

                        data->addEntry( _name.substr( ++pos ), _value );
                    }
                    else
                    {
                        _data->addEntry(_name, _value);
                    }

                    _value.clear();
                    _name.clear();
                }

            private:
                SerializationData* _data;
                unsigned _line;
                unsigned _depth;
                Pt::String _name;
                Pt::String _value;
                Pt::String _section;
        };

    public:
        SettingsReader(std::basic_istream<Pt::Char>& is)
        : _is(&is)
        {
            typedef std::char_traits<Pt::Char> Traits;
            eof = Traits::to_char_type( Traits::eof() );
        }

        ~SettingsReader()
        {}

        void attach(std::basic_istream<Pt::Char>& is)
        {
            _is = &is;
        }

        //! @brief Reads content into an archive
        void read(SerializationData& data)
        {
            this->_read(data);
        }

        //! @brief Reads content into an archive
        void read(Settings& settings)
        {
            this->_read( settings );
        }

    protected:
        void _read(SerializationData& data)
        {
            ParseContext context(data);
            _parse = &SettingsReader::beginStatement;

            Pt::String comment;
            Pt::Char ch;
            while( _is->get(ch) )
            {
                if( ch == Pt::Char(L'#') )
                {
                    getline( *_is, comment, Pt::Char(L'\n') );
                    ch = Pt::Char(L'\n');
                }

                if( ch == Pt::Char(L'\n') )
                {
                    context.endl();
                }

                (this->*_parse)(ch, context);
            }

            (this->*_parse)( eof, context );

            assert( context.depth() == 0 );
        }

        void beginStatement(const Pt::Char& ch, ParseContext& context)
        {
            //if( ch == eof || Pt::Unicode::isSpace(ch) )
            //    return;

            switch( ch.value() )
            {
                case '\n':
                case '\r':
                case '\t':
                case ' ':
                case Pt::uint32_t(-1):
                    break;

                case '[':
                    _parse = &SettingsReader::beginSection;
                    context.section().clear();
                    break;

                case '(':
                case '=':
                    throw ParseError( "Expected token before " + ch.narrow(' '), context.line() );

                default:
                    context.reset();
                    context.name() += ch;
                    _parse = &SettingsReader::parseName;
            }
        }

        void parseName(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Expected \'=\' token", context.line());

            if( Pt::Unicode::isSpace(ch) )
            {
                _parse = &SettingsReader::beginEqual;
                return;
            }
 
            switch( ch.value() )
            {
                case '=':
                    _parse = &SettingsReader::finishEqual;
                    break;

                default:
                    context.name() += ch;
            }
        }

        void beginEqual(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Expected \'=\' token", context.line());

            if( Pt::Unicode::isSpace(ch) )
                return;

            switch( ch.value() )
            {
                case '=':
                    _parse = &SettingsReader::finishEqual;
                    break;

                default:
                    throw ParseError("Expected \'=\' token", context.line());
            }
        }

        void finishEqual(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Expected token after \'=\'", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            switch( ch.value() )
            {
                case '=':
                    throw ParseError("Expected token before \'=\'", context.line());

                case '"':
                    _parse = &SettingsReader::parseQuotedValue;
                    break;

                case '{':
                    context.enterNode();
                    _parse = &SettingsReader::parseArray;
                    break;

                case '(':
                    context.enterNode();
                    _parse = &SettingsReader::beginStatement;
                    break;

                default:
                    context.value() += ch;
                    _parse = &SettingsReader::parseValue;
            }
        }

        void parseValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == Pt::Char(L'=') )
                throw ParseError("Invalid token after \'=\'", context.line());

            if( ch == eof || Pt::Unicode::isSpace(ch) ||
                ch == Pt::Char(L'\n') )
            {
                context.addValue();
                if(context.depth() == 0)
                    _parse = &SettingsReader::beginStatement;
                else
                    _parse = &SettingsReader::finishValue;
                return;
            }

            switch( ch.value() )
            {
                case ',':
                    context.addValue();
                    _parse = &SettingsReader::beginStatement;
                    break;

                case ')':
                    context.addValue();
                    context.leaveNode();

                    if( context.depth() == 0 )
                        _parse = &SettingsReader::beginStatement;
                    else
                        _parse = &SettingsReader::finishValue;

                    break;

                default:
                    context.value() += ch;
            }
        }

        void finishValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
            {
                if( context.depth() )
                    throw ParseError( "Expected token before EOF", context.line() );

                _parse = &SettingsReader::beginStatement;
                return;
            }

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            switch( ch.value() )
            {
                case ',':
                    _parse = &SettingsReader::beginStatement;
                    break;

                case ')':
                    context.leaveNode();

                    if( context.depth() == 0 )
                        _parse = &SettingsReader::beginStatement;

                    break;
            }
        }

        void parseQuotedValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof || ch == Pt::Char(L'\n') || ch == Pt::Char(L'\r') )
                throw ParseError("Expected closing \" token", context.line());

            switch( ch.value() )
            {
                case '\\':
                    this->getEscaped(context);
                    break;

                case '"':
                    _parse = &SettingsReader::finishQuotedValue;
                    break;

                default:
                    context.value() += ch;
            }
        }

        void finishQuotedValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
            {
                if(context.depth() > 0)
                    throw ParseError( "Expected token before EOF", context.line() );

                context.addValue();
                return;
            }

            if(Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n')  )
            {
                return;
            }

            switch( ch.value() )
            {
                case '"':
                    _parse = &SettingsReader::parseQuotedValue;
                    break;

                case ',':
                    context.addValue();
                    _parse = &SettingsReader::beginStatement;
                    break;

                case ')':
                    context.addValue();
                    context.leaveNode();

                    if( context.depth() == 0 )
                        _parse = &SettingsReader::beginStatement;

                    break;

                default:
                    if( context.depth() == 0 )
                    {
                        context.addValue();
                        context.reset();
                        context.name() += ch;
                        _parse = &SettingsReader::parseName;
                        return;
                    }
                    throw ParseError( "Expected closing \')\'", context.line() );
            }
        }

        void parseArray(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
            {
                throw ParseError("Incomplete array", context.line());
            }

            if(Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n')  )
                return;

            switch( ch.value() )
            {
                case ',':
                    throw ParseError("Incomplete array", context.line());

                case '"':
                    _parse = &SettingsReader::parseQuotedArrayValue;
                    break;

                case '}':
                    _parse = &SettingsReader::beginStatement;
                    break;

                default:
                    context.value() += ch;
                    _parse = &SettingsReader::parseArrayValue;
            }
        }

        void parseArrayValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError( "Incomplete array", context.line() );


            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
            {
                _parse = &SettingsReader::finishArrayValue;
                return;
            }

            switch( ch.value() )
            {
                case ',':
                    context.addValue();
                    _parse = &SettingsReader::parseArray;
                    break;

                case '}':
                    context.addValue();
                    context.leaveNode();
                    _parse = &SettingsReader::beginStatement;
                    break;

                default:
                    context.value() += ch;
            }
        }

        void finishArrayValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError( "Incomplete array", context.line() );

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            switch( ch.value() )
            {
                case ',':
                    context.addValue();
                    _parse = &SettingsReader::parseArray;
                    break;

                case '}':
                    context.addValue();
                    context.leaveNode();
                    _parse = &SettingsReader::beginStatement;
                    break;
            }
        }

        void parseQuotedArrayValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError( "Reached EOF in array element", context.line() );

            switch( ch.value() )
            {
                case '"' :
                    _parse = &SettingsReader::finishQuotedArrayValue;
                    break;

                case '\\' :
                    this->getEscaped(context);
                    break;

                default:
                    context.value() += ch;
            }
        }

        void finishQuotedArrayValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError( "Incomplete array", context.line() );

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L'"') )
            {
                _parse = &SettingsReader::parseQuotedArrayValue;
                return;
            }

            if( ch == Pt::Char(L',') )
            {
                context.addValue();
                _parse = &SettingsReader::parseArray;
                return;
            }

            if( ch == Pt::Char(L'}') )
            {
                context.addValue();
                context.leaveNode();
                _parse = &SettingsReader::beginStatement;
                return;
            }

            throw ParseError( "Unrecognized token in array", context.line() );
        }

        void beginSection(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Section not closed", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            context.section() += ch;
            _parse = &SettingsReader::parseSection;
        }

        void parseSection(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Section not closed", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
            {
                _parse = &SettingsReader::finishSection;
                return;
            }

            if( ch == Pt::Char(L']') )
            {
                context.reset();
                _parse = &SettingsReader::beginStatement;
                return;
            }

            context.section() += ch;
        }

        void finishSection(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Section not closed", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L']') )
            {
                context.reset();
                _parse = &SettingsReader::beginStatement;
                return;
            }

            throw ParseError("Invalid section name", context.line());
        }

        void getEscaped(ParseContext& context)
        {
            Pt::Char ch;
            if( ! _is->get(ch) )
                throw ParseError("Reached EOF within qoute", context.line() );

            switch( ch.value() )
            {
                case 'n':
                    context.value() += Pt::Char(L'\n');
                    break;

                case 'r':
                    context.value() += Pt::Char(L'\r');
                    break;

                default:
                    throw ParseError("Unknown escaped character", context.line() );
            }
        }

    private:
        std::basic_istream<Pt::Char>* _is;
        typedef void (SettingsReader::*Parse)(const Pt::Char&, ParseContext&);
        Parse _parse;
};


}

#endif

