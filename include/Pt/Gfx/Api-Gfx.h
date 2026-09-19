/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_GFX_API_GFX_H
#define PT_GFX_API_GFX_H

/** @defgroup Pt-Gfx Graphics and Imaging

    @brief Images and 2D drawing.

    This module is the portable graphics layer, so a caller stores
    pixels, inspects them, and draws shapes and text through the same
    types on every supported platform. It is two reader tasks that
    share geometry, not two separate libraries: images own or wrap
    pixel buffers, and painters render onto paint surfaces. A
    %Bitmap is both a paint surface and an image source, so drawing
    results can be read back as pixels, and an %Image can be drawn
    onto a surface.

    Geometry is shared. %Point, %Size, and %Rect are floating-point
    values used by drawing; %PointI, %SizeI, and %RectI are integer
    pixel values used by images and by rounding. %PointF, %SizeF, and
    %RectF are typedefs of the floating-point types. Drawing commands
    take logical coordinates. A %Scaling on the paint surface converts
    those logical units to physical device pixels. Image width, height,
    and pixel positions are already physical pixels.

    Ownership does not move when one type uses another. An image either
    owns its buffer or wraps a caller buffer that must outlive the
    image. A view refers to an image region and does not copy pixels.
    A %Painter does not own the %PaintSurface or %PaintContext it
    paints on. The code that creates a surface, an image, or a wrapped
    buffer keeps it alive while painting or viewing is in progress.

    Forms uses this module for widget painting. A Forms %PaintSurface
    is another paint target, and a Forms %Painter issues the same
    drawing commands. The Forms updating chapter documents when a
    widget paints; pens, brushes, paths, and text are documented here.

    The rest of this chapter is the image model, then the drawing
    model.
*/

/** @defgroup Pt-Gfx-Images Images and Image Views

    @ingroup Pt-Gfx
*/

/** @defgroup Pt-Gfx-Drawing Drawing Graphics and Text

    @ingroup Pt-Gfx
*/

#endif
