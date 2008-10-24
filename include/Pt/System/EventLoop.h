/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
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

#include <Pt/Event.h>
#include <Pt/Signal.h>
#include <Pt/Allocator.h>
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Runnable.h>
#include <Pt/System/Selector.h>
#include <Pt/System/EventSink.h>
#include <map>
#include <deque>
#include <typeinfo>

namespace Pt {

namespace System {

    class Timer;
    class Application;
    class Selectable;

    /** @brief Thread-safe event loop supporting I/O multiplexing and Timers.
     */
    class PT_SYSTEM_API EventLoopBase : public SelectorBase
                                      , public EventSink
                                      , public Connectable
                                      , public Runnable
    {
        public:
            static const unsigned int WaitInfinite = static_cast<const unsigned int>(-1);

            /** @brief Destructs the EventLoop
             */
            virtual ~EventLoopBase();

            /** @brief Starts the event loop
             */
            void run();

            /** @brief Processes all events which are currently in the event queue
             */
            void processEvents();

            /** @brief Stops the %EventLoop.
             */
            void exit();

            /** @brief Sets the idle timeout
            */
            void setIdleTimeout(unsigned int msecs);

            /** @brief Returns the idle timeout
            */
            unsigned int idleTimeout() const;

            /** @brief Notifies about wait timeouts
                This signal is send when the timeout given to a wait
                call of the selector expires and no activity occured.
            */
            Signal<> timeout;

            /** @brief Reports all events
                TODO: rename to eventReady
            */
            Signal<const Event&> event;

        protected:
            /** @brief Constructs the EventLoop
            */
            EventLoopBase();

            virtual void onRun() = 0;

            virtual void onExit() = 0;

            virtual void onProcessEvents() = 0;

        private:
            unsigned int _timeout;
            void* _reserved;
    };

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
    class PT_SYSTEM_API EventLoop : public EventLoopBase
    {
        public:
            /** @brief Constructs the EventLoop
            */
            EventLoop();

            /** @brief Destructs the EventLoop
             */
            virtual ~EventLoop();

        protected:
            virtual void onAdd( Selectable& s );

            virtual void onRemove( Selectable& s );

            virtual void onChanged(Selectable& s);

            virtual void onRun();

            virtual bool onWait(unsigned int msecs);

            virtual void onWake();

            virtual void onExit();

            virtual void onCommitEvent(const Event& event);

            virtual void onProcessEvents();

        private:
            bool _exitLoop;
            SelectorImpl* _selector;
            Allocator _allocator;
            std::deque<Event* > _eventQueue;
            Mutex _queueMutex;
            void* _reserved;
    };

} // namespace System

} // namespace Ptv

#endif
