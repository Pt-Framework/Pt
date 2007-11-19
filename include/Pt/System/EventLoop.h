/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_SYSTEM_EVENTLOOP_H
#define PT_SYSTEM_EVENTLOOP_H

#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/System/Api.h>
#include <Pt/System/Condition.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/MutexLock.h>
#include <Pt/System/Runnable.h>
#include <Pt/System/Selector.h>
#include <Pt/Event.h>

#include <list>
#include <typeinfo>


namespace Pt {

namespace System {

    /** @brief Thread-safe event loop supporting I/O multiplexing and Timers.

        The System EventLoop can be used as the central entity of a thread or
        process to dispatch application events and wait on multiple IODevices or
        Timers for activity.

        Events can be added to the internal event queue, even from other threads
        using the method EventLoop::commitEvent or EventLoop::queueEvent. The
        first method will add the event to the internal queue and wake the
        event loop, the latter allows queing multiple event and it is up to
        the caller to wake the event loop by calling EventLoop::wake when all
        events are added. When the event loop processes its event, the signal
        "event" is send for each processed event. Events are processes in the
        order they were added.

        To start the %EventLoop the method EventLoop::run must be executed. It block
        until the event loop is stopped. To stop the %EventLoop, EventLoop::exit
        must be called. The delivery of the events occurs inside the Thread that
        started the execution of the event loop.

        %IODevices and %Timers can be added to an %EventLoop just as to Selector.
        In fact a %Selector is used internally to implement the %EventLoop.

        Since the %EventLoop is a Runnable, it can be easily assigned to a Thread
        to give it its own event loop.
     */
    class PT_SYSTEM_API EventLoop : public Connectable, public Runnable
    {
        public:
            /** @brief Constructs the EventLoop
            */
            EventLoop();

            /**
                @brief Destructs the EventLoop

                Delivers all outstanding events, which are still inside the event queue
                to the registered methods and functions.
             */
            ~EventLoop();

            /**
                @brief Starts the event loop

                This method is used to start the event loop. It will block until
                EventLoop::exit is called. During its execution events will be
                processed and send to the registered slots.
             */
            void run();

            /**
                @brief Adds an event and wakes up the loop.

                The event will be processed as soon as possible. If the event queue is
                currently empty, the event will be delivered immediately. If there are
                still events left in the queue, the event will be delivered after all
                events that are still currently in the event queue have been delivered.

                Since the given Event object is cloned before it is added to the event
                queue the Event object may be safely deleted or changed after it has
                been committed.

                This method is thread-safe, so the caller to this method is allowed to
                be any Thread.

                @param event Event to be added to the event loop.
             */
            void commitEvent(const Pt::Event& event);

            /**
                @brief Adds an event without waking the event loop

                The event is added to the end of the event queue without actually starting
                to process it. It be processed after EventLoop::wake was called or another
                event was committed to the event loop and which triggers the processing of
                events. In case the event loop is currently processing events the event
                queued with this method will also be delivered.
                You may use this method to batch-commit several events be queueing them
                and finally calling EventLoop::wake to start the processing of those events.

                Since the given Event object is cloned before it is added to the event
                queue the Event object may be safely deleted or changed after it has
                been committed.

                This method is thread-safe, so the caller to this method is allowed to
                be any Thread.

                @param event Event to be added to the event loop.
             */
            void queueEvent(const Pt::Event& event);

            /**
                @brief Processes all events which are currently in the event queue

                This method will return without work if there is no event in the event
                queue.

                This method is thread-safe, so the caller to this method is allowed to
                be in any Thread.
             */
            void processEvents();

            /**
                @brief Wakes up the %EventLoop to process events.

                This will only have an effect if the EventLoop::run method is currently waiting
                for new events. If it's not, the event queue is currently processed anyway.
             */
            void wake();

            /**
                @brief Stops the %EventLoop.

                Before the event loop is stopped, all events which are still in the
                event queue are processed. Calling this method will allow
                EventLoop::run to return
             */
            void exit();

            /** @copydoc Selector::addDevice
            */
             void add( IOResult& result );

            /** @copydoc Selector::removeDevice
            */
            void remove( IOResult& result );

            /** @copydoc Selector::addTimer
            */
            void add( Timer& timer );

            /** @copydoc Selector::removeTimer
            */
            void remove( Timer& timer );

            /** @brief Sets the idle timeout
                It the set idle timeout expires without any acitvity on
                the %EventtLoop, the signal timeout will be send.

                @param msecs The timeout in milliseconds
            */
            void setIdleTimeout(unsigned int msecs);

            /** @brief Returns the idle timeout
                Returns the idle timeout in milliseconds
            */
            unsigned int idleTimeout() const;

            //! @internal
            virtual bool opened(const Connection& c)
            {
                MutexLock lock(_connectionMutex);
                bool accept = Connectable::opened(c);
                return accept;
            }

            //! @internal
            virtual void closed(const Connection& c)
            {
                MutexLock lock(_connectionMutex);
                Connectable::closed(c);
            }

            /** @brief Reports all events
                Clients can connect themselves to this signal to listen for
                any event that is committed to this event loop's event queue.
            */
            Signal<const Pt::Event&> event;

            /** @brief Reports event loop idle timeuts
                Clients connected to this signal will be called if there was
                no activity in an idle time interval.
            */
            Signal<> timeout;

        private:
            bool _exitLoop;
            Condition _cond;
            std::list<Pt::Event*>   _eventQueue;
            Mutex                   _connectionMutex;
            Mutex                   _mutex;
            Selector                _selector;
            unsigned int            _timeout;
    };

} // namespace System

} // namespace Ptv

#endif
