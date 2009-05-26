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
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationContext.h>

namespace Pt {

/*void SerializationInfo::ValueNode::onClear(SerializationCache& cache)
{ 
	_value.clear(); 
}*/


void SerializationInfo::ValueNode::onClear()
{ 
	_value.clear(); 
}


SerializationInfo::ObjectNode::~ObjectNode()
{
	this->onClear();
	::operator delete(_nodes);
}


void SerializationInfo::ObjectNode::push_back(SerializationInfo* si)
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


void SerializationInfo::ObjectNode::release(SerializationContext& context)
{
    Iterator endIt = end();

	for(Iterator it = begin(); it != endIt; ++it)
	{
		Node* node = (*it)->_node;
		context.push(node);
		(*it)->_node = 0;
		context.push(*it);
	}

    _size = 0;
}


void SerializationInfo::ObjectNode::onClear()
{
    Iterator endIt = end();

    for(Iterator it = begin(); it != endIt; ++it)
    {
        delete *it;
    }
    _size = 0;
}


SerializationInfo::~SerializationInfo()
{
    if(_node)
    {
    	if(_context)
        	_context->push(_node);
        else
        	delete _node;
    }
}

	
void SerializationInfo::clear()
{
    if(_node)
    {
    	if(_context && (_node->category() == Object || _node->category() == Array) )
    	{
        	static_cast<SerializationInfo::ObjectNode*>(_node)->release(*_context);
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


SerializationInfo::ValueNode* SerializationInfo::initValue() const
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


SerializationInfo::ReferenceNode* SerializationInfo::initReference() const
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


SerializationInfo::ObjectNode* SerializationInfo::initObject(Category category) const
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
    return svalue->value();
}


SerializationInfo& SerializationInfo::addMember(const std::string& name)
{
    ObjectNode* onode = initObject(Object);

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

} // namespace Pt
