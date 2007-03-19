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
#include <Pt/System/IOMonitor.h>
#include <Pt/Event.h>


#include <list>


namespace Pt {

namespace System {

class IOMonitor;

    /** \brief An event loop which handles events from multiple sources.
     *
     * Events can be added to the internal event queue from multiple sources using
     * the method "commitEvent(...)". Multiple events receivers (methods and
     * functions) can be registered to the Signal "event". Events from these sources
     * are delivered to these receivers in the order the events were added to this
     * event loop.
     *
     * To start the event loop the method "run()" must be executed. It will only
     * return after the event loop was stopped. To stop the loop "exit()" can be
     * called or the EventLoop object must be deleted. The delivery of the events
     * occurs inside the Thread that startet the execution of the event loop by
     * calling "run()".
     *
     * Events can be committed from arbitrary Threads as this class is thread-safe.
     * Therefore a decoupling between multiple event sources and one or more receivers
     * is possible, where all events from multiple sources are processed in one
     * event loop.
     *
     * This class is thread-safe, so any method may be called from any Thread.
     */
    class PT_SYSTEM_API EventLoop : public Connectable, public Runnable {
        public:
            //! Constructs the EventLoop.
            EventLoop();

            /**
             * \brief Destructs the EventLoop.
             *
             * Delivers all outstanding events, which are still inside the event queue
             * to the registered methods and functions.
             */
            ~EventLoop();

            /**
             * \brief Starts the event loop and delivers events during its execution.
             *
             * This method is used to start the event loop. It will only return after
             * "exit()" was called or this object is deleted. During its execution this
             * method delivers the events of the event queue to the registered slots.
             */
            void run();

            /**
             * \brief Adds the given event to the event loop and wakes it up so it
             * will deliver all outstanding events.
             *
             * The event will be delivered as soon as possible. If the event queue is
             * currently empty, the event will be delivered immediately. If there are
             * still events left in the queue, the event will be delivered after all
             * events that are still currently in the event queue have been delivered.
             *
             * As the given Event object is cloned before it is added to the event queue
             * the Event object may be safely deleted or changed after it has been committed.
             *
             * This method is thread-safe, so the caller to this method is allowed to
             * be any Thread.
             *
             * @param event The Event object to be added to the event queue for alter
             * delivery.
             */
            void commitEvent(const Pt::Event& event);

            /**
             * \brief Adds the given event to the event queue without waking up the event
             * loop. Thus the event may be delivered only later.
             *
             * The event is added to the end of the event queue without actually starting
             * the delivery. It will only be delivered after "wake()" was called or another
             * event was committed to the event loop and which triggers the delivery.
             * In case the event loop is currently delivering events the event queued
             * with this method will also be delivered.
             * You may use this method to batch-commit several events be queueing them
             * and finally calling "wake()" only once to start the delivery of those events.
             *
             * As the given Event object is cloned before it is added to the event queue
             * the Event object may be safely deleted or changed after it has been queued.
             *
             * This method is thread-safe, so the caller to this method is allowed to
             * be any Thread.
             *
             * @param event The Event object to be added to the event queue for alter
             * delivery.
             */
            void queueEvent(const Pt::Event& event);

            /**
             * \brief Delivers all events which are currently inside the event queue
             * to the listeners (methods and functions) that are registered to the
             * event-Signal.
             *
             * This method will return without work if there is no event in the event
             * queue.
             * This method is thread-safe, so the caller to this method is allowed to
             * be in any Thread.
             */
            void processEvents();

            /**
             * \brief Sends a signal to wake up the "run" method to make it deliver
             * outstanding events.
             *
             * This will only have an effect if the "run" method is currently waiting
             * for new events. If it's not, the event queue is currently processed anyway.
             * @see run()
             */
            void wake();

            /**
             * \brief Stops the execution of this event loop and makes the run()-method
             * to return.
             *
             * Before the event loop is stopped, all events which are still in the
             * event queue are delivered to the corresponding listeners.
             * @see run()
             */
            void exit();


            /**
             * \brief Add a device to the event loop.
             *
             * The device events are proccesing throw the internal event loop IOMonitor.
             * A slot can be connected to the returned signal. This signal emit the device 
             * specific events.
             *
             * Attention! The device don't use the signal from the event loop to notify! 
             * Only the returned signal from this method is used to notify a device event.
             *
             * @param device The IODevice
             * @return The event notification signal for this device
             */
             Signal<const IOEvent&>& addDevice( IODevice& device );
            
            /**
             * \brief Remove a device from the event loop IOMonitor.
             *
             * @param device The device to remove.
             */
            void removeDevice( IODevice& device );

            virtual bool opened(const Connection& c)
            {
                MutexLock lock(_connectionMutex);
                bool accept = Connectable::opened(c);
                return accept;
            }

            virtual void closed(const Connection& c)
            {
                MutexLock lock(_connectionMutex);
                Connectable::closed(c);
            }

            /**
             * \brief The signal to which slots can register themselves to listen for
             * any event that is committed to this event loop's event queue.
             */
            Signal<const Pt::Event&> event;

        protected:
            //! Empty Copy-Constructor. Copying not allowed.
            EventLoop(const EventLoop& app)
            {}

            //! Empty assignment operator. (Only returns a this-reference.)
            //! Assignment not allowed.
            EventLoop& operator=(const EventLoop& el)
            { return *this; }

        private:

            /**
             * Flag to indicate that the event loop is (not) running or should be halted.
             * As soon as this bool is set to 'true' the "run"-method will return an so the
             * event delivery will stop.
             */
            bool _exitLoop;

            //! Condition to stop the execution of "run()" until a new event was committed.
            Condition _cond;

            /**
             * Contains the Event objects of the event queue that are delivered one
             * by one in method "processEvents()".
             * @see processEvents()
             */
            std::list<Pt::Event*>   _eventQueue;
            Mutex                   _connectionMutex;
            Mutex                   _mutex;
            IOMonitor               _ioMonitor;
    };

} // namespace System

} // namespace Ptv

#endif
