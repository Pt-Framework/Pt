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

#ifndef PT_FORMS_RESPONDER_H
#define PT_FORMS_RESPONDER_H

#include <Pt/Forms/Api.h>
#include <Pt/Gfx/Point.h>

namespace Pt {

namespace Forms {

class MouseEvent;
class TouchEvent;
class ScrollEvent;
class EnterEvent;
class LeaveEvent;
class KeyEvent;

/** @brief Object that handles input or forwards it.

    A %Responder is the unit of input delivery. %Widget is the usual
    responder. Handle an event by returning true; return false to continue
    to the next responder. Mouse and touch positions are converted to the
    current responder's local coordinates before the handler runs.

    Applications derive visual content from %Control, not from %Responder.
    A custom responder must implement %onNextResponder(), %onToGlobal(),
    and %onFromGlobal() and must not create a cycle in the chain.

    @ingroup Pt-Forms-Input
*/
class PT_FORMS_API Responder
{
    protected:
        /** @brief Creates a responder.
        */
        Responder();

    public:
        /** @brief Destructor.
        */
        virtual ~Responder();

        /** @brief Delivers @a ev along the mouse responder chain.

            Converts the position to each responder's local coordinates.
            Returns true when a responder handles the event.
        */
        bool mouseEvent(const MouseEvent& ev);

        /** @brief Delivers @a ev along the touch responder chain.

            Converts the position to each responder's local coordinates.
        */
        void touchEvent(const TouchEvent& ev);

        /** @brief Delivers @a ev along the scroll responder chain.
        */
        void scrollEvent(const ScrollEvent& ev);

        /** @brief Delivers @a ev along the enter responder chain.
        */
        void enterEvent(const EnterEvent& ev);

        /** @brief Delivers @a ev along the leave responder chain.
        */
        void leaveEvent(const LeaveEvent& ev);

        /** @brief Delivers @a ev along the key responder chain.
        */
        void keyEvent(const KeyEvent& ev);

    protected:
        /** @brief Returns the next responder, or 0 at the end of the chain.
        */
        virtual Responder* onNextResponder() = 0;

        /** @brief Converts local position @a pos to global coordinates.
        */
        virtual Gfx::PointF onToGlobal(const Gfx::PointF& pos) const = 0;

        /** @brief Converts global position @a pos to local coordinates.
        */
        virtual Gfx::PointF onFromGlobal(const Gfx::PointF& pos) const = 0;

    protected:
        /** @brief Handles a mouse event in local coordinates.

            Return true to stop the chain, false to continue.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Handles a touch event in local coordinates.

            Return true to stop the chain, false to continue.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Handles a scroll event.

            Return true to stop the chain, false to continue.
        */
        virtual bool onScrollEvent(const ScrollEvent& ev);

        /** @brief Handles pointer entry.

            Return true to stop the chain, false to continue.
        */
        virtual bool onEnterEvent(const EnterEvent& ev);

        /** @brief Handles pointer leave.

            Return true to stop the chain, false to continue.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

        /** @brief Handles a key event.

            Return true to stop the chain, false to continue.
        */
        virtual bool onKeyEvent(const KeyEvent& ev);

    protected:
        /** @brief Handles a mouse press. The default returns false.
        */
        virtual bool onMousePress(const MouseEvent& ev) 
        { return false; }

        /** @brief Handles a mouse release. The default returns false.
        */
        virtual bool onMouseRelease(const MouseEvent& ev) 
        { return false; }

        /** @brief Handles a mouse move. The default returns false.
        */
        virtual bool onMouseMove(const MouseEvent& ev) 
        { return false; }
};

} // namespace

} // namespace

#endif // include guard