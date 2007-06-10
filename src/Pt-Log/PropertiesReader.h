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
#include <Pt/String.h>
#include <Pt/Exception.h>
#include <Pt/Unicode.h>
#include <Pt/ArchiveReader.h>
#include <iostream>


namespace Pt {

class PropertiesReader : public ArchiveReader
{
    public:
        Pt::Char eof;

        class ParseContext
        {
            public:
                ParseContext(Archive& archive)
                : _archive( &archive )
                {}

                void clear()
                {
                    _name.clear();
                    _value.clear();
                }

                Pt::String& name()
                { return _name; }

                Pt::String& value()
                { return _value; }

                Archive& archive()
                { return *_archive;}

                void push()
                {
                    //std::cerr << "READ: " << "'"<< _name.narrow() << "' = '" << _value.narrow() << "'" << std::endl;

                    // parse the target name dot syntax
                    Archive* archive = _archive;

                    size_t begin = 0;
                    size_t end = 0;
                    Pt::String token;
                    while(end != std::string::npos)
                    {
                        // get next token until '.' if not found we are leaf and bail out
                        end = _name.find('.', begin);
                        if(end == Pt::String::npos)
                            break;

                        token = _name.substr( begin, end - begin );
                        if( token.empty() )
                        {
                            throw std::invalid_argument("Invalid property name" + PT_SOURCEINFO);
                        }

                        // create sub-archive
                        archive = &( archive->addArchive(token) );

                        // if end + 1 is outside the string we have a string ending with a '.'
                        begin = end + 1;
                        if( begin >= _name.size() )
                        {
                            throw std::invalid_argument("Invalid property name" + PT_SOURCEINFO);
                        }
                    }

                    token = _name.substr( begin );
                    if( token.empty() )
                    {
                        throw std::invalid_argument("Invalid property name" + PT_SOURCEINFO);
                    }

                    archive->addValue(token, _value);
                }

            private:
                Archive* _archive;
                Pt::String _name;
                Pt::String _value;
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

                (this->*_parse)(ch, context);
            }

            (this->*_parse)( eof, context );
        }

        void parseBeforeName(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof || Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L'=') )
                throw std::logic_error("expected property name");

            context.clear();

            context.name() += ch;
            _parse = &PropertiesReader::parseName;
        }

        void parseName(const Pt::Char& ch, ParseContext& context)
        {
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

            if( ch == eof )
                throw std::logic_error("expected \'=\'");

            context.name() += ch;
        }

        void parseAfterName(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw std::logic_error("expected \'=\'");

            if( Pt::Unicode::isSpace(ch) )
                return;

            if(ch == Pt::Char(L'='))
            {
                _parse = &PropertiesReader::parseAfterEqual;
                return;
            }

            throw std::logic_error("expected \'=\'");
        }

        void parseAfterEqual(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
                throw std::logic_error("expected value");

            if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L'=') || ch == Pt::Char(L'#') )
                throw std::logic_error("expected value");

            if( ch == Pt::Char('"') )
            {
                _parse = &PropertiesReader::parseQoutedValue;
                return;
            }

            context.value() += ch;
            _parse = &PropertiesReader::parseValue;
        }

        void parseValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == Pt::Char(L'=') )
                std::logic_error("malformed property value");

            if( ch == eof || Pt::Unicode::isSpace(ch) ||
                ch == Pt::Char(L'\n') )
            {
                //std::cerr << "Unqouted: " << context.name().narrow() << " " << context.value().narrow() << std::endl;
                context.push();
                _parse = &PropertiesReader::parseBeforeName;
                return;
            }

            context.value() += ch;
        }

        void parseQoutedValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == Pt::Char(L'\n') || ch == Pt::Char(L'\r') )
            {
                throw std::logic_error("missing \" after value" + PT_SOURCEINFO);
            }

            if( ch == Pt::Char(L'\\') )
            {
                _parse = &PropertiesReader::parseEscaped;
                return;
            }

            if( ch == Pt::Char(L'"') )
            {
                _parse = &PropertiesReader::parseAfterQoutedValue;
                return;
            }

            context.value() += ch;
        }

        void parseEscaped(const Pt::Char& ch, ParseContext& context)
        {
            switch( ch.value() )
            {
                case L'n':
                    context.value() += Pt::Char(L'\n');
                    break;

                case L'r':
                    context.value() += Pt::Char(L'\r');
                    break;

                default:
                    context.value() += ch;
                    break;
            }

            _parse = &PropertiesReader::parseQoutedValue;
        }

        void parseAfterQoutedValue(const Pt::Char& ch, ParseContext& context)
        {
            if( ch == eof )
            {
                //std::cerr << "Qouted: " << context.name().narrow() << " " << context.value().narrow() << std::endl;
                context.push();
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
            context.push();

            _parse = &PropertiesReader::parseBeforeName;
            this->parseBeforeName(ch, context);
        }

    private:
        std::basic_istream<Pt::Char>* _is;
        typedef void (PropertiesReader::*Parse)(const Pt::Char&, ParseContext&);
        Parse _parse;
};


}

#endif

