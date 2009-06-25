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

#include <Pt/SerializationContext.h>
#include <Pt/Formatter.h>

namespace Pt {

class ValueNode : public SerializationInfo::Node
{
    enum Type
    {
        Void,
        String,
        Int,
        UInt,
        Float
    };

    public:
        ValueNode()
        : SerializationInfo::Node(SerializationInfo::Value)
        , _type(String)
        {}

        const Pt::String& getString() const
        {
            switch(_type)
            {
                case Int:
                    convert(_value, _variant.l);
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

        long getInt()
        {
            switch(_type)
            {
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

    protected:
        virtual void onClear();

    private:
        Type _type;
        mutable Pt::String _value;
        union Variant
        {
            long l;
            unsigned long ul;
            double f;
        } _variant;
};

class ReferenceNode : public SerializationInfo::Node
{
    public:
        ReferenceNode()
        : SerializationInfo::Node(SerializationInfo::Reference)
        {}

        const std::string& refId() const
        { return _refid; }

        void setRefId(const std::string& id)
        { _refid = id; }

        void* address() const
        { return _address; }

        void setAddress(void* addr)
        { _address = addr; }
/*
        const std::type_info* typeInfo() const
        { return _fixupInfo; }

        void setTypeInfo(const std::type_info& ti)
        { _fixupInfo = &ti; }
*/
    protected:
        virtual void onClear()
        { _refid.clear(); }

    private:
        void* _address;
        //const std::type_info* _fixupInfo;
        std::string _refid;
};

class ObjectNode : public SerializationInfo::Node
{
    public:
        typedef SerializationInfo** Iterator;
        typedef const SerializationInfo* const* ConstIterator;

        ObjectNode()
        : SerializationInfo::Node(SerializationInfo::Object)
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

        void release(SerializationContext& context);

    protected:
        virtual void onClear();

        ObjectNode& operator=(const ObjectNode&);
        ObjectNode(const ObjectNode&);

    private:
        SerializationInfo** _nodes;
        unsigned _capacity;
        unsigned _size;
};


inline void ValueNode::onClear()
{
    _value.clear();
}


inline ObjectNode::~ObjectNode()
{
    this->onClear();
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


inline void ObjectNode::release(SerializationContext& context)
{
    Iterator endIt = end();

    for(Iterator it = begin(); it != endIt; ++it)
    {
        (*it)->release(context);
        context.push(*it);
    }

    _size = 0;
}


inline void ObjectNode::onClear()
{
    Iterator endIt = end();

    for(Iterator it = begin(); it != endIt; ++it)
    {
        delete *it;
    }
    _size = 0;
}

} // namespace Pt

#endif
