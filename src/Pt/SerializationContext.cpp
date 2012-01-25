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
#include "Pt/MemoryPool.h"

#include <map>
#include <cassert>

namespace Pt {

namespace
{
    // The BlockAllocator allocates multipe chunks of memory at once
    // and returns on each call to allocate the next available chunk.
    // The chunks are not deallocated nor recycled, but the memory
    // is released, when the BlockAllocator is released.
    //
    // This fits perfectly well into the serialiation context, since
    // a bunch of SerializationInfo objects are needed for serialization
    // and are released all at once, when the serialation is finished.

    template <typename T>
    class BlockAllocator
    {
            BlockAllocator(const BlockAllocator&) { }
            BlockAllocator& operator=(const BlockAllocator&)  { return *this; }

        public:
            explicit BlockAllocator(size_t numElementsPerBlock)
                : _blockSize(numElementsPerBlock * sizeof(T)),
                  _offset(_blockSize),
                  _count(0)
            {
                assert(numElementsPerBlock > 0);
            }

            ~BlockAllocator();

            void* allocate();
            void deallocate(void *);

        private:
            size_t _blockSize;
            size_t _offset;
            unsigned _count;
            std::vector<char*> _memory;
    };

    template <typename T>
    BlockAllocator<T>::~BlockAllocator()
    {
        for (std::vector<char*>::iterator it = _memory.begin(); it != _memory.end(); ++it)
            delete[] *it;
    }

    template <typename T>
    void* BlockAllocator<T>::allocate()
    {
        if (_offset >= _blockSize)
        {
            _memory.push_back( new char[_blockSize] );
            _offset = 0;
        }

        void* ptr = static_cast<void*>(_memory.back() + _offset);

        _offset += sizeof(T);
        ++_count;

        return ptr;
    }

    template <typename T>
    void BlockAllocator<T>::deallocate(void*)
    {
        if  (--_count == 0)
        {
            for (std::vector<char*>::iterator it = _memory.begin(); it != _memory.end(); ++it)
                delete[] *it;
            _memory.clear();
            _offset = _blockSize;
        }
    }

}

#define NEWPOOL
//#define OLDPOOL
//#define FREELIST

class SerializationContextImpl
{
    public:
        SerializationContextImpl()
        : _limit(64)
#ifdef FREELIST
        , _alloc(64) // block allocator
#endif

#ifdef NEWPOOL
        , _alloc(sizeof(SerializationInfo))
#endif
        {
#ifdef OLDPOOL
            _alloc.init(sizeof(SerializationInfo), 4096);
#endif
        }

        size_t _limit;

#ifdef FREELIST
        BlockAllocator<SerializationInfo> _alloc;
#endif

#ifdef NEWPOOL
        MemPool _alloc;
#endif

#ifdef OLDPOOL
        MemoryPool _alloc;
#endif
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

} // namespace Pt
