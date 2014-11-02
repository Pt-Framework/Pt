/*
 * Copyright (C) 2014 Laurentiu-Gheorghe Crisan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "PaintSurfaceImpl.h"
#include "PainterImpl.h"
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"

namespace Pt{
namespace Hmi{

PaintSurfaceImpl::PaintSurfaceImpl(const Pt::Gfx::SizeF& size)
{
    create(size);    
}

PaintSurfaceImpl::~PaintSurfaceImpl()
{
    destroy();
}

Pt::Gfx::ARgbImage PaintSurfaceImpl::toImage()
{
	Pt::Gfx::Size size = Application::instance().fromUnit(_size);
	//TODO: conver _bitmapHandle to PT image
	Pt::Gfx::ARgbImage	image(size.width(), size.height());
	return image;
}

void PaintSurfaceImpl::create(const Pt::Gfx::SizeF& size)
{
    Pt::Gfx::Size nsize = Application::instance().fromUnit(size);

    Display* display = Application::instance().impl()->display();
    unsigned int depth = DefaultDepth( display, DefaultScreen(display) );
    _drawable = XCreatePixmap(display, XDefaultRootWindow(display), nsize.width(), nsize.height(), depth);
    XSync(display, false);
    _size = size;    
}

void PaintSurfaceImpl::destroy()
{
    Display* display = Application::instance().impl()->display();
    XFreePixmap(display, _drawable);
}

void PaintSurfaceImpl::resize(const Pt::Gfx::SizeF& size)
{ 
   destroy();
   create(size);
}

}}

