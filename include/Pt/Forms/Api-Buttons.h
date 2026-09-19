/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
