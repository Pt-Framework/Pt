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

#ifndef Pt_SerializationData_h
#define Pt_SerializationData_h

#include <Pt/SerializationInfo.h>
#include <Pt/SerializationContext.h>
#include <Pt/Formatter.h>

namespace Pt {

class SerializationNode
{
	public:
		virtual ~SerializationNode()
		{}

		SerializationInfo::Category category() const
		{ return _category; }

		void setCategory(SerializationInfo::Category cat)
		{ _category = cat; }

		virtual void clear() = 0;

        virtual void clear(SerializationContext& ) = 0;

	protected:
		SerializationNode(SerializationInfo::Category cat)
		: _category(cat)
		{}

	private:
		SerializationInfo::Category _category;
};


class ValueNode : public SerializationNode
{
    enum Type
    {
        Void,
        Bool,
        String,
        Int,
        UInt,
        Float
    };

    public:
        ValueNode()
        : SerializationNode(SerializationInfo::Value)
        , _type(String)
        {}

        const Pt::String& getString() const
        {
            switch(_type)
            {
                case Int:
                    convert(_value, _variant.l);
                    break;

                case Bool:
                    convert(_value, _variant.b);
                    break;

                case UInt:
                    convert(_value, _variant.ul);
                    break;

                case Float:
                    convert(_value, _variant.f);
                    break;

                default:
                    break;
            }

            return _value;
        }

        Pt::String& setString()
        {
            _type = String;
            return _value;
        }

        long getBool()
        {
            switch(_type)
            {
                case Int:
                    return static_cast<bool>(_variant.l);
                
                case UInt:
                    return static_cast<bool>(_variant.ul);

                case Float:
                    return static_cast<bool>(_variant.f);

                case String:
                    convert(_variant.b, _value); // fall trough

                default:
                    break;
            }

            return _variant.b;
        }

        void setBool(bool b)
        {
            _variant.b = b;
            _type = Bool;
        }

        long getInt()
        {
            switch(_type)
            {
                case Bool:
                    return static_cast<long>(_variant.b);
                
                case UInt:
                    return static_cast<long>(_variant.ul);

                case Float:
                    return static_cast<long>(_variant.f);

                case String:
                    convert(_variant.l, _value); // fall trough

                default:
                    break;
            }

            return _variant.l;
        }

        void setInt(long l)
        {
            _variant.l = l;
            _type = Int;
        }

        unsigned long getUInt()
        {
            switch(_type)
            {
                case Bool:
                    return static_cast<unsigned long>(_variant.b);
                
                case Int:
                    return static_cast<unsigned long>(_variant.l);

                case Float:
                    return static_cast<unsigned long>(_variant.f);

                case String:
                    convert(_variant.ul, _value); // fall trough

                default:
                    break;
            }

            return _variant.ul;
        }

        void setUInt(unsigned long ul)
        {
            _variant.ul = ul;
            _type = UInt;
        }

        void setFloat(double f)
        {
            _variant.f = f;
            _type = Float;
        }

        double getFloat()
        {
            switch(_type)
            {
                case Bool:
                    return static_cast<double>(_variant.b);
                
                case Int:
                    return static_cast<double>(_variant.l);

                case UInt:
                    return static_cast<double>(_variant.ul);

                case String:
                    convert(_variant.f, _value); // fall trough

                default:
                    break;
            }

            return _variant.f;
        }

        void format(Formatter& formatter, const std::string& name,
                     const std::string& type, const std::string& id)
        {
            switch(_type)
            {
                case Bool:
                    formatter.addBool( name, type, _variant.b, id );
                    break;
                
                case Int:
                    formatter.addInt( name, type, _variant.l, id );
                    break;

                case UInt:
                    formatter.addUInt( name, type, _variant.ul, id );
                    break;

                case Float:
                    formatter.addFloat( name, type, _variant.f, id );
                    break;
                
                default:
                    formatter.addValue( name, type, _value, id );
                    break;
            }
        }

        virtual void clear();

        virtual void clear(SerializationContext& context);

    private:
        Type _type;
        mutable Pt::String _value;
        union Variant
        {
            bool b;
            long l;
            unsigned long ul;
            double f;
        } _variant;
};

class ReferenceNode : public SerializationNode
{
    public:
        ReferenceNode()
        : SerializationNode(SerializationInfo::Reference)
        {}

        const std::string& refId() const
        { return _refid; }

        void setRefId(const std::string& id)
        { _refid = id; }

        void* address() const
        { return _address; }

        void setAddress(void* addr)
        { _address = addr; }

        virtual void clear()
        { _refid.clear(); }

        virtual void clear(SerializationContext& context)
        { _refid.clear(); }
    
    private:
        void* _address;
        std::string _refid;
};

class ObjectNode : public SerializationNode
{
    public:
        typedef SerializationInfo** Iterator;
        typedef const SerializationInfo* const* ConstIterator;

        ObjectNode()
        : SerializationNode(SerializationInfo::Object)
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

        virtual void clear();

        virtual void clear(SerializationContext& context);
    
    protected:
        ObjectNode& operator=(const ObjectNode&);
        ObjectNode(const ObjectNode&);

    private:
        SerializationInfo** _nodes;
        unsigned _capacity;
        unsigned _size;
};


inline void ValueNode::clear()
{
    _type = ValueNode::String;
    
    if( _value.size() )
        _value.clear();
}


inline void ValueNode::clear(SerializationContext& context)
{
    _type = ValueNode::String;
    
    if( _value.size() )
        _value.clear();
}


inline ObjectNode::~ObjectNode()
{
    this->clear();
    ::operator delete(_nodes);
}


inline void ObjectNode::push_back(SerializationInfo* si)
{
    if(_capacity == _size)
    {
        void* mem = ::operator new( (_capacity+5) * sizeof(SerializationInfo*) );
        SerializationInfo** nodes = (SerializationInfo**) mem;
        _capacity += 5;
        std::memcpy( nodes, _nodes, _size * sizeof(SerializationInfo*) );

        ::operator delete(_nodes);
        _nodes = nodes;
    }

    _nodes[_size] = si;
    ++_size;
}


inline void ObjectNode::clear()
{
    Iterator endIt = end();

    for(Iterator it = begin(); it != endIt; ++it)
    {
        delete *it;
    }
    _size = 0;
}


inline void ObjectNode::clear(SerializationContext& context)
{
    Iterator endIt = end();

    for(Iterator it = begin(); it != endIt; ++it)
    {
        context.push(*it);
    }

    _size = 0;
}

} // namespace Pt

#endif
