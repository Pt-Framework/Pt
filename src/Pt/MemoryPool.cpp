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

#include "Pt/MemoryPool.h"

#include <cassert>
#include <iostream>
#include <algorithm>

namespace Pt{

/// Fewest # of objects managed by a MemoryBlock.
Pt::uint8_t MemoryPool::_MinObjectsPerMemoryBlock = 8;

/// Most # of objects managed by a MemoryBlock - never exceeds UCHAR_MAX.
Pt::uint8_t MemoryPool::_MaxObjectsPerMemoryBlock = UCHAR_MAX;

MemoryPool::MemoryPool(void)
: _blockSize( 0 )
, _numBlocks( 0 )
, _chunks( 0 )
, _allocMemoryBlock( NULL )
, _deallocMemoryBlock( NULL )
, _emptyMemoryBlock( NULL )
{
}

MemoryPool::~MemoryPool(void)
{
#ifndef NDEBUG
    trimEmptyMemoryBlock();
    assert(_chunks.empty() && "Memory leak detected!");
#endif

    for (MemoryBlockIter i = _chunks.begin(); i != _chunks.end(); ++i )
    {
       i->release();
    }
}

void MemoryPool::init(std::size_t blockSize, std::size_t pageSize)
{
    assert(blockSize > 0);
    assert(pageSize >= blockSize);
    _blockSize = blockSize + sizeof(std::size_t);

    std::size_t numBlocks = pageSize / blockSize;
    if (numBlocks > _MaxObjectsPerMemoryBlock)
    {
        numBlocks = _MaxObjectsPerMemoryBlock;
    }
    else if(numBlocks < _MinObjectsPerMemoryBlock)
    {
        numBlocks = _MinObjectsPerMemoryBlock;
    }

    _numBlocks = static_cast<Pt::uint8_t>(numBlocks);
    assert(_numBlocks == numBlocks);
}

void* MemoryPool::allocate()
{
    // prove either _emptyMemoryBlock points nowhere, or points to a truly empty MemoryBlock.
    assert( ( NULL == _emptyMemoryBlock ) || ( _emptyMemoryBlock->hasAvailable(_numBlocks) ) );
    assert( countEmptyMemoryBlocks() < 2 );

    if ((NULL == _allocMemoryBlock) || _allocMemoryBlock->isFilled() )
    {
        if(NULL != _emptyMemoryBlock)
        {
            _allocMemoryBlock = _emptyMemoryBlock;
            _emptyMemoryBlock = NULL;
        }
        else
        {
            for (MemoryBlockIter iter =_chunks.begin(); ; ++iter)
            {
                if (iter == _chunks.end())
                {
                    createMemoryBlock();
                    break;
                }
                if (!iter->isFilled())
                {
                    _allocMemoryBlock = &(*iter);
                    break;
                }
            }
        }
    }
    else if(_allocMemoryBlock == _emptyMemoryBlock)
    {
        // detach _emptyMemoryBlock from _allocMemoryBlock, because after 
        // calling _allocMemoryBlock->allocate(_blockSize); the chunk 
        // is no longer empty.
        _emptyMemoryBlock = NULL;
    }

    assert(_allocMemoryBlock != NULL);
    assert(!_allocMemoryBlock->isFilled());
    void* place = _allocMemoryBlock->allocate(_blockSize);

    // prove either _emptyMemoryBlock points nowhere, or points to a truly empty MemoryBlock.
    assert((NULL == _emptyMemoryBlock) || (_emptyMemoryBlock->hasAvailable(_numBlocks)));
    assert(countEmptyMemoryBlocks() < 2);
#ifndef NDEBUG
    if ( _allocMemoryBlock->isCorrupt( _numBlocks, _blockSize, true ) )
    {
        assert( false );
        return 0;
    }
#endif

    char* index = ((char*)place + _blockSize) - sizeof(std::size_t);
    *(reinterpret_cast<std::size_t*>(index)) = _allocMemoryBlock - &_chunks[0];

    return place;
}


void MemoryPool::createMemoryBlock( void )
{
    std::size_t size = _chunks.size();
    // Calling _chunks.reserve *before* creating and initializing the new
    // MemoryBlock means that nothing is leaked by this function in case an
    // exception is thrown from reserve.
    if (_chunks.capacity() == size)
    {
        if (0 == size)
        {
            size = 4;
        }
        _chunks.reserve(size * 2);
    }
    MemoryBlock newMemoryBlock;
    newMemoryBlock.init( _blockSize, _numBlocks );
    _chunks.push_back(newMemoryBlock );
    _allocMemoryBlock = &_chunks.back();
    _deallocMemoryBlock = &_chunks.front();
}


bool MemoryPool::deallocate(void* p)
{
    assert(!_chunks.empty());
    assert(&_chunks.front() <= _deallocMemoryBlock);
    assert(&_chunks.back() >= _deallocMemoryBlock);
    assert(&_chunks.front() <= _allocMemoryBlock);
    assert(&_chunks.back() >= _allocMemoryBlock);
    assert(countEmptyMemoryBlocks() < 2);

    char* index = ((char*)p + _blockSize) - sizeof(std::size_t);
    std::size_t pos = *(reinterpret_cast<std::size_t*>(index));

    MemoryBlock* foundMemoryBlock = findMemoryBlock(p);
    if ( 0 == foundMemoryBlock )
    {
       return false;
    }

    assert(foundMemoryBlock->hasBlock(p, _numBlocks * _blockSize));
#ifndef NDEBUG
    if ( foundMemoryBlock->isCorrupt( _numBlocks, _blockSize, true ) )
    {
        assert( false );
        return false;
    }
    if ( foundMemoryBlock->isBlockAvailable( p, _numBlocks, _blockSize ) )
    {
        assert( false );
        return false;
    }
#endif

    _deallocMemoryBlock = foundMemoryBlock;
    doDeallocate(p);
    assert(countEmptyMemoryBlocks() < 2);

    return true;
}


void MemoryPool::doDeallocate(void * p)
{
    // Show that _deallocMemoryBlock really owns the block at address p.
    assert(_deallocMemoryBlock->hasBlock(p, _numBlocks * _blockSize));
    // Either of the next two assertions may fail if somebody tries to
    // delete the same block twice.
    assert(_emptyMemoryBlock != _deallocMemoryBlock);
    assert(!_deallocMemoryBlock->hasAvailable(_numBlocks));
    // prove either _emptyMemoryBlock points nowhere, or points to a truly empty MemoryBlock.
    assert((0 == _emptyMemoryBlock) || (_emptyMemoryBlock->hasAvailable(_numBlocks)));

    // call into the chunk, will adjust the inner list but won't release memory
    _deallocMemoryBlock->deallocate(p, _blockSize);

    if (_deallocMemoryBlock->hasAvailable(_numBlocks))
    {
        assert(_emptyMemoryBlock != _deallocMemoryBlock);

        // _deallocMemoryBlock is empty, but a MemoryBlock is only released if there are 2
        // empty chunks.  Since _emptyMemoryBlock may only point to a previously
        // cleared MemoryBlock, if it points to something else besides _deallocMemoryBlock,
        // then MemoryPool currently has 2 empty MemoryBlocks.
        if ( 0 != _emptyMemoryBlock )
        {
            // If last MemoryBlock is empty, just change what _deallocMemoryBlock
            // points to, and release the last.  Otherwise, swap an empty
            // MemoryBlock with the last, and then release it.
            MemoryBlock* lastMemoryBlock = &_chunks.back();
            if (lastMemoryBlock == _deallocMemoryBlock)
            {
                _deallocMemoryBlock = _emptyMemoryBlock;
            }
            else if (lastMemoryBlock != _emptyMemoryBlock)
            {             
                std::swap(*_emptyMemoryBlock, *lastMemoryBlock);
            }

            assert(lastMemoryBlock->hasAvailable(_numBlocks));
            lastMemoryBlock->release();
            _chunks.pop_back();
                        
            if ((_allocMemoryBlock == lastMemoryBlock) || _allocMemoryBlock->isFilled())
            {
                _allocMemoryBlock = _deallocMemoryBlock;
            }
        }
        _emptyMemoryBlock = _deallocMemoryBlock;
    }

    // prove either _emptyMemoryBlock points nowhere, or points to a truly empty MemoryBlock.
    assert((0 == _emptyMemoryBlock) || (_emptyMemoryBlock->hasAvailable(_numBlocks)));
}


bool MemoryPool::trimEmptyMemoryBlock()
{
    if(0 == _emptyMemoryBlock)
    {
        return false;
    }

    // If _emptyMemoryBlock points to valid MemoryBlock, then chunk list is not empty.
    assert(!_chunks.empty());

    // And there should be exactly 1 empty MemoryBlock.
    assert(1 == countEmptyMemoryBlocks());

    MemoryBlock *lastMemoryBlock = &_chunks.back();
    if (lastMemoryBlock != _emptyMemoryBlock)
    {
        std::swap(*_emptyMemoryBlock, *lastMemoryBlock);
    }
    assert(lastMemoryBlock->hasAvailable(_numBlocks));
    lastMemoryBlock->release();
    _chunks.pop_back();

    if (_chunks.empty())
    {
        _allocMemoryBlock = NULL;
        _deallocMemoryBlock = NULL;
    }
    else
    {
        if (_deallocMemoryBlock == _emptyMemoryBlock)
        {
            _deallocMemoryBlock = &_chunks.front();
            // TODO: check whether free blocks still available or not.
            assert( _deallocMemoryBlock->blocksAvailable() < _numBlocks );
        }
        if (_allocMemoryBlock == _emptyMemoryBlock)
        {
            _allocMemoryBlock = &_chunks.back();
            // TODO: check whether free blocks still available or not.
            assert(_allocMemoryBlock->blocksAvailable()  < _numBlocks);
        }
    }

    _emptyMemoryBlock = NULL;
    assert(0 == countEmptyMemoryBlocks());

    return true;
}


bool MemoryPool::trimMemoryBlockList()
{
    if (_chunks.empty())
    {
        assert(NULL == _allocMemoryBlock);
        assert(NULL == _deallocMemoryBlock);
    }

    if (_chunks.size() == _chunks.capacity())
    {
        return false;
    }

    // Use the "make-a-temp-and-swap" trick to remove excess capacity.
    MemoryBlocks( _chunks ).swap( _chunks );

    return true;
}

#ifndef NDEBUG
bool MemoryPool::isCorrupt(void) const
{
    const bool isEmpty = _chunks.empty();
    MemoryBlockCIter start( _chunks.begin() );
    MemoryBlockCIter last( _chunks.end() );
    const size_t emptyMemoryBlockCount = countEmptyMemoryBlocks();

    if (isEmpty)
    {
        if (start != last)
        {
            assert(false);
            return true;
        }
        if (0 < emptyMemoryBlockCount)
        {
            assert(false);
            return true;
        }
        if (NULL != _deallocMemoryBlock)
        {
            assert(false);
            return true;
        }
        if (NULL != _allocMemoryBlock)
        {
            assert(false);
            return true;
        }
        if (NULL != _emptyMemoryBlock)
        {
            assert(false);
            return true;
        }
    }

    else
    {
        const MemoryBlock *front = &_chunks.front();
        const MemoryBlock *back  = &_chunks.back();
        if (start >= last)
        {
            assert( false );
            return true;
        }
        if (back < _deallocMemoryBlock)
        {
            assert( false );
            return true;
        }
        if (back < _allocMemoryBlock)
        {
            assert( false );
            return true;
        }
        if (front > _deallocMemoryBlock)
        {
            assert( false );
            return true;
        }
        if (front > _allocMemoryBlock)
        {
            assert( false );
            return true;
        }

        switch (emptyMemoryBlockCount)
        {
            case 0:
                if (_emptyMemoryBlock != NULL)
                {
                    assert( false );
                    return true;
                }
                break;
            case 1:
                if (_emptyMemoryBlock == NULL)
                {
                    assert(false);
                    return true;
                }
                if (back < _emptyMemoryBlock)
                {
                    assert(false);
                    return true;
                }
                if (front > _emptyMemoryBlock)
                {
                    assert(false);
                    return true;
                }
                if (!_emptyMemoryBlock->hasAvailable(_numBlocks))
                {
                    // This may imply somebody tried to delete a block twice.
                    assert(false);
                    return true;
                }
                break;
            default:
                assert(false);
                return true;
        }
        for (MemoryBlockCIter it(start); it != last; ++it )
        {
            const MemoryBlock & chunk = *it;
            if (chunk.isCorrupt(_numBlocks, _blockSize, true))
            {
                return true;
            }
        }
    }

    return false;
}
#endif

bool MemoryPool::hasBlock(void* p) const
{
    MemoryBlockCIter citer = _chunks.begin();
    for( ; citer != _chunks.end(); ++citer)
    {
        if((*citer).hasBlock(p, _numBlocks * _blockSize))
        {
            return true;
        }
    }
    
	return false;
}

    
MemoryBlock *MemoryPool::findMemoryBlock(void *p) const
{
    if (_chunks.empty())
    {
        return NULL;
    }
    assert(_deallocMemoryBlock);

    const std::size_t chunkLength = _numBlocks * _blockSize;
    MemoryBlock *lo = _deallocMemoryBlock;
    MemoryBlock *hi = _deallocMemoryBlock + 1;
    const MemoryBlock *loBound = &_chunks.front();
    const MemoryBlock *hiBound = &_chunks.back() + 1;

    // Special case: _deallocMemoryBlock is the last in the array
    if (hi == hiBound)
    {
        hi = NULL;
    }

    while (true)
    {
        if (lo)
        {
            if (lo->hasBlock(p, chunkLength))
            {
                return lo;
            }
            if (lo == loBound)
            {
                lo = NULL;
                if (NULL == hi)
                {
                    break;
                }
            }
            else --lo;
        }

        if (hi)
        {
            if (hi->hasBlock(p, chunkLength))
            {
                return hi;
            }
            if (++hi == hiBound)
            {
                hi = NULL;
                if (NULL == lo)
                {
                    break;
                }
            }
        }
    }

    return NULL;
}

#ifndef NDEBUG
std::size_t MemoryPool::countEmptyMemoryBlocks() const
{
    // This code is only used for specialized tests of the allocator.
    // It is #ifdef-ed so that its O(C) complexity does not overwhelm the
    // functions which call it.
    std::size_t count = 0;
    for ( MemoryBlockCIter it( _chunks.begin() ); it != _chunks.end(); ++it )
    {
        const MemoryBlock & chunk = *it;
        if ( chunk.hasAvailable( _numBlocks ) )
        {
            ++count;
        }
    }
    return count;
}
#endif

}

