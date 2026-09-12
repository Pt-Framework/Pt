/* Copyright (C) 2017 Marc Boris Duerner
 
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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_FORMS_POPUP_H
#define PT_FORMS_POPUP_H

#include <Pt/Forms/Window.h>

namespace Pt {

namespace Forms {

/** @brief Borderless transient window.

    A %Popup is a %Window shown for a short interaction such as a menu.
    %setAnchor() relates it to the widget that opened it for popup input
    handling. The anchor does not position or size the popup. Move, size,
    raise, and show it explicitly.

    @ingroup Pt-Forms-Windows
*/
class PT_FORMS_API Popup : public Window
{
    typedef Window Base;

    public:
        Popup();
		
        virtual ~Popup();

        /** @brief Returns the anchor widget, or 0 when none is set.
        */
        Widget* anchor();

        /** @brief Relates this popup to @a anchor for popup input handling.

            Pass 0 to clear the anchor. The popup does not own @a anchor.
            The anchor does not position or size the popup.
        */
        void setAnchor(Widget* anchor = 0);

    protected:
        virtual void onAttachPeer(Widget& peer);

        virtual void onDetachPeer(Widget& peer);

    protected:
        virtual void onShowEvent(const ShowEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

        virtual void onProcessMouseEvent(const MouseEvent& ev);

        virtual void onProcessTouchEvent(const TouchEvent& ev);

    private:
        Widget* _anchor;
};

} // namespace

} // namespace

#endif
