/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_SYSTEM_EVENT_H
#define PT_SYSTEM_EVENT_H

#include <Pt/System/Api.h>
#include <Pt/Event.h>

namespace Pt {

namespace System {

	/**
	 * @see Pt::Event
	 */
	class PT_SYSTEM_API Event : public Pt::Event {
		public:
			/**
			 * @see Pt::Event::Event()
			 */
			Event()
			{}

			/**
			 * @see Pt::Event::~Event()
			 */
			virtual ~Event()
			{}

			//! Clone method.
			virtual Event* clone() const = 0;
	};

} // namespace System

} // namespace Pt

#endif
