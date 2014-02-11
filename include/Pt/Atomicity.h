/*
 * Copyright (C) 2006-2007 by Marc Boris Duerner
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

#ifndef PT_ATOMICITY_H
#define PT_ATOMICITY_H

#include <Pt/Api.h>
#include <Pt/Types.h>

namespace Pt {

/** @class Pt::atomic_t Atomicity.h "Pt/Atomicity.h"
    @brief Atomic integers to be used with atomicity functions.

    @ingroup Concurrency
*/

/** @fn Pt::atomic_t::atomic_t(int v = 0) 
    @memberof Pt::atomic_t
    @public
    @brief Construct with initial value.
*/

union PT_API atomic_t
{
    int     i;
    long    l;
    int32_t i32;
    int64_t i64;
    void*   p;
#ifdef __cplusplus
    explicit atomic_t(int v = 0);
#endif
};

/** @brief Atomically get a value.

    Returns the value and employs a memory fence after the get. Acquire
    semantics prevent memory reordering with any read or write operation
    which follows it in program order.

    @related atomic_t
    @ingroup Concurrency
*/
PT_API int atomicGet(volatile atomic_t& val);

/** @brief Atomically set a value.

    Sets the value and employs a memory fence before the set. Release
    semantics prevent memory reordering with any read or write operation
    which precedes it in program order.

    @related atomic_t
    @ingroup Concurrency
*/
PT_API void atomicSet(volatile atomic_t& val, int n);

/** @brief Increases a value by one as an atomic operation.

    Returns the resulting incremented value.

    @related atomic_t
    @ingroup Concurrency
*/
PT_API int atomicIncrement(volatile atomic_t& val);

/** @brief Decreases a value by one as an atomic operation.

    Returns the resulting decremented value.

    @related atomic_t
    @ingroup Concurrency
  */
PT_API int atomicDecrement(volatile atomic_t& val);

/** @brief Performs an atomic exchange operation.

    Sets \a val to \a exch and returns the initial value of \a val.

    @related atomic_t
    @ingroup Concurrency
*/
PT_API int atomicExchange(volatile atomic_t& val, int exch);

/** @brief Performs an atomic compare-and-exchange operation.

    If \a val is equal to \a comp, \a val is replaced by \a exch. The initial
    value of of \a val is returned.

    @related atomic_t
    @ingroup Concurrency
*/
PT_API int atomicCompareExchange(volatile atomic_t& val, int exch, int comp);

/** @brief Performs atomic addition of two values.

    Returns the initial value of the addend.

    @related atomic_t
    @ingroup Concurrency
*/
PT_API int atomicExchangeAdd(volatile atomic_t& val, int add);

/** @brief Performs an atomic exchange operation.

    Sets \a val to \a exch and returns the initial value of \a val.

    @related atomic_t
    @ingroup Concurrency
*/
PT_API void* atomicExchange(void* volatile& val, void* exch);

/** @brief Performs an atomic compare-and-exchange operation.

    If \a val is equal to \a comp, \a val is replaced by \a exch. The initial
    value of \a ptr is returned.

    @related atomic_t
    @ingroup Concurrency
*/
PT_API void* atomicCompareExchange(void* volatile& val, void* exch, void* comp);

} // namespace Pt

#endif
