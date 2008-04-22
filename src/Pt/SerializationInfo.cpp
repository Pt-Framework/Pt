/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                     *
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
#include <Pt/SerializationInfo.h>

namespace Pt {

SerializationError::SerializationError(const std::string& msg, const SourceInfo& si)
: std::logic_error(msg + "'" + si)
{ }


SerializationError::~SerializationError() throw()
{}


SerializationInfo::SerializationInfo()
: _parent(0)
, _category(Value)
, _fixupAddr(0)
, _fixupInfo(0)
{ }


SerializationInfo::SerializationInfo(const SerializationInfo& si)
: _parent(si._parent)
, _category(si._category)
, _name(si._name)
, _type(si._type)
, _id(si._id)
, _fixupAddr(si._fixupAddr)
, _fixupInfo(si._fixupInfo)
, _value(si._value)
, _nodes(si._nodes)
{
}


SerializationInfo& SerializationInfo::operator =(const SerializationInfo& si)
{
    _parent = si._parent;
    _category = si._category;
    _name = si._name;
    _id = si._id;
    _type = si._type;
    _value = si._value;
    _nodes = si._nodes;
    _fixupAddr = si._fixupAddr;
    _fixupInfo = si._fixupInfo;
    return *this;
}


SerializationInfo::~SerializationInfo()
{
///
    /*Nodes::iterator it;
    for(it = _nodes.begin(); it != _nodes.end(); ++it)
    {
        delete *it;
    }*/
///
}


void SerializationInfo::reserve(size_t n)
{
    _nodes.reserve(n);
}


SerializationInfo::Category SerializationInfo::category() const
{
    return _category;
}


void SerializationInfo::setCategory(Category cat)
{
    _category = cat;
}


SerializationInfo* SerializationInfo::parent()
{
    return _parent;
}


const SerializationInfo* SerializationInfo::parent() const
{
    return _parent;
}


const std::string& SerializationInfo::typeName() const
{
    return _type;
}


void SerializationInfo::setTypeName(const std::string& type)
{
    _type = type;
}


const std::string& SerializationInfo::name() const
{
    return _name;
}


void SerializationInfo::setName(const std::string& name)
{
    _name = name;
}


void SerializationInfo::setId(const std::string& id)
{
    _id = id;
}


const std::string& SerializationInfo::id() const
{
    return _id;
}


void SerializationInfo::setReference(void* ref)
{
    _value = convert<Pt::String>(ref);
    _category = Reference;
}


SerializationInfo& SerializationInfo::addReference(const std::string& name, void* ref)
{
    SerializationInfo& info = this->addMember(name);
    info.setReference(ref);
    return info;
}


void SerializationInfo::getReference(void*& type, const std::type_info& ti) const
{
    _fixupAddr = &type;
    _fixupInfo = &ti;
    //_id = convert<std::string>(&type);
    //type = 0;
}


void* SerializationInfo::fixupAddr() const
{
    return _fixupAddr;
}


const std::type_info& SerializationInfo::fixupInfo() const
{
    return *_fixupInfo;
}


const Pt::String& SerializationInfo::toString() const
{
    return _value;
}


SerializationInfo& SerializationInfo::addMember(const std::string& name)
{
    ///SerializationInfo* info = new SerializationInfo();
    ///info->setParent(*this);
    ///info->setName(name);
    ///_nodes.push_back( info );
    SerializationInfo info;
    _nodes.push_back( info );
    _nodes.back().setParent(*this);
    _nodes.back().setName(name);

    _category = Object;

    ///return *info;
    return _nodes.back();
}


SerializationInfo::Iterator SerializationInfo::begin()
{
    if(_nodes.size() == 0)
        return 0;

    return &( _nodes[0] );
}


SerializationInfo::Iterator SerializationInfo::end()
{
    if(_nodes.size() == 0)
        return 0;

    return &_nodes[0] + _nodes.size();
}


SerializationInfo::ConstIterator SerializationInfo::begin() const
{
    if(_nodes.size() == 0)
        return 0;

    return &( _nodes[0] );
}


SerializationInfo::ConstIterator SerializationInfo::end() const
{
    if(_nodes.size() == 0)
        return 0;

    return &_nodes[0] + _nodes.size();
}


const SerializationInfo& SerializationInfo::getMember(const std::string& name) const
{
    Nodes::const_iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( it->name() == name )
            return *it;
    }

    throw SerializationError("Missing info for '" + name + "'", PT_SOURCEINFO);
}


const SerializationInfo* SerializationInfo::findMember(const std::string& name) const
{
    Nodes::const_iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( it->name() == name )
            return &(*it);
    }

    return 0;
}


SerializationInfo* SerializationInfo::findMember(const std::string& name)
{
    Nodes::iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( it->name() == name )
            return &(*it);
    }

    return 0;
}


size_t SerializationInfo::memberCount() const
{
    return _nodes.size();
}


SerializationInfo::Iterator::Iterator()
: _info(0)
{}


SerializationInfo::Iterator::Iterator(const Iterator& other)
: _info(other._info)
{}


SerializationInfo::Iterator::Iterator(SerializationInfo* info)
: _info(info)
{}


SerializationInfo::Iterator& SerializationInfo::Iterator::operator=(const Iterator& other)
{
    _info = other._info;
    return *this;
}


SerializationInfo::Iterator& SerializationInfo::Iterator::operator++()
{
    ++_info;
    return *this;
}


SerializationInfo& SerializationInfo::Iterator::operator*()
{
    return *_info;
}


SerializationInfo* SerializationInfo::Iterator::operator->()
{
    return _info;
}


bool SerializationInfo::Iterator::operator!=(const Iterator& other) const
{
    return _info != other._info;
}


SerializationInfo::ConstIterator::ConstIterator()
: _info(0)
{}


SerializationInfo::ConstIterator::ConstIterator(const ConstIterator& other)
: _info(other._info)
{}


SerializationInfo::ConstIterator::ConstIterator(const SerializationInfo* info)
: _info(info)
{}


SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator=(const ConstIterator& other)
{
    _info = other._info;
    return *this;
}


SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator++()
{
    ++_info;
    return *this;
}


const SerializationInfo& SerializationInfo::ConstIterator::operator*() const
{
    return *_info;
}


const SerializationInfo* SerializationInfo::ConstIterator::operator->() const
{
    return _info;
}


bool SerializationInfo::ConstIterator::operator!=(const ConstIterator& other) const
{
    return _info != other._info;
}

} // namespace Pt
