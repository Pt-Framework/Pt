/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_APPLICATION_H
#define PT_APPLICATION_H

#include <Pt/Api.h>
#include <Pt/Connectable.h>
#include <Pt/Event.h>
#include <Pt/Signal.h>

namespace Pt {

    /**
     * \brief An interface that can be implemented by application classes that want to 
     * provide an event loop for applications with or without a GUI.
     * 
     * This interface provides methods for running and stopping the application, for
     * adding and processing of events and a signal (event) to which slots can be connected to
     * listen for events that are sent to the event queue.
     * 
     * A class that implements this interface may contain a main event loop, where event
     * sources can be registered and events from those sources are dispatched to listeners,
     * that were registered to the event loop. Events may for example be operating system
     * events (timer, file system changes) or gui-specific events (like repaint, mouse events).
     *
     * The application and therefore the event loop is started with a call to run() and
     * can be exited with a call to exit(). After calling exit() the application should
     * terminate.
     *
     * Events can be committed by calling commitEvent(). Long running operations
     * can call processEvents() to keep the application responsive.
     */
    class PT_EXPORT Application : public Connectable {
        public:
            Application()
            { }

            virtual ~Application()
            { }

            /**
             * \brief Starts this application and the underlying event loop.
             */
            virtual int run() = 0;

            /**
             * \brief Stops the execution of this application.
             */
            virtual void exit() = 0;

            /**
             * \brief Commits the given event to this application's EventLoop and wakes
             * the EventLoop so it delivers events.
             *
             * @param event The event object that will be added to the event queue.
             */
            virtual void commitEvent(const Event& event) = 0;

            /**
             * \brief Queues the given event to this application's EventLoop without
             * waking up the events so they may not be delivered immediately.
             *
             * @param event The event object that will be added to the event queue.
             */
            virtual void queueEvent(const Event& event) = 0;

            /**
             * \brief Delivers all events of this application's EventLoop which are
             * currently inside the event queue to the registered listeners.
             */
            virtual void processEvents() = 0;

            /**
             * \brief The signal to which slots can register themselves to listen for
             * any event that is committed to this application's event loop.
             */
            Signal<const Event&> event;
    };

} // namespace Pt

#endif
