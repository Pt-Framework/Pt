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

#include "ImageCanvas.h"
#include "SkiaContext.h"
#include "FreeType.h"
#include "DrawText.h"
#include "SkiaBlitter.h"

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Math.h>
#include <algorithm>
#include <cmath>

namespace Pt {

namespace Gfx {

ImageCanvas::ImageCanvas(PaintSurface& surface)
: Canvas(surface)
, _image( ImageFormat::argb32() )
, _paint(0)
, _canvas(0)
, _text( new DrawText() )
, _compositionMode(CompositionMode::SourceCopy)
{
    init(_image);
}


ImageCanvas::~ImageCanvas()
{
    delete _text;
}


const Image& ImageCanvas::image() const
{
    return _image;
}


void ImageCanvas::reset(const Gfx::Image& image)
{
    if( image.format() != _image.format() )
    {
        _image.reset( image.width(), image.height(), format() );
        Pt::Gfx::copy( image.begin(), image.end(), _image.begin() );
    }
    else
    {
        _image = image;
    }

    _physicalSize.set( image.width(), image.height() );
    _logicalSize = _scaling.toLogical( Gfx::SizeF( image.width(), 
                                                   image.height() ) );

    init(_image);
}


void ImageCanvas::reset(const Gfx::SizeF& sizeF, std::size_t stride)
{
    long width = lround( sizeF.width() );
    long height = lround( sizeF.height() );

    _image.reset( width, height, stride, _image.format() );

    _physicalSize.set(width, height);
    _logicalSize = _scaling.toLogical( Gfx::SizeF(width, height) );

    init(_image);
}


void ImageCanvas::init(Image& image)
{
    if( image.empty() )
    {
        _surface = 0;
        _canvas = 0;
        return;
    }

    SkImageInfo info = SkImageInfo::MakeCustom(image.width(), image.height(), 
                                               kPremul_SkAlphaType, SkCreateBlitter, 
                                               (void*) &image);
    _surface = SkSurface::MakeRaster(info);
    _canvas = _surface->getCanvas();

    _clip = Gfx::RectF( Gfx::PointF(0, 0), 
                        Gfx::SizeF(image.width(), 
                                   image.height()) );
}


void ImageCanvas::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);

    _physicalSize.set( _image.width(), _image.height() );
    _logicalSize = _scaling.toLogical( Gfx::SizeF( _image.width(), 
                                                   _image.height() ) );
}


const SizeF& ImageCanvas::physicalSize() const
{
    return _physicalSize;
}


const SizeF& ImageCanvas::logicalSize() const
{
    return _logicalSize;
}


const Gfx::ImageFormat& ImageCanvas::onGetFormat() const
{
    return Gfx::ImageFormat::argb32();
}


const Gfx::SizeF& ImageCanvas::onGetSize() const
{
    return _logicalSize;
}


const Scaling& ImageCanvas::onGetScaling() const
{
    return _scaling;
}


bool ImageCanvas::onSetPaint(Gfx::PaintContext* context)
{
    SkiaContext* paintContext = dynamic_cast<SkiaContext*>(context);
    if( ! paintContext )
        return false;

    _paint = paintContext;
    return true;
}


Gfx::PaintContext* ImageCanvas::onCreatePaint()
{
    SkiaContext* paintContext = new SkiaContext();
    
    _paint = paintContext;
    return paintContext;
}


void ImageCanvas::onReleasePaint()
{
    _paint = 0;

    _clip = RectF( _image.width(), _image.height() );
}


void ImageCanvas::onCompositionModeChanged()
{
    if( ! _paint )
        return;

    const Gfx::CompositionMode& mode = _paint->compositionMode();
    _compositionMode = mode;
}


void ImageCanvas::onPenChanged()
{
    if( ! _paint )
        return;

    _text->setPen( _paint->pen() );
}


void ImageCanvas::onBrushChanged()
{
    if( ! _paint )
        return;
}


void ImageCanvas::onFontChanged()
{
    if( ! _paint )
        return;

    const Font& font = _paint->font();
    _text->setFont(font);
}


void ImageCanvas::onClipChanged()
{
    if( ! _paint )
        return;

    const RectF* clip = _paint->clip();
    if( ! clip )
    {
        _clip = RectF( _image.width(),_image.height() );
    }
    else
    {
        _clip = scaling().toPhysical(*clip);       
    }
}


