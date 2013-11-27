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

#include <Pt/SerializationContext.h>
#include <Pt/SerializationSurrogate.h>
#include <Pt/SerializationError.h>
#include <Pt/PoolAllocator.h>
#include <map>
#include <cassert>

namespace Pt {

class SerializationContextImpl
{
    public:
        SerializationContextImpl()
        : _alloc(sizeof(SerializationInfo))

        { }

        MemoryPool _alloc;
        std::map<Pt::TypeInfo, SerializationSurrogate*> _surrmap;
};


SerializationContext::SerializationContext()
: _cache(0)
, _refsEnabled(false)
{
    SerializationInfo::setContextual(*this);
    _cache = new SerializationContextImpl;
}


SerializationContext::~SerializationContext()
{
    std::map<Pt::TypeInfo, SerializationSurrogate*>::iterator siter;
    for(siter = _cache->_surrmap.begin(); siter != _cache->_surrmap.end(); ++siter)
    {
        delete siter->second;
    }

    delete _cache;
}


SerializationInfo* SerializationContext::get()
{
    void* m = _cache->_alloc.allocate();
    return new (m) SerializationInfo(this);
}


void SerializationContext::push(SerializationInfo* si)
{
    si->~SerializationInfo();
    _cache->_alloc.deallocate(si);
}


void SerializationContext::registerSurrogate(const std::type_info& ti, SerializationSurrogate* surrogate)
{
    std::map<Pt::TypeInfo, SerializationSurrogate*>::iterator it = _cache->_surrmap.find( ti );
    if( it != _cache->_surrmap.end() )
    {
        delete it->second;
    }

    _cache->_surrmap[ ti ] = surrogate;
}


const SerializationSurrogate* SerializationContext::getSurrogate(const std::type_info& ti) const
{
    std::map<Pt::TypeInfo, SerializationSurrogate*>::const_iterator it = _cache->_surrmap.find(ti);
    if( it != _cache->_surrmap.end() )
    {
        return it->second;
    }

    return 0;
}


void SerializationContext::onClear()
{
}


bool SerializationContext::onBeginSave(const void* p, const char* name)
{
    return true;
}


void SerializationContext::onFinishSave()
{
}


void SerializationContext::onPrepareId(const void* p)
{
    throw SerializationError("missing unlink information");
}


const char* SerializationContext::onGetId(const void* p)
{
    throw SerializationError("missing unlink information");
    return 0;
}


const char* SerializationContext::onMakeId(const void* p)
{
    return "";
}


void SerializationContext::onBeginLoad(void* obj, const std::type_info& fixupInfo,
                                       const char* name, const char* id)
{
}


void SerializationContext::onFinishLoad()
{
}


void SerializationContext::onRebindTarget(const char* id, void* obj)
{
}


void SerializationContext::onRebindFixup(const char* id, void* obj, void* prev)
{
}


void SerializationContext::onPrepareFixup( void* obj, const char* id, FixupInfo::FixupHandler, unsigned mid)
{
}


void SerializationContext::onFixup()
{
}

} // namespace Pt
