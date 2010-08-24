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

        public:
            static const std::size_t WaitInfinite = static_cast<const std::size_t>(-1);

            /** @brief Destructs the EventLoop
            */
            virtual ~EventLoop();

            Allocator& allocator()
            { return *_usedalloc; }

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

            /** @brief Starts the event loop
            */
            void run();

            /** @brief Stops the %EventLoop.
            */
            void exit();

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
            EventLoop();

            /** @brief Construct an EventLoop with a custom allocator
            */
            EventLoop(Allocator& a);

            /** @internal Update all timers and return true if a timer fired

                @param timeout interval to next expiring timer
            */
            bool updateTimer(size_t& timeout);

            size_t runNext(WaitResult& result);

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

            virtual void onAttach(Selectable&) = 0;

            virtual void onDetach(Selectable&) = 0;

            /** @brief A Selectable is added to this %Selector

                Does not throw exceptions.
            */
            virtual void onAdd(Selectable&) = 0; // TODO: onEnable

            /** @brief A Selectable is removed from this %Selector

                Does not throw exceptions.
            */
            virtual void onRemove(Selectable&) = 0; // TODO: onDisable

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

        private:
            //! @internal
            Allocator _allocator;

            //! @internal
			Allocator* _usedalloc;

            //! @internal
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
            bool _exitLoop;

            //! @internal
            std::deque<Event* > _eventQueue;

            //! @internal
            RecursiveMutex _queueMutex;

            //! @internal
            void* _reserved;
    };

} // namespace System

} // namespace Pt

#endif
