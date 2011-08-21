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

#include "Pt/SerializationContext.h"
#include "Pt/SerializationSurrogate.h"
#include "Pt/SerializationError.h"
#include "Pt/SerializationInfo.h"

#define ALLOCATOR 1

#ifdef ALLOCATOR
#include "PoolFactory.h"
#endif

#include <map>

namespace Pt {

class SerializationCache
{
    public:
        SerializationCache()
        : _limit(64)
        {
#ifdef ALLOCATOR
            _alloc.init(sizeof(SerializationInfo), sizeof(SerializationInfo) * 64);
#endif
        }

        std::vector<SerializationInfo*> _infos;
        size_t _limit;
#ifdef ALLOCATOR
        PoolFactory _alloc;
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


void SerializationContext::rebindTarget(const char* id, void* obj)
{
}


void SerializationContext::rebindFixup(const std::string& id, void* obj, void* prev)
{
}


void SerializationContext::prepareFixup( void* obj, const std::string& id, FixupInfo::FixupHandler, unsigned mid)
{
}


void SerializationContext::fixup()
{
}


void SerializationContext::reset()
{
}


class MemSize : private Pt::SerializationInfo
{
    char where_align;
};

SerializationContext::SerializationContext()
: _cache(0)
, _refsEnabled(false)
{
    SerializationInfo::setContextual(*this);
    _cache = new SerializationCache;
}


SerializationContext::~SerializationContext()
{
    std::vector<SerializationInfo*>::iterator iter;
    for(iter = _cache->_infos.begin(); iter != _cache->_infos.end(); ++iter)
    {
        delete *iter;
    }

    delete _cache;

    std::map<Pt::TypeInfo, SerializationSurrogate*>::iterator siter;
    for(siter = _surrmap.begin(); siter != _surrmap.end(); ++siter)
    {
        delete siter->second;
    }
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
    void* m = _cache->_alloc.allocate();
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
    _cache->_alloc.deallocate(si);
#else
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


void SerializationContext::registerSurrogate(const std::type_info& ti, SerializationSurrogate* surrogate)
{
    std::map<Pt::TypeInfo, SerializationSurrogate*>::iterator it = _surrmap.find( ti );
    if( it != _surrmap.end() )
    {
        delete it->second;
    }

    _surrmap[ ti ] = surrogate;
}


const SerializationSurrogate* SerializationContext::getSurrogate(const std::type_info& ti) const
{
    std::map<Pt::TypeInfo, SerializationSurrogate*>::const_iterator it = _surrmap.find(ti);
    if( it != _surrmap.end() )
    {
        return it->second;
    }

    return 0;
}


// bool SerializationContext::decompose(SerializationInfo& si, const void* type, const std::type_info& ti) const
// {
//     std::map<Pt::TypeInfo, SerializationSurrogate*>::const_iterator it = _surrmap.find(ti);
//     bool found = ( it != _surrmap.end() );

//     if(found)
//     {
//         const SerializationSurrogate* surrogate = it->second;
//         surrogate->decompose(si, type);
//         si.setTypeName( surrogate->typeName() );
//     }

//     return found;
// }


// bool SerializationContext::compose(const SerializationInfo& si, void* type, const std::type_info& ti) const
// {
//     std::map<Pt::TypeInfo, SerializationSurrogate*>::const_iterator it = _surrmap.find(ti);
//     bool found = ( it != _surrmap.end() );

//     if(found)
//     {
//         const SerializationSurrogate* surrogate = it->second;
//         surrogate->compose(si, type);
//     }

//     return found;
// }

} // namespace Pt
