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
    if( _context && _bound )
    {
        const char* id = _context->getId(_bound);
        if(id)
            this->setId( id ); // prevents this method being const
    }

    if(this->category() == SerializationInfo::Scalar)
    {
        static_cast<ValueNode*>(_node)->format( formatter,
                                                this->name(),
                                                this->typeName(),
                                                this->id() );
    }
    else if(this->category() == Pt::SerializationInfo::Reference)
    {
        if( ! _context )
            throw SerializationError("context not available");

        const void* refAddr = static_cast<const ReferenceNode*>(_node)->address();
        const char* id = _context->getId( refAddr );
        if( ! id )
            throw SerializationError("stray reference");

        formatter.addReference( this->name(), id);
    }
    else if(this->category() == SerializationInfo::Struct)
    {
        formatter.beginObject( this->name(), this->typeName(), this->id() );

        SerializationInfo::Iterator it;
        for(it = this->begin(); it != this->end(); ++it)
        {
            formatter.beginMember( it->name(), it->typeName(), it->id() );
            it->format(formatter);
            formatter.finishMember();
        }

        formatter.finishObject();
    }
    else if(this->category() == Pt::SerializationInfo::Sequence)
    {
        formatter.beginArray( this->name(), this->typeName(), this->id() );

        SerializationInfo::Iterator it;
        for(it = this->begin(); it != this->end(); ++it)
        {
            formatter.beginElement( it->typeName(), it->id()  );
            it->format(formatter);
            formatter.finishElement();
        }

        formatter.finishArray();
    }
}


SerializationInfo::Category SerializationInfo::category() const
{
    return _node ? _node->category() : Void;
}


SerializationSurrogate* SerializationInfo::surrogate(const char* name) const
{
    if(_context)
        return _context->surrogate(name);

    return 0;
}


bool SerializationInfo::beginSave(const void* p)
{
    bool first = true;

    if(_parent == 0 || _parent->_bound)
    {
        if(_context)
        {
            first = _context->beginSave(p, _name);
            if(first)
                _bound = p;
        }
    }

    return first;
}


void SerializationInfo::finishSave()
{
    if(_context && _bound)
    {
        _context->finishSave();
    }
}


void SerializationInfo::beginLoad(void* p, const std::type_info& ti) const
{
    if(_parent == 0 || _parent->_bound)
        _bound = p;

    if(_context && _bound)
    {
        _context->beginLoad(p, ti, _name, _id);
    }
}


void SerializationInfo::finishLoad() const
{
    if(_context && _bound)
    {
        _context->finishLoad();
    }
}


SerializationInfo::~SerializationInfo()
{
    //std::cerr << "SerializationInfo::~SerializationInfo() BEGIN" << std::endl;
    if(_node)
    {
        if(_context)
        {
            //std::cerr << "SerializationInfo::~SerializationInfo() PUSH" << std::endl;
            _context->push(_node);
        }
        else
        {
            delete _node;
        }
    }
    
    //std::cerr << "SerializationInfo::~SerializationInfo() END" << std::endl;
}


void SerializationInfo::clear()
{
    if(_node)
    {
        if( _context )
            _node->clear(*_context);
        else
            _node->clear();
    }

    if( _name.size() )
        _name.clear();
    
    if( _type.size() )
        _type.clear();
    
    if( _id.size() )
        _id.clear();
    
    _bound = 0;
}


SerializationNode* SerializationInfo::releaseNode()
{
    SerializationNode* node = _node;
    _node = 0;
    return node;
}


void SerializationInfo::setCategory(Category category)
{
    if( this->category() != category)
    {
        SerializationNode* node = 0;

        if( _context )
        {
            node = _context->get(category);
        }
        else
        {
			switch(category)
			{
				case Scalar:
					node = new ValueNode();
					break;
		
				case Reference:
					node = new ReferenceNode();
					break;
		
				case Sequence:
				case Struct:
					node = new ObjectNode(category);
					break;
		
				default:
					break;
			}
        }
        
        delete _node;
        _node = node;
    }
}


