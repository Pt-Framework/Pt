/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_SerializationInfo_h
#define Pt_SerializationInfo_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/Convert.h>
#include <Pt/SerializationError.h>
#include <vector>
#include <typeinfo>

namespace Pt {

class SerializationCache;

/** @brief Represents arbitrary types during serialization.
*/
class PT_API SerializationInfo
{
    public:
		typedef std::vector<SerializationInfo> Nodes;
     
        enum Category {
            Void = 0, Value = 1, Object = 2, Array = 3, Reference = 4
        };

        class Node // TODO: SerializationEntry
        {
            public:
                virtual ~Node()
                {}

                Category category() const
                { return _category; }

                void setCategory(Category cat)
                {
                    _category = cat;
                }

                void clear()
                {
                    this->onClear();
                }

            protected:
                Node(Category cat)
                : _category(cat)
                {}
                
                virtual void onClear() = 0;

            private:
                Category _category;
        };

        class ValueNode : public Node
        {
            public:
                ValueNode()
                : Node(Value)
                {}

                const Pt::String& value() const
                { return _value; }

                Pt::String& value()
                { return _value; }

                void setValue(const Pt::String& value)
                { _value = value; }

            protected:
                virtual void onClear();

            private:
                Pt::String _value;
        };

        class ReferenceNode : public Node
        {
            public:
                ReferenceNode()
                : Node(Reference)
                {}

                const std::string& refId() const
                { return _refid; }

                void setRefId(const std::string& addr)
                { _refid = addr; }

                void* address() const
                { return _address; }

                void setAddress(void* addr)
                { _address = addr; }

                const std::type_info* typeInfo() const
                { return _fixupInfo; }

                void setTypeInfo(const std::type_info& ti)
                { _fixupInfo = &ti; }

            protected:
                virtual void onClear()
                { _refid.clear(); }

            private:
            	void* _address;
                const std::type_info* _fixupInfo;
                std::string _refid;
        };

        class ObjectNode : public Node
        {
            public:
                typedef SerializationInfo** Iterator;
                typedef const SerializationInfo* const* ConstIterator;

                ObjectNode()
                : Node(Object)
                , _nodes(0)
                , _capacity(0)
                , _size(0)
                {}

                ~ObjectNode();

                void push_back(SerializationInfo* si);

                Iterator begin()
                { return &_nodes[0]; }

                Iterator end()
                { return &_nodes[_size]; }

                ConstIterator begin() const
                { return &_nodes[0]; }

                ConstIterator end() const
                { return &_nodes[_size]; }

                unsigned size() const
                { return _size; }

                SerializationInfo& back()
                { return *( _nodes[_size - 1] ); }

                const SerializationInfo& back() const
                { return *( _nodes[_size - 1] ); }

                void release(SerializationCache& cache);

            protected:
				virtual void onClear();

                ObjectNode& operator=(const ObjectNode&);
                ObjectNode(const ObjectNode&);

            private:
                SerializationInfo** _nodes;
                unsigned _capacity;
                unsigned _size;
        };

        class Iterator;
        class ConstIterator;

    public:
        SerializationInfo()
		: _node(0)
		, _cache(0)
		, _parent(0)
		{ }
        
        explicit SerializationInfo(SerializationCache* cache)
		: _node( 0 )
		, _cache(cache)
		, _parent(0)
		{ }
        
        ~SerializationInfo();

	private:
		SerializationInfo(const SerializationInfo& si)
		{}

		SerializationInfo& operator=(const SerializationInfo& si)
		{ return *this; }
		
	public:
		void clear();

        Category category() const
        {
            return _node ? _node->category() : Void;
        }

        void setCategory(Category category);

        SerializationCache* cache()
        {
            return _cache;
        }

        const SerializationCache* cache() const
        {
            return _cache;
        }

        SerializationInfo* parent()
        {
            return _parent;
        }

        const SerializationInfo* parent() const
        {
            return _parent;
        }

        const std::string& typeName() const
        {
            return _type;
        }

        void setTypeName(const std::string& type)
        {
            _type = type;
        }

        const std::string& name() const
        {
            return _name;
        }

        void setName(const std::string& name)
        {
            _name = name;
        }

        void setId(const std::string& id)
        {
            _id = id;
        }

        const std::string& id() const
        {
            return _id;
        }

        /** @brief Serialization of weak pointers
        */
        void setReference(void* ref);

        /** @brief Serialization of weak pointers
        */
        SerializationInfo& addReference(const std::string& name, void* ref);

