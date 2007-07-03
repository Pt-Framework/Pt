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
        virtual ~SerializationData()
        {}

        SerializationData& addData(const Pt::String& name)
        { return this->_addData(name); }

        void addEntry(const Pt::String& name, const Pt::Variant& value)
        { this->_addEntry(name, value); }

        const SerializationNode* getNode(const Pt::String& name) const
        { return this->_getNode(name); }

        const SerializationData* getData(const Pt::String& name) const
        {
            const SerializationNode* node = this->getNode(name);
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

    protected:
        SerializationData()
        {}

        virtual const SerializationNode* _getNode(const Pt::String& name) const = 0;

        virtual SerializationData& _addData(const Pt::String& name) = 0;

        virtual void _addEntry(const Pt::String& name, const Pt::Variant& value) = 0;

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

    public:
        ObjectData(SerializationData* parent = 0);

        ObjectData(SerializationData* parent, const Pt::String& name);

        ~ObjectData();

    protected:
        virtual SerializationData* _parent();

        virtual const SerializationData* _parent() const;

        virtual const Pt::String& _name() const;

        const SerializationNode* _getNode(const Pt::String& name) const;

        void _addEntry(const Pt::String& name, const Pt::Variant& value);

        ObjectData& _addData(const Pt::String& name);

    private:
        SerializationData* _parentData;
        Pt::String _objectName;
        Nodes _nodes;
};


} // namespace Pt


#endif
