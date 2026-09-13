/*
  Copyright (C) 2026 Marc Boris Duerner

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
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef PT_FORMS_API_INPUT_H
#define PT_FORMS_API_INPUT_H

/** @defgroup Pt-Forms-Input Mouse, Touch, Keyboard Input

    @brief Deliver pointer, keyboard, focus, and text input to controls.

    Platform input enters the %Application event loop and is then targeted
    into the connected visual hierarchy. Pointer, touch, and scroll use
    hit-testing: the %Screen (or a workspace) finds the widget under the
    pointer. Keyboard input uses focus: the %Form delivers keys to the
    focused %Control. These are two targeting models, not two event loops.

    Every %Widget is a %Responder. A responder handles an event or forwards
    it. Returning true stops the chain; returning false continues to the
    next responder. Mouse and touch positions are converted to each
    responder's local coordinates. Pointer capture overrides hit-testing
    until release. When the pointer target changes, the previous widget
    receives a leave event before the new widget receives an enter event.

    A %Form is the focus root of its content tree. It handles Tab
    traversal, shortcuts, and Alt mnemonics before ordinary keys reach the
    focused control. A %Control may take part in that focus set; pointer
    and touch presses on a control typically request focus.

    An %InputMethod is the extension for platform or custom text entry. It
    begins and finishes a session for a control, may show an active window,
    and sends %KeyEvent objects to its current receiver. The application
    always has a default method.

    %MouseEvent, %TouchEvent, %ScrollEvent, %EnterEvent, %LeaveEvent,
    %KeyEvent, and %FocusEvent carry the event data. %Key identifies a key
    and its modifiers.
*/

#endif
