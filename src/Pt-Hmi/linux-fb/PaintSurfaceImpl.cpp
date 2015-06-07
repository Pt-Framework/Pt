#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/Application.h>
#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include <linux/fb.h>

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
  const fb_var_screeninfo& screenInfo = Application::instance().impl()->screenInfo();
	const fb_fix_screeninfo& fixedInfo = Application::instance().impl()->fixedInfo();

	const size_t depth  =  screenInfo.bits_per_pixel;
	const size_t stride =  fixedInfo.line_length -  screenInfo.xres;
	
	if( depth == 16)
		_image.resize(size.width(), size.height(), Ui::ImageFormat::rgb565(), stride );
	else if( depth == 32 )
		_image.resize(size.width(), size.height(), Ui::ImageFormat::argb8888(), stride);
}

Ui::SizeF PaintSurfaceImpl::size() const
{
	return Ui::SizeF(_image.width(), _image.height());
}

}}
