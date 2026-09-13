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

#ifndef PT_FORMS_API_WINDOWS_H
#define PT_FORMS_API_WINDOWS_H

/** @defgroup Pt-Forms-Windows Windows and Workspaces

    @brief Windows and windows management.

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

    %WindowType and %WindowState are presentation of a window, not separate
    object kinds.
*/

#endif
