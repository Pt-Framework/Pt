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

#ifndef PT_GUI_KEYEVENT_H
#define PT_GUI_KEYEVENT_H

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>

#include <cstddef>


namespace Pt {

namespace Gui {
    class Widget;

    /**
     * @brief An event which indicates that a keystroke occurred in a widget.
     *
     * This high-level event is generated and sent when a key is pressed or released while
     * a widget has the focus. The widget for which the key was pressed or released can be
     * retrieved with the method widget().
     *
     * This event object stores the following information for the key event:
     *
     * - %Event %type: Pressed or release to specify if the key was pressed or released. Use
     * the method type() to access this information.
     *
     * - Key %code: For keys that don't have a character representation, like Shift, Control, the
     * function keys etc., a key %code is stored that can be read be the event's receiver using the
     * code() method. If a character key is pressed the key %code is set to %Void but the text
     * attribute is set to the character which was pressed. For example, if the user pressed F2 the
     * keyCode F2 is stored (and no text character is stored).
     *
     * - Text character: For keys that have a character representation, like alphanumeric keys
     * or symbols this character is stored and can be accessed using the method text(). For example,
     * if the user presses the 'a'-key the character 'a' is stored. If the user presses Shift+'a'
     * the character 'A' is stored. For non-character keys the character's code is 0.
     * 
     * !Example
     * For example, pressing the Shift key will cause a 'Press' event with a ShiftL (or ShiftR) keyCode,
     * while pressing the 'a' key will result in an event with the character text of 'A' (and a Void
     * keyCode) as the shift key was pressed. After the 'a' key is released, a 'Release' event will be
     * generated with a chracter text of 'A' (and Void keyCode) as Shift is still be pressed. After the
     * Shift key is released, there will also ge a key event with a keyCode of ShiftL (or ShiftR) and
     * no character text.
     */
    class PT_GUI_API KeyEvent : public Event {
        public:
            /**
             * @brief The type of KeyEvent, either Press or Release as a result of a pressed or released key.
             */
            enum Type {
                Press,   //! Specifies that the key event is for a key which was pressed.
                Release  //! Specifies that the key event is for a key which was released.
            };

            /**
             * @brief The key code for a pressed or released key which does not have a character representation.
             */
            enum KeyCode {
                Void = 0, CtrlL, CtrlR, AltL, AltR, ShiftL, ShiftR,
                F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
                Enter, Backspace, Tab, Cancel, Clear, Pause, CapsLock, Escape, Space, PageUp, PageDown, End, Home, Insert, Delete,
                Left, Right, Up, Down, NumLock, ScrollLock, PrintScreen, Help, Meta, WindowsL, WindowsR, ContextMenu
            };

            //! @brief The type information object (type_info) of this event class.
            static const std::type_info& TYPE_INFO;

        public:
            /**
             * @brief Constructs a new key event with information about the type, keyCode and character text.
             *
             * @param widget The widget for which this key event was generated.
             * @param type The key event type; either Press or Release for a pressed or released key.
             * @param code The key code for the key which was pressed. This is only relevant for keys which
             * don't have a character representation, like for example Shift or the function keys. This
             * value must be Void if this is a key event for character key.
             * @param text The character for the key which was pressed. This is only relevant for keys which
             * have a character representation, like for example 'a' or '/'. This value has to be 0 if a
             * non-character key is pressed.
             */
            KeyEvent(Widget& widget, const Type& type, KeyCode code, wchar_t text);

            //! @brief Empty desctructor.
            virtual ~KeyEvent();

            /**
             * @brief The type of this key event; either Press or Released for a pressed or released key.
             *
             * See the class description for further information on this subject.
             *
             * @return The type of this key event.
             */
            Type type() const
            { return _type; }

            /**
             * @brief The key code of this key event.
             *
             * This is a KeyCode value for non-character keys and Void for character keys.
             *
             * See the class description for further information on this subject.
             *
             * @return The key code of this key event.
             */
            KeyCode code() const
            { return _code; }

            /**
             * @brief The text (character) of this key event.
             *
             * This is a character value for character keys and 0 for non-character keys.
             *
             * See the class description for further information on this subject.
             *
             * @return The key code of this key event.
             */
            wchar_t text() const
            { return _text; }

        protected:
            /**
             * @brief Returns the type info for this event.
             *
             * @return The type info for this event.
             */
            virtual const std::type_info& onTypeInfo() const;

            Pt::Event& onClone(Pt::Allocator& allocator) const
            {
                void* pEvent= allocator.allocate(sizeof(KeyEvent));
                return *(new (pEvent)KeyEvent(*this));
            }

            void onDestroy(Pt::Allocator& allocator)
            {
                allocator.deallocate(this, sizeof(KeyEvent));
            }

        private:
            Type _type;
            KeyCode _code;
            wchar_t _text;
    };

} // namespace Gui

} // namespace Pt

#endif
