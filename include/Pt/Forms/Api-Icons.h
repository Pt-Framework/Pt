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

#ifndef PT_FORMS_API_ICONS_H
#define PT_FORMS_API_ICONS_H

/** @addtogroup Pt-Forms-Icons

    @brief Icons and textual content for controls.

  Controls use %Icon and %TextBlock to supply visual content. These types
  are not widgets and do not appear in the visual hierarchy. Derive custom
  visual content from %Control, not from an icon or text block.

  An %Icon associates one picture with images at one or more logical sizes.
  A control requests the image that fits the area it is about to display.
  Add images directly for the usual case. Implement %IconProvider only when
  images must come from another source.

  A %TextBlock converts a string into positioned %TextLine objects. Controls
  such as %Label use it to lay out text. A custom control configures the
  available width and alignment, calls %TextBlock::layout(), and uses the
  resulting lines when it measures or paints wrapped text.
*/

#endif
