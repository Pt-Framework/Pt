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
#include "Pt/SerializationSurrogate.h"
#include "Pt/SerializationError.h"
#include "Pt/SerializationInfo.h"
#include "Chunk.h"

//#define ALLOCATOR 1

#ifdef ALLOCATOR
#include "Pt/PoolAllocator.h"
#include "Pt/PageAllocator.h"
#endif

#include <map>

namespace Pt {

class SerializationCache
{
    public:
        SerializationCache()
        : _limit(64)
#ifdef ALLOCATOR
        //,  _alloc(4096, 128, 8)
#endif
        {
#ifdef ALLOCATOR
            _chunkSi.init(sizeof(SerializationInfo), 128);
            _chunkNode.init( sizeof(ValueNode), 128 );
#endif
        }

        std::map<std::string, Pt::SerializationSurrogate> _surrogates;
        std::vector<SerializationInfo*> _infos;
        std::vector<SerializationNode*> _scalars;
        std::vector<SerializationNode*> _objects;
        std::vector<SerializationNode*> _refs;
        size_t _limit;
#ifdef ALLOCATOR
        Chunk _chunkSi;
        Chunk _chunkNode;
        //PoolAllocator _alloc;
#endif
};


bool SerializationContext::beginSave(const void* p, const std::string& name)
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


const char* SerializationContext::makeId(const void* p)
{
    return "";
}


void SerializationContext::beginLoad(void* obj, const std::type_info& fixupInfo,
                                     const std::string& name, const std::string& id)
{
}


void SerializationContext::finishLoad()
{
}


void SerializationContext::rebindTarget(const std::string& id, void* obj)
{
}


void SerializationContext::rebindFixup(const std::string& id, void* obj, void* prev)
{
}


void SerializationContext::prepareFixup( void* obj, const std::string& id, FixupHandler, unsigned mid)
{
}


void SerializationContext::fixup()
{
}


void SerializationContext::reset()
{
}


SerializationContext::SerializationContext()
: _cache(0)
, _refsEnabled(false)
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
#ifdef ALLOCATOR
    //void* m = _cache->_alloc.allocate( sizeof(SerializationInfo) );
    void* m = _cache->_chunkSi.allocate( sizeof(SerializationInfo) );
    return new (m) SerializationInfo(this);
#else
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
#endif
}


void SerializationContext::push(SerializationInfo* si)
{
#ifdef ALLOCATOR
    si->~SerializationInfo();
    //_cache->_alloc.deallocate(si, sizeof(SerializationInfo));
    _cache->_chunkSi.deallocate(si, sizeof(SerializationInfo));
#else
    SerializationNode* node = si->releaseNode();

    if(node)
        this->push(node);

    si->clear();

    if(_cache->_infos.size() < _cache->_limit)
    {
        _cache->_infos.push_back(si);
    }
    else
    {
        delete si;
    }
#endif
}


SerializationNode* SerializationContext::get(SerializationInfo::Category category)
{
    SerializationNode* node = 0;

    switch(category)
    {
        case SerializationInfo::Scalar:
        {
#ifdef ALLOCATOR
            //void* m = _cache->_alloc.allocate( sizeof(ValueNode) );
            void* m = _cache->_chunkNode.allocate( sizeof(ValueNode) );
            node = new (m) ValueNode();
#else
			if( _cache->_scalars.empty() )
			{
				node = new ValueNode();
				break;
		    }

            node = _cache->_scalars.back();
			_cache->_scalars.pop_back();
#endif
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

        case SerializationInfo::Context:
            node = new ContextNode();
            break;

        default:
            node = 0;
    }

    return node;
}


void SerializationContext::push(SerializationNode* node)
{
    node->clear(*this);

    switch( node->category() )
    {
        case SerializationInfo::Scalar:
#ifdef ALLOCATOR
            node->~SerializationNode();
            //_cache->_alloc.deallocate(node, sizeof(ValueNode));
            _cache->_chunkNode.deallocate(node, sizeof(ValueNode));
#else
            if(_cache->_scalars.size() < _cache->_limit)
                _cache->_scalars.push_back(node);
            else
                delete node;
#endif
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


void SerializationContext::setSurrogates(const char* name, Deflate def, Inflate inf)
{
    // keep function ptrs in Surrogate object and always use context
    // for temporary SerializationInfos
    _cache->_surrogates[name] = SerializationSurrogate(def, inf);
}


SerializationSurrogate SerializationContext::getSurrogate(const char* name)
{
    std::map<std::string, SerializationSurrogate>::const_iterator it;
    it = _cache->_surrogates.find(name);
    if( it == _cache->_surrogates.end() )
        return SerializationSurrogate();

    return it->second;
}

} // namespace Pt
