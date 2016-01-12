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

#ifndef Pt_Hmi_PointerEvent_h
#define Pt_Hmi_PointerEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>

namespace Pt {

namespace Hmi {

class PointerEvent : public Pt::BasicEvent<PointerEvent>
{
    public:    
        enum Action
        {
            Move = 0,
            Press = 1,
            Release = 2,
            Scroll = 3
        };

        enum Button
        {
            Left = 0,
            Right = 1,
            Middle = 2
        };

        enum Wheel
        {
            Vertical = 0,
            Horizontal = 1,
            Depth = 2
        };

        PointerEvent()
        : _x(0)
        , _y(0)
        , _action(Move)
        , _buttonState(0)
        , _button(0)
        , _wheel(Vertical)
        , _delta(0)
        { }

        void clear()
        {
            _x = 0;
            _y = 0;
            _action = Move;
            _buttonState = 0;
            _button = 0;
            _wheel = Vertical;
            _delta = 0;
        }

        double x() const
        {
            return _x;
        }

        double y() const
        {
            return _y;
        }
    
        void setX(double x)
        {
            _x = x;
        }
    
        void setY(double y)
        {
            _y = y;
        }

        bool isMove() const
        {
            return _action == Move;
        }

        void setMove()
        {
            _action = Move;
            _button = 0;
            _wheel = Vertical;
            _delta = 0;
        }

        bool isScroll(Pt::uint32_t wheel) const
        {
             Pt::uint32_t mask = 0x1 << wheel;
             return (_wheel & mask) == mask && _action == Scroll;
        }

        void setScroll(Pt::uint32_t wheel, double d)
        {
            _action = Scroll;
            _button = 0;
            _wheel = wheel;
            _delta = d;
        }

        double delta() const
        {
            return _delta;
        }

        bool isPressed(Pt::uint32_t button) const
        {
             Pt::uint32_t mask = 0x1 << button;
             return (_buttonState & mask) == mask;
        }

        bool isPress(Pt::uint32_t button) const
        {
             Pt::uint32_t mask = 0x1 << button;
             return (_button & mask) == mask && _action == Press;
        }

        bool isPress() const
        {
             return _action == Press;
        }

        void setPress(Pt::uint32_t button)
        {
            Pt::uint32_t mask = 0x1 << button;

            _action = Press;
            _button = mask;
            _buttonState |= mask;

            _wheel = Vertical;
            _delta = 0;
        }

        bool isRelease(Pt::uint32_t button) const
        {
             Pt::uint32_t mask = 0x1 << button;
             return (_button & mask) == mask && _action == Release;
        }

        bool isRelease() const
        {
             return _action == Release;
        }

        void setRelease(Pt::uint32_t button)
        {
            Pt::uint32_t mask = 0x1 << button;

            _action = Release;
            _button = mask;
            _buttonState &= (~mask);

            _wheel = Vertical;
            _delta = 0;
        }

    private:
        double       _x;
        double       _y;
        Action       _action;
        Pt::uint32_t _buttonState;
        Pt::uint32_t _button;
        Pt::uint32_t _wheel;
        double       _delta;
};

} // namespace

} // namespace

#endif
