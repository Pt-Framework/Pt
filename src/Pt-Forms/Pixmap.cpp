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

#include "PixmapImpl.h"

#include <Pt/Forms/Pixmap.h>

namespace Pt {

namespace Forms {

Pixmap::Pixmap()
: _impl(0)
{
    _impl = new PixmapImpl();
}


Pixmap::~Pixmap()
{
    delete _impl;
}


void Pixmap::reset()
{
    releaseCanvas();
    _impl->reset();
}


void Pixmap::reset(const Gfx::Image& image)
{
    releaseCanvas();
    _impl->reset(image);
}


void Pixmap::reset(const Gfx::SizeF& size)
{
    releaseCanvas();
    _impl->reset(size);
}


bool Pixmap::empty() const
{
    return _impl->size().isNull();
}


void Pixmap::getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect)
{
     _impl->getBitmap(bitmap, rect);
}


void Pixmap::setScaleFactor(double scaling)
{
    _impl->setScaleFactor(scaling);
}


void Pixmap::onDrawPixmap(const Gfx::PointF& to, const Pixmap& pixmap, 
                          const Gfx::Paint& paint, const Gfx::RectF* rect)
{
    _impl->drawPixmap(to, pixmap, paint, rect);
}


const Gfx::ImageFormat& Pixmap::onGetFormat() const
{
    return _impl->format();
}


const Gfx::SizeF& Pixmap::onGetSize() const
{
    return _impl->size();
}


const Gfx::Scaling& Pixmap::onGetScaling() const
{
    return _impl->scaling();
}


Gfx::Canvas* Pixmap::onGetCanvas(Gfx::Canvas* reuse)
{
    Gfx::Canvas* canvas = _impl->getCanvas(reuse);
    if(canvas)
        return canvas;

    return PaintSurface::onGetCanvas(reuse);
}


Gfx::Canvas* Pixmap::onCreateCanvas(Gfx::Canvas* reuse)
{
    return _impl->createCanvas(reuse);
}


void Pixmap::onReleaseCanvas()
{
    _impl->releaseCanvas();
}


void Pixmap::onSync() 
{
    _impl->sync();
}


void Pixmap::onFinish() 
{
    _impl->finish();
}


std::string Pixmap::defaultFont()
{
    return PixmapImpl::defaultFont();
}


void Pixmap::setDefaultFont(const std::string& family)
{
    PixmapImpl::setDefaultFont(family);
}


std::vector<std::string> Pixmap::fontFamilies()
{
    return PixmapImpl::fontFamilies();
}


std::vector<Gfx::FontFace> Pixmap::fontFaces(const std::string& family)
{
    return PixmapImpl::fontFaces(family);
}

} // namespace

} // namespace
