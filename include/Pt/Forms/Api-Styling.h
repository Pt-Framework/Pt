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

#ifndef PT_FORMS_API_STYLING_H
#define PT_FORMS_API_STYLING_H

/** @addtogroup Pt-Forms-Styling

    @brief Styles, options, and renderers for widget appearance.

    A Forms application has a %Style and %StyleOptions. The style is the
    look of the controls. The options are shared colors, pens, brushes,
    and the font. %Application owns both and starts with %PlatinumStyle.

    Change the theme on the application. A widget can overlay local
    options or assign a custom renderer without replacing the style.

    %Styler binds a control to the current style. %Renderer is the
    cloneable facet that draws one control family. Derive a %Style to
    install a different look. Derive a %Renderer to draw a control
    family. Derive a %Styler only when adding a new control family.
*/

#endif