void ImageCanvas::updateClip() const
{
    RectF imageRect( _image.width(),_image.height() );
    RectF clip = _clip.intersect(imageRect);

    if(_canvas)
    {
        SkRect skClip = SkRect::MakeLTRB( clip.left(), clip.top(), 
                                          clip.right(), clip.bottom() );
        _canvas->clipRect(skClip, SkClipOp::kMax_EnumValue);
    }
}


void ImageCanvas::onDrawLine(const PointF& from, const  PointF& to)
{
    if( ! _canvas || ! _paint )
        return;

    SkPoint p1 = SkPoint::Make( from.x(), from.y() );
    SkPoint p2 = SkPoint::Make( to.x(), to.y() );

    updateClip();
    _canvas->drawLine( p1, p2, _paint->skPen() );
}


void ImageCanvas::onDrawPolyline(const Gfx::Polyline& line)
{
    if( ! _canvas || line.size() == 0 )
        return;
    
    SkPath path;

    Gfx::PointF p0 = line.at(0);
    path.moveTo( SkPoint::Make(p0.x(), p0.y()) );

    std::size_t n = line.size();
    for(size_t i = 1; i < n; ++i)
    {
        Gfx::PointF p = line.at(i);
        path.lineTo( SkPoint::Make(p.x(), p.y()) );
    }

    Gfx::PointF last = line.at(n - 1);

    bool isClosed = std::abs( p0.x() - last.x() ) < 0.1 &&
                    std::abs( p0.y() - last.y() ) < 0.1;
    if(isClosed)
        path.close();

    updateClip();
    _canvas->drawPath( path, _paint->skPen() );
}


void ImageCanvas::onFillPolygon(const Gfx::Polyline& line)
{
    if( ! _canvas || line.size() == 0 )
        return;

    SkPath path;

    Gfx::PointF p0 = line.at(0);
    path.moveTo( SkPoint::Make(p0.x(), p0.y()) );

    std::size_t n = line.size();
    for(size_t i = 1; i < n; ++i)
    {
        Gfx::PointF p = line.at(i);
        path.lineTo( SkPoint::Make(p.x(), p.y()) );
    }

    path.close();

    updateClip();
    _canvas->drawPath( path, _paint->brush() );
}


void ImageCanvas::onDrawRect(const RectF& r)
{
    if( ! _canvas )
        return;

    SkRect sr = SkRect::MakeLTRB( r.left(), r.top(), r.right(), r.bottom() );

    updateClip();
    _canvas->drawRect( sr, _paint->skPen() );
}


void ImageCanvas::onFillRect(const RectF& r)
{
    if( ! _canvas )
        return;

    SkRect sr = SkRect::MakeLTRB( r.left(), r.top(), r.right(), r.bottom() );

    updateClip();
    _canvas->drawRect( sr, _paint->brush() );
}


void ImageCanvas::onDrawEllipse(const PointF& topLeftF, const SizeF& sizeF)
{
    if (!_canvas)
        return;

    SkRect sr = SkRect::MakeXYWH( topLeftF.x(), topLeftF.y(),
                                  sizeF.width(), sizeF.height() );

    updateClip();
    _canvas->drawOval( sr, _paint->skPen() );
}


void ImageCanvas::onFillEllipse(const PointF& topLeftF, const SizeF& sizeF)
{
    if (!_canvas)
        return;

    SkRect sr = SkRect::MakeXYWH( topLeftF.x(), topLeftF.y(),
                                  sizeF.width(), sizeF.height() );

    updateClip();
    _canvas->drawOval( sr, _paint->brush() );
}


TextMetrics ImageCanvas::onGetTextMetrics(const String& text) const
{
    return _text->textMetrics(text);
}


void ImageCanvas::onDrawText(const PointF& toF, const Pt::String& text, 
                              const Transform* xform)
{
    if( ! _canvas )
        return;

    Point to = round(toF);

    RectF imageRect( _image.width(),_image.height() );
    RectF clipF = _clip.intersect(imageRect);

    Point pos( Pt::lround(clipF.x()),
               Pt::lround(clipF.y()) );
      
    Size size( Pt::lround(clipF.width()),
               Pt::lround(clipF.height()) );
      
    Rect clip(pos, size);
    _text->setClip(clip);

    if(xform)
        _text->draw(_image, to.x(), to.y(), text, _compositionMode, *xform);
    else
        _text->draw( _image, to.x(), to.y(), text, _compositionMode );
}


