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

#include "Rasterizer.h"

#include <Pt/Gfx/ImageSurface.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/System/Clock.h>
#include <Pt/String.h>
#include <Pt/Math.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// ImageSurface
///////////////////////////////////////////////////////////////////////

ImagePaint::ImagePaint()
{
}


ImagePaint::~ImagePaint()
{
}
  

void ImagePaint::onFinish()
{
}


void ImagePaint::onSetCompositionMode(const Gfx::CompositionMode& mode)
{
}


void ImagePaint::onSetPen(const Gfx::Pen& pen)
{
    Gfx::Canvas* canvas = this->canvas();

    double scaledSize = canvas ? canvas->scaling().toPhysical( pen.size() )
                               : 1.0;

    // keep pen size when downscaling
    size_t penSize = scaledSize < 1.0 ? 1 
                                : static_cast<size_t>(scaledSize);

    _pen = pen;
    _pen.setSize(penSize);
}


void ImagePaint::onSetBrush(const Gfx::Brush& brush)
{
}


void ImagePaint::onSetFont(const Gfx::Font& font)
{
}


void ImagePaint::onSetClip(const Gfx::RectF& rectF)
{
}


void ImagePaint::onResetClip()
{
}

///////////////////////////////////////////////////////////////////////
// ImageSurface
///////////////////////////////////////////////////////////////////////

ImageSurface::ImageSurface()
: _rasterizer(new Rasterizer)
, _paint(0)
{
}


ImageSurface::ImageSurface(const Gfx::Size& size, std::size_t stride)
: _rasterizer(new Rasterizer)
, _paint(0)
{
  _rasterizer->reset(size, stride);
}


ImageSurface::~ImageSurface()
{
  delete _rasterizer;
}


const Gfx::Image& ImageSurface::image() const
{
    return _rasterizer->image();
}


void ImageSurface::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
}


double ImageSurface::onScaleFactor() const
{
    return _scaling.scaleFactor();
}


const Gfx::SizeF& ImageSurface::onSize() const
{
    return _size;
}


void ImageSurface::resize(const Gfx::SizeF& size)
{
    Gfx::SizeF physicalSize = _scaling.toPhysical(size);
    Gfx::Size s = round(physicalSize);
            
    reset(s);
}


void ImageSurface::reset(const Gfx::Image& image)
{
    _size = _scaling.toLogical( SizeF( image.width(), 
                                       image.height() ) );

    _rasterizer->reset(image);
}


void ImageSurface::reset(const Gfx::Size& size, std::size_t stride)
{
    _size = _scaling.toLogical( SizeF( size.width(), 
                                       size.height() ) );

    _rasterizer->reset(size, stride);
}


Gfx::PaintData* ImageSurface::onGetPaint(Gfx::PaintData* p)
{
    ImagePaint* paint = dynamic_cast<ImagePaint*>(p);
    if( ! paint )
      paint = new ImagePaint();

    _paint = paint;
    return _paint;

    //return _rasterizer->begin(p);
}


Gfx::Canvas* ImageSurface::onGetCanvas()
{
    return this;
}


void ImageSurface::onFinish()
{
    //_rasterizer->finish();

    _paint = 0;
}


const ImageFormat& ImageSurface::onGetFormat() const
{
    return _rasterizer->format();
}


void ImageSurface::setCompositionMode(const CompositionMode& mode)
{
  _rasterizer->setCompositionMode(mode);
}


void ImageSurface::setClip(const RectF& rect)
{
  RectF clip = _scaling.toPhysical(rect);
  _rasterizer->setClip(clip);
}


void ImageSurface::resetClip()
{
  _rasterizer->resetClip();
}


void ImageSurface::setPen(const Pen& pen)
{
    const Pen& p = _paint->pen();

  //double scaleFactor = _scaling.scaleFactor();

  //// keep pen size when downscaling
  //double scaledSize = scaleFactor < 1.0 ? pen.size()
  //                                : scaleFactor * pen.size();

  //size_t penSize = static_cast<size_t>(scaledSize);

  //Gfx::Pen scaledPen = pen;
  //scaledPen.setSize(penSize);

  _rasterizer->setPen(p) ;
}


void ImageSurface::setBrush(const Brush& brush)
{
  _rasterizer->setBrush(brush);
}


