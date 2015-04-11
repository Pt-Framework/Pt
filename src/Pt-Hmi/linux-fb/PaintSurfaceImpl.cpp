#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

PaintSurfaceImpl::PaintSurfaceImpl()
: _image(100,100)
{	
}

PaintSurfaceImpl::~PaintSurfaceImpl()
{
}

void PaintSurfaceImpl::resize(const Pt::Gfx::SizeF& size)
{
	_image.resize(size.width(), size.height());
}

Pt::Gfx::SizeF PaintSurfaceImpl::size() const
{
	return Pt::Gfx::SizeF(_image.width(), _image.height());
}

}}
