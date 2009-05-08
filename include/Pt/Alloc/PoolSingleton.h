/***************************************************************************
 *   Copyright (C) 2008-2009 by Bendri Batti                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef POOLSINGLETON_H
#define POOLSINGLETON_H

#include <Pt/System/Mutex.h>
#include <Pt/Alloc/PoolAllocator.h>
#include <Pt/Singleton.h>

#ifndef PT_DEFAULT_CHUNK_SIZE
#define PT_DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef PT_DEFAULT_MAX_SMALL_OBJECT_SIZE
#define PT_DEFAULT_MAX_SMALL_OBJECT_SIZE 256
#endif

#ifndef PT_DEFAULT_OBJECT_ALIGNMENT
#define PT_DEFAULT_OBJECT_ALIGNMENT 4
#endif

namespace Pt{
namespace Alloc{

template <std::size_t chunkSize       = PT_DEFAULT_CHUNK_SIZE,
          std::size_t maxObjectSize   = PT_DEFAULT_MAX_SMALL_OBJECT_SIZE,
          std::size_t objectAlignment = PT_DEFAULT_OBJECT_ALIGNMENT>
class PT_ALLOC_API PoolSingleton : public Pt::Singleton< PoolSingleton<chunkSize, maxObjectSize, objectAlignment> >
{
    friend class Singleton< PoolSingleton<chunkSize, maxObjectSize, objectAlignment> >;

public:

    PoolSingleton(void)
    {
        _poolAlloc = new PoolAllocator(chunkSize, maxObjectSize, objectAlignment);
        _mutex     = new Pt::System::Mutex();
    };

    ~PoolSingleton(void)
    {
        if (_poolAlloc)
        {
            delete _poolAlloc;
            _poolAlloc = NULL;
        }

        if (_mutex)
        {
            delete _mutex;
            _mutex = NULL;
        }
    };

    void* allocate (std::size_t size, bool doThrow)
    { 
        Pt::System::MutexLock mutexLock(*_mutex);        
        return _poolAlloc->allocate(size, doThrow);
    }

    void deallocate (void* p, std::size_t size)
    {
        Pt::System::MutexLock mutexLock(*_mutex);
        _poolAlloc->deallocate (p, size);
    }

    void deallocate (void* p)
    {
        Pt::System::MutexLock mutexLock(*_mutex);
        _poolAlloc->deallocate(p);
    }

    std::size_t getMaxObjectSize() const
    {
        return _poolAlloc->getMaxObjectSize();
    }

    std::size_t getAlignment() const 
    {
        return _poolAlloc->getAlignment(); 
    }

    bool trimExcessMemory()
    {
        Pt::System::MutexLock mutexLock(*_mutex);
        return _poolAlloc->trimExcessMemory();
    }

    bool isCorrupt() const
    {
        Pt::System::MutexLock mutexLock(*_mutex);
        return _poolAlloc->isCorrupt();
    }

private:
    PoolAllocator* _poolAlloc;
    Pt::System::Mutex* _mutex;
};

}
}

#endif

