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
#include <Pt/TypeInfo.h>
#include <vector>


namespace Pt {

class SerializationEntry;
class SerializationData;


/** @brief Error during serialization of a type

    This Exception indicates a error during serialization caused by
    missing attributes or object data.
*/
class PT_API SerializationError : public std::logic_error
{
    public:
        /** @brief Construct with message and source-info
        */
        SerializationError(const std::string& msg, const SourceInfo& si);

        //! @brief Destructor
        ~SerializationError() throw();
};


//! @internal
class PT_API SerializationNode
{
    public:
        enum Category
        {
            Value = 0,
            Object = 1,
        };

    public:
        virtual ~SerializationNode()
        {}

        Category category() const
        { return _category; }

        const std::string& name() const
        { return _name; }

        void setName(const std::string& name)
        { _name = name; }

        const std::string& typeName() const
        { return _type; }

        void setTypeName(const std::string& type)
        { _type = type; }

        SerializationData* parent()
        { return _parent; }

        const SerializationData* parent() const
        { return _parent; }

        bool operator< (const SerializationNode& other) const
        { return this->name() < other.name(); }

        bool operator!= (const SerializationNode& other) const
        { return this->name() != other.name(); }

    protected:
        explicit SerializationNode(Category cat, SerializationData* parent = 0)
        : _parent(parent)
        , _category(cat)
        {}

        SerializationNode(Category cat, SerializationData* parent, const std::string& name)
        : _parent(parent)
        , _category(cat)
        , _name(name)
        {}

    private:
        SerializationData* _parent;
        Category _category;
        std::string _name;
        std::string _type;
};


template <typename T>
inline T node_cast(const SerializationNode* node)
{
    typedef typename Pt::TypeInfo<T>::Value NodeT;

    if(node && node->category() == NodeT::Id)
        return static_cast<T>(node);

    return 0;
}


template <typename T>
inline T node_cast(SerializationNode* node)
{
    typedef typename Pt::TypeInfo<T>::Value NodeT;

    if(node && node->category() == NodeT::Id)
        return static_cast<T>(node);

    return 0;
}


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
        static const int Id = SerializationNode::Value;

    public:
        /** @brief Construct with parent node and name

            Constructs a SerializationEntry, which is the child of \a parent
            and the name \a name. Client code does not need to create entries,
            but call SerializationData::addEntry to add attributes to object data.
        */
        SerializationEntry(SerializationData& parent, const std::string& name);

        /** @brief Construct with parent node, name and value

            Constructs a SerializationEntry, which is the child of \a parent,
            the name \a name and the value \ value. Client code does not need
            to create entries, but call SerializationData::addEntry to add
            attributes to object data.
        */
        SerializationEntry(SerializationData& parent, const std::string& name, const Pt::Variant& value);

        //! @brief Destructor
        ~SerializationEntry()
        {}

        /** @brief Gets the value of the entry
            The requested type must be storable in a Pt::Variant.
        */
        template <typename T>
        void getValue(T& type) const
        {
            if(_value.get(type) == false)
                throw SerializationError("Invalid value for " + this->name() , PT_SOURCEINFO);
        }

        /** @brief Sets the value of the entry
            The value must be storable in a Pt::Variant.
        */
        template <typename T>
        void setValue(const T& val)
        { _value = val; }

        /** Returns the value as string.
        */
        const Pt::String& str() const
        { return _value.str(); }

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
        static const int Id = SerializationNode::Object;

    public:
        typedef std::vector<SerializationNode*> Nodes;

        class ConstNodeIterator
        {
            public:
                ConstNodeIterator()
                {}

                ConstNodeIterator(Nodes::const_iterator it)
                : _it(it)
                {}

                ConstNodeIterator& operator++()
                {
                    ++_it;
                    return *this;
                }

                const SerializationNode& operator*() const
                { return **_it; }

                const SerializationNode* operator->() const
                { return *_it; }

                bool operator==(const ConstNodeIterator& other) const
                { return _it == other._it; }

                bool operator!=(const ConstNodeIterator& other) const
                { return _it != other._it; }

            private:
                Nodes::const_iterator _it;
        };

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
        SerializationData(SerializationData* parent, const std::string& name);

        //! @brief Destructor
        ~SerializationData();

        size_t size() const
        { return _nodes.size(); }

        template <typename T>
        T getValue(const std::string& name) const
        {
            T value;
            this->getEntry(name).getValue(value);
            return value;
        }

        template <typename T>
        void getValue(const std::string& name, T& type) const
        {
            this->getEntry(name).getValue(type);
        }

        const SerializationNode& getNode(size_t n) const;

        const SerializationNode& getNode(const std::string& name) const;

        const SerializationNode* findNode(const std::string& name) const;

        SerializationNode* findNode(const std::string& name);

        SerializationNode& getNode(const std::string& name);

        /** @brief Add subdata as a child

            A new SerializationData node will be created as a child of this
            node. A reference to the created SerializationData is returned.
        */
        SerializationData& addData(const std::string& name);

        /** @brief Add subdata as a child

            A new SerializationData node will be created as a child of this
            node. A reference to the created SerializationData is returned.
        */
        SerializationData& addData();

        /** @brief Find object data by name

            This method returns the object data, if this node has a
            SerializationData child node with the name \a name. Otherwise
            an exception is thrown.
        */
        const SerializationData& getData(const std::string& name) const;

