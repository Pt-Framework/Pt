/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_API_CONCURRENCY_H
#define PT_SYSTEM_API_CONCURRENCY_H

/** @addtogroup Pt-System-Concurrency

    @brief Threads, mutexes, condition variables, queues and processes.

    A process can run more than one thread of control. %Thread is the
    portable thread. Construction does not start it. start() runs a
    %Callable or an %EventLoop. The thread must be joined or detached
    before it is destroyed. %AttachedThread joins in its destructor.
    %DetachedThread runs without a waiter and destroys itself when the
    entry returns.

    %Mutex serializes access to shared data. It is not recursive: the
    same thread must not lock it again. %MutexLock locks in the
    constructor and unlocks in the destructor, including during stack
    unwinding. %RecursiveMutex allows the owning thread to lock again.
    %ReadWriteMutex allows concurrent readers or one writer. %SpinMutex
    is for short critical sections. Atomic integers are documented with
    the core module.

    %Condition waits while a %Mutex or %MutexLock is held. wait() unlocks,
    suspends the caller, and relocks when the wait ends. signal() wakes
    one waiter. broadcast() wakes all. %Semaphore counts. wait()
    decrements when the count is positive. post() increments.

    %Queue is a thread-safe FIFO. get() blocks while the queue is empty.
    put() blocks when a maximum size is set and the queue is full. A
    maximum of zero means no limit.

    %Process starts another program from %ProcessInfo. start() runs it.
    wait() joins it. Redirected stdin, stdout, and stderr are
    %IODevice endpoints.
*/

#endif
