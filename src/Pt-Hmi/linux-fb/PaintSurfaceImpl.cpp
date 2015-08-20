 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/Application.h>
#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include <linux/fb.h>

namespace Pt{
namespace Hmi{


PaintSurfaceImpl::PaintSurfaceImpl()
: _image(100,100)
, _originPos(0,0)
, _originSize(100,100)
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
	const size_t stride =  fixedInfo.line_length/(depth/8) -  screenInfo.xres;
	
	if( depth == 16 )
		_image.resize(size.width(), size.height(), Ui::ImageFormat::rgb565(), stride );
	else if( depth == 32 )
		_image.resize(size.width(), size.height(), Ui::ImageFormat::argb8888(), stride);

	_originSize  = size;
}


Ui::SizeF PaintSurfaceImpl::size() const
{
	return Ui::SizeF(_image.width(), _image.height());
}

void PaintSurfaceImpl::reserve( size_t bytes )
{
	_image.reserve( bytes );
}

}}
