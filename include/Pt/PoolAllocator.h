/*
 * Copyright (C) 2009-2010 by Bendri Batti
 * Copyright (C) 2009-2012 by Marc Boris Duerner
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

#ifndef PT_POOLALLOCATOR_H
#define PT_POOLALLOCATOR_H

#include <Pt/Api.h>
#include <Pt/Allocator.h>
#include <Pt/Types.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <cassert>
#include <cstddef>

namespace Pt {

/** @brief Memory pool for objects of the same size.

    If memory of uniform sizes has to be allocated, a @link Pt::MemoryPool
    MemoryPool @endlink can be used directly, rather than indirectly as part
    of the @link Pt::PoolAllocator PoolAllocator@endlink. This can be faster,
    because the PoolAllocator has to look up the pool for the requested
    size of memory each time it allocates and deallocates. To construct a 
    MemoryPool, the size of the records, i.e. the size of memory it can
    allocate, has to be specified.

    @code
    Pt::MemoryPool pool( sizeof(float), 4096 );

    void* p = pool.allocate();
    float* f = new (p) float(3.1415);

    pool.deallocate(f);
    @endcode

    Optionally, the maximum size of the blocks in the pool can be controlled.
    In the example shown above, the pool can only allocate memory of the size
    required for a float. Each time the pool itself requires more memory,
    it will allocate a new block of 4096 bytes.

    @ingroup Allocator
*/
class PT_API MemoryPool : public NonCopyable
{
    typedef std::size_t Record;
    static const Record RecordSize = sizeof(Record);
    static const Record InvalidIndex = std::size_t(-1);

    //! @internal
    class Block
    {
            Record* block;
            std::size_t firstFreeIndex;
            std::size_t unitSize;
            std::size_t availUnits;
            std::size_t endIndex;
            std::size_t maxUnits;
        
        public:
            Block(std::size_t unitSize_, std::size_t numUnits)
            : block(0)
            , firstFreeIndex(InvalidIndex)
            , unitSize(unitSize_)
            , availUnits(numUnits)
            , endIndex(0)
            , maxUnits(numUnits)
            {}
        
            bool isFull() const
            { return availUnits == 0; }

            bool isEmpty() const
            { return availUnits == maxUnits; }   

            void clear()
            {
                delete[] block;
                block = 0;
                firstFreeIndex = InvalidIndex;
            }
        
            Record* allocate()
            {
                assert(availUnits > 0);

                if( firstFreeIndex != InvalidIndex )
                {
                    assert(firstFreeIndex < endIndex);
                    Record* retval = block + firstFreeIndex;
                    firstFreeIndex = *retval;
                    --availUnits;
                    return retval;
                }

                if( ! block )
                {
                    block = new Record[maxUnits*unitSize];
                    endIndex = 0;
                }
        
                Record* retval = block + endIndex;
                endIndex += unitSize;

                assert(endIndex <= maxUnits*unitSize);
                --availUnits;
                return retval;
            }
        
            void deallocate(Record* ptr)
            {
                assert(availUnits <= maxUnits);

                *ptr = firstFreeIndex;
                firstFreeIndex = ptr - block;
                assert( ptr >= block );
                assert( ptr <= (block + endIndex) );
                ++availUnits;
            }
    };

    public:
        //! @brief Construct with element size and maximum page size.
        MemoryPool(std::size_t elemSize, std::size_t maxPageSize = 8192);

        //! @brief Destructor.
        ~MemoryPool();
        
        //! @brief Allocates memory.
        void* allocate()
        {
            if( _freelist.empty() )
            {
                _freelist.push_back( _blocks.size() );
                _blocks.push_back( Block(_recordsPerUnit, _maxUnits) );
            }
            
            const std::size_t index = _freelist.back();
            Block& block = _blocks[index];

            Record* retval = block.allocate();
            *retval = index;
            ++retval;
            
            if(block.isFull())
                _freelist.pop_back();
            
            return retval;
        }
        
