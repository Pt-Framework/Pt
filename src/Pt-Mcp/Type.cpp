/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <Pt/Mcp/Type.h>
#include <ostream>

namespace Pt {

namespace Mcp {

//
// Type
//

Type::~Type()
{
}


void Type::toSchema(std::ostream& os, const std::string& description) const
{
    const char* name = 0;

    switch(_id)
    {
        case Null:    name = "null";    break;
        case Integer: name = "integer"; break;
        case Number:  name = "number";  break;
        case String:  name = "string";  break;
        case Boolean: name = "boolean"; break;
        default:      name = "string";  break;
    }

    os << "{\"type\":\"" << name << "\"";

    if( ! description.empty() )
        os << ",\"description\":\"" << description << "\"";

    os << "}";
}


namespace {

class NullType : public Type
{
  public:
    NullType() : Type(Type::Null) {}
};

class IntegerType : public Type
{
  public:
    IntegerType() : Type(Type::Integer) {}
};

class NumberType : public Type
{
  public:
    NumberType() : Type(Type::Number) {}
};

class StringType : public Type
{
  public:
    StringType() : Type(Type::String) {}
};

class BooleanType : public Type
{
  public:
    BooleanType() : Type(Type::Boolean) {}
};

} // anonymous namespace


const Type& nullType()
{
    static NullType t;
    return t;
}


const Type& integerType()
{
    static IntegerType t;
    return t;
}


const Type& numberType()
{
    static NumberType t;
    return t;
}


const Type& stringType()
{
    static StringType t;
    return t;
}


const Type& booleanType()
{
    static BooleanType t;
    return t;
}


//
// Property
//

Property::Property(const std::string& name, const Type& type,
                   const std::string& description)
: _name(name)
, _type(&type)
, _description(description)
, _required(true)
{
}


//
// ObjectType
//

ObjectType::ObjectType()
: Type(Type::Object)
, _strict(false)
{
}


ObjectType::~ObjectType()
{
}


ObjectType& ObjectType::addProperty(const std::string& name, const Type& type,
                                    const std::string& description)
{
    _properties.push_back( Property(name, type, description) );
    return *this;
}


ObjectType& ObjectType::setOptional(const std::string& name)
{
    for(std::size_t i = 0; i < _properties.size(); ++i)
    {
        if(_properties[i].name() == name)
        {
            _properties[i].setOptional();
            break;
        }
    }

    return *this;
}


ObjectType& ObjectType::setStrict()
{
    _strict = true;
    return *this;
}


void ObjectType::toSchema(std::ostream& os, const std::string& description) const
{
    os << "{\"type\":\"object\"";

    if( ! description.empty() )
        os << ",\"description\":\"" << description << "\"";

    os << ",\"properties\":{";

    for(std::size_t i = 0; i < _properties.size(); ++i)
    {
        if(i > 0)
            os << ',';

        os << "\"" << _properties[i].name() << "\":";
        _properties[i].type().toSchema(os, _properties[i].description());
    }

    os << "},\"required\":[";

    bool first = true;
    for(std::size_t i = 0; i < _properties.size(); ++i)
    {
        if(_properties[i].isRequired())
        {
            if( ! first)
                os << ',';
            os << "\"" << _properties[i].name() << "\"";
            first = false;
        }
    }

    os << "]";

    if(_strict)
        os << ",\"additionalProperties\":false";

    os << "}";
}


//
// ArrayType
//

ArrayType::ArrayType(const Type& items)
: Type(Type::Array)
, _items(&items)
{
}


ArrayType::~ArrayType()
{
}


void ArrayType::toSchema(std::ostream& os, const std::string& description) const
{
    os << "{\"type\":\"array\"";

    if( ! description.empty() )
        os << ",\"description\":\"" << description << "\"";

    os << ",\"items\":";
    _items->toSchema(os);
    os << '}';
}


//
// EnumType
//

EnumType::EnumType()
: Type(Type::String)
{
}


EnumType::~EnumType()
{
}


EnumType& EnumType::addValue(const std::string& value)
{
    _values.push_back(value);
    return *this;
}


void EnumType::toSchema(std::ostream& os, const std::string& description) const
{
    os << "{\"type\":\"string\"";

    if( ! description.empty() )
        os << ",\"description\":\"" << description << "\"";

    os << ",\"enum\":[";

    for(std::size_t i = 0; i < _values.size(); ++i)
    {
        if(i > 0)
            os << ',';
        os << "\"" << _values[i] << "\"";
    }

    os << "]}";
}


//
// NullableType
//

NullableType::NullableType(const Type& inner)
: Type( inner.typeId() )
, _inner(&inner)
{
}


NullableType::~NullableType()
{
}


void NullableType::toSchema(std::ostream& os,
                            const std::string& description) const
{
    TypeId tid = _inner->typeId();

    if(tid == Type::Object || tid == Type::Array)
    {
        os << "{\"oneOf\":[";
        _inner->toSchema(os);
        os << ",{\"type\":\"null\"}]";

        if( ! description.empty() )
            os << ",\"description\":\"" << description << "\"";

        os << "}";
    }
    else
    {
        const char* typeName = "string";
        switch(tid)
        {
            case Type::Null:    typeName = "null";    break;
            case Type::Integer: typeName = "integer"; break;
            case Type::Number:  typeName = "number";  break;
            case Type::String:  typeName = "string";  break;
            case Type::Boolean: typeName = "boolean"; break;
            default: break;
        }

        os << "{\"type\":[\"" << typeName << "\",\"null\"]";

        if( ! description.empty() )
            os << ",\"description\":\"" << description << "\"";

        os << "}";
    }
}

} // namespace Mcp

} // namespace Pt
