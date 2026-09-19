/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_FORMS_API_WINDOWS_H
#define PT_FORMS_API_WINDOWS_H

/** @addtogroup Pt-Forms-Windows

    @brief Windows and workspace management.

    Forms presents windows in two places: on a platform display, and inside
    a workspace. In both cases a %Window is a %Form attached to a
    %WindowManager. The manager does not own the window. This is the same
    non-owning host pattern as %Application with a %Screen and %Form with a
    content %Control.

    A top-level window uses the primary %Screen and a platform frame. A
    %Workspace nests that windowing model inside a %Control: ordinary
    background content plus a stack of windows in the control's bounds. It
    is not a second screen. A %Popup is still a window, used for a short
    interaction tied to an opener, not a third model.

    A window joins a manager when it is shown, unless the application
    already selected a parent manager. Showing connects it to the screen
    and frame. Closing detaches it and does not destroy it. Geometry,
    visibility, and activation are requests confirmed by events, as with
    other widgets.
*/

#endif
