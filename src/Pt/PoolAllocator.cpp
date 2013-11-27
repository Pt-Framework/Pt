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

#include <Pt/PoolAllocator.h>

namespace Pt {

MemoryPool::MemoryPool(std::size_t elemSize, std::size_t maxPageSize)
: _recordsPerUnit(((elemSize + (RecordSize - 1)) / RecordSize) + 1)
, _maxUnits(maxPageSize / (_recordsPerUnit * RecordSize))
{
    _blocks.reserve(16); 
}


MemoryPool:: ~MemoryPool()
{
    for(std::size_t i = 0; i < _blocks.size(); ++i)
    {
        assert( _blocks[i].isEmpty() );
        _blocks[i].clear();
    }
}


PoolAllocator::PoolAllocator(std::size_t maxElemSize, std::size_t step, std::size_t maxPagesize)
: _maxObjectSize(maxElemSize)
, _objectAlignSize(step)
{
    assert( 0 != _objectAlignSize );

    const std::size_t numPools = (_maxObjectSize + _objectAlignSize - 1) / _objectAlignSize;

    for (std::size_t i = 1; i <= numPools; ++i)
    {
        _pools.push_back( new MemoryPool(i * _objectAlignSize, maxPagesize) );
    }

    assert(numPools == _pools.size());
}

PoolAllocator::~PoolAllocator()
{
    assert( 0 != _objectAlignSize );

    std::vector<MemoryPool*>::iterator it;
    for(it = _pools.begin(); it != _pools.end(); ++it)
    {
        delete *it;
    }
}

} // namespace Pt
