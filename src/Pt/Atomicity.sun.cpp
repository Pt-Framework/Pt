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

#include <Pt/Atomicity.h>

#include <sys/types.h>
#include <sys/atomic.h>

namespace Pt {

atomic_t::atomic_t(int v)
: l(v)
{}

int atomicGet(volatile atomic_t& val)
{
    long temp = val.l;
    membar_enter();
    return temp;
}

void atomicSet(volatile atomic_t& val, int n)
{
    membar_exit();
    val.l = n;
}

int atomicIncrement(volatile atomic_t& val)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&val.l);
    return atomic_inc_ulong_nv(uvalue);
}

int atomicDecrement(volatile atomic_t& val)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&val.l);
    return atomic_dec_ulong_nv(uvalue);
}

int atomicExchange(volatile atomic_t& val, int exch)
{
    volatile ulong_t* uvalue  = reinterpret_cast<volatile ulong_t*>(&val.l);
    volatile ulong_t  unewval = exch;
    return atomic_swap_ulong(uvalue, unewval);
}

int atomicCompareExchange(volatile atomic_t& val, int exch, int comp)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&val.l);
    volatile ulong_t  uex    = exch;
    volatile ulong_t  ucmp   = comp;
    return atomic_cas_ulong(uvalue, ucmp, uex);
}

int atomicExchangeAdd(volatile atomic_t& val, int add)
{
    volatile ulong_t* uvalue = reinterpret_cast<volatile ulong_t*>(&val.l);
    volatile ulong_t  un     = add;
    volatile ulong_t  result = atomic_add_long_nv(uvalue, un);
    return result - add;
}

void* atomicExchange(void* volatile& val, void* exch)
{
    return atomic_swap_ptr(&val, exch);
}

void* atomicCompareExchange(void* volatile& val, void* exch, void* comp)
{
    return atomic_cas_ptr(&val, comp, exch);
}

} // namespace Pt
