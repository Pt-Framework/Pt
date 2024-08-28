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

ImagePaint::ImagePaint(ImageCanvas& canvas)
: _canvas(&canvas)
, _invalid(false)
{
    setCanvas(canvas);
}


ImagePaint::~ImagePaint()
{
}


void ImagePaint::reset(ImageCanvas& canvas)
{
    _canvas = &canvas;
    setCanvas(canvas);
}


void ImagePaint::onSetPainter(Gfx::Painter& painter)
{
    _invalid = true;
}


void ImagePaint::onReleasePainter(Gfx::Painter& painter)
{
}


void ImagePaint::onSetCanvas(Gfx::Canvas& canvas)
{
   const Gfx::Scaling& scaling = canvas.scaling();
    if(_scaling != scaling)
    {
        _scaling = scaling;
        _invalid = true;
    }
}


void ImagePaint::onReleaseCanvas(Gfx::Canvas& canvas)
{
    _canvas = 0;
}


void ImagePaint::onBeginPaint(const Gfx::Paint& paint)
{
    if(_invalid)
    {
        updatePen( paint.pen() );

        _invalid = false;
    }

    if(_canvas)
    {
        _canvas->setCompositionMode( paint.compositionMode() );
        _canvas->setPen(*this);
        _canvas->setBrush( paint.brush() );
        _canvas->setFont( paint.font() );

        const Gfx::RectF& clip = paint.clip();
        if( clip.isNull() )
            _canvas->resetClip();
        else
            _canvas->setClip(clip);
    }
}


void ImagePaint::onFinishPaint()
{
}


void ImagePaint::onSetCompositionMode(const Gfx::CompositionMode& mode)
{
    if(_canvas)
        _canvas->setCompositionMode(mode);
}


void ImagePaint::onSetPen(const Gfx::Pen& pen)
{
    updatePen(pen);

    if(_canvas)
        _canvas->setPen(*this);
}


void ImagePaint::updatePen(const Gfx::Pen& pen)
{
    double scaledSize = _canvas ? _canvas->scaling().toPhysical( pen.size() )
                                : 1.0;

    // keep pen size when downscaling
    size_t penSize = scaledSize < 1.0 ? 1 
                                : static_cast<size_t>(scaledSize);

    _pen = pen;
    _pen.setSize(penSize);
}


void ImagePaint::onSetBrush(const Gfx::Brush& brush)
{
    if(_canvas)
        _canvas->setBrush(brush);
}


void ImagePaint::onSetFont(const Gfx::Font& font)
{
    if(_canvas)
        _canvas->setFont(font);
}


void ImagePaint::onSetClip(const Gfx::RectF& rectF)
{
    if(_canvas)
        _canvas->setClip(rectF);
}


void ImagePaint::onResetClip()
{
    if(_canvas)
        _canvas->resetClip();
}


void ImagePaint::onDrawLine(const Gfx::Line& line)
{
    if(_canvas)
    {
        _canvas->drawLine(line);
    }
}


void ImagePaint::onDrawPolyline(const Gfx::Polyline& line)
{
    if(_canvas)
    {
        _canvas->drawPolyline(line);
    }
}


void ImagePaint::onFillPolygon(const Gfx::Polyline& line)
{
    if(_canvas)
    {
        _canvas->fillPolygon(line);
    }
}



void ImagePaint::onDrawRect(const Gfx::RectF& rect)
{
    if(_canvas)
    {
        _canvas->drawRect(rect);
    }
}


void ImagePaint::onFillRect(const Gfx::RectF& rect)
{
    if(_canvas)
    {
        _canvas->fillRect(rect);
    }
}


void ImagePaint::onDrawEllipse(const Gfx::PointF& topLeft, 
                               const Gfx::SizeF& size)
{
    if(_canvas)
    {
        _canvas->drawEllipse(topLeft, size);
    }
}


void ImagePaint::onFillEllipse(const Gfx::PointF& topLeft, 
                               const Gfx::SizeF& size)
{
    if(_canvas)
    {
        _canvas->fillEllipse(topLeft, size);
    }
}


Gfx::FontMetrics ImagePaint::onGetFontMetrics(const Pt::String& text) const
{
    if(_canvas)
    {
        return _canvas->fontMetrics(text);
    }

    return Gfx::FontMetrics();
}


void ImagePaint::onDrawText(const Gfx::PointF& to, const Pt::String& text)
{
    if(_canvas)
    {
        _canvas->drawText(to, text);
    }
}


void ImagePaint::onDrawText(const Gfx::PointF& to, const Pt::String& text, 
                           const Gfx::Transform& tf)
{
    if(_canvas)
    {
        _canvas->drawText(to, text, tf);
    }
}


