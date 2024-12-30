/* Copyright (C) 2015-2024 Marc Boris Duerner

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

#ifndef PT_HMI_RESPONDER_H
#define PT_HMI_RESPONDER_H

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Point.h>

namespace Pt {

namespace Hmi {

class MouseEvent;
class TouchEvent;
class ScrollEvent;
class EnterEvent;
class LeaveEvent;
class KeyEvent;

class PT_HMI_API Responder
{
    protected:
        Responder();

    public:
        virtual ~Responder();

        bool mouseEvent(const MouseEvent& ev);

        void touchEvent(const TouchEvent& ev);

        void scrollEvent(const ScrollEvent& ev);

        void enterEvent(const EnterEvent& ev);

        void leaveEvent(const LeaveEvent& ev);

        void keyEvent(const KeyEvent& ev);

    protected:
        virtual Responder* onNextResponder() = 0;

        virtual Gfx::PointF onToGlobal(const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromGlobal(const Gfx::PointF& pos) const = 0;

    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onEnterEvent(const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    protected:
        virtual bool onMousePress(const MouseEvent& ev) 
        { return false; }

        virtual bool onMouseRelease(const MouseEvent& ev) 
        { return false; }

        virtual bool onMouseMove(const MouseEvent& ev) 
        { return false; }
};

} // namespace

} // namespace

#endif // include guard