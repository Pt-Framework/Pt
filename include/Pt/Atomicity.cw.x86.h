/***************************************************************************
 *   Copyright (C) 2008 by PTV AG                                          *
 *   Copyright (C) 2008 by Peter Barth                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_ATOMICITY_CW_X86_H_
#define PT_ATOMICITY_CW_X86_H_

namespace Pt {

typedef long atomic_t;

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

inline atomic_t atomicIncrement(volatile atomic_t& value)
{
    return MyInterlockedIncrement( const_cast<atomic_t*>(&value) );
}


inline atomic_t atomicDecrement(volatile atomic_t& value)
{
    return MyInterlockedDecrement( const_cast<atomic_t*>(&value) );
}


inline atomic_t atomicExchangeAdd(volatile atomic_t& value, atomic_t n)
{
    return MyInterlockedExchangeAdd(const_cast<atomic_t*>(&value), n);
}


inline atomic_t atomicExchange(volatile atomic_t& value, atomic_t new_val)
{
    return MyInterlockedExchange(const_cast<atomic_t*>(&value), new_val);
}


inline void* atomicExchange(void* volatile& ptr, void* new_val)
{
    return MyInterlockedExchangePointer( const_cast<void**>(&ptr), new_val );
}


inline atomic_t atomicCompareExchange(volatile atomic_t& value, atomic_t ex, atomic_t cmp)
{
    return MyInterlockedCompareExchange(const_cast<atomic_t*>(&value), ex, cmp);
}


inline void* atomicCompareExchange(void* volatile& ptr, void* ex, void* cmp)
{
    return MyInterlockedCompareExchangePointer(&ptr, ex, cmp);
}

} // namespace Pt

#endif
