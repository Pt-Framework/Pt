/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
