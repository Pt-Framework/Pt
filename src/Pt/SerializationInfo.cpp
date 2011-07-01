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
#include <Pt/SerializationSurrogate.h>

namespace Pt {

bool SerializationInfo::beginFormat(Formatter& formatter)
{
    if(this->category() == SerializationInfo::Scalar)
    {
        static_cast<ValueNode*>(_node)->format( formatter,
                                                this->name(),
                                                this->typeName(),
                                                this->id() );
        return false;
    }
    else if(this->category() == Pt::SerializationInfo::Reference)
    {
        if( ! _context )
            throw SerializationError("context not available");

        const void* refAddr = static_cast<const ReferenceNode*>(_node)->address();
        const char* id = _context->getId( refAddr );
        formatter.addReference( this->name(), id);
        return false;
    }
    else if(this->category() == SerializationInfo::Struct)
    {
        formatter.beginObject( this->name(), this->typeName(), this->id() );
        return true;
    }
    else if(this->category() == Pt::SerializationInfo::Sequence)
    {
        formatter.beginArray( this->name(), this->typeName(), this->id() );
        return true;
    }

    return false;
}


void SerializationInfo::endFormat(Formatter& formatter)
{
    if(this->category() == SerializationInfo::Struct)
    {
        formatter.finishObject();
    }
    else if(this->category() == Pt::SerializationInfo::Sequence)
    {
        formatter.finishArray();
    }
}


void SerializationInfo::format(Formatter& formatter)
{
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
    ///return _node ? _node->category() : Void;
    return _category;
}


SerializationSurrogate SerializationInfo::getSurrogate(const char* name) const
{
    if( _context && this->category() != Context )
        return _context->getSurrogate(name);

    return SerializationSurrogate();
}


SerializationInfo::~SerializationInfo()
{
    if(_node)
    {
        if(_context)
        {
            _context->push(_node, _category);
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
        if( _context )
            _node->clear(*_context);
        else
            _node->clear();

        if(this->category() == Context)
            this->setCategory(Void);
    }

    if( _name.size() )
        _name.clear();

    if( _type.size() )
        _type.clear();

    if( _id.size() )
        _id.clear();

    _bound = 0;
    _category = Void;
}


void SerializationInfo::clear(SerializationContext* context)
{
    if(_node)
    {
        if( _context )
            _node->clear(*_context);
        else
            _node->clear();

        if(this->category() == Context)
            this->setCategory(Void);
    }

    if( _name.size() )
        _name.clear();

    if( _type.size() )
        _type.clear();

    if( _id.size() )
        _id.clear();

    _bound = 0;
    _context = context;
    _category = Void;
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

        if(this->category() == SerializationInfo::Context)
        {
            node = category == SerializationInfo::Void ? 0 : _node;
        }
        else
        {
            node = SerializationNode::createNode(_context, category);
        }

        if(_node != node) // OPTIMIZE
        {
            delete _node; // OPTIMIZE
            _node = node;
        }

        _category = category;
    }
}


// void SerializationInfo::setContext(SerializationContext* context)
// {
//     _context = context;

//     if(_node)
//     {
//         _node->setContext(context);
//     }
// }


Pt::String* SerializationInfo::initString()
{
    if( category() == Context )
        return 0;

    this->setCategory(Scalar);
    return &( static_cast<ValueNode*>(_node)->setString() );
}


// called during serialization, when a reference needs to be unlinked
void SerializationInfo::setReference(const void* ref)
{
    if( category() == Context )
    {
        if( _context && this->context()->referencingEnabled() )
            this->context()->prepareId(ref);

        return;
    }

    this->setCategory(Reference);
    static_cast<ReferenceNode*>(_node)->setAddress( const_cast<void*>(ref) );
}


// called during deserialization, when a reference id was parsed
void SerializationInfo::setReference(const std::string& id)
{
    this->setCategory(Reference);
    static_cast<ReferenceNode*>(_node)->setRefId(id);
    static_cast<ReferenceNode*>(_node)->setAddress(0);
}


// called during deserialization, when a reference needs to be fixed up
void SerializationInfo::load(void* type, FixupHandler fh, unsigned m) const
{
    if( this->category() != Reference)
        throw SerializationError("not a reference");

    const std::string& refId = static_cast<ReferenceNode*>(_node)->refId();
    static_cast<ReferenceNode*>(_node)->setAddress(type);

    if(_context)
    {
        _context->prepareFixup(type, refId, fh, m);
    }
}


const Pt::String& SerializationInfo::toString() const
{
    if( this->category() != Scalar )
        throw SerializationError("not a value");

    ValueNode* svalue = (ValueNode*) _node;
    return svalue->getString();
}


void SerializationInfo::getValue(Pt::String& s) const
{
    if( this->category() != Scalar )
        throw SerializationError("not a value");

    ValueNode* svalue = static_cast<ValueNode*>(_node);
    s = svalue->getString();
}


void SerializationInfo::setValue(const Pt::String& s)
{
    if( category() == Context )
        return;

    this->setCategory(Scalar);
    ValueNode* svalue = static_cast<ValueNode*>(_node);
    svalue->setString() = s;
}


void SerializationInfo::getValue(bool& b) const
{
    if( this->category() != Scalar )
        throw SerializationError("expected boolean value");

    b = static_cast<ValueNode*>(_node)->getBool();
}


void SerializationInfo::setValue(bool b)
{
    if( category() == Context )
        return;

    this->setCategory(Scalar);
    static_cast<ValueNode*>(_node)->setBool(b);
}


void SerializationInfo::getValue(short& s) const
{
    long long l = 0;
    this->getValue(l);
    // TODO: consider SerializationError on overflow
    s = static_cast<short>(l);
}


//void SerializationInfo::getValue(int& i) const



void SerializationInfo::getValue(long& i) const
{
    long long l = 0;
    this->getValue(l);
    // TODO: consider SerializationError on overflow
    i = static_cast<int>(l);
}


void SerializationInfo::getValue(long long & l) const
{
    if( this->category() != Scalar )
        throw SerializationError("expected integer value");

    l = static_cast<ValueNode*>(_node)->getInt();
}


void SerializationInfo::setValue(long long l)
{
    if( category() == Context )
        return;

    //this->setCategory(Scalar);

    SerializationNode* node = 0;
    if( _context )
    {
        node = _context->get(Scalar);
    }
    else
        node = new ValueNode();

    if(_node )
    {
        if(_context)
            _context->push(_node, _category);
        else
            delete _node;
    }
    _node = node;
    _category = Scalar;

    static_cast<ValueNode*>(_node)->setInt(l);
}


void SerializationInfo::getValue(unsigned short& us) const
{
    unsigned long long ul = 0;
    this->getValue(ul);
    // TODO: consider SerializationError on overflow
    us = static_cast<unsigned short>(ul);
}


void SerializationInfo::getValue(unsigned int& ui) const
{
    unsigned long long ul = 0;
    this->getValue(ul);
    // TODO: consider SerializationError on overflow
    ui = static_cast<unsigned int>(ul);
}


void SerializationInfo::getValue(unsigned long& ui) const
{
    unsigned long long ul = 0;
    this->getValue(ul);
    // TODO: consider SerializationError on overflow
    ui = static_cast<unsigned long>(ul);
}


void SerializationInfo::getValue(unsigned long long& ul) const
{
    if( this->category() != Scalar)
        throw SerializationError("expected integer value");

    ul = static_cast<ValueNode*>(_node)->getUInt();
}


void SerializationInfo::setValue(unsigned long long ul)
{
    if( category() == Context )
        return;

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
    if( category() == Context )
        return;

    this->setCategory(Scalar);
    static_cast<ValueNode*>(_node)->setFloat(f);
}


bool SerializationInfo::beginSave(const void* p)
{
    if( ! this->context() || ! this->context()->referencingEnabled() )
        return true;

    if( category() == Context )
    {
        return this->context()->beginSave(p, _name);
    }

    bool first = true;

    if(_parent == 0 || _parent->_bound)
    {
        // all referenced objects are known by the context
        // if this object is referenced, make an id and set _id
        // _id can be written when this type is formatted
        const char* id = _context->makeId(p);
        if(id)
        {
            // the id can be "" or a null-terminated string which means,
            // in either case, the type was saved for the first time.
            _id = id;
        }
        else
        {
            // the id can be 0 if the type has already been saved
            first = false;
        }

        _bound = p;
    }

    return first;
}


void SerializationInfo::finishSave()
{
    if( category() == Context && this->context() && this->context()->referencingEnabled() )
    {
        this->context()->finishSave();
        return;
    }
}


void SerializationInfo::rebind(void* obj) const
{
    _bound = obj;

    if( ! _context )
        return;

    _context->rebindTarget(_id, obj);

    // NOTE: all fixup addresses and child addresses are invalid too
}


void SerializationInfo::rebindFixup(void* obj) const
{
    if( this->category() != Reference )
        throw SerializationError("not a reference");

    const std::string& refId = static_cast<const ReferenceNode*>(_node)->refId();
    void* addr = static_cast<const ReferenceNode*>(_node)->address();

    if(_context)
        _context->rebindFixup( refId, obj, addr );
}


void SerializationInfo::beginLoad(void* p, const std::type_info& ti) const
{
    if(_context && _context->referencingEnabled() && (_parent == 0 || _parent->_bound) )
    {
        _bound = p;
        _context->beginLoad(p, ti, _name, _id);
    }
}


void SerializationInfo::finishLoad() const
{
    if(_context && _context->referencingEnabled() && _bound)
    {
        _context->finishLoad();
    }
}


SerializationInfo& SerializationInfo::addMember(const std::string& name)
{
    if( category() == Context )
    {
        _name = name;
        return *this;
    }

    if( this->category() != Struct)
    	this->setCategory(Struct);

    ObjectNode* onode = static_cast<ObjectNode*>(_node);

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
    si->_next = 0;
    si->setName(name);

    onode->push_back(si);
    return *si;
}


void SerializationInfo::removeMember(const std::string& name)
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        ObjectNode* snode = static_cast<ObjectNode*>(_node);

