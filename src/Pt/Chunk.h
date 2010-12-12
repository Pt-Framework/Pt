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
#ifndef PT_CHUNK_H
#define PT_CHUNK_H

#include <Pt/Api.h>
#include <Pt/Types.h>

class AllocatorTest;

namespace Pt{

/** 
 * @brief Chunk
 * Contains info about each allocated Chunk - which is a collection of
 * contiguous blocks.  Each block is the same size, as specified by the
 * PoolFactory.  The number of blocks in a Chunk depends upon page size.

 * @par Minimal Interface
 * For the sake of runtime efficiency, no constructor, destructor, or
 * copy-assignment operator is defined. The inline functions made by the
 * compiler should be sufficient, and perhaps faster than hand-crafted
 * functions.  The lack of these functions allows vector to create and copy
 * Chunks as needed without overhead.  The Init and Release functions do
 * what the default constructor and destructor would do.  A Chunk is not in
 * a usable state after it is constructed and before calling Init.  Nor is
 * a Chunk usable after Release is called, but before the destructor.

 * @par Efficiency
 * Down near the lowest level of the allocator, runtime efficiencies trump
 * almost all other considerations.  Each function does the minimum required
 * of it.  All functions should execute in constant time to prevent higher-
 * level code from unwittingly using a version of Shlemiel the Painter's
 * Algorithm.

 * @par Stealth Indexes
 * The first char of each empty block contains the index of the next empty
 * block.  These stealth indexes form a singly-linked list within the blocks.
 * A Chunk is corrupt if this singly-linked list has a loop or is shorter
 * than blocksAvailable_.  Much of the allocator's time and space efficiency
 * comes from how these stealth indexes are implemented.
 */
class PT_API Chunk
{
	friend class ::AllocatorTest;
public:

    /**
     * @brief Constructor
     */
    Chunk(void);

    /**
     * @brief Destructor
     */
    ~Chunk(void);

    /**
     * @brief Equal operator
     * @param rhs Chunk object to compare. 
     */
    bool operator== (const Chunk& rhs)
    {
        return (_pData               == rhs._pData && 
                _blocksAvailable     == rhs._blocksAvailable &&
                _firstAvailableBlock == rhs._firstAvailableBlock);
    }

    /**
     * @brief Initializes a just-constructed Chunk.
     * @param blockSize Number of bytes per block.
     * @param blocks Number of blocks per Chunk.    
     */
    void init( std::size_t blockSize, Pt::uint8_t blocks );

    /** 
     * @brief Allocate a block within the Chunk.  
     * Complexity is always O(1), and this will never throw.  
     * Does not actually "allocate" by calling
     * malloc, new, or any other function, but merely adjusts some internal
     * indexes to indicate an already allocated block is no longer available.
     * @return Pointer to block within Chunk.
     */
     void* allocate( std::size_t blockSize );

    /** 
     * @brief Deallocate a block within the Chunk. 
     * Complexity is always O(1), and this will never throw.  
     *
     * For efficiency, this assumes the address is within the block and 
     * aligned along the correct byte boundary.  An assertion checks the alignment, 
     * and a call to hasBlock is done from within method findChunk.  
     * Does not actually "deallocate" by calling free, delete, or other function, 
     * but merely adjusts some internal indexes to indicate a block is now available.
     *
     * @param[in] p Represent the memory address to search.
     * @param[in] blockSize The size of the block, which will be deallocated.
     */
    void deallocate(void* p, std::size_t blockSize);

    /** 
     * @brief Resets the Chunk back to pristine values. 
     * The available count is set back to zero, and 
     * the first available index is set to the zeroth block.  
     * The stealth indexes inside each block are set to point to the next block. 
     * This assumes the Chunk's data was already using Init.
     */
     void reset( std::size_t blockSize, Pt::uint8_t blocks );

    /**
     * @brief Releases the allocated block of memory.
     */
    void release();
	
#ifndef NDEBUG
    /** 
     * @brief Determines if the Chunk has been corrupted.
     * @param numBlocks Total # of blocks in the Chunk.
     * @param blockSize # of bytes in each block.
     * @param checkIndexes True if caller wants to check indexes of available
     * blocks for corruption.  If false, then caller wants to skip some
     * tests tests just to run faster.  (Debug version does more checks, but release version runs faster.)
     * @return True if Chunk is corrupt.
     */
    bool isCorrupt( Pt::uint8_t numBlocks, std::size_t blockSize, bool checkIndexes ) const;
	
	bool isBlockAvailable(void* p, Pt::uint8_t numBlocks, std::size_t blockSize) const;
#endif

    /**
     * @brief Returns true if block at address P is inside this Chunk.
     */
    bool hasBlock( void* p, std::size_t chunkLength ) const
    {
        Pt::uint8_t* pc = static_cast<Pt::uint8_t *>( p );
        return ( _pData <= pc ) && ( pc < _pData + chunkLength );
    }

	/**
     * TODO: rename this method, maybe as isEmpty / empty
	 */
    bool hasAvailable(Pt::uint8_t numBlocks ) const
    { return ( _blocksAvailable == numBlocks ); }

    bool isFilled( void ) const
    { return ( 0 == _blocksAvailable ); }
	
	const Pt::uint8_t blocksAvailable() const
	{
		return _blocksAvailable;
    }

private:
    /// Pointer to array of allocated blocks.
    Pt::uint8_t* _pData;
    /// Index of first empty block.
    Pt::uint8_t _firstAvailableBlock;
    /// Count of empty blocks.
    Pt::uint8_t _blocksAvailable;
};

} // namespace Pt
#endif