        /** @brief Deserialization of weak pointers
        */
        template <typename T>
        void toReference(T*& type) const
        {
            this->getReference( reinterpret_cast<void*&>(type), typeid(T) );
        }

        /** @brief Deserialization of weak member pointers
        */
        template <typename T>
        void getReference(const std::string& name, T*& type) const
        {
            this->getMember(name).getReference( reinterpret_cast<void*&>(type), typeid(T) );
        }

        void* refAddr() const;

        const std::string& refId() const;

        void setRefId(const std::string& ref);

        const std::type_info& refType() const;

        /** @brief Deserialization of flat data-types
        */
        template <typename T>
        T toValue() const
        {
            if(_node == 0 || _node->category() != Value)
                throw SerializationError("not a value");

            const ValueNode* svalue = static_cast<const ValueNode*>(_node);
            return convert<T>( svalue->value() );
        }

        /** @brief Deserialization of flat data-types
        */
        template <typename T>
        void toValue(T& value) const
        {
            if( this->category() != Value)
                throw SerializationError("not a value");

            ValueNode* svalue = (ValueNode*) _node;
            return convert( value, svalue->value() );
        }

        /** @brief Deserialization of flat member data-types
        */
        const Pt::String& toString() const;

        /** @brief Serialization of flat data-types
        */
        template <typename T>
        void setValue(const T& value)
        {
        	ValueNode* svalue = initValue();
            convert(svalue->value(), value);
        }

        /** @brief Serialization of flat member data-types
        */
        template <typename T>
        SerializationInfo& addValue(const std::string& name, const T& value)
        {
            SerializationInfo& info = this->addMember(name);
            info.setValue(value);
            return info;
        }

        /** @brief Serialization of member data
        */
        SerializationInfo& addMember(const std::string& name);

        /** @brief Serialization of member data
        */
        SerializationInfo& addMember();

        /** @brief Deserialization of member data
        */
        const SerializationInfo& getMember(const std::string& name) const;

        /** @brief Compiler workaround.
            This is needed for some compilers (GCC 3.x) to allow access to
            method 'T getValue(const std::string& name) const' below.
         */
        template <typename T>
        friend T getValue(const std::string& name, SerializationInfo* si);

        /** @brief Deserialization of flat child value types
        */
        template <typename T>
        T getValue(const std::string& name) const
        {
            T value;
            const SerializationInfo& info = this->getMember(name);
            info.toValue(value);
            return value;
        }

        /** @brief Deserialization of flat child value types
        */
        template <typename T>
        void getValue(const std::string& name, T& value) const
        {
            const SerializationInfo& info = this->getMember(name);
            return info.toValue(value);
        }

