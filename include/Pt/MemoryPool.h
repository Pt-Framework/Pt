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

#ifndef PT_MEMORYPOOL_H
#define PT_MEMORYPOOL_H

#include <Pt/MemoryBlock.h>
#include <vector>
#include <iostream>
#include <cassert>

namespace Pt{

class MemoryPool
{
public:
    /// Create a MemoryPool which manages blocks of 'blockSize' size.
    MemoryPool();

    /// Destroy the MemoryPool and release all its MemoryBlocks.
    ~MemoryPool();

    /// Initializes a MemoryPool by calculating # of blocks per MemoryBlock.
    void init(std::size_t blockSize, std::size_t pageSize);

    /** 
     * @brief Returns pointer to allocated memory block of fixed size - or NULL
     * if it failed to allocate.
     */
    void* allocate();

    /** 
     * @brief Deallocate a memory block previously allocated with Allocate.  If
     * the block is not owned by this MemoryPool, it returns false so
     * that PoolAllocator can call the default deallocator.  If the
     * block was found, this returns true.
     */
    bool deallocate(void* p);

    /**
     * @brief Returns block size with which the MemoryPool was initialized.
     */
    std::size_t blockSize()const{ return _blockSize - sizeof(std::size_t); }

    /** 
     * @brief Releases the memory used by the empty MemoryBlock.  
     * This will take constant time under any situation.
     * @return True if empty chunk found and released, false if none empty.
     */
    bool trimEmptyMemoryBlock();

    /** 
     * @brief Releases unused spots from MemoryBlockList.  
     * This takes constant time with respect to # of MemoryBlocks, 
     * but actual time depends on underlying memory allocator.
     * @return False if no unused spots, true if some found and released.
     */
    bool trimMemoryBlockList();

#ifndef NDEBUG	
    /** 
     * @brief Determines if MemoryPool is corrupt.  
     * Checks data members to see if any have erroneous values, or violate class invariants.  
     * It also checks if any MemoryBlock is corrupt.  Complexity is O(C) where C is
     * the number of MemoryBlocks.  If any data is corrupt, this will return true
     * in release mode, or assert in debug mode.
     * @return True if MemoryPool is corrupt in Release mode, or assert in debug mode.
     */
    bool isCorrupt(void) const;
	
    /** @brief Returns count of empty MemoryBlocks held by this allocator.  
     * Complexity is O(C) where C is the total number of MemoryBlocks - empty or used.
     * @return count of empty MemoryBlocks held by this allocator.
     */
    std::size_t countEmptyMemoryBlocks( void ) const;
#endif

    /** 
     * @brief Returns true if the block at address p is within a MemoryBlock owned by
     * this MemoryPool.  Complexity is O(C) where C is the total number
     * of MemoryBlocks - empty or used.
     * @param[in] p Represents the address of the memory to search.
     * @return True if block at address p is within a MemoryBlock owned by this MemoryPool.
     */
     bool hasBlock(void* p) const;
	 	 


private:

    /** 
     * @brief Deallocates the block at address p, and then handles the internal
     * bookkeeping needed to maintain class invariants.  This assumes that
     * deallocMemoryBlock_ points to the correct chunk.
     * @param[in] p represents the block at address p.
     */
    void doDeallocate(void * p);

    /** 
     * @brief Creates an empty MemoryBlock and adds it to the end of the MemoryBlockList.
     * All calls to the lower-level memory allocation functions occur inside
     * this function, and so the only try-catch block is inside here.
     */
    void createMemoryBlock( void );

    /** 
     * Finds the MemoryBlock which owns the block at address p.  It starts at
     * deallocMemoryBlock_ and searches in both forwards and backwards directions
     * from there until it finds the MemoryBlock which owns p.  This algorithm
     * should find the MemoryBlock quickly if it is deallocMemoryBlock_ or is close to it
     * in the MemoryBlocks container.  This goes both forwards and backwards since
     * that works well for both same-order and opposite-order deallocations.
     * (Same-order = objects are deallocated in the same order in which they
     * were allocated.  Opposite order = objects are deallocated in a last to
     * first order.  Complexity is O(C) where C is count of all MemoryBlocks.  This
     * never throws.
     * @param[in] p is the address of memory to search.
     * @return Pointer to MemoryBlock that owns p, or NULL if no owner found.
     */
    MemoryBlock* findMemoryBlock(void* p) const;

    /// Not implemented.
    MemoryPool(const MemoryPool&);

    /// Not implemented.
    MemoryPool& operator=(const MemoryPool&);

    /// Type of container used to hold MemoryBlocks.
    typedef std::vector<MemoryBlock> MemoryBlocks;

    /// Iterator through container of MemoryBlocks.
    typedef MemoryBlocks::iterator MemoryBlockIter;

    /// Iterator through const container of MemoryBlocks.
    typedef MemoryBlocks::const_iterator MemoryBlockCIter;

