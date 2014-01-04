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

/** @brief Atomic integers to be used with atomicity functions.

    A variable of this type must not be accessed directly, but used the 
    provided atomicity functions atomicSet() and atomicGet().

    @ingroup CoreTypes
*/
union PT_API atomic_t
{
    int     i;   // 32 bit in both 32 bit and 64 bit platforms
    long    l;   // 32 bit in 32 bit platform and 64 bit in 64 bit platform
    int32_t i32; // Always 32-bit in any platform
    int64_t i64; // Always 64-bit in any platform
    void*   p;   // should follow the system word size (and hence will be 128 bit in a hypothetical 128 bit platform ;)
#ifdef __cplusplus
    //! @brief Construct with initial value.
    explicit atomic_t(int v = 0);
#endif
};

/** @brief Atomically get a value.

    Returns the value after employing a memory fence (before the get).

    @ingroup CoreTypes
*/
PT_API int atomicGet(volatile atomic_t& val);

/** @brief Atomically set a value.

    Sets the value and employs a memory fence (after the set).

    @ingroup CoreTypes
*/
PT_API void atomicSet(volatile atomic_t& val, int n);

/** @brief Increases a value by one as an atomic operation.

    Returns the resulting incremented value.

    @ingroup CoreTypes
*/
PT_API int atomicIncrement(volatile atomic_t& val);

/** @brief Decreases a value by one as an atomic operation.

    Returns the resulting decremented value.

    @ingroup CoreTypes
  */
PT_API int atomicDecrement(volatile atomic_t& val);

/** @brief Performs an atomic exchange operation.

    Sets \a val to \a exch and returns the initial value of \a val.

    @ingroup CoreTypes
*/
PT_API int atomicExchange(volatile atomic_t& val, int exch);

/** @brief Performs an atomic compare-and-exchange operation.

    If \a val is equal to \a comp, \a val is replaced by \a exch. The initial
    value of of \a val is returned.

    @ingroup CoreTypes
*/
PT_API int atomicCompareExchange(volatile atomic_t& val, int exch, int comp);

/** @brief Performs atomic addition of two values.

     Returns the initial value of the addend.

    @ingroup CoreTypes
*/
PT_API int atomicExchangeAdd(volatile atomic_t& val, int add);

/** @brief Performs an atomic exchange operation.

    Sets \a val to \a exch and returns the initial value of \a val.

    @ingroup CoreTypes
*/
PT_API void* atomicExchange(void* volatile& val, void* exch);

/** @brief Performs an atomic compare-and-exchange operation.

    If \a val is equal to \a comp, \a val is replaced by \a exch. The initial
    value of \a ptr is returned.

    @ingroup CoreTypes
*/
PT_API void* atomicCompareExchange(void* volatile& val, void* exch, void* comp);

} // namespace Pt

#endif