        /** @brief Find object data by name

            This method returns the object data, if this node has a
            SerializationData child node with the name \a name
        */
        const SerializationData* findData(const std::string& name) const;

        /** @brief Find object data by name

            This method returns the object data, if this node has a
            SerializationData child node with the name \a name
        */
        SerializationData* findData(const std::string& name);

        /** @brief Find object data by name

            This method returns the object data, if this node has a
            SerializationData child node with the name \a name
        */
        SerializationData& getData(const std::string& name);

        /** @brief Find object attribute by name

            This method returns the object attribute, if this node has a
            SerializationEntry child node with the name \a name
        */
        const SerializationEntry& getEntry(const std::string& name) const;

        /** @brief Add object attribute

            A new SerializationEntry node will be created and added as a
            child of this node.
        */
        void addEntry(const std::string& name, const Pt::Variant& value);

        /** @brief Add object attribute

            A new SerializationEntry node will be created and added as a
            child of this node.
        */
        SerializationEntry& addEntry(const Pt::Variant& value);

        ConstNodeIterator begin() const
        { return _nodes.begin(); }

        ConstNodeIterator end() const
        { return _nodes.end(); }

    private:
        //! @internal
        Nodes _nodes;
};




struct PlainSerializable
{};


struct ComplexSerializable
{};


template <typename T>
struct Serialization
{
    typedef ComplexSerializable Category;

    //static const char* typeName()
    //{ return TypeInfo<T>::typeName(); }
};



template <typename T>
void get(const SerializationNode& node, T& x, PlainSerializable)
{
    const SerializationEntry* entry = node_cast<const SerializationEntry*>(&node);
    if(entry)
    {
        get(*entry, x);
    }
}


template <typename T>
void get(const SerializationNode& node, T& x, ComplexSerializable)
{
    const SerializationData* data = node_cast<const SerializationData*>(&node);
    if(data)
    {
        get(*data, x);
    }
}


template <typename T>
const SerializationNode& getType(const SerializationNode& node, T& x)
{
    typedef typename Serialization<T>::Category Cat;
    get( node, x, Cat() );
    return node;
}




template <typename T>
SerializationNode& insert(SerializationData& parent, const T& x, PlainSerializable)
{
    SerializationEntry& entry = parent.addEntry( Pt::String() );
    set(entry, x);
    return entry;
}


template <typename T>
SerializationNode& insert(SerializationData& parent, const T& x, ComplexSerializable)
{
    SerializationData& data = parent.addData();
    set(data, x);
    return data;
}


template <typename T>
SerializationNode& insert(SerializationData& parent, const T& x )
{
    typedef typename Serialization<T>::Category Cat;
    return insert( parent, x, Cat() );
}


template <>
struct Serialization<int>
{
    typedef PlainSerializable Category;

    static const char* typeName()
    { return "int"; }
};


inline void get(const SerializationEntry& entry, int& x)
{
    entry.getValue(x);
}


inline void set(SerializationEntry& entry, int x)
{
    entry.setValue(x);
}




template <>
struct Serialization<unsigned>
{
    typedef PlainSerializable Category;

    static const char* typeName()
    { return "unsigned"; }
};


inline void get(const SerializationEntry& entry, unsigned& x)
{
    entry.getValue(x);
}


inline void set(SerializationEntry& entry, unsigned x)
{
    entry.setValue(x);
}




template <>
struct Serialization<char>
{
    typedef PlainSerializable Category;

    static const char* typeName()
    { return "char"; }
};


inline void get(const SerializationEntry& entry, char& x)
{
    entry.getValue(x);
}


inline void set(SerializationEntry& entry, char x)
{
    entry.setValue(x);
}




template <>
struct Serialization<bool>
{
    typedef PlainSerializable Category;

    static const char* typeName()
    { return "bool"; }
};


inline void get(const SerializationEntry& entry, bool& x)
{
    entry.getValue(x);
}


inline void set(SerializationEntry& entry, bool x)
{
    entry.setValue(x);
}




template <>
struct Serialization<float>
{
    typedef PlainSerializable Category;

    static const char* typeName()
    { return "float"; }
};


inline void get(const SerializationEntry& entry, float& x)
{
    entry.getValue(x);
}


inline void set(SerializationEntry& entry, float x)
{
    entry.setValue(x);
}




template <>
struct Serialization<double>
{
    typedef PlainSerializable Category;

    static const char* typeName()
    { return "double"; }
};


inline void get(const SerializationEntry& entry, double& x)
{
    entry.getValue(x);
}


inline void set(SerializationEntry& entry, double x)
{
    entry.setValue(x);
}





template <>
struct Serialization<std::string>
{
    typedef PlainSerializable Category;

    static const char* typeName()
    { return "std:.string"; }
};


inline void get(const SerializationEntry& entry, std::string& x)
{
    entry.getValue(x);
}


inline void set(SerializationEntry& entry, const std::string& x)
{
    entry.setValue(x);
}




template <>
struct Serialization<Pt::String>
{
    typedef PlainSerializable Category;

    static const char* typeName()
    { return "Pt::String"; }
};


inline void get(const SerializationEntry& entry, Pt::String& x)
{
    entry.getValue(x);
}


inline void set(SerializationEntry& entry, const Pt::String& x)
{
    entry.setValue(x);
}

} // namespace Pt


#endif
