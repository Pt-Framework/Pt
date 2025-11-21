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

#include "RasterSurface.h"
#include "FontManager.h"

#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Gfx {

Bitmap::Bitmap()
: _rasterSurface(0)
{
    _rasterSurface = new RasterSurface();
}


Bitmap::Bitmap(const Gfx::SizeF& size, std::size_t stride)
: _rasterSurface(0)
{
    _rasterSurface = new RasterSurface();

    reset(size, stride);
}


Bitmap::~Bitmap()
{
    delete _rasterSurface;
}


const Gfx::Image& Bitmap::image() const
{
    return _rasterSurface->image();
}


void Bitmap::reset(const Gfx::Image& image)
{
    _rasterSurface->reset(image);
    releaseContext();
}


void Bitmap::reset(const Gfx::SizeF& sizeF, std::size_t stride)
{
    _rasterSurface->reset(sizeF, stride);
    releaseContext();
}


const SizeF& Bitmap::physicalSize() const
{
    return _rasterSurface->physicalSize();
}


const SizeF& Bitmap::logicalSize() const
{
    return _rasterSurface->logicalSize();
}


void Bitmap::setScaleFactor(double scaleFactor)
{
    _rasterSurface->setScaleFactor(scaleFactor);
    releaseContext();
}


void Bitmap::drawBitmap(const Pt::Gfx::PointF& to, const Bitmap& bitmap,
                        const Gfx::Paint& paint, const Gfx::RectF* rect)
{
    _rasterSurface->drawBitmap(to, bitmap, paint, rect);
}


const Gfx::ImageFormat& Bitmap::onGetFormat() const
{
    return _rasterSurface->format();
}


const Gfx::SizeF& Bitmap::onGetSize() const
{
    return _rasterSurface->size();
}


const Scaling& Bitmap::onGetScaling() const
{
    return _rasterSurface->scaling();
}


Gfx::PaintContext* Bitmap::onCreateContext(Gfx::PaintContext* context)
{
    return _rasterSurface->createContext(context);
}


void Bitmap::onReleaseContext()
{
    _rasterSurface->releaseContext();
}


void Bitmap::onFinish()
{
    _rasterSurface->finish();
}


void Bitmap::setFontDir(const Pt::System::Path& path)
{
    FreeType::instance().setFontDir(path);
}


const std::string& Bitmap::defaultFont()
{
    return  FreeType::instance().defaultFont();
}


void Bitmap::setDefaultFont(const std::string& f)
{
    FreeType::instance().setDefaultFont(f);
}


std::vector<std::string> Bitmap::fontNames()
{
    return FreeType::instance().fontNames();
}

} // namespace

} // namespace
