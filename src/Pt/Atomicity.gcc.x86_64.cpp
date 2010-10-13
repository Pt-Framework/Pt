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

#include <Pt/Atomicity.gcc.x86_64.h>
#include <Pt/Types.h>

namespace Pt {

atomic_t atomicGet(volatile atomic_t& val)
{
    asm volatile ("mfence" : : : "memory");
    return val;
}


void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;
    asm volatile ("mfence" : : : "memory");
}


atomic_t atomicIncrement(volatile atomic_t& val)
{
        static const atomic_t d = 1;
        atomic_t tmp;

        asm volatile ("lock; xaddq %0, %1"
                      : "=r" (tmp), "=m" (val)
                      : "0" (d), "m" (val));

        return tmp+1;
}

atomic_t atomicDecrement(volatile atomic_t& val)
{
        static const atomic_t d = -1;
        volatile register atomic_t tmp;

        asm volatile ("lock; xaddq %0, %1"
                      : "=r" (tmp), "=m" (val)
                      : "0" (d), "m" (val));

        return tmp-1;
}


atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add)
{
        volatile register atomic_t ret;

        asm volatile ("lock; xaddq %0, %1"
                      : "=r" (ret), "=m" (val)
                      : "0" (add), "m" (val));

        return ret;
}


atomic_t atomicExchange(volatile atomic_t& val, atomic_t new_val)
{
        volatile register atomic_t ret;

        // using cmpxchg and a loop here on purpose
        asm volatile ("1:; lock; cmpxchgq %2, %0; jne 1b"
                      : "=m" (val), "=a" (ret)
                      : "r" (new_val), "m" (val), "a" (val));

        return ret;
}


void* atomicExchange(void* volatile& val, void* new_val)
{
        void* ret;

        asm volatile ("1:; lock; cmpxchgq %2, %0; jne 1b"
                      : "=m" (val), "=a" (ret)
                      : "r" (new_val), "m" (val), "a" (val));

        return ret;
}


atomic_t atomicCompareExchange(volatile atomic_t& dest, atomic_t exch, atomic_t comp)
{
        volatile register atomic_t old;

        asm volatile ("lock; cmpxchgq %2, %0"
                      : "=m" (dest), "=a" (old)
                      : "r" (exch), "m" (dest), "a" (comp));
        return old;
}


void* atomicCompareExchange(void* volatile& dest, void* exch, void* comp)
{
        void* old;

        asm volatile ("lock; cmpxchgq %2, %0"
                      : "=m" (dest), "=a" (old)
                      : "r" (exch), "m" (dest), "a" (comp));
        return old;
}

////////////////////////////////////////// BELOW ARE FOR TEMPORARY TESTING ///////////////////////////////////////////

new_atomic_t::new_atomic_t(int v)
: l(v)
{}

int new_atomicGet(volatile new_atomic_t& val)
{
    asm volatile ( "mfence" : : : "memory" );
    return val.l;
}

void new_atomicSet(volatile new_atomic_t& val, int n)
{
    val.l = n;
    asm volatile ( "mfence" : : : "memory" );
}

int new_atomicIncrement(volatile new_atomic_t& val)
{
    volatile register long tmp;

    asm volatile ( "lock; xaddq %0, %1"
                   : "=r"(tmp), "=m"(val.l)
                   :  "0"(1),    "m"(val.l) );
    return tmp + 1;
}

int new_atomicDecrement(volatile new_atomic_t& val)
{
    volatile register long tmp;

    asm volatile ( "lock; xaddq %0, %1"
                   : "=r"(tmp), "=m"(val.l)
                   :  "0"(-1),   "m"(val.l) );
    return tmp - 1;
}

int new_atomicExchange(volatile new_atomic_t& val, int exch)
{
    volatile register long ret;

    // Using cmpxchg and a loop here on purpose
    asm volatile ( "1:; lock; cmpxchgq %2, %0; jne 1b"
                   : "=m"(val.l),      "=a"(ret)
                   :  "r"((long)exch),  "m"(val.l), "a"(val.l) );

    return ret;
}

int new_atomicCompareExchange(volatile new_atomic_t& val, int exch, int comp)
{
    volatile register long old;

    asm volatile ( "lock; cmpxchgq %2, %0"
                   : "=m"(val.l),      "=a"(old)
                   :  "r"((long)exch),  "m"(val.l), "a"((long) comp) );
    return old;
}

int new_atomicExchangeAdd(volatile new_atomic_t& val, int add)
{
    volatile register long ret;

    asm volatile ( "lock; xaddq %0, %1"
                   : "=r"(ret),       "=m"(val.l)
                   :  "0"((long)add),  "m"(val.l) );

    return ret;
}

void* new_atomicExchange(void* volatile& val, void* exch)
{
    void* ret;

    asm volatile ( "1:; lock; cmpxchgq %2, %0; jne 1b"
                   : "=m"(val),  "=a"(ret)
                   :  "r"(exch),  "m"(val), "a"(val) );

    return ret;
}

void* new_atomicCompareExchange(void* volatile& val, void* exch, void* comp)
{
    void* old;

    asm volatile ( "lock; cmpxchgq %2, %0"
                   : "=m"(val),  "=a"(old)
                   :  "r"(exch),  "m"(val), "a" (comp) );
    return old;
}

} // namespace Pt
