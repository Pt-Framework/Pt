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

namespace Pt {

SerializationCache::SerializationCache()
{
}


SerializationCache::~SerializationCache()
{
	std::vector<SerializationInfo::ValueNode*>::iterator it = _scalars.begin();
	for(; it != _scalars.end(); ++it)
	{
        //std::cerr << "destroy value" << std::endl;
		delete *it;
	}
	
	std::vector<SerializationInfo*>::iterator iter = _infos.begin();
	for(; iter != _infos.end(); ++iter)
	{
        //std::cerr << "destroy si" << std::endl;
		delete *iter;
	}
	
	//std::cerr << "cache destroyed" << std::endl;
}


SerializationInfo* SerializationCache::get()
{
    SerializationInfo* si = 0;

	if( _infos.empty() )
	{
        //std::cerr << "create si" << std::endl;
	    si = new SerializationInfo(this);
		_infos.push_back(si);
	}
	else
	{
	    si = _infos.back();
	}

	//std::cerr << "get si" << std::endl;
	_infos.pop_back();
	return si;
}


void SerializationCache::push(SerializationInfo* si)
{
	//std::cerr << "push si" << std::endl;
	_infos.push_back(si);
}


SerializationInfo::ValueNode* SerializationCache::getScalarData()
{
    SerializationInfo::ValueNode* node = 0;

	if( _scalars.empty() )
	{
        //std::cerr << "create value" << std::endl;
	    node = new SerializationInfo::ValueNode();
		_scalars.push_back(node);
	}
	else
	{
	    node = _scalars.back();
	}

    //std::cerr << "get value" << std::endl;
	_scalars.pop_back();
	return node;
}


SerializationInfo::Node* SerializationCache::getObject()
{
    //std::cerr << "get object" << std::endl;
	return new SerializationInfo::ObjectNode();
}


void SerializationCache::push(SerializationInfo::Node* node)
{
	if( node->category() == SerializationInfo::Value )
	{
		SerializationInfo::ValueNode* scalar = static_cast<SerializationInfo::ValueNode*>(node);
		_scalars.push_back(scalar);
	}
	else if( node->category() == SerializationInfo::Object || 
	         node->category() == SerializationInfo::Array )
	{
		static_cast<SerializationInfo::ObjectNode*>(node)->release(*this);
		delete node;
	}
	else
	{
		delete node;
	}
}


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


void SerializationInfo::ObjectNode::release(SerializationCache& cache)
{
    Iterator endIt = end();

	for(Iterator it = begin(); it != endIt; ++it)
	{
		Node* node = (*it)->_node;
		cache.push(node);
		(*it)->_node = 0;
		cache.push(*it);
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
    	if(_cache)
        	_cache->push(_node);
        else
        	delete _node;
    }
}

	
void SerializationInfo::clear()
{
    if(_node)
    {
    	if(_cache && (_node->category() == Object || _node->category() == Array) )
    	{
        	static_cast<SerializationInfo::ObjectNode*>(_node)->release(*_cache);
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
		if( _cache )
		{
			SerializationInfo::Node* node = _cache->getScalarData();
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
	node->setAddr(ref);
}


void SerializationInfo::setReferenceId(const std::string& ref)
{
	ReferenceNode* node = initReference();
	//std::cerr << "setReference " << ref << std::endl;
	node->setRefId(ref);
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
	node->setFixupAddr(&type);
	node->setFixupInfo(ti);
}


void* SerializationInfo::fixupAddr() const
{
	ReferenceNode* node = initReference();
    return node->fixupAddr();
}


const std::type_info& SerializationInfo::fixupInfo() const
{
	ReferenceNode* node = initReference();
    return *( node->fixupInfo() );
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
	if( _cache )
	{
		si = _cache->get();
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
	if( _cache )
	{
		si = _cache->get();
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
