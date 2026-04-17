#include <Pt/Forms/PaintSurface.h>

namespace Pt {

namespace Forms {

PaintSurface::PaintSurface()
{
}


PaintSurface::~PaintSurface()
{
}


void PaintSurface::drawPixmap(const Gfx::PointF& to,
                               const Pixmap& pixmap,
                               const Gfx::Paint& paint,
                               const Gfx::RectF* rect)
{
    onDrawPixmap(to, pixmap, paint, rect);
}


void PaintSurface::drawPixmap(Gfx::Canvas& canvas,
                               const Gfx::PointF& to,
                               const Pixmap& pixmap,
                               const Gfx::RectF* rect)
{
    onDrawPixmap(canvas, to, pixmap, rect);
}

} // namespace

} // namespace
