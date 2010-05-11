
#include <Pt/Alloc/PoolFactory.h>
#include <Pt/Alloc/PoolAllocator.h>

#include <assert.h>

namespace {

/**
 * @brief Calculates index into array where a PoolFactory of numBytes is located.
 */
 std::size_t getOffset(std::size_t numBytes, std::size_t alignment)
 {
    const std::size_t alignExtra = alignment-1;
    return (numBytes + alignExtra) / alignment;
 }

};

namespace Pt {

namespace Alloc{

PoolAllocator::PoolAllocator(std::size_t pageSize, std::size_t maxObjectSize,
                                     std::size_t objectAlignSize):
_pool(0),
_maxObjectSize(maxObjectSize),
_objectAlignSize(objectAlignSize)
{
    assert( 0 != objectAlignSize );
    const std::size_t allocCount = getOffset( maxObjectSize, objectAlignSize );
    _pool = new PoolFactory[allocCount];
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
    if (size > getMaxObjectSize())
    {
        return ::operator new( size );
    }
    assert(NULL != _pool);
    if (0 == size)
    {
        return 0;
    }
    const std::size_t index = getOffset(size, getAlignment()) - 1;
    const std::size_t allocCount = getOffset(getMaxObjectSize(), getAlignment());
    assert(index < allocCount);

    PoolFactory& allocator = _pool[ index ];
    assert(allocator.blockSize() >= size);
    assert(allocator.blockSize() < size + getAlignment());
    void* place = allocator.allocate();

    return place;
}

void PoolAllocator::deallocate(void* p, std::size_t size)
{
    if (NULL == p)
    {
        return;
    }

    if (size > getMaxObjectSize())
    {
        ::operator delete(p);
        return;
    }

    assert(NULL != _pool);
    
    if (0 == size)
    {
        size = 1;
    }
    
    const std::size_t index       = getOffset(size, getAlignment()) - 1;
    const std::size_t allocCount  = getOffset(getMaxObjectSize(), getAlignment());
    (void) allocCount;
    assert(index < allocCount);
    PoolFactory& allocator     = _pool[ index ];
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
        if (_pool[ i ].trimEmptyChunk())
        {
            found = true;
        }
    }
    
    for ( i = 0; i < allocCount; ++i )
    {
        if (_pool[ i ].trimChunkList())
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

}
