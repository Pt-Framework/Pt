/*
 * Copyright (C) 2009-2010 by Bendri Batti
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

#include "Pt/PoolAllocator.h"
#include "Pt/MemoryPool.h"
#include "Pt/MemoryBlock.h"

#include <cassert>

namespace {

/**
 * @brief Calculates index into array where a MemoryPool of numBytes is located.
 */
 inline std::size_t getOffset(std::size_t numBytes, std::size_t alignment)
 {
    const std::size_t alignExtra = alignment - 1;
    return (numBytes + alignExtra) / alignment;
 }

};

namespace Pt {

PoolAllocator::PoolAllocator(std::size_t pageSize, std::size_t maxObjectSize,
                             std::size_t objectAlignSize):
_pool(0),
_maxObjectSize(maxObjectSize),
_objectAlignSize(objectAlignSize)
{
    assert( 0 != objectAlignSize );
    const std::size_t allocCount = getOffset( maxObjectSize, objectAlignSize );
    _pool = new MemoryPool[allocCount];
    for (std::size_t i = 0; i < allocCount; ++i)
    {
        _pool[ i ].init((i+1)*objectAlignSize, pageSize);
    }
}

PoolAllocator::~PoolAllocator(void)
{
    assert( 0 != _objectAlignSize );
    delete [] _pool;
}

void* PoolAllocator::allocate(std::size_t size)
{
    if (size > getMaxObjectSize() || 0 == size)
    {
        return ::operator new( size );
    }

    assert(NULL != _pool);

    const std::size_t index = getOffset( size, getAlignment() ) - 1;

#ifndef NDEBUG
    const std::size_t allocCount = getOffset(getMaxObjectSize(), getAlignment());
    assert(index < allocCount);
#endif

    MemoryPool& pool = _pool[ index ];
    assert(pool.blockSize() >= size);
    assert(pool.blockSize() < size + getAlignment());

    return pool.allocate();
}

void PoolAllocator::deallocate(void* p, std::size_t size)
{
    if (size > getMaxObjectSize() || NULL == p)
    {
        ::operator delete(p);
        return;
    }

    assert(NULL != _pool);

    if (0 == size)
    {
        size = 1;
    }

    const std::size_t index = getOffset(size, getAlignment()) - 1;

#ifndef NDEBUG
    const std::size_t allocCount = getOffset(getMaxObjectSize(), getAlignment());
    assert(index < allocCount);
#endif

    MemoryPool& allocator = _pool[ index ];
    assert(allocator.blockSize() >= size);
    assert(allocator.blockSize()  < size + getAlignment());
    const bool found = allocator.deallocate(p);

    (void) found;
    assert( found );
}

bool PoolAllocator::trim( void )
{
    bool found = false;
    const std::size_t allocCount = getOffset(getMaxObjectSize(), getAlignment());
    std::size_t i = 0;

    for ( ; i < allocCount; ++i )
    {
        if (_pool[ i ].trimEmptyMemoryBlock())
        {
            found = true;
        }
    }
    
    for ( i = 0; i < allocCount; ++i )
    {
        if (_pool[ i ].trimMemoryBlockList())
        {
            found = true;
        }
    }

    return found;
}

bool PoolAllocator::isCorrupt() const
{
#ifndef NDEBUG
    if (NULL == _pool)
    {
        assert(false);
        return true;
    }

    if (0 == getAlignment())
    {
        assert(false);
        return true;
    }

    if (0 == getMaxObjectSize())
    {
        assert( false );
        return true;
    }

    const std::size_t allocCount = getOffset(getMaxObjectSize(), getAlignment());
    for (std::size_t i = 0; i < allocCount; ++i )
    {
        if (_pool[i].isCorrupt() )
        {
            return true;
        }
    }
#endif	
    return false;
}

}
