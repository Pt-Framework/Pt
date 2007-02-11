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

#ifndef Pt_MoveEvent_h
#define Pt_MoveEvent_h

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>
#include <cstddef>


namespace Pt {

namespace Gui {
    class Widget;

    /**
     * @brief An event that indicates that a widget was moved.
     *
     * A widget can be moved by the user -- for example by dragging the window around
     * using the mouse -- or programatically using one of the widget's method, for example
     * Widget::move().
     *
     * The MoveEvent basically stores the new x- and y-position of the widget relative to
     * its parent. For a top-level-windows this parent is the desktop. For child widgets
     * this parent is the direct ancestor of the widget.
     */
    class PT_GUI_API MoveEvent : public Event
    {
        public:
            //! @brief The type information object (type_info) of this event class.
            static const std::type_info& TYPE_INFO;

        public:
            /**
             * @brief Constructs a new MoveEvent using the new x- and y-position of the widget.
             *
             * @param widget The widget for which this move event was created.
             * @param x The new x-position to which the widget was moved, relative to its parent top-left corner.
             * @param y The new y-position to which the widget was moved, relative to its parent top-left corner.
             */
            MoveEvent(Widget& widget, size_t x, size_t y);

            //! @brief Empty desctructor.
            virtual ~MoveEvent();

            // inherit doc
            virtual Event* clone() const
            { return new MoveEvent(*this); }

            /**
             * @brief The new x-position of the widget for which this move event was created for.
             *
             * The position is relative to the top-left corner of its parent widget.
             *
             * @return The x-position of the widget which moved.
             */
            size_t x() const;

            /**
             * @brief The new y-position of the widget for which this move event was created for.
             *
             * The position is relative to the top-left corner of its parent widget.
             *
             * @return The y-position of the widget which moved.
             */
            size_t y() const;

            /**
             * @brief Returns the type info for this event.
             *
             * @return The type info for this event.
             */
            virtual const std::type_info& typeInfo() const;

        private:
            size_t _x;
            size_t _y;
    };

} // namespace Gui

} // namespace Pt

#endif
