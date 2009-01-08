
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

/** 
 * @brief Calls the default allocator when PoolAllocator decides not to handle a
 * request.  
 * 
 * PoolAllocator calls this if the number of bytes is bigger than
 * the size which can be handled by any PoolFactory.
 *
 * @param[in] numBytes Number of bytes
 * @param[in] doThrow True if this function should throw an exception, or false if it
 * should indicate failure by returning a NULL pointer.
 */
void* defaultAllocator(std::size_t numBytes, bool doThrow)
{
#ifdef USE_NEW_TO_ALLOCATE
    return doThrow ? ::operator new( numBytes ) :
        ::operator new( numBytes, std::nothrow_t() );
#else
    void * p = ::std::malloc( numBytes );
    if (doThrow && (NULL == p))
    {
        throw std::bad_alloc();
    }
    return p;
#endif
}

// DefaultDeallocator ---------------------------------------------------------
/** 
 * @brief Calls default deallocator when PoolAllocator decides not to handle a   
 * request.  
 * 
 * The default deallocator could be the global delete operator or the
 * free function.  The free function is the preferred default deallocator since
 * it matches malloc which is the preferred default allocator.  PoolAllocator
 * will call this if an address was not found among any of its own blocks.
 * @param[in] p Memory to be deallocated
 */
void defaultDeallocator( void * p )
{
#ifdef USE_NEW_TO_ALLOCATE
    ::operator delete( p );
#else
    ::std::free( p );
#endif
}

};

namespace Pt {
namespace Alloc{

PoolAllocator::PoolAllocator(std::size_t pageSize, std::size_t maxObjectSize,
                                     std::size_t objectAlignSize):
_pool(NULL),
_maxObjectSize(maxObjectSize),
_objectAlignSize(objectAlignSize)
{
#ifdef DO_EXTRA_PT_TESTS
    std::cout << "PoolAllocator " << this << std::endl;
#endif
    assert( 0 != objectAlignSize );
    const std::size_t allocCount = getOffset( maxObjectSize, objectAlignSize );
    _pool = new(std::nothrow)PoolFactory[allocCount];
    for (std::size_t i = 0; i < allocCount; ++i)
    {
        _pool[ i ].init((i+1)*objectAlignSize, pageSize);
    }
}

PoolAllocator::~PoolAllocator(void)
{
#ifdef DO_EXTRA_PT_TESTS
    std::cout << "~PoolAllocator " << this << std::endl;
#endif
    assert( 0 != _objectAlignSize );
    delete [] _pool;
}

void* PoolAllocator::allocate(std::size_t size, bool doThrow)
{
    if (size > getMaxObjectSize())
    {
        return defaultAllocator( size, doThrow );
    }
    assert(NULL != _pool);
    if (0 == size)
    {
        size = 1;
    }
    const std::size_t index = getOffset(size, getAlignment()) - 1;
    const std::size_t allocCount = getOffset(getMaxObjectSize(), getAlignment());
    (void) allocCount;
    assert(index < allocCount);

    PoolFactory& allocator = _pool[ index ];
    assert(allocator.blockSize() >= size);
    assert(allocator.blockSize() < size + getAlignment());
    void* place = allocator.allocate();

    if ((NULL == place) && trimExcessMemory() )
    {
        place = allocator.allocate();
    }

    if ((NULL == place) && doThrow)
    {
    #ifdef _MSC_VER
        throw std::bad_alloc( "could not allocate small object" );
    #else
        // GCC did not like a literal string passed to std::bad_alloc.
        // so just throw the default-constructed exception.
        throw std::bad_alloc();
    #endif
    }
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
        defaultDeallocator( p );
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

void PoolAllocator::deallocate(void* p)
{   
    if (NULL == p)
    {
        return;
    }
    assert(NULL != _pool);
    const std::size_t allocCount = getOffset(getMaxObjectSize(), getAlignment());
    bool found = false;

    for (std::size_t i = 0; i < allocCount; ++i )
    {
         if (_pool[i].deallocate(p))
         {
             found = true;
             break;
         }
    }

    assert(found);

    if (false == found)
    {
        defaultDeallocator(p);
        return;
    }
    else
    {
        p = NULL;
    }
}

bool PoolAllocator::trimExcessMemory( void )
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
    return false;
}

}
}
