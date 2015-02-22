#include <Pt/Hmi/PaintSurface.h>
#include "PaintSurfaceImpl.h"

namespace Pt{
namespace Hmi{

PaintSurface::PaintSurface(const Pt::Gfx::SizeF& size)
: _impl( new PaintSurfaceImpl(size))
, _painter(*this)
{

}

PaintSurface::~PaintSurface()
{
	delete _impl;
}

Pt::Gfx::ARgbImage PaintSurface::toImage() const
{
	return _impl->toImage();
}

void PaintSurface::resize(const Pt::Gfx::SizeF& size)
{
	_impl->resize(size);
    _painter.setSurface(*this);
}

Pt::Gfx::SizeF PaintSurface::size() const
{
	return _impl->size();
}

}}
