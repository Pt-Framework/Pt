/* Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
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
