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

#ifndef ALLOCATOR_HPP_
#define ALLOCATOR_HPP_

#include <new>
// std::size_t, std::ptrdiff_t
#include <cstddef>
// std::malloc, std::free
#include <cstdlib>
// std::numeric_limits
#include <limits>

// Pt types
#include <Pt/Types.h>

// Singleton class of Pt-Alloc
#include <Pt/Alloc/PoolSingleton.h>

namespace Pt{
namespace Alloc{

template< typename T >
struct PT_ALLOC_API default_user_allocator_new_delete
{
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  static T* malloc(const size_type bytes)
  { return new (std::nothrow) T[bytes]; }
  static void free(T* block)
  { 
      delete [] block; 
  }
};

template < typename T >
struct PT_ALLOC_API default_user_allocator_pool_singleton
{
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef PoolSingleton <PT_DEFAULT_CHUNK_SIZE, 
                         PT_DEFAULT_MAX_SMALL_OBJECT_SIZE, 
                         PT_DEFAULT_OBJECT_ALIGNMENT> PoolSingletonObj;

  static T* malloc(const size_type bytes)
  { return reinterpret_cast<T*>(PoolSingletonObj::instance().allocate(bytes, false)); }
  static void free(T* block)
  { 
      PoolSingletonObj::instance().deallocate(block); 
  }
};

template <typename T,
          typename UserAllocator>
class PT_ALLOC_API Allocator
{
public:
    typedef T value_type;
    typedef UserAllocator user_allocator;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef typename UserAllocator::size_type size_type;
    typedef typename UserAllocator::difference_type difference_type;

    template <typename U>
    struct rebind
    {
        typedef Allocator<U, UserAllocator> other;
    };

public:
    Allocator() { }

    // default copy constructor
    // default assignment operator

    // not explicit, mimicking std::allocator [20.4.1]
    template <typename U>
    Allocator(const Allocator<U, UserAllocator> &)
    { }

    // default destructor

    static pointer address(reference r)
    { return &r; }

    static const_pointer address(const_reference s)
    { return &s; }

    static size_type max_size()
    { return std::numeric_limits<size_type>::max(); }

    static void construct(const pointer ptr, const value_type& t)
    { new (ptr) T(t); }

    static void destroy(const pointer ptr)
    {
        ptr->~T();
        (void) ptr; // avoid unused variable warning
    }

    bool operator==(const Allocator &) const
    { return true; }

    bool operator!=(const Allocator &) const
    { return false; }

    static pointer allocate(const size_type n)
    {
        const pointer ret = UserAllocator::malloc(n);        
        if (ret == 0)
        {
            throw std::bad_alloc();
        }
        return ret;
    }

    static pointer allocate(const size_type n, const void* const)
    { 
        return allocate(n);
    }

    static void deallocate(const pointer ptr, const size_type n)
    {
        if (ptr == 0 || n == 0)
        {
            return;
        }
        UserAllocator::free(ptr);
    }
};

}
}

#endif
