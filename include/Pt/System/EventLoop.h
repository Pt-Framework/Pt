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

/** @brief Thread-safe event loop supporting I/O multiplexing and Timers.

    The %EventLoop can be used as the central entity of a thread or process to
    dispatch application events and wait on multiple Selectables, IODevices or
    Timers for activity.

    Events can be added to the internal event queue, even from other threads
    using the method commitEvent() or queueEvent(). The
    first method will add the event to the internal queue and wake the
    event loop, the latter allows queing multiple events and it is up to
    the caller to wake the event loop by calling wake() when all
    events are added. When the event loop processes its event, the signal
    eventReceived is send for each processed event. Events are processed in
    the order they were added.

    To start the %MainLoop the method run() must be executed. It blocks
    until the event loop is stopped. To stop the loop, exit()
    can be called. The delivery of the events occurs inside the thread that
    started the execution of the event loop.
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
    @ingroup Pt-System
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
