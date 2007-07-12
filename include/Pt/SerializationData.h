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
#include <list>


namespace Pt {

class SerializationEntry;
class SerializationData;


/** @brief Error during serialization of a type

    This Exception indicates a error during serialization caused by
    missing attributes or object data.
*/
class PT_API NoSuchEntry : public std::logic_error
{
    public:
        /** @brief Construct with attribute name and source-info

            The string \a name is the name of the missing or invalid attribute
            and will be inserted into the error message text of the exception.
        */
        NoSuchEntry(const std::string& name, const SourceInfo& si);

        //! @brief Destructor
        ~NoSuchEntry() throw();
};


//! @internal
class PT_API SerializationNode
{
    public:
        virtual ~SerializationNode()
        {}

        const Pt::String& name() const
        { return _name; }

        void setName(const Pt::String& name)
        { _name = name; }

        SerializationData* parent()
        { return _parent; }

        const SerializationData* parent() const
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


/** @brief Object attributes for serialization

    In the serialization process, all objects to be serialized are seen as
    an object graphs. SerializationData objects are the composite nodes and
    SerializationEntry objects are the leaf nodes. When a type is serialized
    it will add the values of its plain members as SerializationEntry objects
    to the SerializationData object representing the whole object.
*/
class PT_API SerializationEntry : public SerializationNode
{
    public:
        /** @brief Construct with parent node and name

            Constructs a SerializationEntry, which is the child of \a parent
            and the name \a name. Client code does not need to create entries,
            but call SerializationData::addEntry to add attributes to object data.
        */
        SerializationEntry(SerializationData& parent, const Pt::String& name);

        /** @brief Construct with parent node, name and value

            Constructs a SerializationEntry, which is the child of \a parent,
            the name \a name and the value \ value. Client code does not need
            to create entries, but call SerializationData::addEntry to add
            attributes to object data.
        */
        SerializationEntry(SerializationData& parent, const Pt::String& name, const Pt::Variant& value);

        //! @brief Destructor
        ~SerializationEntry()
        {}

        //! @brief Returns the value of the entry
        const Pt::Variant& value() const
        { return _value; }

        //! @brief Sets the value of the entry
        void setValue(const Pt::Variant& val)
        { _value = val; }

    private:
        //! @internal
        Pt::Variant _value;
};


/** @brief Object data for serialization

    In the serialization process, all objects to be serialized are seen as
    an object graphs. SerializationData objects are the composite nodes and
    SerializationEntry objects are the leaf nodes. SerializationData objects
    are the intermediates in the two-step serialization process. To serialize
    an object, it is decomposed to a SerializationData object, which can be
    written out by a Serializer. During deserialization, raw data is preprocessed
    into SerializationData objects first and then the object is initialized from it.
*/
class PT_API SerializationData : public SerializationNode
{
    public:
        typedef std::list<SerializationData> SubData;
        typedef std::list<SerializationEntry> Entries;

    public:
        /** @brief Construct with parent node

            Constructs a SerializationData which is the child of \a parent.
            If \a parent is null, this will be a root node.
        */
        explicit SerializationData(SerializationData* parent = 0);

        /** @brief Construct with parent node and name

            Constructs a SerializationData which is the child of \a parent.
            If \a parent is null, this will be a root node.
        */
        SerializationData(SerializationData* parent, const Pt::String& name);

        //! @brief Destructor
        ~SerializationData()
        {}

        /** @brief Add subdata as a child

            A new SerializationData node will be created as a child of this
            node. A reference to the created SerializationData is returned.
        */
        SerializationData& addData(const Pt::String& name);

        /** @brief Find object data by name

            This method returns the object data, if this node has a
            SerializationData child node with the name \a name
        */
        const SerializationData* getData(const Pt::String& name) const;

        /** @brief Find object data by name

            This method returns the object data, if this node has a
            SerializationData child node with the name \a name
        */
        SerializationData* getData(const Pt::String& name);

        /** @brief Find object attribute by name

            This method returns the object attribute, if this node has a
            SerializationEntry child node with the name \a name
        */
        const Pt::Variant* getEntry(const Pt::String& name) const;

        /** @brief Add object attribute

            A new SerializationEntry node will be created and added as a
            child of this node.
        */
        void addEntry(const Pt::String& name, const Pt::Variant& value);

        /** @brief Returns the attributes of this node
        */
        const Entries& entries() const
        { return _entries; }

        /** @brief Returns the attributes of this node
        */
        Entries& entries()
        { return _entries; }

        /** @brief Returns the data-node  for a sub-object
        */
        const SubData& subData() const
        { return _subdata; }

        /** @brief Returns the data-node  for a sub-object
        */
        SubData& subData()
        { return _subdata; }

    private:
        //! @internal
        SubData _subdata;

        //! @internal
        Entries _entries;
};

} // namespace Pt


#endif
