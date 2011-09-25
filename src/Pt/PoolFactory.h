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

#ifndef PT_POOL_FACTORY_H
#define PT_POOL_FACTORY_H

#include "Chunk.h"

#include <vector>

namespace Pt{

class PoolFactory
{
public:
    /// Create a PoolFactory which manages blocks of 'blockSize' size.
    PoolFactory();

    /// Destroy the PoolFactory and release all its Chunks.
    ~PoolFactory();

    /// Initializes a PoolFactory by calculating # of blocks per Chunk.
    void init(std::size_t blockSize, std::size_t pageSize);

    /** 
     * @brief Returns pointer to allocated memory block of fixed size - or NULL
     * if it failed to allocate.
     */
    void* allocate();

    /** 
     * @brief Deallocate a memory block previously allocated with Allocate.  If
     * the block is not owned by this PoolFactory, it returns false so
     * that PoolAllocator can call the default deallocator.  If the
     * block was found, this returns true.
     */
    bool deallocate(void* p);

    /**
     * @brief Returns block size with which the PoolFactory was initialized.
     */
    std::size_t blockSize()const{ return _blockSize - sizeof(std::size_t); }

    /** 
     * @brief Releases the memory used by the empty Chunk.  
     * This will take constant time under any situation.
     * @return True if empty chunk found and released, false if none empty.
     */
    bool trimEmptyChunk();

    /** 
     * @brief Releases unused spots from ChunkList.  
     * This takes constant time with respect to # of Chunks, 
     * but actual time depends on underlying memory allocator.
     * @return False if no unused spots, true if some found and released.
     */
    bool trimChunkList();

#ifndef NDEBUG	
    /** 
     * @brief Determines if PoolFactory is corrupt.  
     * Checks data members to see if any have erroneous values, or violate class invariants.  
     * It also checks if any Chunk is corrupt.  Complexity is O(C) where C is
     * the number of Chunks.  If any data is corrupt, this will return true
     * in release mode, or assert in debug mode.
     * @return True if PoolFactory is corrupt in Release mode, or assert in debug mode.
     */
    bool isCorrupt(void) const;
	
    /** @brief Returns count of empty Chunks held by this allocator.  
     * Complexity is O(C) where C is the total number of Chunks - empty or used.
     * @return count of empty Chunks held by this allocator.
     */
    std::size_t countEmptyChunks( void ) const;
#endif

    /** 
     * @brief Returns true if the block at address p is within a Chunk owned by
     * this PoolFactory.  Complexity is O(C) where C is the total number
     * of Chunks - empty or used.
     * @param[in] p Represents the address of the memory to search.
     * @return True if block at address p is within a Chunk owned by this PoolFactory.
     */
     bool hasBlock(void* p) const;
	 	 


private:

    /** 
     * @brief Deallocates the block at address p, and then handles the internal
     * bookkeeping needed to maintain class invariants.  This assumes that
     * deallocChunk_ points to the correct chunk.
     * @param[in] p represents the block at address p.
     */
    void doDeallocate(void * p);

    /** 
     * @brief Creates an empty Chunk and adds it to the end of the ChunkList.
     * All calls to the lower-level memory allocation functions occur inside
     * this function, and so the only try-catch block is inside here.
     */
    void createChunk( void );

    /** 
     * Finds the Chunk which owns the block at address p.  It starts at
     * deallocChunk_ and searches in both forwards and backwards directions
     * from there until it finds the Chunk which owns p.  This algorithm
     * should find the Chunk quickly if it is deallocChunk_ or is close to it
     * in the Chunks container.  This goes both forwards and backwards since
     * that works well for both same-order and opposite-order deallocations.
     * (Same-order = objects are deallocated in the same order in which they
     * were allocated.  Opposite order = objects are deallocated in a last to
     * first order.  Complexity is O(C) where C is count of all Chunks.  This
     * never throws.
     * @param[in] p is the address of memory to search.
     * @return Pointer to Chunk that owns p, or NULL if no owner found.
     */
    Chunk* findChunk(void* p) const;

    /// Not implemented.
    PoolFactory(const PoolFactory&);

    /// Not implemented.
    PoolFactory& operator=(const PoolFactory&);

    /// Type of container used to hold Chunks.
    typedef std::vector<Chunk> Chunks;

    /// Iterator through container of Chunks.
    typedef Chunks::iterator ChunkIter;

    /// Iterator through const container of Chunks.
    typedef Chunks::const_iterator ChunkCIter;

    /// Fewest # of objects managed by a Chunk.
    static Pt::uint8_t _MinObjectsPerChunk;

    /// Most # of objects managed by a Chunk - never exceeds UCHAR_MAX.
    static Pt::uint8_t _MaxObjectsPerChunk;

    /// Number of bytes in a single block within a Chunk.
    std::size_t _blockSize;

    /// Number of blocks managed by each Chunk.
    Pt::uint8_t _numBlocks;

    /// Container of Chunks.
    Chunks _chunks;

    /// Pointer to Chunk used for last or next allocation.
    Chunk* _allocChunk;

    /// Pointer to Chunk used for last or next deallocation.
    Chunk* _deallocChunk;

    /// Pointer to the only empty Chunk if there is one, else NULL.
    Chunk* _emptyChunk;
};

}
#endif

