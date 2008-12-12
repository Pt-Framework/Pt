/*
 * Copyright (C) 2006 Marc Boris Dürner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Pt_Gui_Event_h
#define Pt_Gui_Event_h

#include <Pt/Event.h>
#include "Pt/Allocator.h"
#include <Pt/Gui/Api.h>
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
    class PT_GUI_API Event : public Pt::Event
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
