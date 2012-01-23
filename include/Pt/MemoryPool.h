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

}

#endif // PT_MEMORYPOOL_H

