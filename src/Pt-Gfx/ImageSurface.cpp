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
#include <Pt/Gfx/Painter.h>
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
: PaintContext()
{
}


ImagePaint::~ImagePaint()
{
}


void ImagePaint::onSetCompositionMode(const Gfx::CompositionMode& mode)
{
}


void ImagePaint::onSetPen(const Gfx::Pen& pen)
{
    double scaledSize = scaling().toPhysical( pen.size() );

    // keep pen size when downscaling
    size_t penSize = scaledSize < 1.0 ? 1 
                                : static_cast<size_t>(scaledSize);

    _pen = pen;
    _pen.setSize(penSize);
}


void ImagePaint::onSetBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


void ImagePaint::onSetFont(const Gfx::Font& font)
{
    _font = font;
}

///////////////////////////////////////////////////////////////////////
// ImageCanvas
///////////////////////////////////////////////////////////////////////

ImageCanvas::ImageCanvas(PaintSurface& surface)
: Canvas(surface)
, _rasterizer(new Rasterizer)
, _paint(0)
{
}


ImageCanvas::ImageCanvas(PaintSurface& surface,
                         const Gfx::Size& size, std::size_t stride)
: Canvas(surface)
, _rasterizer(new Rasterizer)
, _paint(0)
{
  _rasterizer->reset(size, stride);
}


ImageCanvas::~ImageCanvas()
{
  delete _rasterizer;
}


void ImageCanvas::reset(const Gfx::Image& image)
{
    _size = _scaling.toLogical( SizeF( image.width(), 
                                       image.height() ) );
    
    _rasterizer->reset(image);
}


void ImageCanvas::reset(const Gfx::Size& size, std::size_t stride)
{
    _size = _scaling.toLogical( SizeF( size.width(), 
                                       size.height() ) );

    _rasterizer->reset(size, stride);
}


const Gfx::Image& ImageCanvas::image() const
{
    return _rasterizer->image();
}


void ImageCanvas::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
}


void ImageCanvas::resize(const Gfx::SizeF& size)
{
    Gfx::SizeF physicalSize = _scaling.toPhysical(size);
    Gfx::Size s = round(physicalSize);
            
    reset(s);
}


const Gfx::ImageFormat& ImageCanvas::format() const
{
    return ImageFormat::argb32();
}


const Gfx::SizeF& ImageCanvas::size() const
{
    return _size;
}


const Gfx::Scaling& ImageCanvas::onGetScaling() const
{
    return _scaling;
}


bool ImageCanvas::onBeginPaint(Gfx::PaintContext* context)
{
    ImagePaint* paintContext = dynamic_cast<ImagePaint*>(context);
    if( ! paintContext )
        return false;

    _paint = paintContext;

    setCompositionMode( paintContext->compositionMode() );
    setPen( paintContext->pen() );
    setBrush( paintContext->brush() );
    setFont( paintContext->font() );

    _paint = paintContext;
    return true;
}


Gfx::PaintContext* ImageCanvas::onBeginPaint()
{
    ImagePaint* paintContext = new ImagePaint();
    
    _paint = paintContext;
    return paintContext;
}


void ImageCanvas::onReleasePaint()
{
    //_rasterizer->finish();
    _paint = 0;
}


void ImageCanvas::onSetCompositionMode(const CompositionMode& mode)
{
  _rasterizer->setCompositionMode(mode);
}


void ImageCanvas::onSetPen(const Pen& pen)
{
    if( ! _paint )
        return;

    const Pen& p = _paint->pen();
    _rasterizer->setPen(p);
}


void ImageCanvas::onSetBrush(const Brush& brush)
{
    _rasterizer->setBrush(brush);
}


void ImageCanvas::onSetFont(const Font& font)
{
    _rasterizer->setFont(font);
}


void ImageCanvas::onSetClip(const RectF& rect)
{
    RectF clip = _scaling.toPhysical(rect);
    _rasterizer->setClip(clip);
}


void ImageCanvas::onResetClip()
{
    _rasterizer->resetClip();
}


void ImageCanvas::onDrawLine(const PointF& from, const  PointF& to)
{
    _rasterizer->drawLine(from, to);
}


void ImageCanvas::onDrawPolyline(const Gfx::Polyline& line)
{
    std::size_t n = line.size();

    std::vector<Gfx::PointF> ps;

    for(size_t i = 0; i < n; ++i)
        ps.push_back( line.at(i) );

    _rasterizer->drawPolyline(&ps[0], n);
}


