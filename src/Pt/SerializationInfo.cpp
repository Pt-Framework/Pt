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
#include <Pt/Formatter.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationContext.h>

namespace Pt {

void SerializationInfo::format(Formatter& formatter)
{
    if( _context && ! _context->isUnlinked( this->id() ) )
    {
        this->setId("");
    }

    if(this->category() == SerializationInfo::Value)
    {
        static_cast<const ValueNode*>(_node)->format( formatter,
                                                      this->name(),
                                                      this->typeName(),
                                                      this->id() );
    }
    else if(this->category() == Pt::SerializationInfo::Reference)
    {
        if( ! _context )
            throw SerializationError("context not available");

        const void* refAddr = static_cast<const ReferenceNode*>(_node)->address();
        std::string id = _context->getUnlinkId( refAddr );
        formatter.addReference( this->name(), id);
    }
    else if(this->category() == SerializationInfo::Object)
    {
        formatter.beginObject( this->name(), this->typeName(), this->id() );

        SerializationInfo::Iterator it;
        for(it = this->begin(); it != this->end(); ++it)
        {
            formatter.beginMember( it->name() );
            it->format(formatter);
            formatter.finishMember();
        }

        formatter.finishObject();
    }
    else if(this->category() == Pt::SerializationInfo::Array)
    {
        formatter.beginArray( this->name(), this->typeName(), this->id() );

        SerializationInfo::Iterator it;
        for(it = this->begin(); it != this->end(); ++it)
        {
            it->format(formatter);
        }

        formatter.finishArray();
    }
}


bool SerializationInfo::beginSave(const void* p)
{
    bool unlinked = true;

    if(_parent == 0 || _parent->_binder)
        _binder = _context;

    if( _context )
    {
        // TODO: optimize: keep void* to bound type in SerializationInfotype
        //       to show public access and get id later only if required
        std::string id = _context->beginUnlinkTarget(_name, p, unlinked);
        this->setId(id);
    }

    return unlinked;
}


void SerializationInfo::finishSave()
{
    if(_binder)
    {
        _binder->finishUnlinkTarget();
    }
}


void SerializationInfo::beginLoad(void* p, const std::type_info& ti) const
{
    if(_parent == 0 || _parent->_binder)
        _binder = _context;

    if(_context)
    {
        _context->beginLinkTarget( _name, _id, p, ti);
    }
}


void SerializationInfo::finishLoad() const
{
    if(_context)
    {
        _context->finishLinkTarget();
    }
}


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

    _name.clear();
    _type.clear();
    _id.clear();
}


void SerializationInfo::release(SerializationContext& context)
{
    if(_node)
        context.push(_node);

    _node = 0;
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


// called during serialization, when a reference needs to be unlinked
void SerializationInfo::setReference(const void* ref)
{
    if(_context )
    {
        _context->prepareUnlink( ref );
    }

    ReferenceNode* node = initReference();
    node->setAddress( const_cast<void*>(ref) );
}


// called during deserialization, when a reference id was parsed
void SerializationInfo::setReference(const std::string& id)
{
    ReferenceNode* node = initReference();
    node->setRefId(id);
}


// called during deserialization, when a reference needs to be relinked
// by a previously parsed reference id
void SerializationInfo::fixup(void* type, FixupHandler fh) const
{
    if( this->category() != Reference)
        throw SerializationError("not a reference");

    const std::string& refId = static_cast<const ReferenceNode*>(_node)->refId();

    if(_context)
    {
        _context->prepareLink( refId, type, fh );
    }
}


const Pt::String& SerializationInfo::toString() const
{
    if( this->category() != Value)
        throw SerializationError("not a value");

    ValueNode* svalue = (ValueNode*) _node;
    return svalue->getString();
}


void SerializationInfo::getValue(short& s) const
{
    long l = 0;
    this->getValue(l);
    // TODO: consider SerializationError on overflow
    s = static_cast<short>(l);
}


void SerializationInfo::getValue(int& i) const
{
    long l = 0;
    this->getValue(l);
    // TODO: consider SerializationError on overflow
    i = static_cast<int>(l);
}


void SerializationInfo::getValue(long& l) const
{
    if( this->category() != Value)
        throw SerializationError("expected integer value");

    l = static_cast<const ValueNode*>(_node)->getInt();
}

void SerializationInfo::setValue(long l)
{
    initValue()->setInt(l);
}


void SerializationInfo::getValue(unsigned short& us) const
{
    unsigned long ul = 0;
    this->getValue(ul);
    // TODO: consider SerializationError on overflow
    us = static_cast<int>(ul);
}


void SerializationInfo::getValue(unsigned int& ui) const
{
    unsigned long ul = 0;
    this->getValue(ul);
    // TODO: consider SerializationError on overflow
    ui = static_cast<int>(ul);
}


void SerializationInfo::getValue(unsigned long& ul) const
{
    if( this->category() != Value)
        throw SerializationError("expected integer value");

    ul = static_cast<const ValueNode*>(_node)->getUInt();
}


void SerializationInfo::setValue(unsigned long ul)
{
    initValue()->setUInt(ul);
}


void SerializationInfo::getValue(float& f) const
{
    double d = 0.0;
    this->getValue(d);
    // TODO: consider SerializationError on overflow
    f = static_cast<double>(d);
}


void SerializationInfo::getValue(double& f) const
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

    si->_parent = this;
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

    si->_parent = this;
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
