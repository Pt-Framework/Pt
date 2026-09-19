/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_FORMS_API_UPDATING_H
#define PT_FORMS_API_UPDATING_H

/** @addtogroup Pt-Forms-Updating

    @brief Invalidate, layouting, and painting of visual content.

    Visual content updates in one cycle: invalidate, layouting, then
    painting. Call %Widget::invalidate() after a visual-only change. It
    coalesces and later calls %onInvalidate() so the widget can rebuild
    derived visual state. %Control::onInvalidate() also requests relayout.
    Call %Control::relayout() after a content or policy change that affects
    size or geometry.

    Layouting has two passes, not two pipelines. Measure is inside-out:
    %Control::measure() / %onMeasure() compute a preferred size under a
    %SizePolicy and cache it until content, policy, limits, or scaling
    invalidate it. Layout is outside-in: %onLayout() assigns child geometry
    in the available rectangle with %move() and %resize(). A layout event
    then requests a repaint.

    Painting is not synchronous. %Widget::repaint() queues a local dirty
    region. A %PaintEvent carries that rectangle down visible content after
    the window frame makes a %PaintSurface available. A %View adapts that
    surface. %onPaint() draws in local coordinates.

    %PaintSurface, %PaintContext, and %Painter are one mechanism. A
    %PaintSurface is the Forms render target; a view borrows it and does
    not own it. A %PaintContext is the active session on that surface and
    the argument to %onPaint(). A %Painter binds to the surface or the
    context and issues drawing commands. It extends Gfx with pixmap
    drawing. A %Pixmap is an off-screen %PaintSurface used as a back buffer
    or as cached image content. Use @ref Pt-Gfx-Page-Drawing for pens,
    brushes, text, and paths.

    A styled control does not invent a second cycle. %onInvalidate() binds
    the %Styler. Layouting calls named measure then layout methods.
    Painting calls named render methods. @ref Pt-Forms-Page-Styling
    describes that implementation.

    %GraphicsBackend and %IPixmapImpl are platform integration.
    Ordinary applications do not call them.

    The following example implements the three update hooks on a custom
    control. %onMeasure() returns the preferred size for the control.
    %onLayout() places a child in the rectangle assigned by the parent.
    %onPaint() constructs a %Painter on the supplied %PaintContext and
    fills the dirty rectangle. The painter does not own the context or
    the surface.

    @code
    Gfx::SizeF MyControl::onMeasure(const Pt::Forms::SizePolicy&)
    {
        return Gfx::SizeF(80, 24);
    }

    void MyControl::onLayout(const Gfx::RectF& rect)
    {
        if(_content)
        {
            _content->move(rect.topLeft());
            _content->resize(rect.size());
        }
    }

    void MyControl::onPaint(Pt::Forms::PaintContext& context,
                            const Gfx::RectF& rect)
    {
        Pt::Forms::Painter painter(context);
        painter.setBrush(Pt::Gfx::Brush(Pt::Gfx::Color(240, 240, 240)));
        painter.fillRect(rect);
    }
    @endcode
*/

#endif
