/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#ifndef PT_SYSTEM_EVENTLOOP_H
#define PT_SYSTEM_EVENTLOOP_H

#include <Pt/Event.h>
#include <Pt/Signal.h>
#include <Pt/Timespan.h>
#include <Pt/Allocator.h>
#include <Pt/Connectable.h>
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Timer.h>
#include <Pt/System/EventSink.h>
#include <map>
#include <deque>

#if __cplusplus >= 202002L
#include <Pt/Coroutine.h>
#include <Pt/System/Selectable.h>
#endif

namespace Pt {

namespace System {

class Timer;
class Selectable;
class Selector;

#if __cplusplus >= 202002L
class AsyncYield;
#endif

/** @brief Event loop of a thread or process.

    %EventLoop is the dispatch core of a thread or process. It
    monitors %Selectable objects and %Timer objects and delivers
    queued events. A process often runs one loop in the main thread.
    A second loop can run in another %Thread.

    The loop is an %EventSink. commitEvent() queues an event and wakes
    the loop. queueEvent() queues without waking, so several events can
    be added and then released with one wake(). Events are delivered on
    eventReceived in the thread that called run(), in the order they
    were queued.

    run() enters the loop and returns when exit() stops it. processEvents()
    delivers queued events without entering run(). exited is emitted
    when the loop leaves run(). Delivery always happens in the loop
    thread, including events queued from other threads.

    @ingroup Pt-System-EventLoop
*/
class PT_SYSTEM_API EventLoop : public Connectable
                              , public EventSink
{
    friend class Selectable;
    friend class Timer;

    public:
        /** @internal Indicates to wait forever.
        */
        static const std::size_t WaitInfinite = static_cast<const std::size_t>(-1);

        /** @internal Maximum wait interval in milliseconds.
        */
        static const std::size_t WaitMax = WaitInfinite - 1;

        /** @brief Destructor.
        */
        virtual ~EventLoop();

        /** @brief Starts the loop.
        */
        void run();

        /** @brief Stops the loop.
        */
        void exit();

        /** @brief Process all queued events.
        */
        void processEvents();

        /** @brief Reports all events.
        */
        Signal<const Event&>& eventReceived()
        { return _event; }

        /** @brief Emited when the eventloop is exited.
        */
        Signal<>& exited()
        { return _exited; }

#if __cplusplus >= 202002L
        /** @brief Pauses a coroutine and delegates execution back to the loop.
            @return C++20 awaitable yielding execution to the loop processing queue.
        */
        AsyncYield yieldAsync();
#endif

        /** @brief Posts the loop to run a selectable.

            The event loop is woken up and the selectable's run function is
            called in the event loop thread. This function may be called from
            any thread, especially from a selectable, that is based on a
            parallel execution model e.g. using a worker thread.
        */
        void post(Selectable& s)
        {
            onReady(s);
            wake();
        }

        //! @brief Sets the Selectable as ready without waking the loop.
        void setReady(Selectable& s)
        {
            onReady(s);
        }

        //! @internal
        virtual Selector& selector() = 0;

    protected:
        //! @internal Constructor
        EventLoop();

        //! @internal Runs the loop
        virtual void onRun() = 0;

        //! @internal Exits the loop
        virtual void onExit() = 0;

        //! @internal EventSink interface
        virtual void onCommitEvent(const Event& ev) = 0;

        //! @internal EventSink interface
        virtual void onQueueEvent(const Event& ev) = 0;

        //! @internal EventSink interface
        virtual void onWake() = 0;

        //! @internal Process all queued events
        virtual void onProcessEvents() = 0;

        //! @internal A timer is attached
        virtual void onAttachTimer(Timer& timer) = 0;

        //! @internal A Timer is detached
        virtual void onDetachTimer(Timer& timer) = 0;

        //! @internal A Selectable is attached
        virtual void onAttachSelectable(Selectable&) = 0;

        //! @internal A Selectable is detached
        virtual void onDetachSelectable(Selectable&) = 0;

        //! @internal Mark the selectable as ready
        virtual void onReady(Selectable&) = 0;

        //! @internal Mark the selectable as not ready
        virtual void onCancel(Selectable&) = 0;

    private:
        Signal<> _exited;
        Signal<const Event&> _event;
};

//! @internal
class PT_SYSTEM_API EventQueue
{
    public:
        EventQueue();

        EventQueue(Allocator& a);

        virtual ~EventQueue();

        Allocator& allocator()
        { return *_usedalloc; }

        void exit();

        void pushEvent(const Event& event);

        bool processEvents(Signal<const Event&>& eventSignal);

    private:
        Mutex _mutex;
        Allocator _allocator;
        Allocator* _usedalloc;
        std::deque<Event*> _eventQueue;
        bool _exited;
};

//! @internal
class PT_SYSTEM_API TimerQueue
{
    typedef std::multimap<Timespan, Timer*> TimerMap;

    public:
        TimerQueue();

        virtual ~TimerQueue();

        void addTimer(Timer& timer);

        void removeTimer(Timer& timer);

        std::size_t processTimers();

    private:
        TimerMap _timers;
};

#if __cplusplus >= 202002L

/** @brief Awaitable that schedules a resume in the EventLoop.

    This splits heavy coroutine computations into chunks avoiding
    EventLoop starvation for socket or UI events.

    @ingroup Pt-System-EventLoop
*/
class PT_SYSTEM_API AsyncYield : public Pt::Awaiter
                               , private Pt::System::Selectable
{
    public:
        explicit AsyncYield(EventLoop& loop)
        {
            this->setActive(loop);
        }

        virtual ~AsyncYield()
        {
            Selectable::cancel();
        }

        void await_resume() noexcept
        {}

    protected:
        void onBegin() override
        {
            this->post();
        }

        void cancel() override
        {
            Selectable::cancel();
        }

        void onCancel() override
        {
            _handle = nullptr;
        }

        void onAttach(Pt::System::EventLoop&) override
        {}

        void onDetach(Pt::System::EventLoop&) override
        {}

        bool onRun() override
        {
            this->setReady();
            return true;
        }
};

inline AsyncYield EventLoop::yieldAsync()
{
    return AsyncYield(*this);
}

#endif // __cplusplus >= 202002L

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_EVENTLOOP_H