void ImageCanvas::onFillPolygon(const Gfx::Polyline& line)
{
    std::size_t n = line.size();

    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < n; ++i)
        ps.push_back( line.at(i) );

    _rasterizer->fillPolygon(&ps[0], n);
}


void ImageCanvas::onDrawRect(const RectF& r)
{
    _rasterizer->drawRect(r);
}


void ImageCanvas::onFillRect(const RectF& r)
{
    _rasterizer->fillRect(r);
}


void ImageCanvas::onDrawEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->drawEllipse(topLeft, size);
}


void ImageCanvas::onFillEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->fillEllipse(topLeft, size);
}


FontMetrics ImageCanvas::onGetFontMetrics(const String& text) const
{
	return _rasterizer->fontMetrics(text);
}


void ImageCanvas::onDrawText(const PointF& to, const Pt::String& text, 
                            const Transform* xform)
{
    if(xform)
        _rasterizer->drawText(to, text, *xform);
    else
        _rasterizer->drawText(to, text);
}


Gfx::Image ImageCanvas::onGetImage() const
{
    return image();
}


void ImageCanvas::onDrawImage(const PointF& to, const Image& image)
{
    _rasterizer->drawImage( to, image);
}


void ImageCanvas::onDrawImage(const PointF& to, const Image& image, const RectF& imageRect)
{
    _rasterizer->drawImage(to, image, imageRect);
}


void ImageCanvas::onDrawCanvas(const Gfx::PointF& toF, 
                               const Gfx::Canvas& canvas)
{
    Gfx::PointF to = _scaling.toPhysical(toF);

    const ImageCanvas* imageCanvas = dynamic_cast<const ImageCanvas*>(&canvas);
    if(imageCanvas)
    {
        const Gfx::Image& image = imageCanvas->image();
        drawImage(to, image);
        return;
    }

    Pt::Gfx::Image image = canvas.toImage();
    if( image.format() == format() )
    {
        drawImage(to, image);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest);
}


void ImageCanvas::onDrawCanvas(const Gfx::PointF& toF,
                               const Gfx::Canvas& canvas,
                               const Gfx::RectF& rectF)
{
    Gfx::PointF to = _scaling.toPhysical(toF);
    Gfx::RectF rect = _scaling.toPhysical(rectF);

    const ImageCanvas* imageCanvas = dynamic_cast<const ImageCanvas*>(&canvas);
    if(imageCanvas)
    {
        const Gfx::Image& image = imageCanvas->image();
        drawImage(to, image, rect);
        return;
    }

    Pt::Gfx::Image image = canvas.toImage();
    if( image.format() == format() )
    {
        drawImage(to, image, rect);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest, rect);
}

///////////////////////////////////////////////////////////////////////
// ImageSurface
///////////////////////////////////////////////////////////////////////

ImageSurface::ImageSurface()
: _canvas(0)
{
    _canvas = new ImageCanvas(*this);
    //setCanvas(_canvas);
}


ImageSurface::ImageSurface(const Gfx::Size& size, std::size_t stride)
: _canvas(0)
{
    _canvas = new ImageCanvas(*this, size, stride);
    //setCanvas(_canvas);
}


ImageSurface::~ImageSurface()
{
    delete _canvas;
}


const Gfx::Image& ImageSurface::image() const
{
    return _canvas->image();
}


void ImageSurface::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
    _canvas->setScaleFactor(scaleFactor);
}


void ImageSurface::resize(const Gfx::SizeF& size)
{
    _canvas->resize(size);
    _size = _canvas->size();
}


void ImageSurface::reset(const Gfx::Image& image)
{
    _canvas->reset(image);
    _size = _canvas->size();
}


void ImageSurface::reset(const Gfx::Size& size, std::size_t stride)
{
    _canvas->reset(size, stride);
    _size = _canvas->size();
}


const Canvas* ImageSurface::onGetCanvas() const
{
    return _canvas;
}


Gfx::PaintContext* ImageSurface::onBeginPaint(Gfx::PaintContext* reuse) 
{
    return _canvas->beginPaint(reuse);
}


const ImageFormat& ImageSurface::onGetFormat() const
{
    return ImageFormat::argb32();
}


const Gfx::SizeF& ImageSurface::onGetSize() const
{
    return _size;
}


const Scaling& ImageSurface::onGetScaling() const
{
    return _scaling;
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

} // namespace

} // namespace
