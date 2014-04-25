/*
 * Copyright (C) 2009-2010 by Bendri Batti
 * Copyright (C) 2009-2013 by Marc Boris Duerner
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

#ifndef PT_PAGE_ALLOCATOR_H
#define PT_PAGE_ALLOCATOR_H

#include <Pt/Api.h>
#include <Pt/Allocator.h>
#include <Pt/NonCopyable.h>
#include <cstddef>

namespace Pt {

/** @brief Page based allocator.

    The @link Pt::PageAllocator PageAllocator@endlink is useful, when chunks
    of memory have to be allocated, that can be released simultaneously.
    This allows the %PageAllocator to allocate memory consecutively on pages
    and simply release all pages together at the end. Therefore,
    @link Pt::PageAllocator::deallocate() deallocate()@endlink will not do
    anything, but memory will only eventually be released, when @link 
    Pt::PageAllocator::clear() clear()@endlink is called or the %PageAllocator
    is destructed. The next example illustrates this:

    @code
    void useAllocator(Pt::Allocator& a)
    {
        for(std::size_t n = 1; n < 16; ++n)
        {
            void* p = a.allocate(n);

            ...

            // won't do anything if it's a PoolAllocator
            a.deallocate(p, n);
        }
    }

    Pt::PageAllocator allocator;

    useAllocator(allocator);

    // release all allocated memory
    allocator.clear();
    @endcode

    @ingroup Allocator
*/
class PT_API PageAllocator : public Pt::Allocator
                           , protected NonCopyable
{
    public:
        //! @internal
        class Page
        {              
            public:
                Page(Page* nextChunk, std::size_t chunkSize);

                ~Page();

                void clear();

                void* allocate(std::size_t reqSize);

                Page *nextVariableChunk()
                { return _nextChunk; }

                std::size_t spaceAvailable() const
                { return _chunkSize - _bytesAlreadyAllocated; }
        
                std::size_t capacity() const
                { return _chunkSize; }

            private:
                Page* _nextChunk;
                char* _mem;
                std::size_t _chunkSize;
                std::size_t _bytesAlreadyAllocated;
        };

        enum { MinChunkSize = 4096 };

    public:
        //! @brief Construct with minimum page size.
        PageAllocator(std::size_t pageSize = MinChunkSize);

        //! @brief Destructor.
        ~PageAllocator();

        //! @brief Releases all memory.
        void clear();

        // inherit docs
        void* allocate( std::size_t size );

        // inherit docs
        void deallocate( void* p, std::size_t size );

    private:
        //! @internal
        void expandStorage( std::size_t size );

    private:
        Page* _listOfVariableChunk;
};

} // namespace Pt

#endif
