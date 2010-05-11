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
 
#include <Pt/Alloc/Chunk.h>
#include <new> // needed for std::nothrow_t parameter.
#include <cstddef>
#include <assert.h>
#include <bitset>

#include "tests/ChunkTest.h"

namespace Pt{
namespace Alloc{

Chunk::Chunk(void):
_pData(0)
{
}

Chunk::~Chunk(void)
{
}

void Chunk::init( std::size_t blockSize, Pt::uint8_t blocks )
{
    assert(blockSize > 0);
    assert(blocks > 0);

    // Overflow check
    std::size_t allocSize = blockSize * blocks;
    assert( allocSize / blockSize == blocks);

    // If this new operator fails, it will throw, and the exception will get
    // caught one layer up.
    _pData = static_cast< Pt::uint8_t* >( ::operator new ( allocSize ) );

    reset(blockSize, blocks);
}

void Chunk::reset(std::size_t blockSize, Pt::uint8_t blocks)
{
    assert(blockSize > 0);
    assert(blocks > 0);
    // Overflow check
    assert((blockSize * blocks) / blockSize == blocks);

    _firstAvailableBlock = 0;
    _blocksAvailable = blocks;

    Pt::uint8_t i = 0;
    for(Pt::uint8_t* p = _pData; i != blocks; p += blockSize)
    {
        *p = ++i;
    }
}

void Chunk::release()
{
	//std::cerr << "." << std::flush;
    assert( 0 != _pData );
    ::operator delete (_pData);

}

// Chunk::Allocate ------------------------------------------------------------
void* Chunk::allocate(std::size_t blockSize)
{
    if(isFilled())
    {
        return 0;
    }

    assert((_firstAvailableBlock * blockSize) / blockSize == _firstAvailableBlock);
    Pt::uint8_t* pResult = _pData + (_firstAvailableBlock * blockSize);
    _firstAvailableBlock = *pResult;
    --_blocksAvailable;

    return pResult;
}

// Chunk::Deallocate ----------------------------------------------------------

void Chunk::deallocate(void* p, std::size_t blockSize)
{
    assert(p >= _pData);

    Pt::uint8_t* toRelease = static_cast<Pt::uint8_t*>(p);
    // Alignment check
    assert((toRelease - _pData) % blockSize == 0);
    Pt::uint8_t index = static_cast< Pt::uint8_t >((toRelease - _pData) / blockSize);

#ifndef NDEBUG
    // Check if block was already deleted.  Attempting to delete the same
    // block more than once causes Chunk's linked-list of stealth indexes to
    // become corrupt.  And causes count of _blocksAvailable to be wrong.
    if ( 0 < _blocksAvailable )
    {
        assert( _firstAvailableBlock != index );
    }
#endif

    *toRelease = _firstAvailableBlock;
    _firstAvailableBlock = index;
    // Truncation check
    assert(_firstAvailableBlock == (toRelease - _pData) / blockSize);

    ++_blocksAvailable;
}

// Chunk::IsCorrupt -----------------------------------------------------------
#ifndef NDEBUG
bool Chunk::isCorrupt(Pt::uint8_t numBlocks, std::size_t blockSize, bool checkIndexes) const
{
    if(numBlocks < _blocksAvailable)
    {
        // Contents at this Chunk corrupted.  This might mean something has
        // overwritten memory owned by the Chunks container.
        assert( false );
        return true;
    }

    if(isFilled())
    {
        // Useless to do further corruption checks if all blocks allocated.
        return false;
    }

    Pt::uint8_t index = _firstAvailableBlock;
    if(numBlocks <= index)
    {
        // Contents at this Chunk corrupted.  This might mean something has
        // overwritten memory owned by the Chunks container.
        assert( false );
        return true;
    }
    if(!checkIndexes)
    {
        // Caller chose to skip more complex corruption tests.
        return false;
    }

    // If the bit at index was set in foundBlocks, then the stealth index was
    // found on the linked-list.
    std::bitset< UCHAR_MAX > foundBlocks;
    Pt::uint8_t* nextBlock = NULL;

    /* The loop goes along singly linked-list of stealth indexes and makes sure
     that each index is within bounds (0 <= index < numBlocks) and that the
     index was not already found while traversing the linked-list.  The linked-
     list should have exactly _blocksAvailable nodes, so the for loop will not
     check more than _blocksAvailable.  This loop can't check inside allocated
     blocks for corruption since such blocks are not within the linked-list.
     Contents of allocated blocks are not changed by Chunk.

     Here are the types of corrupted link-lists which can be verified.  The
     corrupt index is shown with asterisks in each example.

     Type 1: Index is too big.
      numBlocks == 64
      _blocksAvailable == 7
      _firstAvailableBlock -> 17 -> 29 -> *101*
      There should be no indexes which are equal to or larger than the total
      number of blocks.  Such an index would refer to a block beyond the
      Chunk's allocated domain.

     Type 2: Index is repeated.
      numBlocks == 64
      _blocksAvailable == 5
      _firstAvailableBlock -> 17 -> 29 -> 53 -> *17* -> 29 -> 53 ...
      No index should be repeated within the linked-list since that would
      indicate the presence of a loop in the linked-list.
     */
    for(Pt::uint8_t cc = 0; ; )
    {
        nextBlock = _pData + ( index * blockSize );
        foundBlocks.set( index, true );
        ++cc;
        if ( cc >= _blocksAvailable )
        {
            // Successfully counted off number of nodes in linked-list.
            break;
        }
        index = *nextBlock;
        if ( numBlocks <= index )
        {
            /* This catches Type 1 corruptions as shown in above comments.
             This implies that a block was corrupted due to a stray pointer
             or an operation on a nearby block overran the size of the block.
             */
            assert( false );
            return true;
        }
        if ( foundBlocks.test( index ) )
        {
            /* This catches Type 2 corruptions as shown in above comments.
             This implies that a block was corrupted due to a stray pointer
             or an operation on a nearby block overran the size of the block.
             Or perhaps the program tried to delete a block more than once.
             */
            assert( false );
            return true;
        }
    }
    if ( foundBlocks.count() != _blocksAvailable )
    {
        /* This implies that the singly-linked-list of stealth indexes was
         corrupted.  Ideally, this should have been detected within the loop.
         */
        assert( false );
        return true;
    }

    return false;
}

bool Chunk::isBlockAvailable(void* p, Pt::uint8_t numBlocks, std::size_t blockSize) const
{
    (void) numBlocks;
    
    if(isFilled())
    {
        return false;
    }

    Pt::uint8_t* place = static_cast<Pt::uint8_t*>( p );
    // Alignment check
    assert( ( place - _pData ) % blockSize == 0 );
    Pt::uint8_t blockIndex = static_cast<Pt::uint8_t>((place - _pData) / blockSize);

    Pt::uint8_t index = _firstAvailableBlock;
    assert( numBlocks > index );
    if ( index == blockIndex )
    {
        return true;
    }

    /* If the bit at index was set in foundBlocks, then the stealth index was
     found on the linked-list.
     */
    std::bitset< UCHAR_MAX > foundBlocks;
    Pt::uint8_t* nextBlock = NULL;
    for(Pt::uint8_t cc = 0; ; )
    {
        nextBlock = _pData + ( index * blockSize );
        foundBlocks.set( index, true );
        ++cc;
        if(cc >= _blocksAvailable)
        {
            // Successfully counted off number of nodes in linked-list.
            break;
        }
        index = *nextBlock;
        if( index == blockIndex)
        {
            return true;
        }
        assert( numBlocks > index );
        assert( !foundBlocks.test( index ) );
    }

    return false;
}
#endif
}

}
