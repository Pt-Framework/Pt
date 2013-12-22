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

namespace Pt {

namespace System {

class Timer;
class Selectable;
class Selector;

/** @brief Thread-safe event loop supporting I/O multiplexing and Timers.
*/
class PT_SYSTEM_API EventLoop : public Connectable
                              , public EventSink
{
    friend class Selectable;
    friend class Timer;

    public:
        static const std::size_t WaitInfinite = static_cast<const std::size_t>(-1);
        static const std::size_t WaitMax = WaitInfinite - 1;

        /** @brief Destructs the EventLoop.
        */
        virtual ~EventLoop();

        /** @brief Starts the event loop.
        */
        void run();

        /** @brief Stops the %EventLoop.
        */
        void exit();

        /** @brief Reports all events
        */
        Signal<const Event&>& eventReceived()
        { return _event; }

        /** @brief Emited when the eventloop is exited.
        */
        Signal<>& exited()
        { return _exited; }

        /** @brief Set the Selectable to ready-state.
        */
        void setReady(Selectable& s)
        { this->onReady(s); }

        //! @ internal
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

//! @ internal
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

//! @ internal
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

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_EVENTLOOP_H
