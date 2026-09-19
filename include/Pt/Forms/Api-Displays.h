/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_FORMS_API_DISPLAYS_H
#define PT_FORMS_API_DISPLAYS_H

/** @addtogroup Pt-Forms-Displays

    @brief Text, panels, and progress without editing.

    Use a display when the user should read information without
    editing it.

    %Label is a caption or an icon in a layout. It has no chrome by
    default, so it reads as text or a picture on the parent surface.
    Alignment places it in its bounds.

    %Panel is a surface that groups other controls. An optional fill
    and frame set it apart from the parent, like a banner or a well.

    %ProgressBar shows how far a task has come. A track fills as the
    value moves through its range.

    %Label and %Panel share the panel appearance family. %ProgressBar
    has its own. On invalidate the control binds a family renderer
    through a styler. Widget-local options overlay the application
    style without replacing it.
*/

#endif
