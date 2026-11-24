/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_YIELD_H
#define PT_SYSTEM_YIELD_H

#include <Pt/System/Api.h>
#include <Pt/System/Selectable.h>
#include <Pt/Signal.h>

// windows.h defines a legacy empty Yield() macro that breaks the declarations below
#if defined(_WINBASE_) && defined(Yield)
#  undef Yield
#endif

namespace Pt {

namespace System {

/** @brief Defers a signal to a later turn of an event loop.

    %Yield is the callback counterpart of %AsyncYield. Where
    %AsyncYield resumes a coroutine in a later turn of the loop,
    %Yield emits a signal there. It is a %Selectable without an I/O
    endpoint. run() marks it ready, and the loop emits ready() the
    next time it runs ready selectables. The slot never runs inside
    run(), and it never runs before the code that called run() has
    returned.

    The typical use is an object that must not be destroyed or changed
    inside one of its own signal handlers. The handler calls run() and
    returns. The slot of ready() then does the work from a clean
    stack.

    @code
    Pt::System::MainLoop loop;
    Pt::System::Yield yield;
    yield.ready() += Pt::slot(onLater);
    yield.setActive(loop);
    yield.run();
    loop.run();
    @endcode

    run() is not the %Selectable::run() the loop calls to execute a
    ready selectable. %Yield does not expose that function. Its run()
    only requests the later emission.

    @par Coalescing
    The loop keeps one ready entry per selectable. Calling run()
    several times before the loop gets to run emits ready() once. A
    slot that needs a payload keeps its own list and drains it in the
    slot. Use an event with a payload instead when each request must
    arrive separately.

    @par Lifetime
    The object is attached with setActive() and does not own the loop.
    detach() removes it and drops a request that has not run. cancel()
    drops the request and stays attached. The destructor detaches, so
    no signal is emitted afterwards. When the loop is destroyed first,
    the loop detaches the object. loop() is then null.

    @par Threads
    ready() always runs in the thread of the loop. run() may be called
    from another thread, because the loop is woken the same way as for
    any other %Selectable.

    @par Event processing
    The signal is emitted by the loop in EventLoop::run().
    EventLoop::processEvents() delivers events only and does not emit
    it.

    @ingroup Pt-System-EventLoop
*/
class PT_SYSTEM_API Yield : private Selectable
{
    public:
        /** @brief Default constructor.

            Constructs an inactive object.
        */
        Yield();

        /** @brief Destructor.

            Detaches from the loop.
        */
        ~Yield();

        /** @brief Attaches to @a loop.

            Does nothing if already attached to @a loop.

            @throw %std::logic_error if attached to another loop.
        */
        void setActive(EventLoop& loop);

        /** @brief Removes from the loop and drops a pending request.
        */
        void detach();

        /** @brief Drops a pending request and stays attached.
        */
        void cancel();

        /** @brief Returns the used event loop, or null.
        */
        EventLoop* loop() const;

        /** @brief Requests that ready() is emitted in a later turn of the loop.

            Does nothing more when a request is already waiting.

            @throw %std::logic_error if not attached to a loop.
        */
        void run();

        /** @brief Returns the signal emitted in the loop after run().
        */
        Signal<>& ready();

    protected:
        virtual void onCancel();

        virtual bool onRun();

    private:
        Signal<> _ready;
};

} // namespace System

} // namespace Pt

#endif
