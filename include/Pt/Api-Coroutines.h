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

#ifndef PT_API_COROUTINES_H
#define PT_API_COROUTINES_H

/** @addtogroup Pt-Coroutines

    @brief Cancellable, co_await-able tasks and lazily produced value sequences.

    A %Task is a C++20 coroutine that produces a single result and can
    be cancelled while it is suspended. A %Generator yields a sequence
    of values and may also suspend. Both types integrate with the
    event loop: awaitable operations resume the coroutine when I/O,
    timers or other loop-driven work complete.

    These types are available when the program is compiled as C++20
    or later.

    Use a function that returns %Task as an asynchronous unit of work.
    The coroutine starts suspended. Use %Task::run() to begin execution
    on the current thread. After the task has finished, %Task::done()
    is true and %Task::result() returns the value.

    Inside a task, co_await suspends until an awaitable operation
    completes. Framework types already provide awaitables, for example
    %Pt::System::Timer::waitAsync() for a one-shot delay. Custom
    operations derive from %Awaiter or %BasicAwaiter.

    I/O awaitables typically resume from the event loop. The usual
    pairing is to start the task with %Task::run() and then run the
    loop so completions can resume the coroutine.

    Destroying a task or calling %Task::cancel() aborts the pending
    awaitable and destroys the coroutine frame. Nested tasks cancel
    the inner pending operation the same way. A generator cancels in
    the same manner.

    Exceptions that leave a coroutine body are stored. They are
    rethrown by %Task::result() and by co_await of that task.

    The following example starts a task that waits for a timer and
    then exits the loop:

    @code
    Pt::Task<> delayThenExit(Pt::System::EventLoop& loop,
                             Pt::System::Timer& timer)
    {
        co_await timer.waitAsync(1000);
        loop.exit();
    }

    int main()
    {
        Pt::System::MainLoop loop;

        Pt::System::Timer timer;
        timer.setActive(loop);

        Pt::Task<> task = delayThenExit(loop, timer);
        task.run();
        return loop.run();
    }
    @endcode
*/

#endif
