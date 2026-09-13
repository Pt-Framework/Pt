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
