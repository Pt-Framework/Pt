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
#include "SerializationData.h"
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationContext.h>

namespace Pt {

SerializationInfo::~SerializationInfo()
{
    if(_node)
    {
        if(_context)
        {
            _context->push(_node);
        }
        else
        {
            delete _node;
        }
    }
}


void SerializationInfo::clear()
{
    if(_node)
    {
        if(_context && (_node->category() == Object || _node->category() == Array) )
        {
            static_cast<ObjectNode*>(_node)->release(*_context);
        }
        else
            _node->clear();
    }

    _name = std::string();
    _type = _name;
    _id = _name;
}


void SerializationInfo::setCategory(Category category)
{
    switch(category)
    {
        case Value:
            initValue();
            break;

        case Reference:
            initReference();
            break;

        case Array:
        case Object:
            initObject(category);
            break;

        default:
            break;
    }
}


ValueNode* SerializationInfo::initValue() const
{
    if( this->category() != Value)
    {
        if( _context )
        {
            SerializationInfo::Node* node = _context->getScalarData();
            delete _node;
            _node = node;
        }
        else
        {
            ValueNode* node = new ValueNode();
            delete _node;
            _node = node;
        }
    }

    return static_cast<ValueNode*>(_node);
}


Pt::String& SerializationInfo::initString() const
{
    return initValue()->setString();
}


ReferenceNode* SerializationInfo::initReference() const
{
    if( this->category() != Reference)
    {
        //if( ! _node->cache() )
        {
            ReferenceNode* node = new ReferenceNode();
            delete _node;
            _node = node;
        }
    }

    return static_cast<ReferenceNode*>(_node);
}


ObjectNode* SerializationInfo::initObject(Category category) const
{
    if( this->category() != Object && this->category() != Array)
    {
        //if( ! _node->cache() )
        {
            ObjectNode* node = new ObjectNode();
            delete _node;
            _node = node;
        }
    }

    _node->setCategory(category);
    return static_cast<ObjectNode*>(_node);
}


void SerializationInfo::setReference(void* ref)
{
    ReferenceNode* node = initReference();
    //std::cerr << "setReference " << ref << std::endl;
    node->setAddress(ref);
}


SerializationInfo& SerializationInfo::addReference(const std::string& name, void* ref)
{
    SerializationInfo& info = this->addMember(name);
    info.setReference(ref);
    return info;
}


void SerializationInfo::getReference(void*& type, const std::type_info& ti) const
{
    ReferenceNode* node = initReference();
    node->setAddress(&type);
    node->setTypeInfo(ti);
}


void* SerializationInfo::refAddr() const
{
    if( this->category() != Reference)
        return 0;

    const ReferenceNode* rnode = static_cast<const ReferenceNode*>(_node);
    return rnode->address();
}


const std::string& SerializationInfo::refId() const
{
    if( this->category() != Reference)
        throw SerializationError("not a reference");

    const ReferenceNode* rnode = static_cast<const ReferenceNode*>(_node);
    return rnode->refId();
}


void SerializationInfo::setRefId(const std::string& ref)
{
    ReferenceNode* node = initReference();
    //std::cerr << "setReference " << ref << std::endl;
    node->setRefId(ref);
}


const std::type_info& SerializationInfo::refType() const
{
	ReferenceNode* node = initReference();
    return *( node->typeInfo() );
}


const Pt::String& SerializationInfo::toString() const
{
    if( this->category() != Value)
        throw SerializationError("not a value");

    ValueNode* svalue = (ValueNode*) _node;
    return svalue->getString();
}


void SerializationInfo::toValue(short& s) const
{
    long l = 0;
    this->toValue(l);
    // TODO: consider SerializationError on overflow
    s = static_cast<short>(l);
}


void SerializationInfo::setValue(short s)
{
    initValue()->setInt(s);
}


void SerializationInfo::toValue(int& i) const
{
    long l = 0;
    this->toValue(l);
    // TODO: consider SerializationError on overflow
    i = static_cast<int>(l);
}


void SerializationInfo::setValue(int i)
{
    initValue()->setInt(i);
}


void SerializationInfo::toValue(long& l) const
{
    if( this->category() != Value)
        throw SerializationError("expected integer value");

    l = static_cast<const ValueNode*>(_node)->getInt();
}

void SerializationInfo::setValue(long l)
{
    initValue()->setInt(l);
}


void SerializationInfo::toValue(unsigned short& us) const
{
    unsigned long ul = 0;
    this->toValue(ul);
    // TODO: consider SerializationError on overflow
    us = static_cast<int>(ul);
}


void SerializationInfo::setValue(unsigned short us)
{
    initValue()->setUInt(us);
}


void SerializationInfo::toValue(unsigned int& ui) const
{
    unsigned long ul = 0;
    this->toValue(ul);
    // TODO: consider SerializationError on overflow
    ui = static_cast<int>(ul);
}


void SerializationInfo::setValue(unsigned int ui)
{
    initValue()->setUInt(ui);
}


void SerializationInfo::toValue(unsigned long& ul) const
{
    if( this->category() != Value)
        throw SerializationError("expected integer value");

    ul = static_cast<const ValueNode*>(_node)->getUInt();
}


void SerializationInfo::setValue(unsigned long ul)
{
    initValue()->setUInt(ul);
}


void SerializationInfo::toValue(float& f) const
{
    double d = 0.0;
    this->toValue(d);
    // TODO: consider SerializationError on overflow
    f = static_cast<double>(d);
}


void SerializationInfo::setValue(float f)
{
    initValue()->setFloat(f);
}


void SerializationInfo::toValue(double& f) const
{
    if( this->category() != Value)
        throw SerializationError("expected float value");

    f = static_cast<const ValueNode*>(_node)->getFloat();
}


void SerializationInfo::setValue(double f)
{
    initValue()->setFloat(f);
}


SerializationInfo& SerializationInfo::addMember(const std::string& name)
{
    ObjectNode* onode = initObject(Object);

    //std::cerr << "added member " << name << " "<< _context << std::endl;

    SerializationInfo* si = 0;
    if( _context )
    {
        si = _context->get();
    }
    else
    {
        si = new SerializationInfo();
    }

    si->setParent(this);
    si->setName(name);
    onode->push_back(si);
    return onode->back();
}


SerializationInfo& SerializationInfo::addMember()
{
    ObjectNode* onode = initObject(Array);

    //std::cerr << "added member " <<  _node->cache() << std::endl;

    SerializationInfo* si = 0;
    if( _context )
    {
        si = _context->get();
    }
    else
    {
        si = new SerializationInfo();
    }

    si->setParent(this);
    onode->push_back(si);
    return onode->back();
}


SerializationInfo::Iterator SerializationInfo::begin()
{
    if(! _node || (_node->category() != Object && _node->category() != Array) )
    {
        return 0;
    }

    ObjectNode* snode = (ObjectNode*) _node;

    //if(snode->nodes().size() == 0)
    //    return 0;

    return snode->begin();
}


SerializationInfo::Iterator SerializationInfo::end()
{
    if(! _node || (_node->category() != Object && _node->category() != Array) )
    {
        return 0;
    }

    ObjectNode* snode = (ObjectNode*) _node;

    //if(snode->nodes().size() == 0)
    //    return 0;

    return snode->end();
}


SerializationInfo::ConstIterator SerializationInfo::begin() const
{
    if(! _node || (_node->category() != Object && _node->category() != Array) )
    {
        return 0;
    }

    ObjectNode* snode = (ObjectNode*) _node;

    //if(snode->nodes().size() == 0)
    //    return 0;

    return snode->begin();
}


SerializationInfo::ConstIterator SerializationInfo::end() const
{
    if(! _node || (_node->category() != Object && _node->category() != Array) )
    {
        return 0;
    }

    ObjectNode* snode = (ObjectNode*) _node;

    //if(snode->nodes().size() == 0)
    //    return 0;

    return snode->end();
}


const SerializationInfo& SerializationInfo::getMember(const std::string& name) const
{
    if(_node && (_node->category() == Object || _node->category() == Array) )
    {
        ObjectNode* snode = (ObjectNode*) _node;
        Iterator it = snode->begin();
        for(; it != snode->end(); ++it)
        {
            if( it->name() == name )
                return *it;
        }
    }

    throw SerializationError("Missing info for '" + name + "'", PT_SOURCEINFO);
}


const SerializationInfo* SerializationInfo::findMember(const std::string& name) const
{
    if(_node && (_node->category() == Object || _node->category() == Array) )
    {
        ObjectNode* snode = (ObjectNode*) _node;
        ObjectNode::ConstIterator it = snode->begin();
        for(; it != snode->end(); ++it)
        {
            if( (*it)->name() == name )
                return *it;
        }
    }

    return 0;
}


SerializationInfo* SerializationInfo::findMember(const std::string& name)
{
    if(_node && (_node->category() == Object || _node->category() == Array) )
    {
        ObjectNode* snode = (ObjectNode*) _node;
        ObjectNode::Iterator it = snode->begin();
        for(; it != snode->end(); ++it)
        {
            if( (*it)->name() == name )
                return *it;
        }
    }

    return 0;
}


size_t SerializationInfo::memberCount() const
{
    if(_node->category() == Object || _node->category() == Array)
    {
        return static_cast<const ObjectNode*>(_node)->size();
    }

    return 0;
}

} // namespace Pt
