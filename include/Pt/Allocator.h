/*
 * Copyright (C) 2008-2012 by Marc Boris Duerner
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
#ifndef PT_ALLOCATOR_H
#define PT_ALLOCATOR_H

#include <cstddef>

namespace Pt {

/** @brief %Allocator interface.

    Allocators allow a program to use different methods of allocating and
    deallocating raw memory. The default implementation will simply use new
    and delete. Custom allocators are implemented by overriding the two 
    methods @link Pt::Allocator::allocate() allocate()@endlink and 
    @link Pt::Allocator::deallocate() deallocate()@endlink of the Pt::Allocator
    base class. The following example tracks the amount of allocated memory:

    @code
    class CheckedAllocator : public Pt::Allocator
    {
        public:
            CheckedAllocator()
            : _allocated(0)
            {}
            
            virtual void* allocate(std::size_t size)
            {
                void* p = Pt::Allocator::allocate(size);
                _allocated += size;
                return p; 
            }

            virtual void deallocate(void* p, std::size_t size)
            {
                Pt::Allocator::deallocate(p, size);
                _allocated -= size;
            }

            std:size_t allocated() const
            { return _allocated; }

        private:
            std::size_t _allocated;
    };
    @endcode

    This interface differs from std::allocator used for STL containers,
    because it allows to allocate memory of different sizes through the same
    interface. The std::allocator is meant to allocate and also construct
    objects of the same size. It is however possible, to implement a
    std::allocator using the raw memory allocators described here.

    @ingroup Allocator
*/
class Allocator
{
    public:
        /** @brief Default constructor.
        */
        Allocator()
        {}

        /** @brief Destructor.
        */
        virtual ~Allocator()
        {}

        /** @brief Allocates @a size bytes of memory.
        */
        virtual void* allocate(std::size_t size)
        {
            return operator new(size);
        }

        /** @brief Deallocates memory of @a size bytes.
        */
        virtual void deallocate(void* p, std::size_t)
        {
            operator delete(p);
        }
};

}

#endif
