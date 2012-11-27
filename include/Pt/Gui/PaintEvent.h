/*
 * Copyright (C) 2006 Marc Boris Duerner
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

#ifndef Ptv_PaintEvent_h
#define Ptv_PaintEvent_h

#include <Pt/Types.h>
#include <Pt/Gfx/Region.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>

#include <cstddef>


namespace Pt {

namespace Gui {

    class Widget;

    /**
     * @brief An event that indicates that a (re)paint of a certain area should be done.
     *
     * This event is fired whenever all or only a part of a widget should be (re)paint.
     * This may be necessary for example because another window or widget did hide a part
     * of the widget or when the widget has to be updated because it's presentation has
     * to be changed due to changes of its internal state. A resize, position change, the
     * change from hidden to visible or minimizing/maximizing the widget (or its parent)
     * will usually lead to a paint event.
     *
     * If only parts of the widget have to be updated, the rectangular area which can be
     * determined by calling rect() specifies this update area. If all of the widget has
     * to be updated the rectangle does cover all of the widget.
     *
     * Paint events are usually only used internally by widget or one of its sub-classes
     * to do a (re)paint of the widget area.
     */
    class PT_GUI_API PaintEvent : public Event
    {
        public:
            //! @brief The type information object (type_info) of this event class.
            static const std::type_info& TYPE_INFO;

        public:

            /**
             * @brief Construct a new paint event with the given region as dirty area.
             *
             * The given region marks the dirty area of the widget for which
             * this paint event is created. The coordinates are in widget coordinate space
             * and start in the left-top corner of the widget. Usually only the area
             * specified with this region should be repainted to reduce the time that
             * is used for painting.
             *
             * @param widget The widget for which this paint event was created and that
             * should be repainted.
             * @param region The rectangular area of the widget which needs to be painted.
             */
            PaintEvent(Widget& widget, Pt::Gfx::Region region);

            /**
             * @brief Construct a new paint event with the given point and size as dirty area.
             *
             * The point in combination with the size span a rectangle which marks the
             * dirty area of the widget for which this paint event is created. The
             * coordinates are in widget coordinate space and start in the left-top corner
             * of the widget. Usually only the area specified with this region (point +
             * size) should be repainted to reduce the time that is used for painting.
             *
             * @param widget The widget for which this paint event was created and that
             * should be repainted.
             * @param point The top-left corner of the area which is supposed to be repainted.
             * @param size The width and height of the area which is supposed to be repainted.
             */
            PaintEvent(Widget& widget, Pt::Gfx::Point point, Pt::Gfx::Size size);

            //! @brief Empty destructor.
            virtual ~PaintEvent();

            /**
             * @brief Defines the rectangular area which is supposed to be (re)painted.
             *
             * Only this area is dirty and should only be (re)painted to reduce the time
             * that is necessary for painting.
             *
             * @return Returns the dirty area.
             */
            const Pt::Gfx::Region& region() const
            { return _region; }

            /**
             * @brief Returns the top-left corner of the area which is supposed to be (re)painted.
             *
             * Only this area is dirty and should only be (re)painted to reduce the time
             * that is necessary for painting.
             *
             * @return Returns the top-left corner of the dirty area.
             */
            Pt::Gfx::Point origin() const
            {
                return _region.topLeft();
            }

        protected:
            /**
             * @brief Returns the type info for this event.
             *
             * @return The type info for this event.
             */
            virtual const std::type_info& onTypeInfo() const;

            Pt::Event& onClone(Pt::Allocator& allocator) const
            {
                void* pEvent= allocator.allocate(sizeof(PaintEvent));
                return *(new (pEvent)PaintEvent(*this));
            }

            void onDestroy(Pt::Allocator& allocator)
            {
                allocator.deallocate(this, sizeof(PaintEvent));
            }

        private:
             Pt::Gfx::Region _region;
    };

} // namespace Gui

} // namespace Pt

#endif
