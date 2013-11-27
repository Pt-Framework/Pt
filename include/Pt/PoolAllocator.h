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

    @ingroup Allocator
*/
class PT_API MemoryPool : public NonCopyable
{
    typedef std::size_t Record;
    static const Record RecordSize = sizeof(Record);
    static const Record InvalidIndex = std::size_t(-1);

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
        MemoryPool(std::size_t elemSize, std::size_t maxPageSize = 8192);

        ~MemoryPool();
        
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

    @ingroup Allocator
*/
class PT_API PoolAllocator : public Allocator 
                           , protected NonCopyable
{
    public:
        PoolAllocator(std::size_t maxElemSize, std::size_t step = 16, std::size_t maxPagesize = 8192);
        
        ~PoolAllocator();

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
