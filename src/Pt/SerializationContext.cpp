/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#include "Pt/SerializationContext.h"
#include "Pt/SerializationError.h"
#include <Pt/SerializationInfo.h>

namespace Pt {

class SerializationCache
{
    public:
        SerializationCache()
        : _limit(64)
        {}
        
        std::vector<SerializationInfo*> _infos;
        std::vector<SerializationNode*> _scalars;
        std::vector<SerializationNode*> _objects;
        std::vector<SerializationNode*> _refs;
        size_t _limit;
};


bool SerializationContext::beginSave(const std::string& name, const void* p)
{
    return true;
}


void SerializationContext::finishSave()
{
}


void SerializationContext::prepareId(const void* p)
{
    throw SerializationError("missing unlink information");
}


const char* SerializationContext::getId(const void* p)
{
    throw SerializationError("missing unlink information");
    return 0;
}


void SerializationContext::beginLoad(const std::string& name, const std::string& id,
                                    void* obj, const std::type_info& fixupInfo)
{
}


void SerializationContext::finishLoad()
{
}


void SerializationContext::prepareFixup(const std::string& id, void* obj, FixupHandler)
{
}


void SerializationContext::fixup()
{
}


SerializationContext::SerializationContext()
: _cache(0)
{
    _cache = new SerializationCache;
}


SerializationContext::~SerializationContext()
{
    std::vector<SerializationNode*>::iterator it = _cache->_scalars.begin();
    for(; it != _cache->_scalars.end(); ++it)
    {
        delete *it;
    }
    
    it = _cache->_objects.begin();
    for(; it != _cache->_objects.end(); ++it)
    {
        delete *it;
    }
    
    it = _cache->_refs.begin();
    for(; it != _cache->_refs.end(); ++it)
    {
        delete *it;
    }
    
    std::vector<SerializationInfo*>::iterator iter;
    for(iter = _cache->_infos.begin(); iter != _cache->_infos.end(); ++iter)
    {
        delete *iter;
    }
    
    delete _cache;
}


size_t SerializationContext::limit() const
{
    return _cache->_limit;
}


void SerializationContext::setLimit(size_t n)
{
	_cache->_limit = n;
}


SerializationInfo* SerializationContext::get()
{
    SerializationInfo* si = 0;

    if( _cache->_infos.empty() )
    {
        si = new SerializationInfo(this);
    }
    else
    {
        si = _cache->_infos.back();
        _cache->_infos.pop_back();
    }

    return si;
}


void SerializationContext::push(SerializationInfo* si)
{
    si->clear();

    SerializationNode* node = si->releaseNode();

    if(node)
        this->push(node);

    if(_cache->_infos.size() < _cache->_limit)
    {
        _cache->_infos.push_back(si);
    }
    else
    {
        delete si;
    }
}


SerializationNode* SerializationContext::get(SerializationInfo::Category category)
{
    SerializationNode* node = 0;

    switch(category)
    {
        case SerializationInfo::Scalar:
        {
			if( _cache->_scalars.empty() )
			{
				node = new ValueNode();
				break;
		    }
				
            node = _cache->_scalars.back();
			_cache->_scalars.pop_back();
			break;
	    }

        case SerializationInfo::Reference:
			if( _cache->_refs.empty() )
			{
				node = new ReferenceNode();
				break;
		    }
				
            node = _cache->_refs.back();
			_cache->_refs.pop_back();
            break;

        case SerializationInfo::Sequence:
        case SerializationInfo::Struct:
			if( _cache->_objects.empty() )
			{
				node = new ObjectNode(category);
				break;
		    }

            node = _cache->_objects.back();
			_cache->_objects.pop_back();
			node->setCategory(category);
            break;
            
        default:
            node = 0;
    }
    
    return node;
}


void SerializationContext::push(SerializationNode* node)
{
    switch( node->category() )
    {
        case SerializationInfo::Scalar:
            if(_cache->_scalars.size() < _cache->_limit)
                _cache->_scalars.push_back(node);
            else
                delete node;
            break;

    	case SerializationInfo::Struct: 
    	case SerializationInfo::Sequence:
    	    if(_cache->_objects.size() < _cache->_limit)
                _cache->_objects.push_back(node);
            else
                delete node;
            break;

    	case SerializationInfo::Reference:
    	    if(_cache->_refs.size() < _cache->_limit)
                _cache->_refs.push_back(node);
            else
                delete node;
            break;

        default:
            delete node;
    }
}

} // namespace Pt
