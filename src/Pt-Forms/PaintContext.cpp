#include <Pt/Forms/PaintContext.h>

namespace Pt {

namespace Forms {

PaintContext::PaintContext(PaintSurface& surface)
: Gfx::PaintContext( surface )
, _surface(surface)
{
}


PaintContext::~PaintContext()
{
}


PaintSurface& PaintContext::surface()
{
    return _surface;
}


void PaintContext::drawPixmap(const Gfx::PointF& to,
                              const Pixmap& pixmap,
                              const Gfx::Paint& paint,
                              const Gfx::RectF* rect) const
{
    _surface.drawPixmap(to, pixmap, paint, rect);
}

} // namespace

} // namespace