void ImagePaint::onDrawImage(const Gfx::PointF& to, 
                            const Gfx::Image& image)
{
    if(_canvas)
    {
        _canvas->drawImage(to, image);
    }
}


void ImagePaint::onDrawImage(const Gfx::PointF& to, 
                            const Gfx::Image& image, 
                            const Gfx::RectF& rect)
{
    if(_canvas)
    {
        _canvas->drawImage(to, image, rect);
    }
}


void ImagePaint::onDrawSurface(const Gfx::PointF& to, 
                               const Gfx::PaintSurface& surface)
{
    if( ! _canvas)
        return;

    _canvas->drawSurface(to, surface);
}


void ImagePaint::onDrawSurface(const Gfx::PointF& to,
                               const Gfx::PaintSurface& surface,
                               const Gfx::RectF& rect)
{
    if( ! _canvas)
        return;

    _canvas->drawSurface(to, surface, rect);
}

///////////////////////////////////////////////////////////////////////
// ImageCanvas
///////////////////////////////////////////////////////////////////////

ImageCanvas::ImageCanvas(PaintSurface& surface)
: Canvas(surface)
, _rasterizer(new Rasterizer)
{
}


ImageCanvas::ImageCanvas(PaintSurface& surface,
                         const Gfx::Size& size, std::size_t stride)
: Canvas(surface)
, _rasterizer(new Rasterizer)
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


const Scaling& ImageCanvas::scaling() const
{
    return _scaling;
}


PaintContext* ImageCanvas::onBeginPaint(Gfx::PaintContext* context)
{
    ImagePaint* paintContext = dynamic_cast<ImagePaint*>(context);
    if( ! paintContext )
        paintContext = new ImagePaint(*this);

    paintContext->reset(*this);
    return paintContext;
}


void ImageCanvas::onFinishPaint()
{
    //_rasterizer->finish();
}


void ImageCanvas::setCompositionMode(const CompositionMode& mode)
{
  _rasterizer->setCompositionMode(mode);
}


void ImageCanvas::setClip(const RectF& rect)
{
  RectF clip = _scaling.toPhysical(rect);
  _rasterizer->setClip(clip);
}


void ImageCanvas::resetClip()
{
  _rasterizer->resetClip();
}


void ImageCanvas::setPen(const ImagePaint& paint)
{
    const Pen& p = paint.pen();

    _rasterizer->setPen(p);
}


//void ImageCanvas::setPen(const Pen& pen)
//{
//    const Pen& p = _paint->pen();
//
//  //double scaleFactor = _scaling.scaleFactor();
//
//  //// keep pen size when downscaling
//  //double scaledSize = scaleFactor < 1.0 ? pen.size()
//  //                                : scaleFactor * pen.size();
//
//  //size_t penSize = static_cast<size_t>(scaledSize);
//
//  //Gfx::Pen scaledPen = pen;
//  //scaledPen.setSize(penSize);
//
//  _rasterizer->setPen(p) ;
//}


void ImageCanvas::setBrush(const Brush& brush)
{
  _rasterizer->setBrush(brush);
}


void ImageCanvas::setFont(const Font& font)
{
    _rasterizer->setFont(font);
}


FontMetrics ImageCanvas::fontMetrics(const String& text) const
{
	return _rasterizer->fontMetrics(text);
}


void ImageCanvas::drawLine(const PointF& from, const  PointF& to)
{
    _rasterizer->drawLine( _scaling.toPhysical(from), 
                           _scaling.toPhysical(to) );
}


void ImageCanvas::drawLine(const Gfx::Line& line)
{
    _rasterizer->drawLine( _scaling.toPhysical( line.from() ), 
                           _scaling.toPhysical( line.to() ) );
}


void ImageCanvas::drawText(const PointF& to, const String& text)
{
    double scaleFactor = _scaling.scaleFactor();

    Gfx::Transform trans;
    trans.scale(scaleFactor, scaleFactor);

    _rasterizer->drawText(_scaling.toPhysical(to), text, trans);
}


void ImageCanvas::drawText(const PointF& to, const Pt::String& text, 
                            const Transform& t)
{
    double scaleFactor = _scaling.scaleFactor();

    Gfx::Transform trans = t;
    trans.scale(scaleFactor, scaleFactor);

    _rasterizer->drawText(_scaling.toPhysical(to), text, trans);
}


void ImageCanvas::drawRect(const RectF& r)
{
    _rasterizer->drawRect( _scaling.toPhysical(r) );
}


