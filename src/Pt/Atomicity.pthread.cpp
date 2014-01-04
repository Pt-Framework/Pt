/*
 * Copyright (C) 2006-2010 by Dr. Marc Boris Duerner
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

#include <cassert>
#include <pthread.h>

namespace Pt {

typedef void(*cleanup_proc_t)(void *);

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

atomic_t::atomic_t(int v)
: i(v)
{}

int atomicGet(volatile atomic_t& val)
{
    int ret = 0;

    pthread_cleanup_push((cleanup_proc_t)pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ret = val.i;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);
    pthread_cleanup_pop(0);

    return ret;
}

void atomicSet(volatile atomic_t& val, int n)
{
    pthread_cleanup_push((cleanup_proc_t)pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    val.i = n;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);
    pthread_cleanup_pop(0);
}

int atomicIncrement(volatile atomic_t& val)
{
    int ret = 0;

    pthread_cleanup_push((cleanup_proc_t)pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ++val.i;
    ret = val.i;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop(0);

    return ret;
}

int atomicDecrement(volatile atomic_t& val)
{
    int ret = 0;

    pthread_cleanup_push((cleanup_proc_t)pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    --val.i;
    ret = val.i;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop(0);

    return ret;
}

int atomicExchange(volatile atomic_t& val, int exch)
{
    int ret = 0;

    pthread_cleanup_push((cleanup_proc_t)pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ret = val.i;
    val.i = exch;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop(0);

    return ret;
}

int atomicCompareExchange(volatile atomic_t& val, int exch, int comp)
{
    int ret = 0;

    pthread_cleanup_push((cleanup_proc_t)pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ret = val.i;
    if(ret == comp) val.i = exch;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop(0);

    return ret;
}

int atomicExchangeAdd(volatile atomic_t& val, int add)
{
    int ret = 0;

    pthread_cleanup_push((cleanup_proc_t)pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ret = val.i;
    val.i += add;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop(0);

    return ret;
}

void* atomicExchange(void* volatile& val, void* exch)
{
    void* ret = 0;

    pthread_cleanup_push((cleanup_proc_t)pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ret = val;
    val = exch;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop(0);

    return ret;
}

void* atomicCompareExchange(void* volatile& val, void* exch, void* comp)
{
    void* ret = 0;

    pthread_cleanup_push((cleanup_proc_t)pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ret = val;
    if(ret == comp) val = exch;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop(0);

    return ret;
}

} // namespace Pt


