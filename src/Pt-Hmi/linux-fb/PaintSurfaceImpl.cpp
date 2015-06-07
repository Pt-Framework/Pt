#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/Application.h>
#include "ScreenImpl.h"

namespace Pt{
namespace Hmi{


PaintSurfaceImpl::PaintSurfaceImpl()
: _image(100,100)
{	
}


PaintSurfaceImpl::~PaintSurfaceImpl()
{
}

void PaintSurfaceImpl::resize(const Ui::SizeF& size)
{
	const size_t depth  =  Application::instance().mainScreen().impl()->depth();
	const size_t stride =  Application::instance().mainScreen().impl()->stride();		
	
	if( depth == 16)
		_image.resize(size.width(), size.height(), stride, Ui::ImageFormat::rgb565() );
	else if( depth == 32 )
		_image.resize(size.width(), size.height(), stride, Ui::ImageFormat::argb8888() );
}

Ui::SizeF PaintSurfaceImpl::size() const
{
	return Ui::SizeF(_image.width(), _image.height());
}

}}
