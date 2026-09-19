/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
