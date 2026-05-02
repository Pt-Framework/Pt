#include <Pt/Forms/PaintContext.h>

namespace Pt {

namespace Forms {

PaintContext::PaintContext(PaintSurface& surface)
: Gfx::PaintContext( surface )
, _surface(&surface)
{
}


PaintContext::~PaintContext()
{
}


PaintSurface& PaintContext::surface()
{
    return *_surface;
}


void PaintContext::onDetachSurface(Gfx::PaintSurface& surface)
{
    _surface = 0;
    Gfx::PaintContext::onDetachSurface(surface);
}

} // namespace

} // namespace
