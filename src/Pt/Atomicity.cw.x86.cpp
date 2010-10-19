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

#include <Pt/Atomicity.cw.x86.h>

namespace Pt {

static long MyInterlockedIncrement(volatile long* param)
{
    __asm
    {
        mov ecx, param
        mov eax,1
        lock xadd dword ptr [ecx],eax
        inc eax
    }
}

static long MyInterlockedDecrement(volatile long* param)
{
    __asm
    {
        mov ecx, param
        mov eax,-1
        lock xadd dword ptr [ecx],eax
        dec eax
    }
}

static long MyInterlockedExchangeAdd(volatile long* param, long t)
{
    __asm
    {
        mov ecx, param
        mov eax, t
        lock xadd dword ptr [ecx],eax
    }
}

static long MyInterlockedExchange(volatile long* param, long t)
{
    __asm
    {
        mov ecx,param
        mov edx,t
        mov eax,dword ptr [ecx]
_loop:
        lock cmpxchg dword ptr [ecx],edx
        jne _loop
    }
}

static void* MyInterlockedExchangePointer(void* volatile* param, void* t)
{
    __asm
    {
        mov ecx,dword ptr param
        mov edx,dword ptr t
        mov eax,dword ptr [ecx]
_loop:
        lock cmpxchg dword ptr [ecx],edx
        jne _loop
    }
}

static long MyInterlockedCompareExchange(volatile long* param, long t, long c)
{
    __asm
    {
        mov ecx,param
        mov edx,t
        mov eax,c
        lock cmpxchg dword ptr [ecx],edx
    }
}

static void* MyInterlockedCompareExchangePointer(void* volatile* param, void* t, void* c)
{
    __asm
    {
        mov ecx,param
        mov edx,t
        mov eax,c
        lock cmpxchg dword ptr [ecx],edx
    }
}


atomic_t atomicGet(volatile atomic_t& val)
{
    atomic_t Barrier;
    __asm
    {
        xchg Barrier, eax
    }

    return val;
}


void atomicSet(volatile atomic_t& val, atomic_t n)
{
    val = n;

    atomic_t Barrier;
    __asm
    {
        xchg Barrier, eax
    }
}


atomic_t atomicIncrement(volatile atomic_t& value)
{
    return MyInterlockedIncrement( const_cast<atomic_t*>(&value) );
}


atomic_t atomicDecrement(volatile atomic_t& value)
{
    return MyInterlockedDecrement( const_cast<atomic_t*>(&value) );
}


atomic_t atomicExchangeAdd(volatile atomic_t& value, atomic_t n)
{
    return MyInterlockedExchangeAdd(const_cast<atomic_t*>(&value), n);
}


atomic_t atomicExchange(volatile atomic_t& value, atomic_t new_val)
{
    return MyInterlockedExchange(const_cast<atomic_t*>(&value), new_val);
}


void* atomicExchange(void* volatile& ptr, void* new_val)
{
    return MyInterlockedExchangePointer( const_cast<void**>(&ptr), new_val );
}


atomic_t atomicCompareExchange(volatile atomic_t& value, atomic_t ex, atomic_t cmp)
{
    return MyInterlockedCompareExchange(const_cast<atomic_t*>(&value), ex, cmp);
}


void* atomicCompareExchange(void* volatile& ptr, void* ex, void* cmp)
{
    return MyInterlockedCompareExchangePointer(&ptr, ex, cmp);
}


////////////////////////////////////////// BELOW ARE FOR TEMPORARY TESTING ///////////////////////////////////////////

new_atomic_t::new_atomic_t(int v)
: l(v)
{}

int new_atomicGet(volatile new_atomic_t& val)
{
    long Barrier;
    __asm
    {
        xchg Barrier, eax
    }

    return val.l;
}

void new_atomicSet(volatile new_atomic_t& val, int n)
{
    val.l = n;

    atomic_t Barrier;
    __asm
    {
        xchg Barrier, eax
    }
}

int new_atomicIncrement(volatile new_atomic_t& val)
{
    __asm
    {
        mov       ecx, val.l
        mov       eax, 1
        lock xadd dword ptr [ecx], eax
        inc       eax
    }
}

int new_atomicDecrement(volatile new_atomic_t& val)
{
    __asm
    {
        mov       ecx, val.l
        mov       eax, -1
        lock xadd dword ptr [ecx], eax
        dec       eax
    }
}

int new_atomicExchange(volatile new_atomic_t& val, int exch)
{
    __asm
    {
        mov          ecx, val.l
        mov          edx, exch
        mov          eax, dword ptr [ecx]
_loop:
        lock cmpxchg dword ptr [ecx], edx
        jne          _loop
    }
}

int new_atomicCompareExchange(volatile new_atomic_t& val, int exch, int comp)
{
    __asm
    {
        mov          ecx, val.l
        mov          edx, exch
        mov          eax, comp
        lock cmpxchg dword ptr [ecx], edx
    }
}

int new_atomicExchangeAdd(volatile new_atomic_t& val, int add)
{
    __asm
    {
        mov       ecx, val.l
        mov       eax, add
        lock xadd dword ptr [ecx], eax
    }
}

void* new_atomicExchange(void* volatile& val, void* exch)
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

void* new_atomicCompareExchange(void* volatile& val, void* exch, void* comp)
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

