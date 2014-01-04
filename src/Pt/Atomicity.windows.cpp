/*
 * Copyright (C) 2006 by PTV AG
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

#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>

#include <Pt/Atomicity.h>

namespace Pt {

atomic_t::atomic_t(int v)
: l(v)
{}

int atomicGet(volatile atomic_t& val)
{
    long temp = val.l;
#if !defined(_WIN32_WCE) && (_MSC_VER >= 1400) && !defined(__GNUC__)
    MemoryBarrier();
#endif
    return temp;
}

void atomicSet(volatile atomic_t& val, int n)
{
#if !defined(_WIN32_WCE) && (_MSC_VER >= 1400) && !defined(__GNUC__)
    MemoryBarrier();
#endif
    val.l = n;
}

int atomicIncrement(volatile atomic_t& val)
{
    return InterlockedIncrement( const_cast<LONG*>(&val.l) );
}

int atomicDecrement(volatile atomic_t& val)
{
    return InterlockedDecrement( const_cast<LONG*>(&val.l) );
}

int atomicExchange(volatile atomic_t& val, int exch)
{
    return InterlockedExchange( const_cast<LONG*>(&val.l), exch );
}

int atomicCompareExchange(volatile atomic_t& val, int exch, int comp)
{
    return InterlockedCompareExchange( const_cast<LONG*>(&val.l), exch, comp );
}

int atomicExchangeAdd(volatile atomic_t& val, int add)
{
    return InterlockedExchangeAdd( const_cast<LONG*>(&val.l), add );
}

void* atomicExchange(void* volatile& val, void* exch)
{
    return InterlockedExchangePointer( const_cast<void**>(&val), exch );
}

void* atomicCompareExchange(void* volatile& val, void* exch, void* comp)
{
    return InterlockedCompareExchangePointer( const_cast<void**>(&val), exch, comp );
}

} // namespace Pt