        SerializationInfo* si = snode->remove(name);

        if(_context)
            _context->push(si);
        else
            delete si;
    }
}


SerializationInfo& SerializationInfo::addElement()
{
    if( category() == Context )
    {
        if( _name.size() )
            _name.clear();
        return *this;
    }

    if( this->category() != Sequence)
    	this->setCategory(Sequence);

    ObjectNode* onode = static_cast<ObjectNode*>(_node);

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
    si->_next = 0;
    onode->push_back(si);
    return *si;
}


SerializationInfo::Iterator SerializationInfo::begin()
{
    if( ! _node || (this->category() != Struct && this->category() != Sequence) )
    {
        return SerializationInfo::Iterator(0);
    }

    ObjectNode* snode = static_cast<ObjectNode*>(_node);
    return SerializationInfo::Iterator( snode->begin() );
}


SerializationInfo::ConstIterator SerializationInfo::begin() const
{
    if( ! _node || (this->category() != Struct && this->category() != Sequence) )
    {
        return SerializationInfo::ConstIterator(0);
    }

    const ObjectNode* snode = static_cast<const ObjectNode*>(_node);
    return SerializationInfo::ConstIterator( snode->begin() );
}


const SerializationInfo& SerializationInfo::getMember(const std::string& name) const
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        const ObjectNode* snode = static_cast<const ObjectNode*>(_node);
        ConstIterator it( snode->begin() );
        for(; it != ConstIterator( snode->end() ); ++it)
        {
            if( it->name() == name )
                return *it;
        }
    }

    throw SerializationError("Missing info for '" + name + "'", PT_SOURCEINFO);
}


const SerializationInfo* SerializationInfo::findMember(const std::string& name) const
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        ObjectNode* snode = (ObjectNode*) _node;
        ConstIterator it( snode->begin() );
        for(; it != ConstIterator( snode->end() ); ++it)
        {
            if( it->name() == name )
                return &(*it);
        }
    }

    return 0;
}


SerializationInfo* SerializationInfo::findMember(const std::string& name)
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        ObjectNode* snode = (ObjectNode*) _node;
        Iterator it ( snode->begin() );
        for(; it != Iterator( snode->end()); ++it)
        {
            if( it->name() == name )
                return &(*it);
        }
    }

    return 0;
}


size_t SerializationInfo::memberCount() const
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        return static_cast<const ObjectNode*>(_node)->size();
    }

    return 0;
}

} // namespace Pt
