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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include "BitmapSurface.h"

#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Gfx {

Bitmap::Bitmap()
: _surface(0)
{
    _surface = new BitmapSurface();
}


Bitmap::Bitmap(const Gfx::SizeF& size, std::size_t stride)
: _surface(0)
{
    _surface = new BitmapSurface();

    reset(size, stride);
}


Bitmap::~Bitmap()
{
    delete _surface;
}


void Bitmap::reset(const Gfx::Image& image)
{
    _surface->reset(image);
    releaseCanvas();
}


void Bitmap::reset(const Gfx::SizeF& sizeF, std::size_t stride)
{
    _surface->reset(sizeF, stride);
    releaseCanvas();
}


bool Bitmap::empty() const
{
    return _surface->image().empty();
}


const Gfx::Image& Bitmap::image() const
{
    return _surface->image();
}


void Bitmap::setScaleFactor(double scaleFactor)
{
    _surface->setScaleFactor(scaleFactor);
    releaseCanvas();
}


void Bitmap::drawBitmap(const Pt::Gfx::PointF& to, const Bitmap& bitmap,
                        const Gfx::Paint& paint, const Gfx::RectF* rect)
{
    _surface->drawBitmap(to, bitmap, paint, rect);
}


const Gfx::ImageFormat& Bitmap::onGetFormat() const
{
    return _surface->format();
}


const Gfx::SizeF& Bitmap::onGetSize() const
{
    return _surface->size();
}


const Scaling& Bitmap::onGetScaling() const
{
    return _surface->scaling();
}


Gfx::Canvas* Bitmap::onCreateCanvas(Gfx::Canvas* reuse)
{
    return _surface->createCanvas(reuse);
}


void Bitmap::onReleaseCanvas()
{
    _surface->releaseCanvas();
}


void Bitmap::onSync()
{
    _surface->sync();
}


void Bitmap::onFinish()
{
    _surface->finish();
}


const std::string& Bitmap::defaultFont()
{
    return BitmapSurface::defaultFont();
}


void Bitmap::setDefaultFont(const std::string& f)
{
    BitmapSurface::setDefaultFont(f);
}


std::vector<FontFace> Bitmap::fonts()
{
    return BitmapSurface::fonts();
}

} // namespace

} // namespace
