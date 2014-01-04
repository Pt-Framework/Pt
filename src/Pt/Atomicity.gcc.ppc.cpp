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

#include <csignal>

namespace Pt {

atomic_t::atomic_t(int v)
: i32(v)
{}

int atomicGet(volatile atomic_t& val)
{
    asm volatile ( "sync" : : : "memory" );
    return val.i32;
}

void atomicSet(volatile atomic_t& val, int n)
{
    val.i32 = n;
    asm volatile ( "sync" : : : "memory" );
}

int atomicIncrement(volatile atomic_t& val)
{
    int result = 0, tmp;
    asm volatile ( "\n1:\n\t"
                   "lwarx  %0, 0, %2\n\t"
                   "addi   %1, %0, 1\n\t"
                   "stwcx. %1, 0, %2\n\t"
                   "bne-   1b\n"
                   "isync\n"
                   : "=&b"(result),  "=&b"(tmp)
                   :   "r"(&val.i32)
                   :  "cc", "memory" );
    return result + 1;
}

int atomicDecrement(volatile atomic_t& val)
{
    int result = 0, tmp;
    asm volatile ( "\n1:\n\t"
                   "lwarx  %0, 0, %2\n\t"
                   "addi   %1, %0, -1\n\t"
                   "stwcx. %1, 0, %2\n\t"
                   "bne-   1b\n"
                   "isync\n"
                   : "=&b"(result),  "=&b"(tmp)
                   :   "r"(&val.i32)
                   : "cc", "memory" );

    return result - 1;
}

int atomicExchange(volatile atomic_t& val, int exch)
{
    volatile int ret = 0;

    asm volatile (
        "\n1:\n\t"
        "lwarx  %0, 0, %2\n\t"
        "stwcx. %3, 0, %2\n\t"
        "bne    1b\n"
        "isync\n"
        : "=r"(ret)
        :  "0"(ret), "b"(&val.i32), "r"(exch)
        : "cc", "memory" );

    return ret;}

int atomicCompareExchange(volatile atomic_t& val, int exch, int comp)
{

    int tmp = 0;
    asm volatile ( "\n1:\n\t"
                   "lwarx  %0, 0, %1\n\t"
                   "cmpw   %0, %2\n\t"
                   "bne-   2f\n\t"
                   "stwcx. %3, 0, %1\n\t"
                   "bne-   1b\n"
                   "2:\n"
                   "isync\n"
                   : "=&r"(tmp)
                   :   "b"(&val.i32), "r"(comp), "r"(exch)
                   : "cc", "memory" );

    return tmp;
}

int atomicExchangeAdd(volatile atomic_t& val, int add)
{
    int result, tmp;
    asm volatile ( "\n1:\n\t"
                   "lwarx  %0, 0, %2\n\t"
                   "add    %1, %0, %3\n\t"
                   "stwcx. %1, 0, %2\n\t"
                   "bne    1b\n"
                   "isync\n"
                   : "=&r"(result),   "=&r"(tmp)
                   :   "r"(&val.i32),   "r"(add)
                   : "cc", "memory" );
    return result;
}

void* atomicExchange(void* volatile& val, void* exch)
{
    void* ret = 0;

    asm volatile (
        "0:    lwarx  %0, 0, %1\n\t"
        "      stwcx. %2, 0, %1\n\t"
        "      bne-   0b       \n\t"
        "      isync"
        : "=&r"(ret)
        :   "r"(&val), "r"(exch)
        : "cr0", "memory", "r0" );

    return ret;
}

void* atomicCompareExchange(void* volatile& val, void* exch, void* comp)
{
    void* tmp = 0;
    asm volatile ( "\n1:\n\t"
                   "lwarx  %0, 0, %1\n\t"
                   "cmpw   %0, %2\n\t"
                   "bne-   2f\n\t"
                   "stwcx. %3, 0, %1\n\t"
                   "bne-   1b\n"
                   "2:"
                   "isync\n"
                   : "=&r"(tmp)
                   :   "b"(&val), "r"(comp), "r"(exch)
                   : "cc", "memory" );

    return tmp;
}

} // namespace Pt
