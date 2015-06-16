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

const Ui::SizeF& PaintSurface::originSize() const
{
  return _impl->originSize();
}

void PaintSurface::setOrigin(const Ui::PointF& pos, const Ui::SizeF& size)
{
  _impl->setOrigin(pos,size);
}


const Ui::PointF& PaintSurface::originPos() const
{
  return _impl->originPos();
}

Ui::SizeF PaintSurface::size() const
{
	return _impl->size();
}

}}
