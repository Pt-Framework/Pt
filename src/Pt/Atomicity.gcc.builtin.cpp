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
#include <cstddef>

namespace Pt {

atomic_t::atomic_t(int v)
: i(v)
{}

int atomicGet(volatile atomic_t& val)
{
    __atomic_load_n((&val.i), __ATOMIC_ACQUIRE );
    return val.i;
}

void atomicSet(volatile atomic_t& val, int n)
{
    __atomic_store_n((&val.i), n, __ATOMIC_RELEASE);
}

int atomicIncrement(volatile atomic_t& val)
{
    return __atomic_add_fetch((&val.i), 1, __ATOMIC_SEQ_CST);
}

int atomicDecrement(volatile atomic_t& val)
{
    return __atomic_sub_fetch((&val.i), 1, __ATOMIC_SEQ_CST);
}

int atomicExchange(volatile atomic_t& val, int exch)
{
  return __atomic_exchange_n((&val.i), exch, __ATOMIC_SEQ_CST);
}

int atomicCompareExchange(volatile atomic_t& val, int exch, int comp)
{
    __atomic_compare_exchange((&val.i), &comp, &exch, false, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED);
    return comp;
}

int atomicExchangeAdd(volatile atomic_t& val, int add)
{
    return __atomic_fetch_add((&val.i), add, __ATOMIC_SEQ_CST);
}

void* atomicExchange(void* volatile& val, void* exch)
{
    return __atomic_exchange_n( &val, exch, __ATOMIC_SEQ_CST);
}

void* atomicCompareExchange(void* volatile& val, void* exch, void* comp)
{
    __atomic_compare_exchange_n( &val, &comp, exch, false, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED);
    return comp;
}

} // namespace Pt
