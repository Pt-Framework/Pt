/*
   Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301 USA
*/

#ifndef Pt_Forms_MouseEvent_h
#define Pt_Forms_MouseEvent_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Event.h>

namespace Pt {

namespace Forms {

/** @brief Mouse button identity.

    @ingroup Pt-Forms-Input
*/
class MouseButton
{
    public:
        /** @brief Defines a mouse button.
        */
        enum Type
        {
            Left = 0,
            Right = 1,
            Middle = 2,
        };

        /** @brief Creates a mouse button with @a type.
        */
        MouseButton(Type type = Left)
        : _type(type)
        { }

        /** @brief Returns the numeric button value.
        */
        operator Pt::uint32_t() const
        {
            return _type;
        }

    private:
        Pt::uint32_t _type;
};


/** @brief Pressed or released state of the mouse buttons.

    @ingroup Pt-Forms-Input
*/
class MouseState
{
    public:
        /** @brief Creates a state in which no button is pressed.
        */
        MouseState()
        : _buttonState(0)
        { }

        /** @brief Returns true if the button is in pressed state.
        */
        bool isPressed(MouseButton button) const
        {
             Pt::uint32_t mask = 0x1 << button;
             return (_buttonState & mask) == mask;
        }

        /** @brief Records @a button as pressed.
        */
        void setPressed(MouseButton button)
        {
            Pt::uint32_t mask = 0x1 << button;
            _buttonState |= mask;
        }

        /** @brief Returns true if the button is in released state.
        */
        bool isReleased(MouseButton button) const
        {
             Pt::uint32_t mask = 0x1 << button;
             return (_buttonState & mask) != mask;
        }

        /** @brief Records @a button as released.
        */
        void setReleased(MouseButton button)
        {
            Pt::uint32_t mask = 0x1 << button;
            _buttonState &= (~mask);
        }
    private:
        Pt::uint32_t _buttonState;
};


/** @brief Pointer movement or button change.

    The event carries a local position, the action, the button, and the
    button state. The widget pointer and widget ID identify the target.

    @ingroup Pt-Forms-Input
*/
class MouseEvent : public Pt::BasicEvent<MouseEvent>
{
    public:
        /** @brief Defines the mouse action.
        */
        enum Action
        {
            Move = 0,
            Press = 1,
            Release = 2
        };

        /** @brief Defines a mouse button.
        */
        enum Button
        {
            Left = 0,
            Right = 1,
            Middle = 2,
        };

        /** @brief Creates an event without a target.
        */
        explicit MouseEvent()
        : _widgetId_(0)
        , _widget(0)
        , _pos(0, 0)
        , _action(Move)
        , _buttonState(0)
        , _button(0)
        { }

        /** @brief Creates an event targeted at @a widget.
        */
        explicit MouseEvent(Widget& widget)
        : _widgetId_( widget.id() )
        , _widget(&widget)
        , _pos(0, 0)
        , _action(Move)
        , _buttonState(0)
        , _button(0)
        { }

        /** @brief Returns the target widget ID, or 0 when no target is set.
        */
        Pt::uint64_t widgetId() const
        {
            return _widgetId_;
        }

        /** @brief Returns the target widget, or 0 when no target is set.
        */
        Widget* widget() const
        {
            return _widget;
        }

        /** @brief Sets the target widget and its ID.
        */
        void setWidget(Widget* widget)
        {
            _widget = widget;
            _widgetId_ = widget ? widget->id() : 0;
        }

        /** @brief Returns the pointer position in local coordinates.
        */
        const Gfx::PointF& position() const
        {
            return _pos;
        }

        /** @brief Sets the pointer position to @a pos.
        */
        void setPosition(const Gfx::PointF& pos)
        {
            _pos = pos;
        }

        /** @brief Returns the local horizontal pointer position.
        */
        double x() const
        {
            return _pos.x();
        }

        /** @brief Sets the local horizontal pointer position to @a x.
        */
        void setX(double x)
        {
            _pos.setX(x);
        }

        /** @brief Returns the local vertical pointer position.
        */
        double y() const
        {
            return _pos.y();
        }

        /** @brief Sets the local vertical pointer position to @a y.
        */
        void setY(double y)
        {
            _pos.setY(y);
        }

        /** @brief Returns true when the event reports pointer movement.
        */
        bool isMove() const
        {
            return _action == Move;
        }

        /** @brief Changes the event to pointer movement.
        */
        void setMove()
        {
            _action = Move;
            _button = 0;
        }

        /** @brief Returns true if the button is in pressed state.
        */
        bool isPressed(Pt::uint32_t button = Left) const
        {
             Pt::uint32_t mask = 0x1 << button;
             return (_buttonState & mask) == mask;
        }

        /** @brief Returns true if the button was just pressed.
        */
        bool isPress(Pt::uint32_t button = Left) const
        {
             Pt::uint32_t mask = 0x1 << button;
             return (_button & mask) == mask && _action == Press;
        }

        /** @brief Changes the event to a press of @a button.
        */
        void setPress(Pt::uint32_t button = Left)
        {
            Pt::uint32_t mask = 0x1 << button;

            _action = Press;
            _button = mask;
            _buttonState |= mask;
        }

        /** @brief Returns true if the button is in released state.
        */
        bool isReleased(Pt::uint32_t button = Left) const
        {
             Pt::uint32_t mask = 0x1 << button;
             return (_buttonState & mask) != mask;
        }

        /** @brief Returns true if the button was just released.
        */
        bool isRelease(Pt::uint32_t button = Left) const
        {
             Pt::uint32_t mask = 0x1 << button;
             return (_button & mask) == mask && _action == Release;
        }

        /** @brief Changes the event to a release of @a button.
        */
        void setRelease(Pt::uint32_t button = Left)
        {
            Pt::uint32_t mask = 0x1 << button;

            _action = Release;
            _button = mask;
            _buttonState &= (~mask);
        }

    private:
        Pt::uint64_t  _widgetId_;
        Widget*       _widget;
        Gfx::PointF   _pos;
        Action        _action;
        Pt::uint32_t  _buttonState;
        Pt::uint32_t  _button;
};

} // namespace

} // namespace

#endif
