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

#include <Pt/Math.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/String.h>
#include "Pt/System/Clock.h"
#include "Rasterizer.h"


namespace Pt {

namespace Gfx {

ImageSurface::ImageSurface(Image& image)
: _rasterizer( new Rasterizer(image))
{
}


ImageSurface::~ImageSurface()
{
  delete _rasterizer;
}


const Gfx::Image& ImageSurface::image() const
{
    return _rasterizer->image();
}


void ImageSurface::setImage(Image& image)
{
    _rasterizer->setImage(image);
}


double ImageSurface::onScaleFactor() const
{
    return 1.0;
}


const Gfx::SizeF& ImageSurface::onSize() const
{
    //TODO: Remove mutable _size!
    _size = SizeF(_rasterizer->image().size().width(), _rasterizer->image().size().height());
    return _size;
}


void ImageSurface::onBegin(Painter& painter)
{
    _rasterizer->begin(painter);
}


void ImageSurface::onFinish()
{
    _rasterizer->finish();
}


const ImageFormat& ImageSurface::format() const
{
    return _rasterizer->format();
}


void ImageSurface::setCompositionMode(const CompositionMode& mode)
{
  _rasterizer->setCompositionMode(mode);
}


void ImageSurface::setClip( const RectF& clipIn )
{
  _rasterizer->setClip(clipIn);
}


void ImageSurface::resetClip()
{
  _rasterizer->resetClip();
}


void ImageSurface::setPen( const Pen& pen )
{
  _rasterizer->setPen( pen ) ;
}


void ImageSurface::setBrush(const Brush& brush)
{
  _rasterizer->setBrush( brush);
}


void ImageSurface::setFont(const Font& font)
{
    _rasterizer->setFont( font );
}


FontMetrics ImageSurface::fontMetrics(const String& text) const
{
	return _rasterizer->fontMetrics( text );
}


void ImageSurface::drawLine(const PointF& from, const  PointF& to)
{
    _rasterizer->drawLine(from, to);
}


void ImageSurface::drawText(const PointF& to, const String& text)
{
    _rasterizer->drawText(to, text);
}


void ImageSurface::drawText(const PointF& to, const Pt::String& text, 
                            const Transform& trans)
{
    _rasterizer->drawText(to, text, trans);
}


void ImageSurface::drawRect(const RectF& r)
{
    _rasterizer->drawRect(r);
}


void ImageSurface::fillRect(const RectF& r)
{
    _rasterizer->fillRect(r);
}


void ImageSurface::drawEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->drawEllipse(topLeft,  size);
}


void ImageSurface::fillEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->fillEllipse(topLeft, size);
}


void ImageSurface::drawPolyline(const PointF* ps, const size_t n)
{
  _rasterizer->drawPolyline( ps, n );
}


void ImageSurface::fillPolygon(const PointF* ps, const size_t n)
{
    _rasterizer->fillPolygon( ps, n );
}


void ImageSurface::drawImage(const PointF& to, const Image& image)
{
  _rasterizer->drawImage( to, image);
}


void ImageSurface::drawImage(const PointF& to, const Image& image, const RectF& imageRect)
{
  _rasterizer->drawImage(to, image, imageRect);
}


void ImageSurface::drawPath(const Gfx::Path& path, float smoothness)
{
}


void ImageSurface::fillPath(const Path& path, float smoothness)
{
}


void ImageSurface::drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void ImageSurface::fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void ImageSurface::drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void ImageSurface::fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void ImageSurface::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


FontMetrics ImageSurface::fontMetrics( const Font& font, const Pt::String& text )
{
  return Rasterizer::fontMetrics( font, text );
}


void ImageSurface::setFontDir(const Pt::System::Path& path)
{
    Rasterizer::setFontDir(path);
}


std::string ImageSurface::defaultFont()
{
    return Rasterizer::defaultFont();
}


void ImageSurface::setDefaultFont(const std::string& f)
{
    Rasterizer::setDefaultFont(f);
}


std::vector<std::string> ImageSurface::fontNames()
{
    return Rasterizer::fontNames();
}


void ImageSurface::drawSurface(const Gfx::PointF& toF, const PaintSurface& surface)
{
    const ImageSurface* isurface = dynamic_cast<const ImageSurface*>(&surface);
    if (isurface)
    {
        drawImage(toF, isurface->_rasterizer->image());
        return;
    }

    Image image = surface.toImage(format());
    drawImage(toF, image);
}


void ImageSurface::drawSurface(const Gfx::PointF& toF, const PaintSurface& surface, const Gfx::RectF& pmRect)
{
    const ImageSurface* isurface = dynamic_cast<const ImageSurface*>(&surface);
    if (isurface)
    {
        drawImage(toF, isurface->_rasterizer->image(), pmRect);
        return;
    }

    Image image = surface.toImage(format());
    drawImage(toF, image, pmRect);
}


Image ImageSurface::toImage(const Gfx::ImageFormat& format) const
{
    return _rasterizer->toImage(format);
}

} // namespace

} // namespace
