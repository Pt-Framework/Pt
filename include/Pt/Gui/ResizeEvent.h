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

#ifndef Pt_ResizeEvent_h
#define Pt_ResizeEvent_h

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>

#include <cstddef>


namespace Pt {

namespace Gui {

    class Widget;

    /**
     * @brief An event that indicates that a widget was resized.
     *
     * A widget can be resized by the user -- for example by dragging the window around
     * using the mouse -- or programatically using one of the widget's method, for example
     * Widget::resize(). Minimizing, maximizing and restoring the window are special resize
     * types which are only valid for top-level widgets like windows:
     * - Resize - The widget or window was resized "normally", but not minimized, maximized or restored
     * - Minimized - The window was minimized. The new width and height are 0.
     * - Maximized - The window was maximized so it fills the screen completely.
     * - Restored - The window was restored from minimized state to its previous state. The
     * width and height are the same as the last time the window was maxmimized or "normal".
     *
     * Use the method resizeType() to access this information.
     *
     * The ResizeEvent stores the new width and height of the widget.
     */
    class PT_GUI_API ResizeEvent : public Event
    {
        public:
            //! The resize types of this Resize Event, either Reizse, Minimized, Maximized or Restored.
            enum Type {
                Resize,    //! The widget or window was resized normally (not minimized, maximized or restored).
                Minimized, //! The widget was minimized. The new width and height are 0.
                Maximized, //! The widget was maximized so it fills the screen completely.
                Restored   //! The widget was restored, either from minimized or maximized state.
            };

            //! @brief The type information object (type_info) of this event class.
            static const std::type_info& TYPE_INFO;

        public:
            /**
             * @brief Constructs a new ResizeEvent using the new width and height of the widget.
             *
             * @param widget The widget for which this resize event was created.
             * @param width The new width of the widget after the resize.
             * @param height The new height of the widget after the resize.
             * @param resizeType The type of resize which happened. Either resize (for normal
             * resize), maximize, minimize or restored. This parameter is optional and defaults
             * to Resize.
             */
            ResizeEvent(Widget& widget, size_t width, size_t height, Type resizeType = Resize);

            //! @brief Empty destructor.
            virtual ~ResizeEvent();

            /**
             * @brief Returns the new width of the widget for which this event was generated for.
             * @return The new width of the widget.
             */
            size_t width() const;

            /**
             * @brief Returns the new height of the widget for which this event was generated for.
             * @return The new height of the widget.
             */
            size_t height() const;
            
            /**
             * @brief Returns the resize type of this event.
             * @return The resize type of this event.
             */
            Type resizeType() const;

        protected:
            /**
             * @brief Returns the type info for this event.
             * @return The type info for this event.
             */
            virtual const std::type_info& onTypeInfo() const;

            Pt::Event& onClone(Pt::Allocator& allocator) const
            {
                void* pEvent= allocator.allocate(sizeof(ResizeEvent));
                return *(new (pEvent)ResizeEvent(*this));
            }

            void onDestroy(Pt::Allocator& allocator)
            {
                allocator.deallocate(this, sizeof(ResizeEvent));
            }

        private:
            size_t _width;
            size_t _height;
            Type   _resizeType;
    };

} // namespace Gui

} // namespace Pt

#endif