void ImageSurface::setFont(const Font& font)
{
    _rasterizer->setFont(font);
}


FontMetrics ImageSurface::fontMetrics(const String& text) const
{
	return _rasterizer->fontMetrics(text);
}


void ImageSurface::drawLine(const PointF& from, const  PointF& to)
{
    _rasterizer->drawLine( _scaling.toPhysical(from), 
                           _scaling.toPhysical(to) );
}


void ImageSurface::drawLine(const Gfx::Line& line)
{
    _rasterizer->drawLine( _scaling.toPhysical( line.from() ), 
                           _scaling.toPhysical( line.to() ) );
}


void ImageSurface::drawText(const PointF& to, const String& text)
{
    double scaleFactor = _scaling.scaleFactor();

    Gfx::Transform trans;
    trans.scale(scaleFactor, scaleFactor);

    _rasterizer->drawText(_scaling.toPhysical(to), text, trans);
}


void ImageSurface::drawText(const PointF& to, const Pt::String& text, 
                            const Transform& t)
{
    double scaleFactor = _scaling.scaleFactor();

    Gfx::Transform trans = t;
    trans.scale(scaleFactor, scaleFactor);

    _rasterizer->drawText(_scaling.toPhysical(to), text, trans);
}


void ImageSurface::drawRect(const RectF& r)
{
    _rasterizer->drawRect( _scaling.toPhysical(r) );
}


void ImageSurface::fillRect(const RectF& r)
{
    _rasterizer->fillRect( _scaling.toPhysical(r) );
}


void ImageSurface::drawEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->drawEllipse( _scaling.toPhysical(topLeft), 
                              _scaling.toPhysical(size) );
}


void ImageSurface::fillEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->fillEllipse( _scaling.toPhysical(topLeft), 
                              _scaling.toPhysical(size) );
}


void ImageSurface::drawPolyline(const PointF* points, const size_t n)
{
    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < n; ++i)
        ps.push_back( _scaling.toPhysical(points[i]) );

    _rasterizer->drawPolyline(&ps[0], n);
}


void ImageSurface::fillPolygon(const PointF* points, const size_t n)
{
    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < n; ++i)
        ps.push_back( _scaling.toPhysical(points[i]) );

    _rasterizer->fillPolygon(&ps[0], n);
}


void ImageSurface::drawPolyline(const Gfx::Polyline& line)
{
    std::size_t n = line.size();

    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < n; ++i)
        ps.push_back( _scaling.toPhysical( line.at(i) ) );

    _rasterizer->drawPolyline(&ps[0], n);
}


void ImageSurface::fillPolygon(const Gfx::Polyline& line)
{
    std::size_t n = line.size();

    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < n; ++i)
        ps.push_back( _scaling.toPhysical( line.at(i) ) );

    _rasterizer->fillPolygon(&ps[0], n);
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


const std::string& ImageSurface::defaultFont()
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


void ImageSurface::drawSurface(const Gfx::PointF& toF2, const PaintSurface& surface)
{
    Gfx::PointF to = _scaling.toPhysical(toF2);

    const ImageSurface* isurface = dynamic_cast<const ImageSurface*>(&surface);
    if(isurface)
    {
        const Gfx::Image& image = isurface->image();
        drawImage(to, image);
        return;
    }

    Pt::Gfx::Image image = surface.toImage();
    if( image.format() == format() )
    {
        drawImage(toPhysical(to), image);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest);
}


void ImageSurface::drawSurface(const Gfx::PointF& toF2, 
                               const PaintSurface& surface, const Gfx::RectF& pmRect2)
{
    Gfx::PointF to = _scaling.toPhysical(toF2);
    Gfx::RectF rect = _scaling.toPhysical(pmRect2);

    const ImageSurface* isurface = dynamic_cast<const ImageSurface*>(&surface);
    if(isurface)
    {
        const Gfx::Image& image = isurface->image();
        drawImage(to, image, rect);
        return;
    }

    Pt::Gfx::Image image = surface.toImage();
    if( image.format() == format() )
    {
        drawImage(toPhysical(to), image, rect);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest, rect);
}


Image ImageSurface::toImage() const
{
    return _rasterizer->toImage();
}

} // namespace

} // namespace
