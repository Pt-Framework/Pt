/* Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_API_ALLOCATOR_H
#define PT_API_ALLOCATOR_H

/** @defgroup Allocator Allocators

    @brief Custom allocation strategies for optimized memory management.

    The Pt::Allocator interface can be used to optimize or customize allocation
    strategies. Two allocators are provided, which can be approached by the
    Allocator interface, a pool based allocator and a page based allocator.
    A pool based allocator is beneficial in all cases where many small objects
    of small sizes are created. This is for example used to optimize memory
    usage during serialization. The page based allocator simply places data
    consecutively in memory and frees the whole block when its no longer in
    use. This is useful in situations where chunks of memory or objects are
    created and destroyed at the same time. 
*/

#endif
