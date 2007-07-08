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
#include <list>


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

        bool operator< (const SerializationNode& other) const
        { return this->name() < other.name(); }

        bool operator< (const Pt::String& other) const
        { return this->name() < other; }

        bool operator!= (const SerializationNode& other) const
        { return this->name() != other.name(); }

    protected:
        explicit SerializationNode(SerializationData* parent = 0)
        : _parent(parent)
        {}

        SerializationNode(SerializationData* parent, const Pt::String& name)
        : _parent(parent)
        , _name(name)
        {}

    private:
        SerializationData* _parent;
        Pt::String _name;
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

    private:
        Pt::Variant _value;
};


class PT_API SerializationData : public SerializationNode
{
    public:
        typedef std::list<SerializationData> SubData;
        typedef std::list<SerializationEntry> Entries;

    public:
        explicit SerializationData(SerializationData* parent = 0);

        SerializationData(SerializationData* parent, const Pt::String& name);

        virtual ~SerializationData();

        SerializationData& addData(const Pt::String& name);

        const SerializationData* getData(const Pt::String& name) const;

        SerializationData* getData(const Pt::String& name);

        const Pt::Variant* getEntry(const Pt::String& name) const;

        void addEntry(const Pt::String& name, const Pt::Variant& value);

        const Entries& entries() const
        { return _entries; }

        Entries& entries()
        { return _entries; }

        const SubData& subData() const
        { return _subdata; }

        SubData& subData()
        { return _subdata; }

    private:
        SubData _subdata;
        Entries _entries;
};

} // namespace Pt


#endif
