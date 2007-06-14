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

                bool isTop() const
                {
                    return _archive->parent() == 0;
                }

                void pushNode()
                {
                    //std::cerr << "push node: " << _name.narrow()  << std::endl;
                    _archive = &( _archive->addArchive( _name ) );
                    _name.clear();
                }

                void popNode()
                {
                    _archive = _archive->parent();
                }

                void pushValue()
                {
                    //std::cerr << "push value: " << _name.narrow()  << " - " << _value.narrow() << std::endl;
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
            _parse = &PropertiesReader::parseBeforeName;

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

            assert( context.isTop() );
        }

        void parseBeforeName(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof || Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L'[') )
            {
                _parse = &PropertiesReader::parseSection;
                context.section().clear();
                return;
            }

            if( ch == Pt::Char(L'=') )
                throw ParseError( "expected token before =", context.line() );

            context.reset();

            context.name() += ch;
            _parse = &PropertiesReader::parseName;
        }

        void parseSection(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Section not closed", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            context.section() += ch;
            _parse = &PropertiesReader::parseSectionName;
        }

        void parseSectionName(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Section not closed", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
            {
                _parse = &PropertiesReader::parseSectionEnd;
                return;
            }

            if( ch == Pt::Char(L']') )
            {
                context.reset();
                _parse = &PropertiesReader::parseBeforeName;
                return;
            }

            context.section() += ch;
        }

        void parseSectionEnd(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Section not closed", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L']') )
            {
                context.reset();
                _parse = &PropertiesReader::parseBeforeName;
                return;
            }

            throw ParseError("Invalid section name", context.line());
        }

        void parseName(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Expected \'=\' token", context.line());

            if( ch == Pt::Char(L'=') )
            {
                _parse = &PropertiesReader::parseAfterEqual;
                return;
            }

            if( Pt::Unicode::isSpace(ch) )
            {
                _parse = &PropertiesReader::parseAfterName;
                return;
            }

            context.name() += ch;
        }

        void parseAfterName(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError("Expected \'=\' token", context.line());

            if( Pt::Unicode::isSpace(ch) )
                return;

            if(ch == Pt::Char(L'='))
            {
                _parse = &PropertiesReader::parseAfterEqual;
                return;
            }

            throw ParseError("Expected \'=\' token", context.line());
        }

        void parseAfterEqual(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof || ch == Pt::Char(L'=') )
                throw ParseError("Expected token after \'=\'", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char('"') )
            {
                _parse = &PropertiesReader::parseQoutedValue;
                return;
            }

            if( ch == Pt::Char('{') )
            {
                context.pushNode();
                _parse = &PropertiesReader::parseArray;
                return;
            }

            if( ch == Pt::Char('[') )
            {
                context.pushNode();
                _parse = &PropertiesReader::beginComposed;
                return;
            }

            context.value() += ch;
            _parse = &PropertiesReader::parseValue;
        }

        void parseValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == Pt::Char(L'=') )
                throw ParseError("Invalid token after \'=\'", context.line());

            if( ch == eof || Pt::Unicode::isSpace(ch) ||
                ch == Pt::Char(L'\n') )
            {
                //std::cerr << "Unqouted: " << context.name().narrow() << " " << context.value().narrow() << std::endl;
                context.pushValue();
                _parse = &PropertiesReader::parseBeforeName;
                return;
            }

            context.value() += ch;
        }

        void parseQoutedValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof || ch == Pt::Char(L'\n') || ch == Pt::Char(L'\r') )
            {
                throw ParseError("Expected closing\" token", context.line());
            }

            if( ch == Pt::Char(L'\\') )
            {
                this->getEscaped(context);
                return;
            }

            if( ch == Pt::Char(L'"') )
            {
                _parse = &PropertiesReader::finishQoutedValue;
                return;
            }

            context.value() += ch;
        }

        void finishQoutedValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
            {
                //std::cerr << "Qouted: " << context.name().narrow() << " " << context.value().narrow() << std::endl;
                context.pushValue();
                return;
            }

            if(Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n')  )
                return;

            if( ch == Pt::Char('"') )
            {
                _parse = &PropertiesReader::parseQoutedValue;
                return;
            }

            //std::cerr << "Qouted: " << context.name().narrow() << " " << context.value().narrow() << std::endl;
            context.pushValue();

            _parse = &PropertiesReader::parseBeforeName;
            this->parseBeforeName(ch, context);
        }

        void parseArray(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
            {
                throw ParseError("Incomplete array", context.line());
            }

            if(Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n')  )
                return;

            if( ch == Pt::Char(L',') )
            {
                throw ParseError("Incomplete array", context.line());
            }

            if( ch == Pt::Char(L'"') )
            {
                _parse = &PropertiesReader::parseQoutedArrayValue;
                return;
            }

            if( ch == Pt::Char(L'}') )
            {
                _parse = &PropertiesReader::parseBeforeName;
                return;
            }

            context.value() += ch;
            _parse = &PropertiesReader::parseArrayValue;
        }

        void parseArrayValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError( "Incomplete array", context.line() );

            if( ch == Pt::Char(L',') )
            {
                context.pushValue();
                _parse = &PropertiesReader::parseArray;
                return;
            }

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
            {
                context.pushValue();
                _parse = &PropertiesReader::finishArrayValue;
                return;
            }

            if( ch == Pt::Char(L'}') )
            {
                context.popNode();
                _parse = &PropertiesReader::parseBeforeName;
                return;
            }

            context.value() += ch;
        }

        void finishArrayValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError( "Incomplete array", context.line() );

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L',') )
            {
                _parse = &PropertiesReader::parseArray;
                return;
            }

            if( ch == Pt::Char(L'}') )
            {
                context.popNode();
                _parse = &PropertiesReader::parseBeforeName;
                return;
            }
        }

        void parseQoutedArrayValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError( "Reached EOF in array element", context.line() );

            switch( ch.value() )
            {
                case L'"' :
                    context.pushValue();
                    _parse = &PropertiesReader::finishQoutedArrayValue;
                    break;

                case L'\\' :
                    this->getEscaped(context);
                    break;

                default:
                    context.value() += ch;
            }
        }

        void finishQoutedArrayValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw ParseError( "Incomplete array", context.line() );

            if( ch == Pt::Char(L',') )
            {
                _parse = &PropertiesReader::parseArray;
                return;
            }

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            throw ParseError( "Unrecognized token in array", context.line() );
        }

        void beginComposed(const Pt::Char& ch, ParseContext& context)
        { //std::cerr << "parseComposed: " << ch.narrow('_') << std::endl;
            if( ch == eof )
            {
                throw ParseError("Expected token before EOF", context.line());
            }

            if(Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n')  )
                return;

            if( ch == Pt::Char(L',') )
            {
                throw ParseError("Expected token before \',\'", context.line());
            }

            context.name() += ch;
            _parse = &PropertiesReader::parseComposedValueName;
        }

        void parseComposedValueName(const Pt::Char& ch, ParseContext& context)
        {//std::cerr << "parseComposedValueName: " << ch.narrow('_') << std::endl;
            if( ch == eof )
                throw ParseError("Expected \'=\' token 1", context.line());

            if( ch == Pt::Char(L'=') )
            {
                _parse = &PropertiesReader::afterComposedEqual;
                return;
            }

            if( Pt::Unicode::isSpace(ch) )
            {
                _parse = &PropertiesReader::beforeComposedEqual;
                return;
            }

            context.name() += ch;
        }

        void beforeComposedEqual(const Pt::Char& ch, ParseContext& context)
        { //std::cerr << "beforeComposedEqual: " << ch.narrow('_') << std::endl;
            if( ch == eof )
                throw ParseError("Expected \'=\' token 2", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L'=') )
            {
                _parse = &PropertiesReader::afterComposedEqual;
                return;
            }

            throw ParseError("Expected \'=\' token 3", context.line());
        }

        void afterComposedEqual(const Pt::Char& ch, ParseContext& context)
        { //std::cerr << "afterComposedEqual: " << ch.narrow('_') << std::endl;
            if( ch == eof || ch == Pt::Char(L'=') )
                throw ParseError("Expected token after \'=\' xxx", context.line());

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            //if( ch == Pt::Char('"') )
            //{
            //    _parse = &PropertiesReader::parseQoutedValue;
            //    return;
            //}

//             if( ch == Pt::Char('{') )
//             {
//                 context.pushNode();
//                 _parse = &PropertiesReader::parseArray;
//                 return;
//             }
// 
             if( ch == Pt::Char('[') )
             {
                 context.pushNode();
                 _parse = &PropertiesReader::beginComposed;
                 return;
             }

            context.value() += ch;
            _parse = &PropertiesReader::parseComposedValue;
        }

        void parseComposedValue(const Pt::Char& ch, ParseContext& context)
        { //std::cerr << "parseComposedValue: " << ch.narrow('_') << std::endl;
            if( ch == eof )
                throw ParseError( "Expected token before EOF", context.line() );

            if( ch == Pt::Char(L',') )
            {
                context.pushValue();
                _parse = &PropertiesReader::beginComposed;
                return;
            }

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
            {
                context.pushValue();
                _parse = &PropertiesReader::finishComposedValue;
                return;
            }

            if( ch == Pt::Char(L']') )
            {
                context.pushValue();
                context.popNode();

                if( context.isTop() )
                    _parse = &PropertiesReader::parseBeforeName;
                else
                    _parse = &PropertiesReader::finishComposedValue;

                return;
            }

            context.value() += ch;
        }

        void finishComposedValue(const Pt::Char& ch, ParseContext& context)
        { //std::cerr << "finishComposedValue: " << ch.narrow('_') << std::endl;
            if( ch == eof )
                throw ParseError( "Expected token before EOF", context.line() );

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L',') )
            {
                _parse = &PropertiesReader::parseComposedValueName;
                return;
            }

            if( ch == Pt::Char(L']') )
            {
                context.popNode();

                if( context.isTop() )
                    _parse = &PropertiesReader::parseBeforeName;
                else
                    _parse = &PropertiesReader::finishComposedValue;

                return;
            }
        }

        void getEscaped(ParseContext& context)
        {
            Pt::Char ch;
            if( ! _is->get(ch) )
                throw ParseError("Reached EOF within qoute", context.line() );

            switch( ch.value() )
            {
                case L'n':
                    context.value() += Pt::Char(L'\n');
                    break;

                case L'r':
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

