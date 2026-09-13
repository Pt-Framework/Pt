/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan

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
   MA  02110-1301  USA
*/

#ifndef Pt_Forms_Button_H
#define Pt_Forms_Button_H

#include <Pt/Forms/Control.h>

namespace Pt {

namespace Forms {

/** @brief Interaction control for press, release, and click.

    A %Button holds a caption and hover, and runs a press, release, and
    cancel protocol that ends in %clicked(). It is not a look. Derive
    %PushButton for a command or %CheckBox for a boolean choice.

    %setText() sets the caption and records a mnemonic from an ampersand.
    Pointer, touch, mnemonic, shortcut, and the action key all run the
    protocol. A release outside the widget, or a scroll during a press,
    cancels.

    Connect %clicked() to handle a completed gesture. Override
    %onPressed(), %onReleased(), and %onCanceled() in a subclass to
    implement the command or choice.

    @code
    download.setText("&Download");
    download.clicked() += Pt::slot(*this, &MainWindow::onStartDownload);

    void MainWindow::onStartDownload()
    {
        client.fetch(url);
    }
    @endcode

    @ingroup Pt-Forms-Buttons
*/
class PT_FORMS_API Button : public Control
{
    public:
        typedef Control Base;

    public:
        /** @brief Creates a button that accepts focus.
        */
        Button();
    
        /** @brief Destroys the button.
        */
        virtual ~Button();

        /** @brief Returns the caption.
        */
        const Pt::String& text() const;

        /** @brief Sets the caption to @a t and records its mnemonic.
        */
        void setText(const Pt::String& t);

        /** @brief Returns true if the pointer is over the button.
        */
        bool isHovered() const;

        /** @brief Runs press then release as a completed click.
        */
        void click();

        /** @brief Returns the signal emitted after a completed click.
        */
        Signal<>& clicked();
    
    protected:
        /** @brief Called when a click gesture begins.
        */
        virtual void onPressed();

        /** @brief Called when a click gesture completes.
        */
        virtual void onReleased();

        /** @brief Called when a click gesture is abandoned.
        */
        virtual void onCanceled();

    protected:
        /** @brief Runs a completed click for the mnemonic character @a m.
        */
        virtual void onMnemonic(Pt::Char m);

        /** @brief Runs a completed click for the shortcut @a key.
        */
        virtual void onShortcut(const Key& key);

        /** @brief Presses or releases the button for the action key @a kev.
        */
        virtual void onActionKey(const KeyEvent& kev);

    protected:
        /** @brief Marks the button hovered and requests a repaint.
        */
        virtual bool onEnterEvent(const EnterEvent& ev);

        /** @brief Clears hover and requests a repaint.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

        /** @brief Forwards the focus change to the base control.
        */
        virtual void onFocusEvent(const FocusEvent& ev);

        /** @brief Presses, releases, or cancels from the mouse event @a ev.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Presses, releases, or cancels from the touch event @a ev.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Cancels an in-progress click and forwards @a ev.
        */
        virtual bool onScrollEvent(const ScrollEvent& ev);

    private:
        Signal<>   _clicked;
        bool       _onClickBegin;
        Pt::String _text;
        bool       _isHovered;
};

} // namespace

} // namespace

#endif
