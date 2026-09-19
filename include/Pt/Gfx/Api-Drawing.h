/* Copyright (C) 2015 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_GFX_API_DRAWING_H
#define PT_GFX_API_DRAWING_H

/** @addtogroup Pt-Gfx-Drawing

    @brief Rendering 2D shapes, text and images on paint surfaces.

    Drawing always targets a %PaintSurface. The surface reports format,
    physical size, and %Scaling, and it supplies the backend resources
    used to rasterize commands. %Bitmap is the portable in-memory
    surface. After painting, %Bitmap::image() returns the pixels so
    they can be copied, encoded, or drawn again. Other modules provide
    further surfaces: a Forms paint surface is the same abstraction
    on a window or control. @ref Pt-Forms-Page-Updating documents
    when Forms paints; this chapter documents the drawing commands.

    A %PaintContext is an active session on a surface. It can install
    a default clip that every painter on that session intersects.
    Constructing a context attaches it to the surface; destroying it
    detaches it. The surface must outlive the context.

    %Painter is the type a caller constructs. It begins painting on a
    surface or on an existing context, through a constructor or
    %begin(), and %finish() ends the session. %PainterBase is the
    command API: pens, brushes, fonts, transforms, clips, and the
    draw and fill operations. %Painter does not own the surface or
    the context. One painter is bound to at most one target at a
    time.

    Paint state is independent of geometry. %Pen strokes outlines,
    %Brush fills closed shapes, %Font selects a typeface for text,
    and %CompositionMode chooses source-copy or source-over blending.
    %Paint bundles those four so the same state can be reused.
    %setTransform() maps user coordinates before they reach the
    surface. %setClip() restricts drawing to a rectangle. Coordinates
    passed to drawing commands are logical; the surface %Scaling
    converts them to physical pixels.

    %Path stores moves, lines, and curves independently of a painter.
    A painter can stroke or fill its current path, or stroke or fill
    a path passed to %drawPath() and %fillPath(). Containment tests
    on the path use a %FillRule and do not require a surface.

    Text uses one font request for measurement and drawing. %Font
    names a family, size, and style; it is not a loaded face.
    %fontMetrics() returns ascent, descent, and line height for the
    current font. %textMetrics() measures one string. %drawText()
    draws that string at a baseline origin. %Font::addFont() and
    %Font::addFonts() register font files for later requests.
    %Bitmap lists the families and faces the backend can resolve.

    %drawImage() and %drawBitmap() composite image content onto the
    current target. The image and bitmap APIs remain the storage
    model; these operations only sample them.

    %Canvas is the backend that executes commands. Its constructor
    is protected. Ordinary callers do not create a canvas; the
    surface creates one while a painter is active.

    The example paints into a bitmap, then reads the result as an
    image. The painter does not own the bitmap.

    @code
    Pt::Gfx::Bitmap bitmap(Pt::Gfx::SizeF(320, 240));
    Pt::Gfx::Painter painter(bitmap);
    painter.setBrush(Pt::Gfx::Brush(Pt::Gfx::Color(255, 255, 255)));
    painter.fillRect(Pt::Gfx::RectF(Pt::Gfx::SizeF(320, 240)));
    painter.setPen(Pt::Gfx::Pen(Pt::Gfx::Color(0, 0, 0)));
    painter.drawLine(Pt::Gfx::PointF(0, 0), Pt::Gfx::PointF(320, 240));
    painter.setFont(Pt::Gfx::Font("Sans", 12));
    painter.drawText(Pt::Gfx::PointF(10, 24), Pt::String("Hello"));
    painter.finish();
    const Pt::Gfx::Image& image = bitmap.image();
    @endcode
*/

#endif
