/*
 * Copyright (C) 2006 PTV AG
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

#ifndef Pt_Gui_MouseMoveEvent_h
#define Pt_Gui_MouseMoveEvent_h

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>

#include <cstddef>


namespace Pt {

namespace Gui {

    class Widget;

    /**
     * @brief An event that indicates that the mouse was moved.
     *
     * The event is generated whenever the mouse is moved over a widget or when the
     * mouse enters or leaves a widget. Moved events are generated when the mouse is
     * moved "inside" a widget. Entered and Exited event are generated when the mouse
     * pointer moves from a widget into another widget. Use action()
     *
     * - When the mouse moves from outside any widget into a widget, one single Entered
     * event is generated for the target widget.
     * - When the mouse moves from inside a widget to another area that is not a widget,
     * one single Exited event is generated for the source widget.
     * - When the mouse moves from inside a widget into another widget two events are
     * generated: One Exited event for the source widget and one Entered event for the
     * target widget.
     *
     * Every MouseMoveEvent stores the following information:
     * - Action: The move action, either Moved, Entered or Released as explained above.
     * Use action() to access this information.
     * - Modifier: The modifiers which where active while the MouseEvent was generated.
     * Possible Modifiers are pressed Shift, Alt and Control key and also a currently
     * pressed left, middle or right mouse button. Use modifiers() to access this information.
     *
     * Furthermore the x- and y-position of the mouse cursor during the generation of
     * this event can be determined using the methods x() and y(). This position is relative
     * to the top-left edge of the widget for which this mouse event was created.
     */
    class PT_GUI_API MouseMoveEvent : public Event
    {
        public:
            //! @brief The action which occured for the button or mouse wheel.
            enum Action {
                Moved,    //! The mouse was moved inside of a widget
                Entered,  //! The mouse has entered a widget
                Exited    //! The mouse has left a widget
            };

            //! @brief The modifier which were active while the MouseEvent occured.
            enum Modifier {
                NoButton         = 0,
                ShiftDown        = 1 << 0,  //! Shift was pressed
                AltDown          = 1 << 1,  //! Alt was pressed
                CtrlDown         = 1 << 2,  //! Control was pressed
                LeftButtonDown   = 1 << 3,  //! The left mouse button was down
                RightButtonDown  = 1 << 4,  //! The right mouse button was down
                MiddleButtonDown = 1 << 5   //! The middle mouse button was down
            };
            
            //! @brief The type information object (type_info) of this event class.
            static const std::type_info& TYPE_INFO;

        public:
            /**
             * @brief Constructs a new mouse move event
             * 
             * Constructs a new mouse move event with information about position of the mouse relative
             * to the top-left edge of the widget the mouse was in, the action and the modifiers.
             *
             * @param widget The widget for which this mouse move event was generated.
             * @param x The x-position of the mouse cursor at the time this event was created.
             * @param y The y-position of the mouse cursor at the time this event was created.
             * @param action The move action, either Moved, Entered or Exited.
             * @param modifiers The modifiers that where active at the time this event was created.
             */
            MouseMoveEvent(Widget& widget, size_t x, size_t y, Action action, unsigned int modifiers);

            //! @brief Empty desctructor.
            virtual ~MouseMoveEvent();

            /**
             * @brief The x-position of the mouse cursor at the time this event was created.
             *
             * The position is relative to the top-left edge of the widget this mouse event
             * was created for.
             *
             * @return The x-position of the mouse.
             */
            size_t x() const;

            /**
             * @brief The y-position of the mouse cursor at the time this event was created.
             *
             * The position is relative to the top-left edge of the widget this mouse event
             * was created for.
             *
             * @return The y-position of the mouse.
             */
            size_t y() const;
            
            /**
             * @brief Returns the move action which occured.
             *
             * Possible actions are Moved, Entered and Exited for moving inside a widget,
             * entering a widget and exiting a widget.
             *
             * @return The action of this mouse move event.
             */
            Action action() const;

            /**
             * @brief Returns the modifiers which where active while this MouseEvent occured.
             *
             * Possible Modifiers are pressed Shift, Alt and Control key and also a currently
             * pressed left, middle or right mouse button.
             *
             * @return The modifiers which where active.
             */
            unsigned int modifiers() const;

        protected:
            /**
             * @brief Returns the type info for this event.
             *
             * @return The type info for this event.
             */
            virtual const std::type_info& onTypeInfo() const;

            Pt::Event& onClone(Pt::Allocator& allocator) const
            {
                void* pEvent= allocator.allocate(sizeof(MouseMoveEvent));
                return *(new (pEvent)MouseMoveEvent(*this));
            }

            void onDestroy(Pt::Allocator& allocator)
            {
                allocator.deallocate(this, sizeof(MouseMoveEvent));
            }
        
        private:
            size_t _x;
            size_t _y;
            Action _action;
            int    _modifiers;
    };

} // namespace Gui

} // namespace Pt

#endif
