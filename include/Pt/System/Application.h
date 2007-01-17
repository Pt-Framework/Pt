/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PTV_SYSTEM_APPLICATION_H
#define PTV_SYSTEM_APPLICATION_H

#include <Pt/Connectable.h>
#include <Pt/Application.h>
#include <Pt/System/Api.h>
#include <Pt/System/Event.h>
#include <Pt/System/EventLoop.h>

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
	class PT_SYSTEM_API Application : public Pt::Application {
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

		private:
			//! The EventLoop of this application.
			EventLoop _loop;
	};

} // namespace system

} // namespace ptv

#endif
