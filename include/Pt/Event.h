/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *                                                                         *
 ***************************************************************************/

#ifndef PT_EVENT_H
#define PT_EVENT_H

#include <Pt/Api.h>


namespace Pt {

	/**
	 * \brief The root event class for all events.
	 *
	 * The implemenation of this class is empty. Specific Event objects, like for
	 * example MouseEvents, must subclass from Event and implement all necessary
	 * behaviour.
	 *
	 * Events are used by the EventLoop class and can be added using the method
	 * commitEvent(). These Event object will be delivered to the functions
	 * and methods, that where previously registered to receive events.
	 *
	 * The method clone() is abstract and needs to be defined by subclasses to
	 * allow cloning of the specific Event object.
	 */
	class PT_EXPORT Event {
		public:
			/**
			 * \brief Constructor to initialize the Event class.
			 * For the super-class Event the constructor is empty.
			 */
			Event()
			{}

			/**
			 * \brief Destructor for Event.
			 * For the super-class Event the destructor is empty.
			 */
			virtual ~Event()
			{}

			//! Clone method.
			virtual Event* clone() const = 0;
	};

} // namespace Pt

#endif
