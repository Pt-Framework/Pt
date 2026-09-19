/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_FORMS_API_INPUT_H
#define PT_FORMS_API_INPUT_H

/** @addtogroup Pt-Forms-Input

    @brief Pointer, keyboard, and text-entry input.

  Platform input enters the %Application event loop and is targeted into the
  connected visual hierarchy. Pointer, touch, and scroll input are directed
  by hit testing, while keyboard input is directed by focus. The %Screen or a
  workspace finds the widget below the pointer. These are two targeting
  models, not two event loops. Every %Widget is a %Responder. A responder
  handles an event by returning true; returning false continues delivery
  through the responder chain. Mouse and touch positions are converted to
  each responder's local coordinates before its handler runs.

  Hit testing selects the frontmost eligible widget below the pointer.
  %Widget::setCapture() temporarily overrides hit testing and directs pointer
  input to the capturing widget until it releases capture. Forms sends a
  %LeaveEvent before an %EnterEvent when the pointer target changes. A
  %MouseEvent reports movement and button state, a %TouchEvent reports an
  individual touch and pressure, and a %ScrollEvent reports the scroll axis
  and delta.

  A %Form is the focus root for its content tree. A focused control receives
  key input, subject to the form's interaction rules: an active control is
  considered first, then registered shortcuts, Alt mnemonics, and Tab focus
  traversal, followed by the focused control and responder chain. A control
  can take part in that focus set; pointer and touch presses on a control
  typically request focus. A %KeyEvent carries a %Key, press or release
  action, and optional Unicode text. Controls register their focus policy,
  action key, shortcut, and mnemonic through their public APIs.

  %InputMethod is the extension point for platform or custom text entry. It
  begins a session for a control, retains only a non-owning receiver, and
  sends %KeyEvent objects to that receiver. Ordinary controls use the method
  installed on the application, which always provides a default method;
  derive an input method only to implement a text-entry interface.
*/

#endif
