#include <Pt/Hmi/PaintSurface.h>
#include "PaintSurfaceImpl.h"

namespace Pt{
namespace Hmi{

PaintSurface::PaintSurface()
: _impl( new PaintSurfaceImpl())
, _painter(*this)
{

}

PaintSurface::~PaintSurface()
{
	delete _impl;
}

void PaintSurface::resize(const Ui::SizeF& size)
{
	_impl->resize(size);
  _painter.setSurface(*this);
}

Ui::SizeF PaintSurface::size() const
{
	return _impl->size();
}

}}
