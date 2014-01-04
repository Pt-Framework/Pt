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

#ifndef Pt_CloseEvent_h
#define Pt_CloseEvent_h

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>


namespace Pt {

namespace Gui {

    class Widget;

    /**
     * @brief An event that indicates that a widget was closed.
     *
     * This event is used to indicate that a widget was closed. This usually means that
     * the Widget object was deleted. The widget, which was closed, can be accessed using
     * the method widget().
     */
    class PT_GUI_API CloseEvent : public Event
    {
        public:
            //! @brief The type information object (type_info) of this event class.
            static const std::type_info& TYPE_INFO;

            /**
             * @brief Constructs a CloseEvent for the given widget.
             *
             * @param widget The widget which was closed.
             */
            CloseEvent(Widget& widget);

            //! Empty destructor
            virtual ~CloseEvent();

        protected:
            /**
             * @brief Returns the type info for this event.
             *
             * @return The type info for this event.
             */
            virtual const std::type_info& onTypeInfo() const;

            Pt::Event& onClone(Pt::Allocator& allocator) const
            {
                void* pEvent= allocator.allocate(sizeof(CloseEvent));
                return *(new (pEvent)CloseEvent(*this));
            }

            void onDestroy(Pt::Allocator& allocator)
            {
                allocator.deallocate(this, sizeof(CloseEvent));
            }
    };

} // namespace Gui

} // namespace Pt

#endif
