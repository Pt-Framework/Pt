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

#include "ImageCanvas.h"

#include <Pt/Gfx/ImageSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// ImageSurface
///////////////////////////////////////////////////////////////////////

ImageSurface::ImageSurface()
: _canvas(0)
{
    _canvas = new ImageCanvas(*this);
    setCanvas(_canvas);
}


ImageSurface::ImageSurface(const Gfx::SizeF& size, std::size_t stride)
: _canvas(0)
{
    _canvas = new ImageCanvas(*this);
    setCanvas(_canvas);

    reset(size, stride);
}


ImageSurface::~ImageSurface()
{
    delete _canvas;
}


void ImageSurface::reset(const Gfx::Image& image)
{
    _canvas->reset(image);
}


void ImageSurface::reset(const Gfx::SizeF& size, std::size_t stride)
{
    _canvas->reset(size, stride);
}


const Gfx::Image& ImageSurface::image() const
{
    return _canvas->image();
}


const Gfx::SizeF& ImageSurface::size() const
{
    return _canvas->physicalSize();
}


void ImageSurface::setScaleFactor(double scaleFactor)
{
    _canvas->setScaleFactor(scaleFactor);
}


void ImageSurface::setFontDir(const Pt::System::Path& path)
{
    ImageCanvas::setFontDir(path);
}


const std::string& ImageSurface::defaultFont()
{
    return ImageCanvas::defaultFont();
}


void ImageSurface::setDefaultFont(const std::string& f)
{
    ImageCanvas::setDefaultFont(f);
}


std::vector<std::string> ImageSurface::fontNames()
{
    return ImageCanvas::fontNames();
}

///////////////////////////////////////////////////////////////////////
// ImageLayer
///////////////////////////////////////////////////////////////////////

ImageLayer::ImageLayer()
{
    setSurface(&_surface);
}


ImageLayer::ImageLayer(const Gfx::SizeF& size, std::size_t stride)
: _surface(size, stride)
{
    setSurface(&_surface);
}


ImageLayer::~ImageLayer()
{
}


void ImageLayer::reset(const Gfx::Image& image)
{
    _surface.reset(image);
}


void ImageLayer::reset(const Gfx::SizeF& size, std::size_t stride)
{
    _surface.reset(size, stride);
}


const Gfx::Image& ImageLayer::image() const
{
    return _surface.image();
}


const Gfx::SizeF& ImageLayer::size() const
{
    return _surface.size();
}


void ImageLayer::setScaleFactor(double scaleFactor)
{
    _surface.setScaleFactor(scaleFactor);
}


void ImageLayer::onDraw(PaintSurface& surface,
                        const Paint& paint, 
                        const Gfx::PointF& to,
                        const Gfx::RectF* rect) const
{
    Gfx::Painter painter(surface);
    painter.setCompositionMode( paint.compositionMode() );
    
    const CanvasBase* canvas = _surface.canvas();
    if( ! canvas )
        return;

    const Gfx::Image& image = this->image();
    if(rect)
    {
        Gfx::RectF imageRect = canvas->scaling().toPhysical(*rect);
        painter.drawImage(to, image, imageRect);
    }
    else
    {
        painter.drawImage(to, image);
    }
}

} // namespace

} // namespace
