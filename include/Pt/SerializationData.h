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
#ifndef Pt_SerializationData_h
#define Pt_SerializationData_h

#include <Pt/String.h>
#include <Pt/Variant.h>
#include <Pt/Exception.h>
#include <Pt/RefCounted.h>
#include <Pt/SmartPtr.h>
#include <map>


namespace Pt {

class SerializationEntry;
class SerializationData;


class PT_API NoSuchEntry : public std::logic_error
{
    public:
        NoSuchEntry(const std::string& name, const SourceInfo& si);

        ~NoSuchEntry() throw();
};


class PT_API SerializationNode
{
    public:
        virtual ~SerializationNode()
        {}

        const Pt::String& name() const
        { return _name; }

        void setName(const Pt::String& name)
        { _name = name; }
        
        virtual SerializationData* parent()
        { return _parent; }

        virtual const SerializationData* parent() const
        { return _parent; }

        const SerializationEntry* toEntry() const
        { return this->_toEntry(); }

        SerializationEntry* toEntry()
        { return this->_toEntry(); }

        const SerializationData* toData() const
        { return this->_toData(); }

        SerializationData* toData()
        { return this->_toData(); }

        bool operator< (const SerializationNode& other) const
        { return this->name() < other.name(); }

        bool operator!= (const SerializationNode& other) const
        { return this->name() != other.name(); }

    protected:
        SerializationNode(SerializationData* parent = 0)
        : _parent(parent)
        {}

        SerializationNode(SerializationData* parent, const Pt::String& name)
        : _parent(parent)
        , _name(name)
        {}
        
        virtual const SerializationEntry* _toEntry() const = 0;

        virtual SerializationEntry* _toEntry() = 0;

        virtual const SerializationData* _toData() const = 0;

        virtual SerializationData* _toData() = 0;
        
    private:
        Pt::String _name;
        SerializationData* _parent;
};


class PT_API SerializationEntry : public SerializationNode
{
    public:
        SerializationEntry(SerializationData& parent, const Pt::String& name);

        SerializationEntry(SerializationData& parent, const Pt::String& name, const Pt::Variant& value);
        
        virtual ~SerializationEntry()
        {}

        const Pt::Variant& value() const
        { return _value; }

    protected:
        virtual const SerializationEntry* _toEntry() const
        { return this; }

        virtual SerializationEntry* _toEntry()
        { return this; }

        virtual const SerializationData* _toData() const
        { return 0; }

        virtual SerializationData* _toData()
        { return 0; }
        
    private:
        Pt::Variant _value;
};


class PT_API SerializationData : public SerializationNode
{
    public:
        typedef std::multimap<Pt::String, SerializationNode*> Nodes;
    
    public: 
        class Iterator
        {
            public:
                Iterator()
                {}
                
                Iterator(Nodes::iterator it)
                : _it( it )
                {}
                
                Iterator& operator++()
                {
                    ++_it;
                    return *this;
                }

                SerializationNode& operator*()
                { return *_it->second; }

                SerializationNode* operator->()
                { return _it->second; }

                bool operator!= (const Iterator& other) const
                { return _it != other._it; }

            private:
                Nodes::iterator _it;
        };

        class ConstIterator
        {
            public:
                ConstIterator()
                {}
                
                ConstIterator(Nodes::const_iterator it)
                : _it( it )
                {}

                ConstIterator& operator++()
                {
                    ++_it;
                    return *this;
                }

                SerializationNode& operator*() const
                { return *_it->second; }

                SerializationNode* operator->() const
                { return _it->second; }

                bool operator!= (const ConstIterator& other) const
                { return _it != other._it; }

            private:
                Nodes::const_iterator _it;
        };

    public:
        SerializationData(SerializationData* parent = 0);

        SerializationData(SerializationData* parent, const Pt::String& name);
        
        virtual ~SerializationData();

        SerializationData& addData(const Pt::String& name);

        void addEntry(const Pt::String& name, const Pt::Variant& value);

        const SerializationNode* getNode(const Pt::String& name) const;

        SerializationNode* getNode(const Pt::String& name);

        const SerializationData* getData(const Pt::String& name) const;

        SerializationData* getData(const Pt::String& name);

        const Pt::Variant* getEntry(const Pt::String& name) const;

        Iterator begin()
        { return Iterator( _nodes.begin() ); }

        Iterator end()
        { return Iterator(_nodes.end() ); }

        ConstIterator begin() const
        { return ConstIterator( _nodes.begin() ); }

        ConstIterator end() const
        { return ConstIterator( _nodes.end() ); }

    protected:
        virtual const SerializationEntry* _toEntry() const
        { return 0; }

        virtual SerializationEntry* _toEntry()
        { return 0; }

        virtual const SerializationData* _toData() const
        { return this; }

        virtual SerializationData* _toData()
        { return this; }

    private:
        Nodes _nodes;
};

} // namespace Pt


#endif
