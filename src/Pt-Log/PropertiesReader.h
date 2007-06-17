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
#ifndef Pt_PropertiesReader_h
#define Pt_PropertiesReader_h

#include <Pt/Api.h>
#include <Pt/Archive.h>
#include <Pt/String.h>
#include <Pt/Exception.h>
#include <Pt/Unicode.h>
#include <Pt/ArchiveReader.h>
#include <iostream>
#include <sstream>
#include <cassert>


namespace Pt {

class PropertiesReader : public ArchiveReader
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
                ParseContext(Archive& archive)
                : _archive( &archive )
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
                    _archive = &( _archive->addArchive( _name ) );
                    _name.clear();
                    ++_depth;
                }

                void leaveNode()
                {
                    assert(_depth > 0);
                    _archive = _archive->parent();
                    --_depth;
                }

                void addValue()
                {
                    size_t pos  = _name.rfind( Pt::Char(L'.') );

                    if(pos != Pt::String::npos)
                    {
                        Archive* archive = &( _archive->addArchive( _name.substr( 0, pos ) ) );
                        archive->addValue( _name.substr( ++pos ), _value );
                    }
                    else
                    {
                        _archive->addValue(_name, _value);
                    }

                    _value.clear();
                    _name.clear();
                }

            private:
                Archive* _archive;
                unsigned _line;
                unsigned _depth;
                Pt::String _name;
                Pt::String _value;
                Pt::String _section;
        };

    public:
        PropertiesReader(std::basic_istream<Pt::Char>& is)
        : ArchiveReader()
        , _is(&is)
        {
            typedef std::char_traits<Pt::Char> Traits;
            eof = Traits::to_char_type( Traits::eof() );
        }

        ~PropertiesReader()
        {}

        void attach(std::basic_istream<Pt::Char>& is)
        {
            _is = &is;
        }

    protected:
        void _read(Archive& archive)
        {
            ParseContext context(archive);
            _parse = &PropertiesReader::beginStatement;

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
                    _parse = &PropertiesReader::beginSection;
                    context.section().clear();
                    break;

                case '(':
                case '=':
                    throw ParseError( "Expected token before " + ch.narrow(' '), context.line() );

                default:
                    context.reset();
                    context.name() += ch;
                    _parse = &PropertiesReader::parseName;
            }
        }

        void parseName(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Expected \'=\' token", context.line());

            if( Pt::Unicode::isSpace(ch) )
            {
                _parse = &PropertiesReader::beginEqual;
                return;
            }
 
            switch( ch.value() )
            {
                case '=':
                    _parse = &PropertiesReader::finishEqual;
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
                    _parse = &PropertiesReader::finishEqual;
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
                    _parse = &PropertiesReader::parseQuotedValue;
                    break;

                case '{':
                    context.enterNode();
                    _parse = &PropertiesReader::parseArray;
                    break;

                case '(':
                    context.enterNode();
                    _parse = &PropertiesReader::beginStatement;
                    break;

                default:
                    context.value() += ch;
                    _parse = &PropertiesReader::parseValue;
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
                    _parse = &PropertiesReader::beginStatement;
                else
                    _parse = &PropertiesReader::finishValue;
                return;
            }

            switch( ch.value() )
            {
                case ',':
                    context.addValue();
                    _parse = &PropertiesReader::beginStatement;
                    break;

                case ')':
                    context.addValue();
                    context.leaveNode();

                    if( context.depth() == 0 )
                        _parse = &PropertiesReader::beginStatement;
                    else
                        _parse = &PropertiesReader::finishValue;

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

                _parse = &PropertiesReader::beginStatement;
                return;
            }

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            switch( ch.value() )
            {
                case ',':
                    _parse = &PropertiesReader::beginStatement;
                    break;

                case ')':
                    context.leaveNode();

                    if( context.depth() == 0 )
                        _parse = &PropertiesReader::beginStatement;

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
                    _parse = &PropertiesReader::finishQuotedValue;
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
                    _parse = &PropertiesReader::parseQuotedValue;
                    break;

                case ',':
                    context.addValue();
                    _parse = &PropertiesReader::beginStatement;
                    break;

                case ')':
                    context.addValue();
                    context.leaveNode();

                    if( context.depth() == 0 )
                        _parse = &PropertiesReader::beginStatement;

                    break;

                default:
                    if( context.depth() == 0 )
                    {
                        context.addValue();
                        context.reset();
                        context.name() += ch;
                        _parse = &PropertiesReader::parseName;
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
                    _parse = &PropertiesReader::parseQuotedArrayValue;
                    break;

                case '}':
                    _parse = &PropertiesReader::beginStatement;
                    break;

                default:
                    context.value() += ch;
                    _parse = &PropertiesReader::parseArrayValue;
            }
        }

        void parseArrayValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError( "Incomplete array", context.line() );


            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
            {
                _parse = &PropertiesReader::finishArrayValue;
                return;
            }

            switch( ch.value() )
            {
                case ',':
                    context.addValue();
                    _parse = &PropertiesReader::parseArray;
                    break;

                case '}':
                    context.addValue();
                    context.leaveNode();
                    _parse = &PropertiesReader::beginStatement;
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
                    _parse = &PropertiesReader::parseArray;
                    break;

                case '}':
                    context.addValue();
                    context.leaveNode();
                    _parse = &PropertiesReader::beginStatement;
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
                    _parse = &PropertiesReader::finishQuotedArrayValue;
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
                _parse = &PropertiesReader::parseQuotedArrayValue;
                return;
            }

            if( ch == Pt::Char(L',') )
            {
                context.addValue();
                _parse = &PropertiesReader::parseArray;
                return;
            }

            if( ch == Pt::Char(L'}') )
            {
                context.addValue();
                context.leaveNode();
                _parse = &PropertiesReader::beginStatement;
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
            _parse = &PropertiesReader::parseSection;
        }

        void parseSection(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Section not closed", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
            {
                _parse = &PropertiesReader::finishSection;
                return;
            }

            if( ch == Pt::Char(L']') )
            {
                context.reset();
                _parse = &PropertiesReader::beginStatement;
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
                _parse = &PropertiesReader::beginStatement;
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
        typedef void (PropertiesReader::*Parse)(const Pt::Char&, ParseContext&);
        Parse _parse;
};


}

#endif

