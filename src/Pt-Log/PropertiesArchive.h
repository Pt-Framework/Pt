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
#ifndef Pt_PropertiesArchive_h
#define Pt_PropertiesArchive_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/StringStream.h>
#include <Pt/SourceInfo.h>
#include <Pt/Unicode.h>
#include <Pt/Archive.h>
#include <map>


namespace Pt {

class PropertiesArchive : public Archive
{
    typedef std::multimap< Pt::String, PropertiesArchive> NodeMap;
    typedef std::multimap< Pt::String, Pt::String> ValueMap;

    public:
        PropertiesArchive()
        {}

        PropertiesArchive(const Pt::String& name)
        : _name(name)
        { }

        ~PropertiesArchive()
        { }

    protected:
        const Pt::String* _value(const Pt::String& name) const
        {
            ValueMap::const_iterator it = _values.find(name);
            if( it == _values.end() )
                return 0;

            return &(it->second);
        }

        void _addValue(const Pt::String& name, const Pt::String& value)
        {
            _values.insert( std::make_pair(name, value) );
        }

        const Archive* _findArchive(const Pt::String& name) const
        {
            NodeMap::const_iterator it = _nodes.find(name);
            if( it == _nodes.end() )
                return 0;

            return &(it->second);
        }

        Archive& _addArchive(const Pt::String& name)
        {
            NodeMap::iterator it = _nodes.find(name);
            if( it != _nodes.end() )
                return it->second;

            PropertiesArchive node(name);
            it = _nodes.insert( std::make_pair(name, node) );
            return it->second;
        }

    private:
        Pt::String _name;
        ValueMap _values;
        NodeMap _nodes;
};


class PropertiesReader
{
    public:
        Pt::Char eof;

    public:
        PropertiesReader(std::basic_istream<Pt::Char>& is)
        : _is(is)
        {
            typedef std::char_traits<Pt::Char> Traits;
            eof = Traits::to_char_type( Traits::eof() );
        }

        ~PropertiesReader()
        {}

        void read(Archive& archive)
        {
            _root = &archive;
            this->parse();
        }

    protected:
        void parse()
        {
            PropertiesReader& self = *this;
            _parse = &PropertiesReader::parseBeforeName;

            Pt::Char ch;
            while( _is.get(ch) )
            {
                (self.*_parse)(ch);
            }

            (self.*_parse)( eof );
        }

        void parseBeforeName(const Pt::Char& ch)
        {
            if( ch == eof || Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L'#') )
            {
                _parse = &PropertiesReader::parseComment ;
                return;
            }

            if( ch == Pt::Char(L'=') )
                throw std::logic_error("expected property name");

            _currentName.clear();
            _currentValue.clear();

            _currentName += ch;
            _parse = &PropertiesReader::parseName;
        }

        void parseComment(const Pt::Char& ch)
        {
            if( ch != Pt::Char(L'\n') )
                return;

            _parse = &PropertiesReader::parseBeforeName;
        }

        void parseName(const Pt::Char& ch)
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

            _currentName += ch;
        }

        void parseAfterName(const Pt::Char& ch)
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

        void parseAfterEqual(const Pt::Char& ch)
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

            _currentValue += ch;
            _parse = &PropertiesReader::parseValue;
        }

        void parseValue(const Pt::Char& ch)
        {
            if( ch == Pt::Char(L'=') )
                std::logic_error("malformed property value");

            if( ch == eof || Pt::Unicode::isSpace(ch) ||
                ch == Pt::Char(L'\n') )
            {
                //std::cerr << "Unqouted: " << _currentName.narrow() << " " << _currentValue.narrow() << std::endl;
                this->addNode(_currentName, _currentValue);
                _parse = &PropertiesReader::parseBeforeName;
                return;
            }

            _currentValue += ch;
        }

        void parseQoutedValue(const Pt::Char& ch)
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

            _currentValue += ch;
        }

        void parseEscaped(const Pt::Char& ch)
        {
            switch( ch.value() )
            {
                case L'n':
                    _currentValue += Pt::Char(L'\n');
                    break;

                case L'r':
                    _currentValue += Pt::Char(L'\r');
                    break;

                default:
                    _currentValue += ch;
                    break;
            }

            _parse = &PropertiesReader::parseQoutedValue;
        }

        void parseAfterQoutedValue(const Pt::Char& ch)
        {
            if( ch == eof )
            {
                //std::cerr << "Qouted: " << _currentName.narrow() << " " << _currentValue.narrow() << std::endl;
                this->addNode(_currentName, _currentValue);
                return;
            }

            if(Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n')  )
                return;

            if( ch == Pt::Char('"') )
            {
                _parse = &PropertiesReader::parseQoutedValue;
                return;
            }

            //std::cerr << "Qouted: " << _currentName.narrow() << " " << _currentValue.narrow() << std::endl;
            this->addNode(_currentName, _currentValue);

            _parse = &PropertiesReader::parseBeforeName;
            this->parseBeforeName(ch);
        }

        void addNode(const Pt::String& name, const Pt::String value)
        {
            //std::cerr << "READ: " << "'"<< name.narrow() << "' = '" << value.narrow() << "'" << std::endl;

            // parse the target name dot syntax
            Archive* archive = _root;

            size_t begin = 0;
            size_t end = 0;
            Pt::String token;
            while(end != std::string::npos)
            {
                // get next token either until '.' or rest if the string
                end = name.find('.', begin);
                if(end == Pt::String::npos)
                {
                    token = name.substr( begin );
                    break;
                }

                token = name.substr( begin, end - begin );
                if( token.empty() )
                {
                    throw std::invalid_argument("Invalid property name" + PT_SOURCEINFO);
                }

                // if end + 1 is outside the string we have a string ending with a '.'
                begin = end + 1;
                if( begin >= name.size() )
                {
                    throw std::invalid_argument("Invalid property name" + PT_SOURCEINFO);
                }

                // create sub-archive
                archive = &( archive->addArchive(token) );
            }

            if( token.empty() )
            {
                throw std::invalid_argument("Invalid property name" + PT_SOURCEINFO);
            }

            archive->addValue(token, value);
        }

    private:
        std::basic_istream<Pt::Char>& _is;
        typedef void (PropertiesReader::*Parse)(const Pt::Char&);
        Parse _parse;
        Pt::String _currentName;
        Pt::String _currentValue;
        Archive* _root;
};


}

#endif

