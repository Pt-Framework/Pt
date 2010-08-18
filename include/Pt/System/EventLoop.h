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
#include <Pt/Allocator.h>
#include <Pt/Timespan.h>
#include <Pt/NonCopyable.h>
#include <Pt/Connectable.h>
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/EventSink.h>
#include <deque>
#include <map>
#include <list>

namespace Pt {

namespace System {

    class Timer;
    class Selectable;
    class Application;
    class SelectorImpl;

    class WaitResult
    {
        enum ResultType
        {
            Timeout = 0x00,
            Event   = 0x01,
            Device  = 0x02,
            Timer   = 0x04
        };

        public:
            WaitResult()
            : _type(Timeout)
            {}

            bool isTimeout() const
            { return _type == WaitResult::Timeout; }

            bool isActive() const
            { return _type != WaitResult::Timeout; }

            bool isEvent() const
            { return WaitResult::Event == (_type & WaitResult::Event); }

             WaitResult& setEvent()
            {
                _type |= WaitResult::Event;
                return *this;
            }

            bool isDevice() const
            { return WaitResult::Device == (_type & WaitResult::Device); }

             WaitResult& setDevice()
            {
                _type |= WaitResult::Device;
                return *this;
            }

            bool isTimer() const
            { return WaitResult::Timer == (_type & WaitResult::Timer); }

            WaitResult& setTimer()
            {
                _type |= WaitResult::Timer;
                return *this;
            }

        private:
            int _type;
    };

    /** @brief Thread-safe event loop supporting I/O multiplexing and Timers.
    */
    class PT_SYSTEM_API EventLoopBase : public Connectable
                                      , public EventSink
    {
        friend class Selectable;
        friend class Timer;

        public:
            static const std::size_t WaitInfinite = static_cast<const std::size_t>(-1);

            /** @brief Destructs the EventLoop
            */
            virtual ~EventLoopBase();

            /** @brief Adds an IOResult

                Adds an IOResult to the selector. IOResult are removed
                automatically when they get destroyed.
            */
            void add(Selectable& s);

            /** @brief Cancel an IOResult.
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

            /** @brief Wakes the selctor from waiting

                This method can be used to end a Selector::wait call
                before the timeout expires. It is supposed to be used from
                another thread and thus is thread-safe.
            */
            void wake();

            /** @brief Starts the event loop
            */
            void run()
            { this->onRun(); }

            /** @brief Processes all events which are currently in the event queue
            */
            void processEvents()
            { this->onProcessEvents(); }

            /** @brief Stops the %EventLoop.
            */
            void exit()
            { this->onExit(); }

            /** @brief Sets the idle timeout
            */
            void setIdleTimeout(size_t msecs)
            { _timeout = msecs; }

            /** @brief Returns the idle timeout
            */
            unsigned int idleTimeout() const
            { return _timeout; }

            /** @brief Notifies about wait timeouts
                This signal is send when the timeout given to a wait
                call of the selector expires and no activity occured.
            */
            Signal<> timeout;

            /** @brief Reports all events
                TODO: rename to eventReady
            */
            Signal<const Event&> event;

            /** @brief Emited when the eventloop is exited
            */
            Signal<> exited;

        protected:
            /** @brief Constructs the EventLoop
            */
            EventLoopBase();

            /** @internal Update all timers and return true if a timer fired

                @param timeout interval to next expiring timer
            */

            bool updateTimer(size_t& timeout);
            /** @brief Wait for activity

                This method will wait for activity on the registered
                Selectables and Timers. Use Selector::WaitInfinite to
                wait without timeout.

                @param true on timeout
            */
            bool wait(std::size_t msecs = WaitInfinite);

            void onAddTimer(Timer& timer);

            void onRemoveTimer( Timer& timer );

            void onTimerChanged( Timer& timer );

            //void attach(Selectable& s)
            //{
            //    _selectables.push_back(&s);
            //    this->onAdd(s); // TODO: use onAttach
            //}

            //void detach(Selectable& s)
            //{
            //    _selectables.remove(&s);
            //    this->onRemove(s); // TODO: use onDetach
            //}

            /** @brief A Selectable is added to this %Selector

                Do not throw exceptions.
            */
            virtual void onAdd(Selectable&) = 0; // TODO: onEnable

            /** @brief A Selectable is removed from this %Selector

                Do not throw exceptions.
            */
            virtual void onRemove(Selectable&) = 0; // TODO: onDisable

            /** @brief A Selectable is reinitialised and needs to be updated

                Do not throw exceptions.
            */
            virtual void onReinit(Selectable&) = 0; // TODO: maybe obsolete

            /** @brief A Selectable in this %Selector has changed

                Do not throw exceptions.
            */
            virtual void onChanged(Selectable& s) = 0; // TODO: onAvail

            virtual bool onWait(std::size_t msecs) = 0;

            virtual void onWake() = 0;

            virtual void onRun() = 0;

            virtual void onExit() = 0;

            virtual void onProcessEvents() = 0;

        private:
            size_t _timeout;

            //! @internal
            typedef std::multimap<Timespan, Timer*> TimerMap;

            //! @internal
            //typedef std::list<Selectable*> SelectableList;

            //! @internal
            TimerMap _timers;

            //! @internal
            //SelectableList _selectables;

            //! @internal
            void* _reserved;
    };

    class EventLoopImpl;

    /** @brief Thread-safe event loop supporting I/O multiplexing and Timers.

        The System EventLoop can be used as the central entity of a thread or
        process to dispatch application events and wait on multiple IODevices or
        Timers for activity.

        Events can be added to the internal event queue, even from other threads
        using the method EventLoop::commitEvent or EventLoop::queueEvent. The
        first method will add the event to the internal queue and wake the
        event loop, the latter allows queing multiple events and it is up to
        the caller to wake the event loop by calling EventLoop::wake when all
        events are added. When the event loop processes its event, the signal
        "event" is send for each processed event. Events are processes in the
        order they were added.

        To start the %EventLoop the method EventLoop::run must be executed. It blocks
        until the event loop is stopped. To stop the %EventLoop, EventLoop::exit
        must be called. The delivery of the events occurs inside the thread that
        started the execution of the event loop.

        %IODevices and %Timers can be added to an %EventLoop just as to Selector.
        In fact a %Selector is used internally to implement the %EventLoop.

        Since the %EventLoop is a Runnable, it can be easily assigned to a Thread
        to give it its own event loop.
     */
    class PT_SYSTEM_API EventLoop : public EventLoopBase
    {
        public:
            /** @brief Constructs the EventLoop
            */
            EventLoop();

			EventLoop(Allocator& a);

            /** @brief Destructs the EventLoop
             */
            virtual ~EventLoop();

            WaitResult waitNext(size_t msecs);

        protected:
            virtual void onAdd( Selectable& s );

            virtual void onRemove( Selectable& s );

            virtual void onReinit(Selectable& s);

            virtual void onChanged(Selectable& s);

            virtual void onRun();

            virtual bool onWait(std::size_t msecs);

            virtual void onWake();

            virtual void onExit();

            virtual void onCommitEvent(const Event& event);

            virtual void onProcessEvents();

        private:
            bool _exitLoop;
            EventLoopImpl* _impl;
            Allocator _allocator;
			Allocator* _usedalloc;
            std::deque<Event* > _eventQueue;
            RecursiveMutex _queueMutex;
    };

} // namespace System

} // namespace Pt

#endif
