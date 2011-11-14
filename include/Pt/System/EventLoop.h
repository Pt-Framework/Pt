/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2010 Marc Boris Duerner
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
#include <Pt/System/EventSink.h>
#include <map>
#include <deque>

namespace Pt {

namespace System {

class Timer;
class Selectable;

class WaitResult
{
    enum ResultType
    {
        Timeout = 0,
        Event   = 1,
        Device  = 2,
        Timer   = 4,
        Exit    = 8,
        Init    = 16,
    };

    public:
        WaitResult()
        : _type(Timeout)
        {}

        void clear()
        { _type = Timeout; }

        bool isTimeout() const
        { return _type == WaitResult::Timeout; }

        bool isActive() const
        { return _type != WaitResult::Timeout; }

        bool isInit() const
        { return WaitResult::Init == (_type & WaitResult::Init); }

        void setInit()
        {
            _type |= WaitResult::Init;
        }

        bool isExit() const
        { return WaitResult::Exit == (_type & WaitResult::Exit); }

        void setExit()
        {
            _type |= WaitResult::Exit;
        }

        bool isEvent() const
        { return WaitResult::Event == (_type & WaitResult::Event); }

        void setEvent()
        {
            _type |= WaitResult::Event;
        }

        bool isDevice() const
        { return WaitResult::Device == (_type & WaitResult::Device); }

        void setDevice()
        {
            _type |= WaitResult::Device;
        }

        bool isTimer() const
        { return WaitResult::Timer == (_type & WaitResult::Timer); }

        void setTimer()
        {
            _type |= WaitResult::Timer;
        }

    private:
        int _type;
};

/** @brief Thread-safe event loop supporting I/O multiplexing and Timers.
*/
class PT_SYSTEM_API EventLoop : public Connectable
                              , public EventSink
{
    friend class Selectable;
    friend class Timer;
    friend class EventLoopImpl;
    friend class MainLoopImpl;

    //! @internal
    typedef std::multimap<Timespan, Timer*> TimerQueue;

    //! @internal
    typedef std::deque<Event*> EventQueue;

    public:
        static const std::size_t WaitInfinite = static_cast<const std::size_t>(-1);

        /** @brief Destructs the EventLoop
        */
        virtual ~EventLoop();

        Allocator& allocator();

        /** @brief Adds a Selectable

            Adds a Selectable to the selector. Selectable are removed
            automatically when they get destroyed.
        */
        void add(Selectable& s);

        /** @brief Removes a Selectable.
        */
        void remove(Selectable& s);

        /** @brief Adds a Timer

            Adds a Timer to the selector. Timers are removed
            automatically when they get destroyed.

            @param timer The device to add
        */
        void add(Timer& timer);

        /** @brief Removes a Timer

            @param timer The timer to remove
        */
        void remove(Timer& timer);

        /** @brief Starts the event loop
        */
        void run();

        /** @brief Stops the %EventLoop.
        */
        void exit();

        /** @brief Sets the idle timeout
        */
        void setIdleTimeout(size_t msecs);

        /** @brief Returns the idle timeout
        */
        size_t idleTimeout() const;

        /** @brief Notifies about wait timeouts
            This signal is send when the timeout given to a wait
            call of the selector expires and no activity occured.
        */
        Signal<>& timeout();

        /** @brief Reports all events
            TODO: rename to eventReady
        */
        Signal<const Event&>& event();

        /** @brief Emited when the eventloop is exited
        */
        Signal<>& exited();

    protected:
        /** @brief Constructs the EventLoop
        */
        EventLoop();

        /** @brief Construct an EventLoop with a custom allocator
        */
        EventLoop(Allocator& a);

        size_t runNext(WaitResult& result);

        /** @brief A Selectable is attached to this %Selector

            Does not throw exceptions.
        */
        virtual void onAttach(Selectable&) = 0;

        /** @brief A Selectable is detached from this %Selector

            Does not throw exceptions.
        */
        virtual void onDetach(Selectable&) = 0;

        /** @brief A Selectable is enabled

            Does not throw exceptions.
        */
        virtual void onEnable(Selectable&) = 0;

        /** @brief A Selectable is disabled

            Does not throw exceptions.
        */
        virtual void onDisable(Selectable&) = 0;

        /** @brief A Selectable is reinitialised and needs to be updated

            Does not throw exceptions.
        */
        virtual void onReinit(Selectable&) = 0; // TODO: maybe obsolete

        /** @brief A Selectable in this %Selector has changed

            Does not throw exceptions.
        */
        virtual void onChanged(Selectable& s) = 0; // TODO: onAvail

        virtual void onRun() = 0;

        virtual void onExit() = 0;

        virtual void onCommitEvent(const Event& event);

        virtual void onQueueEvent(const Event& event);

        virtual void onProcessEvents();

        //! @internal
        void onAddTimer(Timer& timer);

        //! @internal
        void onRemoveTimer( Timer& timer );

    private:
        EventLoopImpl* _impl;
};

//! @internal
class EventLoopImpl
{
    typedef std::multimap<Timespan, Timer*> TimerQueue;
    typedef std::deque<Event*> EventQueue;

    public:
        EventLoopImpl();

        EventLoopImpl(Allocator& a);

        virtual ~EventLoopImpl();

        void run(EventLoop& el);

        void exit(EventLoop& el);

        size_t runNext(WaitResult& result);

        Allocator& allocator()
        { return *_usedalloc; }

        void setIdleTimeout(size_t msecs)
        { _timeout = msecs; }

        size_t idleTimeout() const
        { return _timeout; }

        Signal<>& timeout()
        { return _timeoutSignal; }

        Signal<const Event&>& event()
        { return _event; }

        Signal<>& exited()
        { return _exited; }

        void commitEvent(EventLoop& el, const Event& event);

        void queueEvent(const Event& event);

        void processEvents();

        bool updateTimer(size_t& timeout);

        void addTimer(Timer& timer);

        void removeTimer( Timer& timer );

    private:
        RecursiveMutex _queueMutex;
        Allocator _allocator;
        Allocator* _usedalloc;
        EventQueue _eventQueue;
        TimerQueue _timers;
        size_t _timeout;
        int _state;
        Signal<> _timeoutSignal;
        Signal<const Event&> _event;
        Signal<> _exited;
};

} // namespace System

} // namespace Pt

#endif
