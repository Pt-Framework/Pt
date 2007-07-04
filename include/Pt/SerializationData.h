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
        { return this->_name(); }

        virtual SerializationData* parent()
        { return _parent(); }

        virtual const SerializationData* parent() const
        { return _parent(); }

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
        SerializationNode()
        {}

        virtual const Pt::String& _name() const = 0;

        virtual SerializationData* _parent() = 0;

        virtual const SerializationData* _parent() const = 0;

        virtual const SerializationEntry* _toEntry() const = 0;

        virtual SerializationEntry* _toEntry() = 0;

        virtual const SerializationData* _toData() const = 0;

        virtual SerializationData* _toData() = 0;
};


class PT_API SerializationEntry : public SerializationNode
{
    public:
        virtual ~SerializationEntry()
        {}

        const Pt::Variant& value() const
        { return this->_value(); }

    protected:
        SerializationEntry()
        {}

        virtual const Pt::Variant& _value() const = 0;

        virtual const SerializationEntry* _toEntry() const
        { return this; }

        virtual SerializationEntry* _toEntry()
        { return this; }

        virtual const SerializationData* _toData() const
        { return 0; }

        virtual SerializationData* _toData()
        { return 0; }
};


class PT_API SerializationData : public SerializationNode
{
    public:
        class IteratorBase : public RefCounted
        {
            public:
                virtual ~IteratorBase()
                { }

                virtual bool advance() = 0;

                virtual SerializationNode& current() const = 0;
        };

        class Iterator
        {
            public:
                Iterator(IteratorBase* base = 0)
                : _base(base)
                { }

                Iterator& operator++()
                {
                    if( ! _base->advance() )
                        _base = 0;

                    return *this;
                }

                SerializationNode& operator*()
                { return _base->current(); }

                bool operator!= (const Iterator& other) const
                { return _base != other._base; }

            private:
                SmartPtr<IteratorBase, InternalRefCounted<IteratorBase> > _base;
        };

        class ConstIterator
        {
            public:
                ConstIterator(IteratorBase* base = 0)
                : _base(base)
                { }

                ConstIterator& operator++()
                {
                    if( ! _base->advance() )
                        _base = 0;

                    return *this;
                }

                const SerializationNode& operator*() const
                { return _base->current(); }

                bool operator!= (const ConstIterator& other) const
                { return _base != other._base; }

            private:
                SmartPtr<IteratorBase, InternalRefCounted<IteratorBase> > _base;
        };

    public:
        virtual ~SerializationData()
        {}

        SerializationData& addData(const Pt::String& name)
        { return this->_addData(name); }

        void addEntry(const Pt::String& name, const Pt::Variant& value)
        { this->_addEntry(name, value); }

        const SerializationNode* getNode(const Pt::String& name) const
        { return this->_getNode(name); }

        SerializationNode* getNode(const Pt::String& name)
        { return this->_getNode(name); }

        const SerializationData* getData(const Pt::String& name) const
        {
            const SerializationNode* node = this->getNode(name);
            if( node && node->toData() )
                return node->toData();

            return 0;
        }

        SerializationData* getData(const Pt::String& name)
        {
            SerializationNode* node = this->getNode(name);
            if( node && node->toData() )
                return node->toData();

            return 0;
        }

        const Pt::Variant* getEntry(const Pt::String& name) const
        {
            const SerializationNode* node = this->getNode(name);

            if( node && node->toEntry() )
                return &( node->toEntry()->value() );

            return 0;
        }

        Iterator begin()
        { return this->_begin(); }

        Iterator end()
        { return Iterator(); }

        ConstIterator begin() const
        { return this->_begin(); }

        ConstIterator end() const
        { return ConstIterator(); }

    protected:
        SerializationData()
        {}

        virtual const SerializationNode* _getNode(const Pt::String& name) const = 0;

        virtual SerializationNode* _getNode(const Pt::String& name)= 0;

        virtual SerializationData& _addData(const Pt::String& name) = 0;

        virtual void _addEntry(const Pt::String& name, const Pt::Variant& value) = 0;

        /** @brief Returns the begin of the Archive contents

            The deriving class is suposed to return a pointer to its
            type of iterator created with new. If the archive is empty
            0 must be returned.
        */
        virtual IteratorBase* _begin() const = 0;

        virtual const SerializationEntry* _toEntry() const
        { return 0; }

        virtual SerializationEntry* _toEntry()
        { return 0; }

        virtual const SerializationData* _toData() const
        { return this; }

        virtual SerializationData* _toData()
        { return this; }
};



class PT_API ObjectEntry : public SerializationEntry
{
    public:
        ObjectEntry(SerializationData& parent, const Pt::String& name);

        ObjectEntry(SerializationData& parent, const Pt::String& name, const Pt::Variant& value);

    protected:
        virtual SerializationData* _parent();

        virtual const SerializationData* _parent() const;

        virtual const Pt::String& _name() const;

        virtual const Pt::Variant& _value() const;

    private:
        SerializationData* _parentData;
        Pt::String _objectName;
        Pt::Variant _objectValue;
};


class PT_API ObjectData : public SerializationData
{
    public:
        typedef std::multimap<Pt::String, SerializationNode*> Nodes;

        class Iterator : public SerializationData::IteratorBase
        {
            public:
                Iterator(const Nodes& entries)
                : _entries(entries)
                , _it( entries.begin() )
                {}

                virtual bool advance()
                {
                    ++_it;
                    return _it != _entries.end();
                }

                virtual SerializationNode& current() const
                { return *_it->second; }

            private:
                const Nodes& _entries;
                Nodes::const_iterator _it;
        };

    public:
        ObjectData(SerializationData* parent = 0);

        ObjectData(SerializationData* parent, const Pt::String& name);

        ~ObjectData();

    protected:
        virtual SerializationData* _parent();

        virtual const SerializationData* _parent() const;

        virtual const Pt::String& _name() const;

        const SerializationNode* _getNode(const Pt::String& name) const;

        SerializationNode* _getNode(const Pt::String& name);

        void _addEntry(const Pt::String& name, const Pt::Variant& value);

        ObjectData& _addData(const Pt::String& name);

        SerializationData::IteratorBase* _begin() const
        { return _nodes.empty() ? 0 : new Iterator( _nodes ); }

    private:
        SerializationData* _parentData;
        Pt::String _objectName;
        Nodes _nodes;
};


} // namespace Pt


#endif
