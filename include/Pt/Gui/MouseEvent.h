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

#ifndef Pt_Gui_MouseEvent_h
#define Pt_Gui_MouseEvent_h

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>

#include <cstddef>


namespace Pt {

namespace Gui {

    class Widget;

    /**
     * @brief An event that indicates that a mouse action occured in a widget.
     *
     * These actions concern the left, right or middle mouse button or the mouse
     * wheel. While the left, right and middle mouse button can be pressed, released
     * and double-clicked, the mouse wheel only produces press events.
     *
     * Every MouseEvent stores the following information:
     * - Button: The mouse button (left, right, middle) or wheel (up, down) for which
     * this MouseEvent occured. Use button() to access this information.
     * - Action: The action which occured. Available actions are Press, Release and
     * DoubleClick for a press, release or double-click event. Mouse-Wheel events always
     * have the Press action. Use action() to access this information.
     * - Modifier: The modifiers which where active while the MouseEvent was generated.
     * Possible Modifiers are pressed Shift, Alt and Control key and also a currently
     * pressed left, middle or right mouse button. Use modifiers() to access this information.
     *
     * Furthermore the x- and y-position of the mouse cursor during the generation of
     * this event can be determined using the methods x() and y(). This position is relative
     * to the top-left edge of the widget for which this mouse event was created.
     */
    class PT_GUI_API MouseEvent : public Event {
        public:
            //! @brief The button or mouse wheel this event was created for.
            enum Button {
                LeftButton,   //! The left mouse button was pressed, released or double-clicked.
                MiddleButton, //! The middle mouse button was pressed, released or double-clicked.
                RightButton,  //! The right mouse button was pressed, released or double-clicked.
                WheelUp,      //! The mouse-wheel was moved up.
                WheelDown     //! The mouse-wheel was moved down.
            };

            //! @brief The action which occured for the button or mouse wheel.
            enum Action {
                Press,        //! A mouse button was pressed or a mouse-wheel moved.
                Release,      //! A mouse button was released.
                DoubleClick   //! A mouse button was double-clicked.
            };

            //! @brief The modifier which were active while the MouseEvent occured.
            enum Modifier {
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
             * @brief Constructs a new mouse event.
             *
             * Constructs a new mouse event with information about the button, action and modifiers
             * and position of the mouse cursor.
             *
             * @param widget The widget for which this mouse event was generated.
             * @param x The x-position of the mouse cursor at the time this event was created.
             * @param y The y-position of the mouse cursor at the time this event was created.
             * @param button The button (or mouse-wheel) which was pressed, released or double-click.
             * @param action The action of the button or mouse-wheel, either pressed, released or double-clicked.
             * @param modifiers The modifiers that where active at the time this event was created.
             */
            MouseEvent(Widget& widget, size_t x, size_t y, const Button& button, const Action& action, unsigned int modifiers);

            //! @brief Empty desctructor.
            virtual ~MouseEvent();

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
             * @brief Returns the mouse button or whell for which the MouseEvent occured.
             *
             * The mouse button (left, right, middle) where either pressed, released or double-clicked.
             * The mouse-wheel was moved up or down. (Mouse-wheels only have the action 'pressed'.)
             *
             * @return The mouse button or mouse-wheel of this MouseEvent.
             */
            Button button() const;

            /**
             * @brief Returns the action which occured for the button or wheel.
             *
             * Possible actions are Press, Release and DoubleClick for a press, release or 
             * ouble-click event. Mouse-Wheel events always have the Press action.
             *
             * @return The action which occured.
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

            /**
             * @brief Returns the type info for this event.
             *
             * @return The type info for this event.
             */
            virtual const std::type_info& onTypeInfo() const;

            Pt::Event& onClone(Pt::Allocator& allocator) const
            {
                void* pEvent= allocator.allocate(sizeof(MouseEvent));
                return *(new (pEvent)MouseEvent(*this));
            }

            void onDestroy(Pt::Allocator& allocator)
            {
                allocator.deallocate(this, sizeof(MouseEvent));
            }

        private:
            size_t _x;
            size_t _y;
            Button _button;
            Action _action;
            int    _modifiers;
    };

} // namespace Gui

} // namespace Pt

#endif
