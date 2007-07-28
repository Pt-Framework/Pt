/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#include "Pt/Reflectable.h"


namespace Pt {

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
    std::string identifierName;
};


Reflectable::Reflectable(const std::string& name)
: _data(0)
{
    _data = new ReflectableData();
    _data->identifierName = name;
}


Reflectable::Reflectable(const Reflectable& other)
{
    _data = new ReflectableData();
    _data->identifierName = other._data->identifierName;
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


const std::string& Reflectable::getIdentifierName() const
{
    return _data->identifierName;
}


Pt::Any Reflectable::property(const std::string& name) const
{
    Properties::const_iterator it;
    for( it = _data->properties.begin(); it != _data->properties.end(); ++it)
    {
        if( (*it)->name() == name)
            return (*it)->get();
    }

    throw NoSuchProperty(getIdentifierName() + "." + name, PT_SOURCEINFO);
}


void Reflectable::setProperty(const std::string& name, const Pt::Any& value)
{
    Properties::iterator it;
    for( it = _data->properties.begin(); it != _data->properties.end(); ++it)
    {
        if( (*it)->name() == name)
            return (*it)->set(value);
    }

    throw NoSuchProperty(getIdentifierName() + "." + name, PT_SOURCEINFO);
}


void Reflectable::call(const std::string& name)
{
    CallableInfo& ci = this->methodInfo(name);
    ci.call(0, 0);
}


Pt::Any Reflectable::call(const std::string& name, const Any* args, size_t argCount)
{
    return this->methodInfo(name).call(args, argCount);
}


void Reflectable::call(const std::string& name, const SerializationData& sd)
{
    return this->methodInfo(name).call(sd);
}


PropertyInfo& Reflectable::propertyInfo(const std::string& name)
{
    Properties::iterator it;
    for( it = _data->properties.begin(); it != _data->properties.end(); ++it)
    {
        if( (*it)->name() == name)
            return **it;
    }

    throw NoSuchProperty(getIdentifierName() + "." + name, PT_SOURCEINFO);
}


const CallableInfo& Reflectable::methodInfo(const std::string& name) const
{
    Methods::const_iterator it;
    for( it = _data->methods.begin(); it != _data->methods.end(); ++it)
    {
        if( name == (*it)->name() )
            return **it;
    }

    throw NoSuchMethod(getIdentifierName() + "." + name, PT_SOURCEINFO);
}


CallableInfo& Reflectable::methodInfo(const std::string& name)
{
    Methods::iterator it;
    for( it = _data->methods.begin(); it != _data->methods.end(); ++it)
    {
        if( name == (*it)->name() )
            return **it;
    }

    throw NoSuchMethod(getIdentifierName() + "." + name, PT_SOURCEINFO);
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
    _data->identifierName = other.getIdentifierName();
    return *this;
}


const SerializationNode& operator>>(const SerializationNode& node, Reflectable& r)
{
    const Pt::SerializationData* data = Pt::node_cast<const Pt::SerializationData*>(&node);
    if(!data)
        throw NoSuchEntry("Reflectable", PT_SOURCEINFO);

    Reflectable::PropertyIterator it;
    for( it = r.propertiesBegin(); it != r.propertiesEnd(); ++it)
    {
        Pt::String propName = Pt::String::widen( it->name() );

        if( const Pt::SerializationNode* node = data->getNode(propName) )
        {
            it->set(*node);
        }
    }

    return node;
}


SerializationData& operator<<(SerializationData& data, const Reflectable& r)
{
    Reflectable::ConstPropertyIterator it;
    for( it = r.propertiesBegin(); it != r.propertiesEnd(); ++it)
    {
        Pt::String propName = Pt::String::widen( it->name() );
        SerializationNode& added = it->get(data);
        added.setName(propName);
    }

    return data;
}
} // namespace Pt
