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

#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Event.h>
#include <Pt/Allocator.h>
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Runnable.h>
#include <Pt/System/Selector.h>
#include <map>
#include <deque>
#include <typeinfo>

namespace Pt {

namespace System {

    class Timer;
    class Application;
    class Selectable;

    struct PT_SYSTEM_API CompareTypeInfo
    {
        bool operator()(const std::type_info* t1, 
                        const std::type_info* t2) const;
    };

    /** @brief Thread-safe event loop supporting I/O multiplexing and Timers.
     */
    class PT_SYSTEM_API EventLoopBase : public SelectorBase
                                      , public Connectable
                                      , public Runnable
    {
        public:
            static const unsigned int WaitInfinite = static_cast<const unsigned int>(-1);

            //! @internal
            struct IDispatcher
            {
                virtual ~IDispatcher() {}
                virtual void send(const Event&) = 0;
            };

            //! @internal
            template <typename EventT>
            struct Dispatcher: public IDispatcher
            {
                virtual void send(const Event& e)
                {
                    const EventT& event = static_cast<const EventT&>(e);
                    signal.send(event);
                }

                Signal<const EventT&> signal;
            };

            /** @brief Destructs the EventLoop
             */
            virtual ~EventLoopBase();

            /** @brief Starts the event loop
             */
            void run();

            /** @brief Adds an event and wakes up the loop.
             */
            void commitEvent(const Event& event);

            /** @brief Adds an event without waking the event loop
             */
            void queueEvent(const Event& event);

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
            */
            Signal<const Event&> event;

            template <typename EventT>
            void addEventHandler( const BasicSlot<void, const EventT&>& slot )
            {
                const std::type_info& ti = typeid(EventT);
                DispatchTable::iterator it = _dispatchTable.lower_bound( &ti );

                Dispatcher<EventT>* disp = 0;
                if(it != _dispatchTable.end() && !(_dispatchTable.key_comp()(&ti, it->first)))
                {
                    IDispatcher* d = it->second;
                    disp = static_cast<Dispatcher<EventT>*>(d);
                }
                else
                {
                    disp = new Dispatcher<EventT>;
                    std::pair<const std::type_info*const, EventLoopBase::IDispatcher*> p( &ti, disp);
                    _dispatchTable.insert( it, p );
                }

                disp->signal.connect(slot);
            }

        protected:
            /** @brief Constructs the EventLoop
            */
            EventLoopBase();

            virtual void onRun() = 0;

            virtual void onExit() = 0;

            virtual void onCommitEvent(const Event& event) = 0;

            virtual void onQueueEvent(const Event& event) = 0;

            virtual void onProcessEvents() = 0;

            void dispatchEvent(const Event& ev);

        private:
            typedef std::map< const std::type_info*, 
                              IDispatcher*, 
                              CompareTypeInfo > DispatchTable;

            DispatchTable _dispatchTable;
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

            //! @internal
            virtual bool opened(const Connection& c);

            //! @internal
            virtual void closed(const Connection& c);

        protected:
            virtual void onSetParent(Application* app);
            
            virtual void onAdd( Selectable& s );

            virtual void onRemove( Selectable& s );

            virtual void onChanged(Selectable& s);

            virtual void onRun();

            virtual bool onWait(unsigned int msecs);

            virtual void onWake();

            virtual void onExit();

            virtual void onCommitEvent(const Event& event);

            virtual void onQueueEvent(const Event& event);

            virtual void onProcessEvents();

        private:
            bool _exitLoop;
            Mutex _connectionMutex;
            SelectorImpl* _selector;
            Allocator _allocator;
            std::deque<Event* > _eventQueue;
            Mutex _queueMutex;
            void* _reserved;
    };

} // namespace System

} // namespace Ptv

#endif