    /// Fewest # of objects managed by a MemoryBlock.
    static Pt::uint8_t _MinObjectsPerMemoryBlock;

    /// Most # of objects managed by a MemoryBlock - never exceeds UCHAR_MAX.
    static Pt::uint8_t _MaxObjectsPerMemoryBlock;

    /// Number of bytes in a single block within a MemoryBlock.
    std::size_t _blockSize;

    /// Number of blocks managed by each MemoryBlock.
    Pt::uint8_t _numBlocks;

    /// Container of MemoryBlocks.
    MemoryBlocks _chunks;

    /// Pointer to MemoryBlock used for last or next allocation.
    MemoryBlock* _allocMemoryBlock;

    /// Pointer to MemoryBlock used for last or next deallocation.
    MemoryBlock* _deallocMemoryBlock;

    /// Pointer to the only empty MemoryBlock if there is one, else NULL.
    MemoryBlock* _emptyMemoryBlock;
};


class MemPool
{
    typedef std::size_t Record;
    static const Record RecordSize = sizeof(Record);
    static const Record InvalidIndex = std::size_t(-1);

    class MemBlock
    {
            Record* block;
            std::size_t firstFreeIndex;
            std::size_t availUnits;
            std::size_t endIndex;
            std::size_t unitSize;
            std::size_t maxUnits;
        
        public:
            MemBlock(std::size_t unitSize_, std::size_t numUnits)
            : block(0)
            , firstFreeIndex(InvalidIndex)
            , unitSize(unitSize_)
            , availUnits(numUnits)
            , maxUnits(numUnits)
            {}
        
            bool isFull() const
            {
                return availUnits == 0;
            }

            bool isEmpty() const
            {
                return availUnits == maxUnits;
            }   

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
        MemPool(std::size_t elemSize, std::size_t maxPageSize = 8192)
        : _recordsPerUnit(((elemSize + (RecordSize - 1)) / RecordSize) + 1)
        , _maxUnits(maxPageSize / (_recordsPerUnit * RecordSize))
        {
            _blocks.reserve(16); 
        }

        ~MemPool()
        {
            for(std::size_t i = 0; i < _blocks.size(); ++i)
            {
                assert( _blocks[i].isEmpty() );
                _blocks[i].clear();
            }
        }
        
        void* allocate()
        {
            if( _freelist.empty() )
            {
                _freelist.push_back( _blocks.size() );
                _blocks.push_back( MemBlock(_recordsPerUnit, _maxUnits) );
            }
            
            const std::size_t index = _freelist.back();
            MemBlock& block = _blocks[index];

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
            MemBlock& block = _blocks[blockIndex];
            
            if( block.isFull() )
                _freelist.push_back(blockIndex);

            block.deallocate(unitPtr);

            // keep the first block
            if(  block.isEmpty() && blockIndex > 0 )
                block.clear();
        }

    private:
        std::vector<MemBlock> _blocks;
        std::vector<std::size_t> _freelist;
        std::size_t _recordsPerUnit; /// Number of records to store one element and the control record
        std::size_t _maxUnits;
};


class MemPoolAllocator
{
    public:
        MemPoolAllocator(std::size_t maxElemSize, std::size_t step = 16, std::size_t maxPagesize = 8192)
        : _maxObjectSize(maxElemSize)
        , _objectAlignSize(step)
        {
            assert( 0 != _objectAlignSize );

            const std::size_t numPools = (_maxObjectSize + _objectAlignSize - 1) / _objectAlignSize;

            for (std::size_t i = 1; i <= numPools; ++i)
            {
                _pools.push_back( new MemPool(i * _objectAlignSize, maxPagesize) );
            }

            assert(numPools == _pools.size());
        }
        
        ~MemPoolAllocator()
        {
            assert( 0 != _objectAlignSize );

            std::vector<MemPool*>::iterator it;
            for(it = _pools.begin(); it != _pools.end(); ++it)
            {
                delete *it;
            }
        }


        void* allocate(std::size_t size)
        {
            if (size > _maxObjectSize || 0 == size)
            {
                return ::operator new( size );
            }
        
            const std::size_t index = (size-1) / _objectAlignSize;

            assert (index < _pools.size() );
            MemPool* pool = _pools[index];
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
               
            // WTF ???
            //if (0 == size)
            //{
            //    size = 1;
            //}
        
            const std::size_t index = (size-1) / _objectAlignSize;
            assert (index < _pools.size() );
            MemPool* pool = _pools[index];
            pool->deallocate(p);
        }

    private:
        std::vector<MemPool*> _pools;

        /**
         * @brief Largest object size supported by allocators.
         */
        const std::size_t _maxObjectSize;
    
        /**
         * @brief Size of alignment boundaries.
         */
        const std::size_t _objectAlignSize;
};

}

#endif // PT_MEMORYPOOL_H