void ImageCanvas::fillRect(const RectF& r)
{
    _rasterizer->fillRect( _scaling.toPhysical(r) );
}


void ImageCanvas::drawEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->drawEllipse( _scaling.toPhysical(topLeft), 
                              _scaling.toPhysical(size) );
}


void ImageCanvas::fillEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->fillEllipse( _scaling.toPhysical(topLeft), 
                              _scaling.toPhysical(size) );
}


void ImageCanvas::drawPolyline(const PointF* points, const size_t n)
{
    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < n; ++i)
        ps.push_back( _scaling.toPhysical(points[i]) );

    _rasterizer->drawPolyline(&ps[0], n);
}


void ImageCanvas::fillPolygon(const PointF* points, const size_t n)
{
    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < n; ++i)
        ps.push_back( _scaling.toPhysical(points[i]) );

    _rasterizer->fillPolygon(&ps[0], n);
}


void ImageCanvas::drawPolyline(const Gfx::Polyline& line)
{
    std::size_t n = line.size();

    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < n; ++i)
        ps.push_back( _scaling.toPhysical( line.at(i) ) );

    _rasterizer->drawPolyline(&ps[0], n);
}


void ImageCanvas::fillPolygon(const Gfx::Polyline& line)
{
    std::size_t n = line.size();

    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < n; ++i)
        ps.push_back( _scaling.toPhysical( line.at(i) ) );

    _rasterizer->fillPolygon(&ps[0], n);
}


void ImageCanvas::drawImage(const PointF& to, const Image& image)
{
    _rasterizer->drawImage( to, image);
}


void ImageCanvas::drawImage(const PointF& to, const Image& image, const RectF& imageRect)
{
    _rasterizer->drawImage(to, image, imageRect);
}


void ImageCanvas::drawPath(const Gfx::Path& path, float smoothness)
{
}


void ImageCanvas::fillPath(const Path& path, float smoothness)
{
}


void ImageCanvas::drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void ImageCanvas::fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void ImageCanvas::drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void ImageCanvas::fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void ImageCanvas::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void ImageCanvas::drawSurface(const Gfx::PointF& toF, const PaintSurface& surface)
{
    Gfx::PointF to = _scaling.toPhysical(toF);

    const Canvas* canvas = surface.canvas();
    const ImageCanvas* imageCanvas = dynamic_cast<const ImageCanvas*>(canvas);
    if(imageCanvas)
    {
        const Gfx::Image& image = imageCanvas->image();
        drawImage(to, image);
        return;
    }

    //const ImageSurface* imageSurface = dynamic_cast<const ImageSurface*>(&surface);
    //if(imageSurface)
    //{
    //    const Gfx::Image& image = imageSurface->image();
    //    drawImage(to, image);
    //    return;
    //}

    Pt::Gfx::Image image = surface.toImage();
    if( image.format() == format() )
    {
        drawImage(to, image);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest);
}


void ImageCanvas::drawSurface(const Gfx::PointF& toF, 
                               const PaintSurface& surface, const Gfx::RectF& pmRect2)
{
    Gfx::PointF to = _scaling.toPhysical(toF);
    Gfx::RectF rect = _scaling.toPhysical(pmRect2);

    const Canvas* canvas = surface.canvas();
    const ImageCanvas* imageCanvas = dynamic_cast<const ImageCanvas*>(canvas);
    if(imageCanvas)
    {
        const Gfx::Image& image = imageCanvas->image();
        drawImage(to, image, rect);
        return;
    }

    //const ImageSurface* imageSurface = dynamic_cast<const ImageSurface*>(&surface);
    //if(imageSurface)
    //{
    //    const Gfx::Image& image = imageSurface->image();
    //    drawImage(to, image, rect);
    //    return;
    //}

    Pt::Gfx::Image image = surface.toImage();
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
, _owner(0)
{
    _canvas = new ImageCanvas(*this);
    _owner = _canvas;
}


ImageSurface::ImageSurface(ImageCanvas& canvas)
: _canvas(0)
, _owner(0)
{
    _canvas = &canvas;
}


ImageSurface::ImageSurface(const Gfx::Size& size, std::size_t stride)
: _canvas(0)
, _owner(0)
{
    _canvas = new ImageCanvas(*this, size, stride);
    _owner = _canvas;
}


ImageSurface::~ImageSurface()
{
    delete _owner;
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


PaintContextPtr ImageSurface::onBeginPaint(Gfx::PaintContext* context)
{
    return _canvas->beginPaint(context);
}


Image ImageSurface::onGetImage() const
{
    return _canvas->image();
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