        //! @brief deallocates memory.
        void deallocate(void* ptr)
        {
            if( ! ptr )
                return;
            
            Record* unitPtr = reinterpret_cast<Record*>(ptr);
            --unitPtr;

            const std::size_t blockIndex = *unitPtr;
            Block& block = _blocks[blockIndex];
            
            if( block.isFull() )
                _freelist.push_back(blockIndex);

            block.deallocate(unitPtr);

            // keep the first block
            if(  block.isEmpty() && blockIndex > 0 )
                block.clear();
        }

    private:
        std::vector<Block> _blocks;
        std::vector<std::size_t> _freelist;

        //! @internal @brief Number of records to store one element and the control record
        std::size_t _recordsPerUnit;
        std::size_t _maxUnits;
};

/** @brief Pool based allocator.

    The @link Pt::PoolAllocator PoolAllocator@endlink uses pools to allocate
    memory. Each pool consists of blocks of equally sized records, which can
    be used for allocations up to the size of a record. The record sizes
    increase from pool to pool. When memory is allocated, a record is used from
    the pool, which handles the requested size. When memory is deallocated, the
    record is returned to the corresponding pool. This method of allocation is
    effective, because larger blocks of memory are allocated and then reused in
    the form of many smaller records. An advantage of this kind of allocator,
    compared to free list based allocators, is that it is able to release
    completely unused blocks.

    @code
    // Contruct with max. record size, alignment and block size
    Pt::PoolAllocator allocator(32, 8, 4096);

    // will use a record from the pools
    void* p1 = allocator.allocate( sizeof(float) );

    // too large, will use operator new
    void* p2 = allocator.allocate( 64 );
    @endcode

    When a PoolAllocator is constructed, the maximum size for records has
    to be specified. The reason for this is that this type of allocator is
    ineffective for large allocations. Therefore, memory which is larger
    than this limit will be allocated using the new operator, instead of a
    record from a memory pool. Optionally, the alignment and the maximum
    block size can be set. The record sizes of the pools will be multiples
    of the alignment. So if the alignment is 8, the first pool will have
    records of size 8, the second pool records of size 16 and so forth, until
    the maximum size is reached. The maximum block size controls the number
    of records per block. A new block of records is added, when a pool is
    depleted and has to be extended to allow more allocations.
    
    @ingroup Allocator
*/
class PT_API PoolAllocator : public Allocator 
                           , protected NonCopyable
{
    public:
        //! @brief Contruct with maximum record size, alignment and block size.
        PoolAllocator(std::size_t maxSize, std::size_t align = 16, std::size_t maxBlock = 8192);
        
        //! @brief Destructor.
        ~PoolAllocator();

        // inherit docs
        void* allocate(std::size_t size)
        {
            if (size > _maxObjectSize || 0 == size)
            {
                return ::operator new( size );
            }
        
            const std::size_t index = (size-1) / _objectAlignSize;

            assert (index < _pools.size() );
            MemoryPool* pool = _pools[index];
            return pool->allocate();
        }
        
        // inherit docs
        void deallocate(void* p, std::size_t size)
        {
            if (size > _maxObjectSize || NULL == p)
            {
                ::operator delete(p);
                return;
            }

            assert(size > 0);

            const std::size_t index = (size-1) / _objectAlignSize;
            assert (index < _pools.size() );
            MemoryPool* pool = _pools[index];
            pool->deallocate(p);
        }

    private:
        // TODO: use vector<PoolStorage>
        // struct PoolStorage 
        // {
        //     char buffer[sizeof(MemoryPool)];
        // };
        //
        std::vector<MemoryPool*> _pools;

        //! @internal @brief Largest object size supported by allocators.
        const std::size_t _maxObjectSize;
    
        //! @internal @brief Size of alignment boundaries.
        const std::size_t _objectAlignSize;
};

} // namespace Pt

#endif
