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
#ifndef Pt_IniArchive_h
#define Pt_IniArchive_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/StringStream.h>
#include <Pt/SourceInfo.h>
#include <Pt/Unicode.h>
#include <map>
#include "Archive.h"


namespace Pt {

class IniArchiveNode : public Archive
{
    typedef std::multimap< Pt::String, IniArchiveNode> NodeMap;
    typedef std::multimap< Pt::String, Pt::String> ValueMap;

    public:
        IniArchiveNode()
        {}

        IniArchiveNode(const Pt::String& name)
        : _name(name)
        { }

        ~IniArchiveNode()
        { }

        const Pt::String* value(const Pt::String& name) const
        {
            ValueMap::const_iterator it = _values.find(name);
            if( it == _values.end() )
                return 0;

            return &(it->second);
        }

        void addValue(const Pt::String& name, const Pt::String& value)
        {
            _values.insert( std::make_pair(name, value) );
        }

        const Archive* findArchive(const Pt::String& name) const
        {
            NodeMap::const_iterator it = _nodes.find(name);
            if( it == _nodes.end() )
                return 0;

            return &(it->second);
        }

        Archive& addArchive(const Pt::String& name)
        {
            NodeMap::iterator it = _nodes.find(name);
            if( it != _nodes.end() )
                return it->second;

            IniArchiveNode node(name);
            it = _nodes.insert( std::make_pair(name, node) );
            return it->second;
        }

    protected:
        virtual const Archive* _extract(const Pt::String& typeName)
        {
            const Archive* archive = this->findArchive(typeName);
            return archive;
        }

    private:
        Pt::String _name;
        ValueMap _values;
        NodeMap _nodes;
};


class IniArchive : public Archive
{
    private:
        std::basic_istream<Pt::Char>& _is;
        IniArchiveNode _root;

    public:
        IniArchive(std::basic_istream<Pt::Char>& is)
        : _is(is)
        {
            this->beforeName();
        }

        ~IniArchive()
        {}

        const Pt::String* value(const Pt::String& name) const
        {
            return _root.value(name);
        }

        void addValue(const Pt::String& name, const Pt::String& value)
        {
            _root.addValue(name, value);
        }

        const Archive* findArchive(const Pt::String& name) const
        {
            return _root.findArchive(name);
        }

        Archive& addArchive(const Pt::String& name)
        {
            return _root.addArchive(name);
        }

    protected:
        virtual const Archive* _extract(const Pt::String& typeName)
        {
            const Archive* archive = _root.findArchive(typeName);
            return archive;
        }
/*
        typedef void (IniArchive::*Parse)(const Pt::Char&);

        Parse _parse;

        void parseBeforeName(const Pt::Char& ch)
        {
            Pt::Char equal(L'=');

            if( Pt::Unicode::isSpace(ch) )
                return;

            if( ch == Pt::Char(L'\n') )
                return;

            if( ch == Pt::Char(L'#') )
            {
                //_parse = ;
                return;
            }

            if(ch == equal)
                throw std::logic_error("expected property name");
        }
*/
        void beforeName()
        {
            Pt::Char equal(L'=');

            Pt::Char ch;
            while( _is.get(ch) )
            {
                if( Pt::Unicode::isSpace(ch) )
                    continue;

                if( ch == Pt::Char(L'\n') )
                    continue;

                if( ch == Pt::Char(L'#') )
                {
                    this->onComment();
                    return;
                }

                if(ch == equal)
                    throw std::logic_error("expected property name");

                break;
            }

            if( !_is )
                return;

            this->onName(ch);
        }

        void onComment()
        {
            Pt::Char ch;
            while( _is.get(ch) )
            {
                if( ch == Pt::Char(L'\n') )
                    break;
            }

            this->beforeName();
        }

        void onName(Pt::Char ch)
        {
            Pt::Char equal(L'=');
            Pt::String name;
            name += ch;
            while( true )
            {
                _is.get(ch);

                if( !_is )
                    throw std::logic_error("expected property value");

                if(ch == equal)
                {
                    this->onEqual(name);
                    break;
                }
                if( Pt::Unicode::isSpace(ch) )
                {
                    this->afterName(name);
                    break;
                }

                name += ch;
            }
        }

        void afterName(Pt::String& name)
        {
            Pt::Char equal(L'=');
            Pt::Char ch;
            while( _is.get(ch) )
            {
                if(ch == equal)
                    break;

                if( Pt::Unicode::isSpace(ch) )
                    continue;

                throw std::logic_error("space in property name");
            }

            if( !_is )
                throw std::logic_error("malformed property name EOF");

            this->onEqual(name);
        }

        void onEqual(Pt::String& name)
        {
            Pt::Char equal(L'=');
            Pt::Char ch;

            while( _is.get(ch) )
            {
                if( Pt::Unicode::isSpace(ch) )
                    continue;

                if(ch == Pt::Char(L'='))
                    std::logic_error("malformed property value");

                break;
            }

            if( !_is )
                throw std::logic_error("malformed property value EOF");

            this->onValue(name, ch);
        }

        void onValue(Pt::String& name, Pt::Char ch)
        {
            Pt::Char equal(L'=');
            Pt::String value;
            value += ch;
            while( _is.get(ch) )
            {
                if(ch == Pt::Char(L'='))
                    std::logic_error("malformed property value");

                if( Pt::Unicode::isSpace(ch) )
                {
                    break;
                }

                if( ch == Pt::Char(L'\n') )
                {
                    break;
                }

                value += ch;
            }

            this->addNode(name, value);
            this->beforeName();
        }

        void addNode(const Pt::String& name, const Pt::String value)
        {
            //std::cerr << "READ: " << "'"<< name.narrow() << "' = '" << value.narrow() << "'" << std::endl;

            // parse the target name dot syntax
            Archive* archive = &_root;

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
};

}

#endif

