/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
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
#ifndef PT_SYSTEM_APPLICATION_H
#define PT_SYSTEM_APPLICATION_H

#include <Pt/System/Api.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Connectable.h>
#include <Pt/Event.h>
#include <Pt/Signal.h>

namespace Pt {

namespace System {

    /**
     * \brief The Application class provides an event loop for console applications
     * without a GUI.
     *
     * This class is used by non-GUI applications to provide the applications's event
     * loop. There should be only exactly one instance of Application (or one of its
     * subclasses) per application. This is not ensured, though.
     *
     * Application contains the main event loop, where event sources can be registered
     * and events from those sources are dispatched to listeners, that were registered
     * to the event loop. Events may for example be operating system events (timer, file
     * system changes).
     *
     * The application and therefore the event loop is started with a call to run() and
     * can be exited with a call to exit(). After calling exit() the application should
     * terminate.
     *
     * The event loop can be access by calling eventLoop(). Events can be committed by
     * calling EventLoop::commitEvent(). Long running operations can call
     * EventLoop::processEvents() to keep the application responsive.
     *
     * There are convenience methods available for easier access to functionality of
     * the underlying event loop. commitEvent() delegates to EventLoop::commitEvent(),
     * queueEvent() delegates to EventLoop::delegateEvent() and processEvents() delegates
     * to EventLoop::processEvents() without making it necessary to first obtain the
     * event loop manually.
     */
    class PT_SYSTEM_API Application : public Pt::Connectable
    {
        public:
            Application();

            //! Empty Destructor.
            virtual ~Application()
            {}

            /**
             * \brief Starts this application.
             *
             * This method only starts this application's EventLoop. You can access the
             * EventLoop directly by using getEventLoop(). There are some delegate methods
             * for convenient access to certain functionality of the event loop:
             * commitEvent(Event), queueEvent(Event) and processEvents().
             */
            virtual int run();

            /**
             * \brief Stops the execution of this application.
             *
             * This method only stops this application's EventLoop by calling its
             * exit-method.
             */
            virtual void exit();

            /**
             * \brief Commits the given event to this application's EventLoop and wakes
             * the EventLoop so it delivers events.
             *
             * This method delegates directly to EventLoop::commitEvent(const Event&).
             * @param event The event object that will be added to the event queue.
             */
            virtual void commitEvent(const Pt::Event& event);

            /**
             * \brief Queues the given event to this application's EventLoop without
             * waking up the events so they may not be delivered immediately.
             *
             * This method delegates directly to EventLoop::queueEvent(const Event&).
             * @param event The event object that will be added to the event queue.
             */
            virtual void queueEvent(const Pt::Event& event);

            /**
             * \brief Delivers all events of this application's EventLoop which are
             * currently inside the event queue to the registered listeners.
             *
             * This method delegates directly to EventLoop.processEvent().
             */
            virtual void processEvents();

            /**
             * \brief Returns this application's EventLoop.
             *
             * This class provides some delegate methods so you can access
             * certain functionality of the EventLoop without first having to
             * retrieve it. See commitEvent(), queueEvent(), processEvents().
             */
            EventLoop& eventLoop();

            /**
             * \brief The signal to which slots can register themselves to listen for
             * any event that is committed to this application's event loop.
             */
            Signal<const Pt::Event&> event;

        private:
            //! The EventLoop of this application.
            EventLoop _loop;
    };

} // namespace system

} // namespace ptv

#endif
