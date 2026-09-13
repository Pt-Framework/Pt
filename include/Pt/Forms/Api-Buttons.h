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

#ifndef PT_FORMS_API_BUTTONS_H
#define PT_FORMS_API_BUTTONS_H

/** @addtogroup Pt-Forms-Buttons

    @brief Commands and boolean choices.

    Use a button when the user should run a command or turn a choice
    on or off. A completed gesture runs a press, release, and cancel
    protocol and ends in %clicked().

    %Button is the shared interaction control. It is not a look. It
    holds the caption and hover, and is the type %PushButton and
    %CheckBox share.

    %PushButton is a classic button: a caption on a clickable face,
    sometimes with an icon beside the text. Clicking it runs a
    command. As a toggle it stays visually pressed to show that a
    mode is on.

    %CheckBox is a clickable choice with a caption beside a checkable
    area. The area shows whether the choice is on or off.

    Appearance follows the application style. On invalidate the
    control binds a family renderer through a styler. Widget-local
    brushes, pens, colors, fonts, and %setRenderer() overlay that
    style without replacing it.
*/

#endif
