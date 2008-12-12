/*
 * Copyright (C) 2005 by Marc Boris Duerner
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

#include "Pt/Reflex/Reflectable.h"
#include "Pt/SerializationInfo.h"
#include <algorithm>


namespace Pt {

namespace Reflex {

NoSuchProperty::NoSuchProperty(const std::string& propertyName, const SourceInfo& si)
: std::logic_error("Property '" + propertyName + "' not found" + si)
{
}

NoSuchProperty::~NoSuchProperty() throw()
{
}


NoSuchMethod::NoSuchMethod(const std::string& methodName, const SourceInfo& si)
: std::logic_error("Method '" + methodName + "' not found" + si)
{
}

NoSuchMethod::~NoSuchMethod() throw()
{
}


typedef std::vector<PropertyInfo*> Properties;
typedef std::vector<CallableInfo*> Methods;


struct ReflectableData
{
    Methods methods;
    Properties properties;
    std::string objectName;
};


Reflectable::Reflectable(const std::string& name)
: _data(0)
{
    _data = new ReflectableData();
    _data->objectName = name;
}


Reflectable::Reflectable(const Reflectable& other)
{
    _data = new ReflectableData();
    _data->objectName = other._data->objectName;
}


Reflectable::~Reflectable()
{
    Properties::iterator iter;
    for( iter = _data->properties.begin(); iter != _data->properties.end(); ++iter)
    {
        delete *iter;
    }

    Methods::iterator it;
    for( it = _data->methods.begin(); it != _data->methods.end(); ++it)
    {
        delete *it;
    }

    delete _data;
}


const std::string& Reflectable::objectName() const
{
    return _data->objectName;
}


Pt::Any Reflectable::property(const std::string& name) const
{
    Properties::const_iterator it;
    for( it = _data->properties.begin(); it != _data->properties.end(); ++it)
    {
        if( (*it)->name() == name)
            return (*it)->get();
    }

    throw NoSuchProperty(objectName() + "." + name, PT_SOURCEINFO);
}


void Reflectable::setProperty(const std::string& name, const Pt::Any& value)
{
    Properties::iterator it;
    for( it = _data->properties.begin(); it != _data->properties.end(); ++it)
    {
        if( (*it)->name() == name)
            return (*it)->set(value);
    }

    throw NoSuchProperty(objectName() + "." + name, PT_SOURCEINFO);
}


void Reflectable::invoke(const std::string& name, const Any* args, size_t argCount)
{
    CallableInfo& ci = this->methodInfo(name);
    ci.call(args, argCount);
}


Pt::Any Reflectable::call(const std::string& name, const Any* args, size_t argCount)
{
    return this->methodInfo(name).call(args, argCount);
}


PropertyInfo& Reflectable::propertyInfo(const std::string& name)
{
    Properties::iterator it;
    for( it = _data->properties.begin(); it != _data->properties.end(); ++it)
    {
        if( (*it)->name() == name)
            return **it;
    }

    throw NoSuchProperty(objectName() + "." + name, PT_SOURCEINFO);
}


const CallableInfo& Reflectable::methodInfo(const std::string& name) const
{
    Methods::const_iterator it;
    for( it = _data->methods.begin(); it != _data->methods.end(); ++it)
    {
        if( name == (*it)->name() )
            return **it;
    }

    throw NoSuchMethod(objectName() + "." + name, PT_SOURCEINFO);
}


CallableInfo& Reflectable::methodInfo(const std::string& name)
{
    Methods::iterator it;
    for( it = _data->methods.begin(); it != _data->methods.end(); ++it)
    {
        if( name == (*it)->name() )
            return **it;
    }

    throw NoSuchMethod(objectName() + "." + name, PT_SOURCEINFO);
}


Reflectable::PropertyIterator Reflectable::propertiesBegin()
{
    if(_data->properties.size() == 0)
        return 0;

    return &( _data->properties[0] );
}


Reflectable::PropertyIterator Reflectable::propertiesEnd()
{
    if(_data->properties.size() == 0)
        return 0;

    return &_data->properties[0] + _data->properties.size();
}


Reflectable::ConstPropertyIterator Reflectable::propertiesBegin() const
{
    if(_data->properties.size() == 0)
        return 0;

    return &( _data->properties[0] );
}


Reflectable::ConstPropertyIterator Reflectable::propertiesEnd() const
{
    if(_data->properties.size() == 0)
        return 0;

    return &( _data->properties[0]) + _data->properties.size();
}


Reflectable::MethodIterator Reflectable::methodsBegin()
{
    if(_data->methods.size() == 0)
        return 0;

    return &(_data->methods[0]);
}


Reflectable::MethodIterator Reflectable::methodsEnd()
{
    if(_data->methods.size() == 0)
        return 0;

    return &(_data->methods[0]) + _data->methods.size();
}


Reflectable::ConstMethodIterator Reflectable::methodsBegin() const
{
    if(_data->methods.size() == 0)
        return 0;

    return &(_data->methods[0]);
}


Reflectable::ConstMethodIterator Reflectable::methodsEnd() const
{
    if(_data->methods.size() == 0)
        return 0;

    return &(_data->methods[0]) + _data->methods.size();
}


void Reflectable::registerCallableInfo(CallableInfo* ci)
{
    _data->methods.push_back( ci );
}


void Reflectable::registerPropertyInfo(PropertyInfo* pi)
{
    _data->properties.push_back( pi );
}


Reflectable& Reflectable::operator=(const Reflectable& other)
{
    _data->objectName = other.objectName();
    return *this;
}


void Reflectable::include(Reflectable& child)
{
    std::copy( child._data->properties.begin(),
               child._data->properties.end(),
               std::back_inserter<Properties>(_data->properties) );

    child._data->properties.clear();
}


void Reflectable::deserialize(const SerializationInfo& si)
{
    si >>= *this;
}


void Reflectable::serialize(SerializationInfo& si) const
{
    si <<= *this;
}


void operator >>= (const SerializationInfo& si, Reflectable& r)
{
    Reflectable::PropertyIterator it;
    for( it = r.propertiesBegin(); it != r.propertiesEnd(); ++it)
    {
        if(it->isWritable() == false)
            continue;

        const SerializationInfo* pinfo = si.findMember( it->name() );
        if(pinfo == 0)
            continue;

        *pinfo >>= *it;
    }
}


void operator <<= (SerializationInfo& si, const Reflectable& r)
{
    Reflectable::ConstPropertyIterator it;
    for( it = r.propertiesBegin(); it != r.propertiesEnd(); ++it)
    {
        if(it->isWritable() == false)
            continue;

        SerializationInfo& pinfo = si.addMember( it->name() );
        pinfo <<= *it;
    }

    si.setTypeName( r.objectName() );
}

} // namespace Reflex

} // namespace Pt
