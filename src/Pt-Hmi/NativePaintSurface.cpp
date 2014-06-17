#include <Pt/Hmi/NativePaintSurface.h>
#include "PaintSurfaceImpl.h"

namespace Pt{
namespace Hmi{

NativePaintSurface::NativePaintSurface(const Pt::Gfx::SizeF& size)
: _impl( new PaintSurfaceImpl(size))
, _painter(*this)
{

}

NativePaintSurface::~NativePaintSurface()
{
	delete _impl;
}

Pt::Gfx::ARgbImage NativePaintSurface::toImage() const
{
	return _impl->toImage();
}

void NativePaintSurface::resize(const Pt::Gfx::SizeF& size)
{
	_impl->resize(size);
}

Pt::Gfx::SizeF NativePaintSurface::size() const
{
	return _impl->size();
}

}}
