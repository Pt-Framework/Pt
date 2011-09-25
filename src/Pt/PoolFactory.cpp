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

#include "PoolFactory.h"

#include <cassert>
#include <iostream>
#include <algorithm>

namespace Pt{

/// Fewest # of objects managed by a Chunk.
Pt::uint8_t PoolFactory::_MinObjectsPerChunk = 8;

/// Most # of objects managed by a Chunk - never exceeds UCHAR_MAX.
Pt::uint8_t PoolFactory::_MaxObjectsPerChunk = UCHAR_MAX;

PoolFactory::PoolFactory(void)
: _blockSize( 0 )
, _numBlocks( 0 )
, _chunks( 0 )
, _allocChunk( NULL )
, _deallocChunk( NULL )
, _emptyChunk( NULL )
{
}

PoolFactory::~PoolFactory(void)
{
#ifndef NDEBUG
    trimEmptyChunk();
    assert(_chunks.empty() && "Memory leak detected!");
#endif

    for (ChunkIter i = _chunks.begin(); i != _chunks.end(); ++i )
    {
       i->release();
    }
}

void PoolFactory::init(std::size_t blockSize, std::size_t pageSize)
{
    assert(blockSize > 0);
    assert(pageSize >= blockSize);
    _blockSize = blockSize + sizeof(std::size_t);

    std::size_t numBlocks = pageSize / blockSize;
    if (numBlocks > _MaxObjectsPerChunk)
    {
        numBlocks = _MaxObjectsPerChunk;
    }
    else if(numBlocks < _MinObjectsPerChunk)
    {
        numBlocks = _MinObjectsPerChunk;
    }

    _numBlocks = static_cast<Pt::uint8_t>(numBlocks);
    assert(_numBlocks == numBlocks);
}

void* PoolFactory::allocate()
{
    // prove either _emptyChunk points nowhere, or points to a truly empty Chunk.
    assert( ( NULL == _emptyChunk ) || ( _emptyChunk->hasAvailable(_numBlocks) ) );
    assert( countEmptyChunks() < 2 );

    if ((NULL == _allocChunk) || _allocChunk->isFilled() )
    {
        if(NULL != _emptyChunk)
        {
            _allocChunk = _emptyChunk;
            _emptyChunk = NULL;
        }
        else
        {
            for (ChunkIter iter =_chunks.begin(); ; ++iter)
            {
                if (iter == _chunks.end())
                {
                    createChunk();
                    break;
                }
                if (!iter->isFilled())
                {
                    _allocChunk = &(*iter);
                    break;
                }
            }
        }
    }
    else if(_allocChunk == _emptyChunk)
    {
        // detach _emptyChunk from _allocChunk, because after 
        // calling _allocChunk->allocate(_blockSize); the chunk 
        // is no longer empty.
        _emptyChunk = NULL;
    }

    assert(_allocChunk != NULL);
    assert(!_allocChunk->isFilled());
    void* place = _allocChunk->allocate(_blockSize);
    char* index = ((char*)place + _blockSize) - sizeof(std::size_t);
    std::size_t pos = _allocChunk - &_chunks[0];
    *( reinterpret_cast<std::size_t*>(index) ) = pos;  

    // prove either _emptyChunk points nowhere, or points to a truly empty Chunk.
    assert((NULL == _emptyChunk) || (_emptyChunk->hasAvailable(_numBlocks)));
    assert(countEmptyChunks() < 2);
#ifndef NDEBUG
    if ( _allocChunk->isCorrupt( _numBlocks, _blockSize, true ) )
    {
        assert( false );
        return 0;
    }
#endif
    return place;
}

bool PoolFactory::deallocate(void* p)
{
    assert(!_chunks.empty());
    assert(&_chunks.front() <= _deallocChunk);
    assert(&_chunks.back() >= _deallocChunk);
    assert(&_chunks.front() <= _allocChunk);
    assert(&_chunks.back() >= _allocChunk);
    assert(countEmptyChunks() < 2);

    char* index = ((char*)p + _blockSize) - sizeof(std::size_t);
    std::size_t pos = *(reinterpret_cast<std::size_t*>(index));

    Chunk* foundChunk = &_chunks[pos];//findChunk(p);
    if ( 0 == foundChunk )
    {
       return false;
    }
    assert(foundChunk->hasBlock(p, _numBlocks * _blockSize));
#ifndef NDEBUG
    if ( foundChunk->isCorrupt( _numBlocks, _blockSize, true ) )
    {
        assert( false );
        return false;
    }
    if ( foundChunk->isBlockAvailable( p, _numBlocks, _blockSize ) )
    {
        assert( false );
        return false;
    }
#endif
    _deallocChunk = foundChunk;
    doDeallocate(p);
    assert(countEmptyChunks() < 2);

    return true;
}


bool PoolFactory::trimEmptyChunk()
{
    if(0 == _emptyChunk)
    {
        return false;
    }

    // If _emptyChunk points to valid Chunk, then chunk list is not empty.
    assert(!_chunks.empty());

    // And there should be exactly 1 empty Chunk.
    assert(1 == countEmptyChunks());

    Chunk *lastChunk = &_chunks.back();
    if (lastChunk != _emptyChunk)
    {
        std::swap(*_emptyChunk, *lastChunk);
    }
    assert(lastChunk->hasAvailable(_numBlocks));
    lastChunk->release();
    _chunks.pop_back();

    if (_chunks.empty())
    {
        _allocChunk = NULL;
        _deallocChunk = NULL;
    }
    else
    {
        if (_deallocChunk == _emptyChunk)
        {
            _deallocChunk = &_chunks.front();
            // TODO: check whether free blocks still available or not.
            assert( _deallocChunk->blocksAvailable() < _numBlocks );
        }
        if (_allocChunk == _emptyChunk)
        {
            _allocChunk = &_chunks.back();
            // TODO: check whether free blocks still available or not.
            assert(_allocChunk->blocksAvailable()  < _numBlocks);
        }
    }

    _emptyChunk = NULL;
    assert(0 == countEmptyChunks());

    return true;
}


bool PoolFactory::trimChunkList()
{
    if (_chunks.empty())
    {
        assert(NULL == _allocChunk);
        assert(NULL == _deallocChunk);
    }

    if (_chunks.size() == _chunks.capacity())
    {
        return false;
    }

    // Use the "make-a-temp-and-swap" trick to remove excess capacity.
    Chunks( _chunks ).swap( _chunks );

    return true;
}

#ifndef NDEBUG
bool PoolFactory::isCorrupt(void) const
{
    const bool isEmpty = _chunks.empty();
    ChunkCIter start( _chunks.begin() );
    ChunkCIter last( _chunks.end() );
    const size_t emptyChunkCount = countEmptyChunks();

    if (isEmpty)
    {
        if (start != last)
        {
            assert(false);
            return true;
        }
        if (0 < emptyChunkCount)
        {
            assert(false);
            return true;
        }
        if (NULL != _deallocChunk)
        {
            assert(false);
            return true;
        }
        if (NULL != _allocChunk)
        {
            assert(false);
            return true;
        }
        if (NULL != _emptyChunk)
        {
            assert(false);
            return true;
        }
    }

    else
    {
        const Chunk *front = &_chunks.front();
        const Chunk *back  = &_chunks.back();
        if (start >= last)
        {
            assert( false );
            return true;
        }
        if (back < _deallocChunk)
        {
            assert( false );
            return true;
        }
        if (back < _allocChunk)
        {
            assert( false );
            return true;
        }
        if (front > _deallocChunk)
        {
            assert( false );
            return true;
        }
        if (front > _allocChunk)
        {
            assert( false );
            return true;
        }

        switch (emptyChunkCount)
        {
            case 0:
                if (_emptyChunk != NULL)
                {
                    assert( false );
                    return true;
                }
                break;
            case 1:
                if (_emptyChunk == NULL)
                {
                    assert(false);
                    return true;
                }
                if (back < _emptyChunk)
                {
                    assert(false);
                    return true;
                }
                if (front > _emptyChunk)
                {
                    assert(false);
                    return true;
                }
                if (!_emptyChunk->hasAvailable(_numBlocks))
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
        for (ChunkCIter it(start); it != last; ++it )
        {
            const Chunk & chunk = *it;
            if (chunk.isCorrupt(_numBlocks, _blockSize, true))
            {
                return true;
            }
        }
    }

    return false;
}
#endif

bool PoolFactory::hasBlock(void* p) const
{
    ChunkCIter citer = _chunks.begin();
    for( ; citer != _chunks.end(); ++citer)
    {
        if((*citer).hasBlock(p, _numBlocks * _blockSize))
        {
            return true;
        }
    }
    
	return false;
}

void PoolFactory::doDeallocate(void * p)
{
    // Show that _deallocChunk really owns the block at address p.
    assert(_deallocChunk->hasBlock(p, _numBlocks * _blockSize));
    // Either of the next two assertions may fail if somebody tries to
    // delete the same block twice.
    assert(_emptyChunk != _deallocChunk);
    assert(!_deallocChunk->hasAvailable(_numBlocks));
    // prove either _emptyChunk points nowhere, or points to a truly empty Chunk.
    assert((0 == _emptyChunk) || (_emptyChunk->hasAvailable(_numBlocks)));

    // call into the chunk, will adjust the inner list but won't release memory
    _deallocChunk->deallocate(p, _blockSize);

    if (_deallocChunk->hasAvailable(_numBlocks))
    {
        assert(_emptyChunk != _deallocChunk);

        // _deallocChunk is empty, but a Chunk is only released if there are 2
        // empty chunks.  Since _emptyChunk may only point to a previously
        // cleared Chunk, if it points to something else besides _deallocChunk,
        // then PoolFactory currently has 2 empty Chunks.
        if ( 0 != _emptyChunk )
        {
            // If last Chunk is empty, just change what _deallocChunk
            // points to, and release the last.  Otherwise, swap an empty
            // Chunk with the last, and then release it.
            Chunk* lastChunk = &_chunks.back();
            if (lastChunk == _deallocChunk)
            {
                _deallocChunk = _emptyChunk;
            }
            else if (lastChunk != _emptyChunk)
            {             
                std::swap(*_emptyChunk, *lastChunk);
            }
            assert(lastChunk->hasAvailable(_numBlocks));
            lastChunk->release();
            _chunks.pop_back();
            if ((_allocChunk == lastChunk) || _allocChunk->isFilled())
            {
                _allocChunk = _deallocChunk;
            }
        }
        _emptyChunk = _deallocChunk;
    }

    // prove either _emptyChunk points nowhere, or points to a truly empty Chunk.
    assert((0 == _emptyChunk) || (_emptyChunk->hasAvailable(_numBlocks)));
}

void PoolFactory::createChunk( void )
{
	std::size_t size = _chunks.size();
	// Calling _chunks.reserve *before* creating and initializing the new
	// Chunk means that nothing is leaked by this function in case an
	// exception is thrown from reserve.
	if (_chunks.capacity() == size)
	{
		if (0 == size)
		{
			size = 4;
		}
		_chunks.reserve(size * 2);
	}
	Chunk newChunk;
	newChunk.init( _blockSize, _numBlocks );
	_chunks.push_back(newChunk );
    _allocChunk = &_chunks.back();
    _deallocChunk = &_chunks.front();
}
    
Chunk *PoolFactory::findChunk(void *p) const
{
    if (_chunks.empty())
    {
        return NULL;
    }
    assert(_deallocChunk);

    const std::size_t chunkLength = _numBlocks * _blockSize;
    Chunk *lo = _deallocChunk;
    Chunk *hi = _deallocChunk + 1;
    const Chunk *loBound = &_chunks.front();
    const Chunk *hiBound = &_chunks.back() + 1;

    // Special case: _deallocChunk is the last in the array
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
std::size_t PoolFactory::countEmptyChunks() const
{
    // This code is only used for specialized tests of the allocator.
    // It is #ifdef-ed so that its O(C) complexity does not overwhelm the
    // functions which call it.
    std::size_t count = 0;
    for ( ChunkCIter it( _chunks.begin() ); it != _chunks.end(); ++it )
    {
        const Chunk & chunk = *it;
        if ( chunk.hasAvailable( _numBlocks ) )
        {
            ++count;
        }
    }
    return count;
}
#endif

}

