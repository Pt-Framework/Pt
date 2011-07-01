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

		// SerializationInfo::Category category() const
		// { return _category; }

		// void setCategory(SerializationInfo::Category cat)
		// { _category = cat; }

        //virtual void setContext(SerializationContext* context) = 0;

		virtual void clear() = 0;

        virtual void clear(SerializationContext& ) = 0;

        static SerializationNode* createNode(SerializationContext* context,
                                             SerializationInfo::Category category);

    protected:
		SerializationNode(/*SerializationInfo::Category cat*/)
		//: _category(cat)
		{}

	private:
		//SerializationInfo::Category _category;
};


class ContextNode : public SerializationNode
{
    public:
        ContextNode()
        : SerializationNode(/*SerializationInfo::Context*/)
        , _context(0)
        {}

        virtual ~ContextNode()
		{}

        //virtual void setContext(SerializationContext* context)
        //{ _context = context; }

		virtual void clear()
        {
            _context = 0;
        }

        virtual void clear(SerializationContext& ctx)
        {
            _context = 0;
        }

	private:
		SerializationContext* _context;
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
        : SerializationNode(/*SerializationInfo::Scalar*/)
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

        bool getBool()
        {
            switch(_type)
            {
                case Int:
                    return 0 != _variant.l;

                case UInt:
                    return 0 != _variant.ul;

                case Float:
                    return 0 != _variant.f;

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

        long long getInt()
        {
            switch(_type)
            {
                case Bool:
                    return static_cast<long long>(_variant.b);

                case UInt:
                    return static_cast<long long>(_variant.ul);

                case Float:
                    return static_cast<long long>(_variant.f);

                case String:
                    convert(_variant.l, _value); // fall trough

                default:
                    break;
            }

            return _variant.l;
        }

        void setInt(long long l)
        {
            _variant.l = l;
            _type = Int;
        }

        unsigned long long getUInt()
        {
            switch(_type)
            {
                case Bool:
                    return static_cast<unsigned long long>(_variant.b);

                case Int:
                    return static_cast<unsigned long long>(_variant.l);

                case Float:
                    return static_cast<unsigned long long>(_variant.f);

                case String:
                    convert(_variant.ul, _value); // fall trough

                default:
                    break;
            }

            return _variant.ul;
        }

        void setUInt(unsigned long long ul)
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
                    formatter.addBool( name, _variant.b, id );
                    break;

                case Int:
                    formatter.addInt( name, _variant.l, id );
                    break;

                case UInt:
                    formatter.addUInt( name, _variant.ul, id );
                    break;

                case Float:
                    formatter.addFloat( name, _variant.f, id );
                    break;

                default:
                    formatter.addValue( name, type, _value, id );
                    break;
            }
        }

        //virtual void setContext(SerializationContext* context)
        //{ }

        virtual void clear()
		{
			_type = ValueNode::String;

			if( _value.size() )
				 _value.clear();
		}

        virtual void clear(SerializationContext& context)
		{
			_type = ValueNode::String;

			if( _value.size() )
				_value.clear();
		}

    private:
        Type _type;
        mutable Pt::String _value;
        union Variant
        {
            bool b;
            long long l;
            unsigned long long ul;
            double f;
        } _variant;
};

// class SpecialNode : public SerializationNode
// {
//     public:
//         typedef void (*Format)(const Pt::Any&, Formatter& formatter, const std::string& name,
//                                const std::string& type, const std::string& id);

//         SpecialNode()
//         : SerializationNode(SerializationInfo::Void)
//         {}

//         const Any& get() const
//         { return _value; }

//         void set(const Any& v, Format)
//         { _value = v; }

//         virtual void setContext(SerializationContext* context)
//         { }

//         virtual void clear()
//         { _value.clear(); }

//         virtual void clear(SerializationContext& context)
//         { _value.clear(); }

//         void format(Formatter& formatter, const std::string& name,
//                      const std::string& type, const std::string& id)
//         {
//             _format(_value, formatter, name, type, id);
//         }

//     private:
//         Any _value;
//         Format _format;
// };

class ReferenceNode : public SerializationNode
{
    public:
        ReferenceNode()
        : SerializationNode(/*SerializationInfo::Reference*/)
        {}

        const std::string& refId() const
        { return _refid; }

        void setRefId(const std::string& id)
        { _refid = id; }

        void* address() const
        { return _address; }

        void setAddress(void* addr)
        { _address = addr; }

        //virtual void setContext(SerializationContext* context)
        //{ }

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
        ObjectNode(/*SerializationInfo::Category category*/)
        : SerializationNode(/*category*/)
        , _first(0)
        , _last(0)
        , _size(0)
        {}

        ~ObjectNode()
		{
			this->clear();
		}

        void push_back(SerializationInfo* si)
		{
            ++_size;
            if(_first)
            {
                _last->setSibling(si);
                _last = si;
                return;
            }

            _first = si;
            _last = si;
		}

        SerializationInfo* remove(const std::string& name)
		{
            SerializationInfo* prev = 0;

            for(SerializationInfo* it = begin(); it != end(); it = it->sibling())
			{
                if(it->name() == name)
                {
                    SerializationInfo* next = it->sibling();
                    if( prev )
                    {
                        prev->setSibling( next );
                    }

                    if(it == _first)
                    {
                        _first = next;
                    }

                    if(it == _last)
                    {
                        _last = prev;
                    }

                    --_size;
                    it->setSibling(0);
                    return it;
                }

                prev = it;
			}

            return 0;
		}

        SerializationInfo* begin()
        { return _first; }

        SerializationInfo* end()
        { return 0; }

        const SerializationInfo* begin() const
        { return _first; }

        const SerializationInfo* end() const
        { return 0; }

        unsigned size() const
        { return _size; }

        // virtual void setContext(SerializationContext* context)
        // {
		// 	for(SerializationInfo* it = begin(); it != end(); it = it->sibling())
		// 	{
		// 		it->setContext(context);
		// 	}
        // }

		virtual void clear()
		{
			for(SerializationInfo* it = begin(); it != end(); )
			{
                SerializationInfo* tmp = it;
                it = it->sibling();
				delete tmp;
			}

            _size = 0;
            _first = 0;
            _last = 0;
		}

		virtual void clear(SerializationContext& context)
		{
			for(SerializationInfo* it = begin(); it != end(); )
			{
                SerializationInfo* tmp = it;
                it = it->sibling();
				tmp->setSibling(0);

                context.push(tmp);
			}

            _size = 0;
            _first = 0;
            _last = 0;
		}

    protected:
        ObjectNode& operator=(const ObjectNode&);
        ObjectNode(const ObjectNode&);

    private:
        SerializationInfo* _first;
        SerializationInfo* _last;
        unsigned _size;
};


inline SerializationNode* SerializationNode::createNode(SerializationContext* context,
                                                        SerializationInfo::Category category)
{
    SerializationNode* node = 0;

    if( context )
    {
        node = context->get(category);
    }
    else
    {
        switch(category)
        {
            case SerializationInfo::Scalar:
                node = new ValueNode();
                break;

            case SerializationInfo::Reference:
                node = new ReferenceNode();
                break;

            case SerializationInfo::Sequence:
            case SerializationInfo::Struct:
                node = new ObjectNode(/*category*/);
                break;

            case SerializationInfo::Context:
                node = new ContextNode();

            default:
                break;
        }
    }

    return node;
}

} // namespace Pt

#endif
