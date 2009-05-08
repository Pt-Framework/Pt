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

#ifndef POOLBASE_H
#define POOLBASE_H

#include <Pt/System/Mutex.h>
#include <Pt/Alloc/PoolSingleton.h>

namespace Pt{

template< std::size_t chunkSize          = PT_DEFAULT_CHUNK_SIZE,
          std::size_t maxSmallObjectSize = PT_DEFAULT_MAX_SMALL_OBJECT_SIZE,
          std::size_t objectAlignSize    = PT_DEFAULT_OBJECT_ALIGNMENT >
class PT_ALLOC_API PoolBase
{
public:
    typedef Mutex mutex;
    typedef PoolSingleton<chunkSize, maxSmallObjectSize, objectAlignSize> PoolSingletonObj;
    /// Throwing single-object new throws bad_alloc when allocation fails.
#ifdef _MSC_VER
    /// @note MSVC complains about non-empty exception specification lists.
    static void* operator new(std::size_t size)
#else
    static void* operator new (std::size_t size) throw (std::bad_alloc)
#endif
    {
        return PoolSingletonObj::instance().allocate( size, true );
    }

   /**
    * @brief Non-throwing single-object new returns NULL if allocation fails.
    */
    static void* operator new(std::size_t size, const std::nothrow_t&) throw ()
    {
        return PoolSingletonObj::instance().allocate(size, false);
    }

   /** 
    * @brief Placement single-object new merely calls global placement new.
    */
    static void* operator new(std::size_t size, void* place)
    {
        return ::operator new(size, place);
    }

   /**
    * @brief Single-object delete.
    */
    static void operator delete(void* p, std::size_t size) throw ()
    {
        PoolSingletonObj::instance().deallocate(p, size);
    }

    /** 
     * @brief Non-throwing single-object delete is only called when nothrow
     *        new operator is used, and the constructor throws an exception.
     */
    static void operator delete(void* p, const std::nothrow_t&) throw()
    {
        PoolSingletonObj::instance().deallocate(p);
    }

   /**
    * @brief Placement single-object delete merely calls global placement delete.
    */
    static void operator delete(void* p, void* place)
    {
        ::operator delete(p, place);
    }

#ifdef PT_SMALL_OBJECT_USE_NEW_ARRAY

        /// Throwing array-object new throws bad_alloc when allocation fails.
#ifdef _MSC_VER
        /// @note MSVC complains about non-empty exception specification lists.
    static void* operator new [](std::size_t size)
#else
    static void* operator new [](std::size_t size) throw (std::bad_alloc)
#endif
    {
        return PoolSingletonObj::instance().allocate(size, true);
    }

   /**
    * @brief Non-throwing array-object new returns NULL if allocation fails.
    */
    static void* operator new [](std::size_t size, const std::nothrow_t&) throw ()
    {
        return PoolSingletonObj::instance().allocate(size, false);
    }

   /**
    * @brief Placement array-object new merely calls global placement new.
    */
    static void* operator new [](std::size_t size, void* place)
    {
        return ::operator new(size, place);
    }

   /** 
    * @brief Array-object delete.
    */
    static void operator delete [](void* p, std::size_t size) throw ()
    {
        PoolSingletonObj::instance().deallocate( p, size );
    }

   /** 
    * @brief Non-throwing array-object delete is only called when nothrow
    *        new operator is used, and the constructor throws an exception.
    */
    static void operator delete [](void* p, const std::nothrow_t&) throw()
    {
        PoolSingletonObj::instance().deallocate(p);
    }

   /**
    * @brief Placement array-object delete merely calls global placement delete.
    */
    static void operator delete [](void* p, void* place)
    {
        ::operator delete(p, place);
    }
#endif  // #if use new array functions.

protected:
    PoolBase(void){};
    virtual ~PoolBase(void){};
    PoolBase(const PoolBase&) {}
    PoolBase& operator=(const PoolBase &)
    { return *this; }
};

} // namespace Alloc
} // namespace Pt

#endif
