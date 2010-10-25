/*
 * Copyright (C) 2008 by PTV AG
 * Copyright (C) 2008 by Peter Barth
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

namespace Pt {

atomic_t::atomic_t(int v)
: i32(v)
{}

int atomicGet(volatile atomic_t& val)
{
    long Barrier;
    __asm
    {
        xchg Barrier, eax
    }

    return val.i32;
}

void atomicSet(volatile atomic_t& val, int n)
{
    val.i32 = n;

    atomic_t Barrier;
    __asm
    {
        xchg Barrier, eax
    }
}

int atomicIncrement(volatile atomic_t& val)
{
    __asm
    {
        mov       ecx, val.i32
        mov       eax, 1
        lock xadd dword ptr [ecx], eax
        inc       eax
    }
}

int atomicDecrement(volatile atomic_t& val)
{
    __asm
    {
        mov       ecx, val.i32
        mov       eax, -1
        lock xadd dword ptr [ecx], eax
        dec       eax
    }
}

int atomicExchange(volatile atomic_t& val, int exch)
{
    __asm
    {
        mov          ecx, val.i32
        mov          edx, exch
        mov          eax, dword ptr [ecx]
_loop:
        lock cmpxchg dword ptr [ecx], edx
        jne          _loop
    }
}

int atomicCompareExchange(volatile atomic_t& val, int exch, int comp)
{
    __asm
    {
        mov          ecx, val.i32
        mov          edx, exch
        mov          eax, comp
        lock cmpxchg dword ptr [ecx], edx
    }
}

int atomicExchangeAdd(volatile atomic_t& val, int add)
{
    __asm
    {
        mov       ecx, val.i32
        mov       eax, add
        lock xadd dword ptr [ecx], eax
    }
}

void* atomicExchange(void* volatile& val, void* exch)
{
    __asm
    {
        mov          ecx, dword ptr val
        mov          edx, dword ptr exch
        mov          eax, dword ptr [ecx]
_loop:
        lock cmpxchg dword ptr [ecx], edx
        jne          _loop
    }
}

void* atomicCompareExchange(void* volatile& val, void* exch, void* comp)
{
    __asm
    {
        mov          ecx, val
        mov          edx, exch
        mov          eax, cmp
        lock cmpxchg dword ptr [ecx], edx
    }
}

} // namespace Pt

