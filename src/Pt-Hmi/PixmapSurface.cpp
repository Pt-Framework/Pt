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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Algorithm.h>

namespace Pt {

namespace Hmi {

PixmapSurface::PixmapSurface()
: _impl(0)
{
    _impl = new PixmapSurfaceImpl(*this);
}


PixmapSurface::~PixmapSurface()
{
    delete _impl;
}


void PixmapSurface::set(const Gfx::Image& image)
{
    _logicSize = scaling().toLogical( Gfx::SizeF( image.size().width(), 
                                                  image.size().height() ) );
    _impl->set(image);
}


bool PixmapSurface::empty() const
{
    return _impl->size().isNull();
}


void PixmapSurface::resize(const Gfx::SizeF& size)
{
    _logicSize = size;
    _impl->resize(size);
}


void PixmapSurface::clear(const Gfx::Color& c)
{
    _impl->clear(c);
}


void PixmapSurface::setScaleFactor(double scaling)
{
    if(_impl->scaling().scaleFactor() == scaling)
        return;

    _impl->setScaleFactor(scaling);

    // should be done in setScaleFactor
    _impl->resize(_logicSize);
}


const Gfx::ImageFormat& PixmapSurface::onGetFormat() const
{
    return _impl->format();
}


const Gfx::SizeF& PixmapSurface::onGetSize() const
{
    return _logicSize;
}


const Gfx::Scaling& PixmapSurface::onGetScaling() const
{
    return _impl->scaling();
}


Gfx::PaintContextPtr PixmapSurface::onBeginPaint(Gfx::PaintContext* context)
{
    return _impl->beginPaint(context);
}

/*
Gfx::PaintContext* PixmapSurface::onBeginPaint(Gfx::PaintContext* context)
{
    PaintDevice* device = ...;

    device->setCanvas
}
*/

Gfx::Image PixmapSurface::onGetImage() const
{
    return _impl->toImage();
}


void PixmapSurface::setFontDir(const System::Path& path)
{
    PixmapSurfaceImpl::setFontDir(path);
}


std::string PixmapSurface::defaultFont()
{
    return PixmapSurfaceImpl::defaultFont();
}


void PixmapSurface::setDefaultFont(const std::string& name)
{
    PixmapSurfaceImpl::setDefaultFont(name);
}


std::vector<std::string> PixmapSurface::fontNames()
{
    return PixmapSurfaceImpl::fontNames();
}

} // namespace

} // namespace
