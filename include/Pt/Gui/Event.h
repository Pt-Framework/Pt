/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
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

#ifndef Pt_Gui_Event_h
#define Pt_Gui_Event_h

#include <Pt/Api.h>
#include <Pt/System/Event.h>
#include <Pt/Gui/Widget.h>

namespace Pt {

namespace Gui {

	/**
	 * @brief The root event class for all GUI events.
	 *
	 * This class basically stores the widget, for which the specific event
	 * occured. The widget can be accessed using the method widget().
	 *
	 * Specific GUI events must sub-class this class.
	 *
	 * This class is a sub-class of system::Event, which is the root class of
	 * all (non-GUI) events.
	 */
	class PT_API Event : public System::Event
	{
		public:
			/**
			 * @brief Constructs a new Event object and stores the given widget.
			 *
			 * @param widget The widget this event was created for.
			 */
			Event(Widget& widget);

			//! @brief Empty desctructor.
			virtual ~Event();

			/**
			 * @brief The widget on which this event originally occured.
			 *
			 * @param widget The widget on which this event originally occured.
			 */
			Widget& widget() const
			{ return _widget; }

			/**
			 * @brief Returns the type information object (type_info) of this event class.
			 * 
			 * This pure virtual method has to be implemented by any sub-class of Gui::Event
			 * in a way that it returns the event's type info. Caching of this information
			 * is recommended.
			 *
			 * @return The type information object of this event.
			 */
			virtual const std::type_info& typeInfo() const = 0;

		public:
			//! @brief The type information object (type_info) of this event class.
			static const std::type_info& TYPE_INFO;

		private:
			//! @brief The widget on which this event originally occured.
			Widget& _widget;
	};

} // namespace Gui

} // namespace Pt

#endif
