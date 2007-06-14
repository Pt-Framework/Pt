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
#include <Pt/Archive.h>
#include <map>


namespace Pt {

class PropertiesArchiveValue : public ArchiveValue
{
    public:
        PropertiesArchiveValue(Archive* parent, const Pt::String& name)
        : _parentArchive(parent)
        , _nodeName(name)
        {}

        PropertiesArchiveValue(Archive* parent, const Pt::String& name, const Pt::String value)
        : _parentArchive(parent)
        , _nodeName(name)
        , _nodeValue(value)
        {}

    protected:
        virtual Archive* _parent()
        {
            return _parentArchive;
        }

        virtual const Archive* _parent() const
        {
            return _parentArchive;
        }

        virtual const Pt::String& _name() const
        { return _nodeName; }

        virtual const Pt::String& _value() const 
        { return _nodeValue; }

    private:
        Archive* _parentArchive;
        Pt::String _nodeName;
        Pt::String _nodeValue;
};


class PropertiesArchive : public Archive
{
    public:
        typedef std::multimap<Pt::String, ArchiveNode*> Entries;

        class Iterator : public Archive::IteratorBase
        {
            public:
                Iterator(const Entries& entries)
                : _entries(entries)
                , _it( entries.begin() )
                {}

                virtual bool advance()
                {
                    ++_it;
                    return _it != _entries.end();
                }

                virtual ArchiveNode& current() const
                { return *_it->second; }

            private:
                const Entries& _entries;
                Entries::const_iterator _it;
        };

    public:
        PropertiesArchive(Archive* parent = 0)
        : _parentArchive(parent)
        {}

        PropertiesArchive(Archive* parent, const Pt::String& name)
        : _parentArchive(parent)
        , _nodeName(name)
        { }

    protected:
        virtual Archive* _parent()
        {
            return _parentArchive;
        }

        virtual const Archive* _parent() const
        {
            return _parentArchive;
        }

        virtual const Pt::String& _name() const
        { return _nodeName; }

        Archive::IteratorBase* _begin() const
        { return _entries.empty() ? 0 : new Iterator( _entries ); }

        const ArchiveNode* _getNode(const Pt::String& name) const
        {
            Entries::const_iterator it = _entries.find(name);
            if( it == _entries.end() )
                return 0;

            return it->second;
        }

        void _addValue(const Pt::String& name, const Pt::String& value)
        {
            ArchiveValue* av = new PropertiesArchiveValue(this, name, value);
            _entries.insert( std::make_pair(name, av) );
        }

        Archive& _addArchive(const Pt::String& name)
        {
            Entries::const_iterator it = _entries.find(name);
            if( it != _entries.end() && it->second->toArchive() )
                return *( it->second->toArchive() );

            PropertiesArchive* arch = new PropertiesArchive(this, name);
            _entries.insert( std::make_pair(name, arch) );
            return *arch;
        }

    private:
        Archive* _parentArchive;
        Pt::String _nodeName;
        Entries _entries;
};

}

#endif