        /** @brief Find member data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        const SerializationInfo* findMember(const std::string& name) const;

        /** @brief Find member data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        SerializationInfo* findMember(const std::string& name);

        size_t memberCount() const
        {
            if(_node->category() == Object || _node->category() == Array)
            {
                return static_cast<const ObjectNode*>(_node)->size();
            }

            return 0;
        }

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;        

    protected:
        void getReference(void*& type, const std::type_info& ti) const;

        void setParent(SerializationInfo* si)
        {
            _parent = si;
        }

        ValueNode* initValue() const;

        ReferenceNode* initReference() const;

        ObjectNode* initObject(Category category) const;

    private:
        mutable Node* _node;
        SerializationCache* _cache;
        SerializationInfo* _parent;
        std::string _name;
        std::string _type;
        std::string _id;
};


class PT_API SerializationCache
{
    public:
        SerializationCache();

        ~SerializationCache();

        SerializationInfo* get();

		void push(SerializationInfo::Node* node);

        void push(SerializationInfo* si);

        SerializationInfo::ValueNode* getScalarData();

        SerializationInfo::Node* getObject();

    private:
        std::vector<SerializationInfo*> _infos;
        std::vector<SerializationInfo::ValueNode*> _scalars;
        std::vector<SerializationInfo::ObjectNode*> _objects;
        std::vector<SerializationInfo::ReferenceNode*> _refs;
};


class SerializationInfo::Iterator
{
    public:
        Iterator();

        Iterator(const Iterator& other);

        Iterator(SerializationInfo** info);

        Iterator& operator=(const Iterator& other);

        Iterator& operator++();

        SerializationInfo& operator*();

        SerializationInfo* operator->();

        bool operator!=(const Iterator& other) const;

    private:
        SerializationInfo** _info;
};


class SerializationInfo::ConstIterator
{
    public:
        ConstIterator();

        ConstIterator(const ConstIterator& other);

        ConstIterator(const SerializationInfo* const* info);

        ConstIterator& operator=(const ConstIterator& other);

        ConstIterator& operator++();

        const SerializationInfo& operator*() const;

        const SerializationInfo* operator->() const;

        bool operator!=(const ConstIterator& other) const;

    private:
        const SerializationInfo* const* _info;
};


inline SerializationInfo::Iterator::Iterator()
: _info(0)
{}


inline SerializationInfo::Iterator::Iterator(const Iterator& other)
: _info(other._info)
{}


inline SerializationInfo::Iterator::Iterator(SerializationInfo** info)
: _info(info)
{}


inline SerializationInfo::Iterator& SerializationInfo::Iterator::operator=(const Iterator& other)
{
    _info = other._info;
    return *this;
}


inline SerializationInfo::Iterator& SerializationInfo::Iterator::operator++()
{
    ++_info;
    return *this;
}


inline SerializationInfo& SerializationInfo::Iterator::operator*()
{
    return **_info;
}


inline SerializationInfo* SerializationInfo::Iterator::operator->()
{
    return *_info;
}


inline bool SerializationInfo::Iterator::operator!=(const Iterator& other) const
{
    return _info != other._info;
}


inline SerializationInfo::ConstIterator::ConstIterator()
: _info(0)
{}


inline SerializationInfo::ConstIterator::ConstIterator(const ConstIterator& other)
: _info(other._info)
{}


inline SerializationInfo::ConstIterator::ConstIterator(const SerializationInfo* const* info)
: _info(info)
{}


inline SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator=(const ConstIterator& other)
{
    _info = other._info;
    return *this;
}


inline SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator++()
{
    ++_info;
    return *this;
}


inline const SerializationInfo& SerializationInfo::ConstIterator::operator*() const
{
    return **_info;
}


inline const SerializationInfo* SerializationInfo::ConstIterator::operator->() const
{
    return *_info;
}


inline bool SerializationInfo::ConstIterator::operator!=(const ConstIterator& other) const
{
    return _info != other._info;
}


inline void operator >>=(const SerializationInfo& si, bool& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, bool n)
{
    si.setValue(n);
    si.setTypeName("bool");
}


inline void operator >>=(const SerializationInfo& si, signed char& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, signed char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, unsigned char& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, char& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, short& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned short& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, int& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned int& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, long& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned long& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, float& n)
{
    si.toValue<float>(n);
}


inline void operator <<=(SerializationInfo& si, float n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator >>=(const SerializationInfo& si, double& n)
{
    si.toValue<double>(n);
}


inline void operator <<=(SerializationInfo& si, double n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator >>=(const SerializationInfo& si, std::string& n)
{
    si.toValue<std::string>(n);
}


inline void operator <<=(SerializationInfo& si, const std::string& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


inline void operator >>=(const SerializationInfo& si, Pt::String& n)
{
    si.toValue<Pt::String>(n);
}


inline void operator <<=(SerializationInfo& si, const Pt::String& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


template <typename T>
inline void operator >>=(const SerializationInfo& si, std::vector<T>& vec)
{
	T elem = T();
    vec.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        //vec.resize( vec.size() + 1 );
        //*it >>=  vec.back();
        
        *it >>= elem;
        vec.push_back( elem );
    }
}


template <typename T>
inline void operator <<=(SerializationInfo& si, const std::vector<T>& vec)
{
    typename std::vector<T>::const_iterator it;

    for(it = vec.begin(); it != vec.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
        newSi <<= *it;
        newSi.setName( newSi.typeName() );
    }

    si.setTypeName("array");
    si.setCategory(SerializationInfo::Array);
}


inline void operator >>=(const SerializationInfo& si, std::vector<int>& vec)
{
	int n = 0;
    vec.clear();
	SerializationInfo::ConstIterator end = si.end();
    for(SerializationInfo::ConstIterator it = si.begin(); it != end; ++it)
    {
    	*it >>= n;
        vec.push_back( n );
    }
}

inline void operator <<=(SerializationInfo& si, const std::vector<int>& vec)
{
    std::vector<int>::const_iterator it;

    for(it = vec.begin(); it != vec.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
        newSi <<= *it;
    }

    si.setTypeName("array");
    si.setCategory(SerializationInfo::Array);
}

} // namespace Pt


#endif