void ImageCanvas::onDrawImage(const PointF& toL, const Image& image, 
                              const RectF* imageRectF_)
{
    Gfx::PointF toF = scaling().toPhysical(toL);

    // round image rect
    RectF imageRectF( image.width(), image.height() );
    if(imageRectF_)
        imageRectF = *imageRectF_;

    Point imagePos( Pt::lround(imageRectF.x()),
                    Pt::lround(imageRectF.y()) );
    Size imageSize( Pt::lround(imageRectF.width()),
                    Pt::lround(imageRectF.height()) );
    Rect imageRect(imagePos, imageSize);

    // round clip rect
    RectF clipF = _clip.intersect( RectF(_image.width(),_image.height()) );

    Point clipPos( Pt::lround(clipF.x()),
                   Pt::lround(clipF.y()) );
    Size clipSize( Pt::lround(clipF.width()),
                   Pt::lround(clipF.height()) );
    Rect currentClip(clipPos, clipSize);

    // clip against source boundaries
    Rect fromRect( image.width(), image.height() );
    fromRect = fromRect.intersect(imageRect);

    // update target position if rect got smaller
    Point toPos = round(toF);
    toPos += fromRect.topLeft() - imageRect.topLeft();

    // clip against target boundaries
    Rect toRect = Rect( toPos, fromRect.size() );
    toRect = toRect.intersect(currentClip);

    // update source position if rect got smaller
    Point fromPos = fromRect.topLeft();
    fromPos += toRect.topLeft() - toPos;
    fromRect.setOrigin(fromPos);

    // update source size if rect got smaller
    fromRect.setSize( toRect.size() );

    _image.format().copy(_image.view(), toRect.x(), toRect.y(),
                         image.view(), fromRect.x(), fromRect.y(), 
                         fromRect.width(), fromRect.height(), _compositionMode);
}


void ImageCanvas::onDrawPath(const Gfx::Path& path, float smoothness) 
{
    if( ! _canvas )
        return;

    updateClip();

    SkPath skPath = toSkia(path);
    _canvas->drawPath( skPath, _paint->skPen() );
}


void ImageCanvas::onFillPath(const Gfx::Path& path, float smoothness) 
{
    if( ! _canvas )
        return;

    updateClip();
    SkPath skPath = toSkia(path);
    _canvas->drawPath( skPath, _paint->brush() );
}


const std::string& ImageCanvas::defaultFont()
{
    return  FreeType::instance().defaultFont();
}


void ImageCanvas::setDefaultFont(const std::string& f)
{
    FreeType::instance().setDefaultFont(f);
}


std::vector<FontFace> ImageCanvas::fonts()
{
    return FreeType::instance().fonts();
}


SkPath ImageCanvas::toSkia(const Gfx::Path& p)
{
    SkPath skp;

    std::size_t s = p.size();

    for(std::size_t n = 0; n < s; n++)
    {
        const Gfx::Element& e = p.at(n);

        switch (e.type)
        {
        case Gfx::Element::IT_Close:
            skp.close();
            break;

        case Gfx::Element::IT_MoveTo:
        {
            double x = e.pxy.at(0);
            double y = e.pxy.at(1);
            skp.moveTo(x, y);
            break;
        }

        case Gfx::Element::IT_LineTo:
        {
            double x = e.pxy.at(0);
            double y = e.pxy.at(1);
            skp.lineTo(x, y);
            break;
        }

        case Gfx::Element::IT_QuadBezierTo:
        {
            double x1 = e.pxy.at(0);
            double y1 = e.pxy.at(1);
            double x2 = e.pxy.at(2);
            double y2 = e.pxy.at(3);
            skp.quadTo(x1, y1, x2, y2);
            break;
        }

        case Gfx::Element::IT_CubicBezierTo:
        {
            double x1 = e.pxy.at(0);
            double y1 = e.pxy.at(1);
            double x2 = e.pxy.at(2);
            double y2 = e.pxy.at(3);
            double x3 = e.pxy.at(4);
            double y3 = e.pxy.at(5);
            skp.cubicTo(x1, y1, x2, y2, x3, y3);
            break;
        }

        default:
            break;
        }
    }

    return skp;
}

} // namespace

} // namespace
