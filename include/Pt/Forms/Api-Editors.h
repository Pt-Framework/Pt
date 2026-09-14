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

#ifndef PT_FORMS_API_EDITORS_H
#define PT_FORMS_API_EDITORS_H

/** @addtogroup Pt-Forms-Editors

    @brief Text, numbers, lists, and ranges for entering or picking a value.

    Use an editor or selector when the user should enter or pick a value.

    %LineEdit is a single-line text field. An entry shows the text, a
    caret, and an optional selection. Placeholder text can appear when
    the field is empty.

    %SpinBox is an integer in a range. An entry shows the number. Up
    and down controls step it. A style may merge those controls with
    the entry or place them beside it.

    %ComboBox is a value chosen from a list, or typed when the box is
    editable. An entry shows the current text. A button opens a popup
    list.

    %Slider is a value chosen along a range by dragging. A track shows
    the range. A handle shows the current value.

    %LineEditor is not a control. It stores the text, caret, and scroll
    of one line. %LineEdit, %SpinBox, and %ComboBox use it.

    Appearance follows the application style. On invalidate the control
    binds a family renderer through a styler. Widget-local brushes,
    pens, colors, fonts, and %setRenderer() overlay that style without
    replacing it.
*/

#endif
