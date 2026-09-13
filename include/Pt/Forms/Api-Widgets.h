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

#ifndef PT_FORMS_API_WIDGETS_H
#define PT_FORMS_API_WIDGETS_H

/** @defgroup Pt-Forms-Widgets Widgets and Views

    @brief Widget and view object model.

    Every visual %Forms object is a %Widget. %Screen and %WindowManager are
    widgets but not views. %Form and %Control are the usual %View
    implementations; a view is the host and paint-surface boundary for
    controls. When a control joins a form that is shown on a screen, the
    screen, scaling, coordinates, and paint surface become available.
    Removing it reverses that relationship. Measurement determines the
    preferred size for a %SizePolicy; layout then assigns geometry within
    the available rectangle. Changes to visual state, geometry, or drawing
    request an update through the containing views to the window. Painting
    then travels back down through visible content after the window frame
    makes a paint surface available.

    Derive custom visual content from %Control. Derive from %View only when a
    custom content host needs different paint-surface or coordinate behavior.
    %WindowManager, %WindowFrame, and %GraphicsBackend support platform and
    embedded-window implementations; applications normally use the window
    manager provided by a %Screen or %Workspace.

    @ingroup Pt-Forms
*/

#endif
