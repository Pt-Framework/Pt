#include <Pt/Hmi/ImagePaintSurface.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

ImagePaintSurface::ImagePaintSurface(const Pt::Gfx::SizeF& size)
: _image(100,100)
, _painter(*this)
{
	resize(size);
}

ImagePaintSurface::~ImagePaintSurface()
{
}

void ImagePaintSurface::resize(const Pt::Gfx::SizeF& size)
{
	Pt::Gfx::Size nsize = Pt::Hmi::Application::instance().fromUnit(size);
	_image.resize(nsize.width(), nsize.height());
}

Pt::Gfx::SizeF ImagePaintSurface::size() const
{
	Pt::Gfx::Size size(_image.width(), _image.height());
	return Pt::Hmi::Application::instance().toUnit(size);
}

}}
