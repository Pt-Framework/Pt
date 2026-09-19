/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
