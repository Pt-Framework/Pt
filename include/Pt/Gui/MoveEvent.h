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

        protected:
            /**
             * @brief Returns the type info for this event.
             *
             * @return The type info for this event.
             */
            virtual const std::type_info& onTypeInfo() const;

            Pt::Event& onClone(Pt::Allocator& allocator) const
            {
                void* pEvent= allocator.allocate(sizeof(MoveEvent));
                return *(new (pEvent)MoveEvent(*this));
            }

            void onDestroy(Pt::Allocator& allocator)
            {
                allocator.deallocate(this, sizeof(MoveEvent));
            }

        private:
            size_t _x;
            size_t _y;
    };

} // namespace Gui

} // namespace Pt

#endif
