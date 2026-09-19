/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_API_ATOMICS_H
#define PT_API_ATOMICS_H

/** @addtogroup Pt-Atomics

    @brief Lock-free atomic integer and pointer operations.

    These functions are the portable lock-free cell. They operate on
    %Pt::atomic_t, a union that holds an integer or a pointer at the
    width the platform uses for atomic instructions. Construct an
    %atomic_t with an initial integer, or leave it at zero. Pass it as
    a volatile reference to every operation. Do not read or write the
    union members directly; that bypasses the atomic instruction and
    the memory fence.

    %atomicGet() returns the current integer and then issues an acquire
    fence, so later loads and stores in program order cannot move
    before the get. %atomicSet() issues a release fence and then stores
    a new integer, so earlier loads and stores cannot move after the
    set. Together they publish a value from one thread and observe it
    on another without a mutex.

    %atomicIncrement() and %atomicDecrement() add or subtract one and
    return the resulting value. They are the usual way to implement a
    reference count: increment on each new owner, decrement on each
    release, and destroy when the decrement returns zero.

    %atomicExchange() stores a new integer and returns the previous
    one. %atomicCompareExchange() stores @a exch only when the cell
    still holds @a comp, and returns the value that was actually
    present. The compare-exchange is the building block for lock-free
    updates: read, compute a new value, and retry until the cell has
    not changed in between. %atomicExchangeAdd() adds an integer and
    returns the previous value.

    Pointer overloads of %atomicExchange() and %atomicCompareExchange()
    do the same operations on a @c void* volatile cell. Use them for
    lock-free lists and other pointer structures. The integer and
    pointer forms are separate overloads; they do not convert.

    These operations are the primitives under @ref Pt-System-Concurrency.
    A mutex, a condition variable, or a queue is the right tool when
    the critical section is more than one cell. Use this group when a
    single integer or pointer must change without a lock, and when the
    acquire or release fence is the only ordering the algorithm needs.
*/

#endif
