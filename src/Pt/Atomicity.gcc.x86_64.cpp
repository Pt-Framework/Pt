/*
 * Copyright (C) 2006-2010 by Marc Boris Duerner
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

#include <Pt/Atomicity.h>

#include <Pt/Types.h>

namespace Pt {

atomic_t::atomic_t(int v)
: i64(v)
{}

int atomicGet(volatile atomic_t& val)
{
    asm volatile ( "mfence" : : : "memory" );
    return val.i64;
}

void atomicSet(volatile atomic_t& val, int n)
{
    val.i64 = n;
    asm volatile ( "mfence" : : : "memory" );
}

int atomicIncrement(volatile atomic_t& val)
{
    volatile register int64_t tmp;

    asm volatile ( "lock; xaddq %0, %1"
                   : "=r"(tmp), "=m"(val.i64)
                   :  "0"(1),    "m"(val.i64) );
    return tmp + 1;
}

int atomicDecrement(volatile atomic_t& val)
{
    volatile register int64_t tmp;

    asm volatile ( "lock; xaddq %0, %1"
                   : "=r"(tmp), "=m"(val.i64)
                   :  "0"(-1),   "m"(val.i64) );
    return tmp - 1;
}

int atomicExchange(volatile atomic_t& val, int exch)
{
    volatile register int64_t ret;

    // Using cmpxchg and a loop here on purpose
    asm volatile ( "1:; lock; cmpxchgq %2, %0; jne 1b"
                   : "=m"(val.i64),    "=a"(ret)
                   :  "r"((int64_t)exch),  "m"(val.i64), "a"(val.i64) );

    return ret;
}

int atomicCompareExchange(volatile atomic_t& val, int exch, int comp)
{
    volatile register int64_t old;

    asm volatile ( "lock; cmpxchgq %2, %0"
                   : "=m"(val.i64),    "=a"(old)
                   :  "r"((int64_t)exch),  "m"(val.i64), "a"((int64_t) comp) );
    return old;
}

int atomicExchangeAdd(volatile atomic_t& val, int add)
{
    volatile register int64_t ret;

    asm volatile ( "lock; xaddq %0, %1"
                   : "=r"(ret),       "=m"(val.i64)
                   :  "0"((int64_t)add),  "m"(val.i64) );

    return ret;
}

void* atomicExchange(void* volatile& val, void* exch)
{
    void* ret;

    asm volatile ( "1:; lock; cmpxchgq %2, %0; jne 1b"
                   : "=m"(val),  "=a"(ret)
                   :  "r"(exch),  "m"(val), "a"(val) );

    return ret;
}

void* atomicCompareExchange(void* volatile& val, void* exch, void* comp)
{
    void* old;

    asm volatile ( "lock; cmpxchgq %2, %0"
                   : "=m"(val),  "=a"(old)
                   :  "r"(exch),  "m"(val), "a" (comp) );
    return old;
}

} // namespace Pt
