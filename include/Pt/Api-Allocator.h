/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_API_ALLOCATOR_H
#define PT_API_ALLOCATOR_H

/** @addtogroup Pt-Allocator

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