void SerializationInfo::setContext(SerializationContext* context)
{ 
    _context = context; 
    
    if(_node)
    {
        _node->setContext(context);
    }
}


Pt::String& SerializationInfo::initString()
{
    this->setCategory(Scalar);
    return static_cast<ValueNode*>(_node)->setString();
}


// called during serialization, when a reference needs to be unlinked
void SerializationInfo::saveReference(const void* ref)
{
    if(_context )
    {
        _context->prepareId( ref );
    }

    this->setCategory(Reference);
    static_cast<ReferenceNode*>(_node)->setAddress( const_cast<void*>(ref) );
}


// called during deserialization, when a reference id was parsed
void SerializationInfo::setReference(const std::string& id)
{
    this->setCategory(Reference);
    static_cast<ReferenceNode*>(_node)->setRefId(id);
}


// called during deserialization, when a reference needs to be relinked
// by a previously parsed reference id
void SerializationInfo::load(void* type, FixupHandler fh) const
{
    if( this->category() != Reference)
        throw SerializationError("not a reference");

    const std::string& refId = static_cast<const ReferenceNode*>(_node)->refId();

    if(_context)
    {
        _context->prepareFixup(type, refId, fh);
    }
}


const Pt::String& SerializationInfo::toString() const
{
    if( this->category() != Scalar)
        throw SerializationError("not a value");

    ValueNode* svalue = (ValueNode*) _node;
    return svalue->getString();
}


void SerializationInfo::getValue(bool& b) const
{
    if( this->category() != Scalar)
        throw SerializationError("expected boolean value");

    b = static_cast<ValueNode*>(_node)->getBool();
}


void SerializationInfo::setValue(bool b)
{
    this->setCategory(Scalar);
    static_cast<ValueNode*>(_node)->setBool(b);
    //initValue()->setBool(b);
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
    if( this->category() != Scalar)
        throw SerializationError("expected integer value");

    l = static_cast<ValueNode*>(_node)->getInt();
}

void SerializationInfo::setValue(long l)
{
    this->setCategory(Scalar);
    static_cast<ValueNode*>(_node)->setInt(l);
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
    if( this->category() != Scalar)
        throw SerializationError("expected integer value");

    ul = static_cast<ValueNode*>(_node)->getUInt();
}


void SerializationInfo::setValue(unsigned long ul)
{
    this->setCategory(Scalar);
    static_cast<ValueNode*>(_node)->setUInt(ul);
}


void SerializationInfo::getValue(float& f) const
{
    double d = 0.0;
    this->getValue(d);
    // TODO: consider SerializationError on overflow
    f = static_cast<float>(d);
}


void SerializationInfo::getValue(double& f) const
{
    if( this->category() != Scalar)
        throw SerializationError("expected float value");

    f = static_cast<ValueNode*>(_node)->getFloat();
}


void SerializationInfo::setValue(double f)
{
    this->setCategory(Scalar);
    static_cast<ValueNode*>(_node)->setFloat(f);
}


SerializationInfo& SerializationInfo::addMember(const std::string& name)
{
    if( this->category() != Struct)
    	this->setCategory(Struct);
    
    ObjectNode* onode = static_cast<ObjectNode*>(_node);

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


SerializationInfo& SerializationInfo::addElement()
{
    if( this->category() != Sequence)
    	this->setCategory(Sequence);
    
    ObjectNode* onode = static_cast<ObjectNode*>(_node);

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
    if(! _node || (_node->category() != Struct && _node->category() != Sequence) )
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
    if(! _node || (_node->category() != Struct && _node->category() != Sequence) )
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
    if(! _node || (_node->category() != Struct && _node->category() != Sequence) )
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
    if(! _node || (_node->category() != Struct && _node->category() != Sequence) )
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
    if(_node && (_node->category() == Struct || _node->category() == Sequence) )
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
    if(_node && (_node->category() == Struct || _node->category() == Sequence) )
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
    if(_node && (_node->category() == Struct || _node->category() == Sequence) )
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
    if(_node && (_node->category() == Struct || _node->category() == Sequence) )
    {
        return static_cast<const ObjectNode*>(_node)->size();
    }

    return 0;
}

} // namespace Pt
