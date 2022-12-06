/* Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 1988, 1998  The Open Group, MIT X Consortium

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
#include "FreeType.h"
#include "DrawText.h"
#include "SkiaBlitter.h"

#include "PaintData.h"
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Math.h>
#include <algorithm>
#include <cmath>

namespace Pt {

namespace Gfx {

Rasterizer::Rasterizer(Image& image)
: _image(&image)
, _canvas(0)
, _text( new DrawText() )
, _paintData(0)
, _painter(0)
{
}


Rasterizer::~Rasterizer()
{
    delete _text;
}


void Rasterizer::setImage(Image& image)
{
    _image = &image;

    if(image.size().isNull())
    {
        _surface = 0;
        _canvas = 0;
        return;
    }

    SkImageInfo info = SkImageInfo::MakeCustom(image.width(), image.height(), 
                                               kPremul_SkAlphaType, SkCreateBlitter, (void*)&image);
    _surface = SkSurface::MakeRaster(info);
    _canvas = _surface->getCanvas();

    // TODO: replace with something independent of Painter
    resetClip();
}


void Rasterizer::begin(Gfx::Painter& painter)
{
    Gfx::PaintData* pd = painter.paintData();
    _paintData = dynamic_cast<SkiaPaintData*>(pd);

    if (_paintData == 0)
    {
        delete pd;

        _paintData = new SkiaPaintData();
        painter.setPaintData(_paintData);
    }

    _painter = &painter;
}


void Rasterizer::finish()
{
    _paintData = 0;
    _painter = 0;
}


const ImageFormat& Rasterizer::format() const
{
  return _image->format();
}


void Rasterizer::setCompositionMode(const CompositionMode& mode)
{
    _compositionMode = mode;
}


void Rasterizer::setPen( const Pen& pen )
{
    _paintData->setPen(pen);
}


void Rasterizer::setBrush( const Brush& brush )
{
    _paintData->setBrush(brush);
}


void Rasterizer::updateClip() const
{
    RectF imageRect( SizeF(_image->width(), 
                           _image->height()) );

    RectF currentClip = _clip.intersect(imageRect);
    _paintData->setClip(currentClip);

    _canvas->clipRect(toSkia(currentClip), SkClipOp::kMax_EnumValue);
}


void Rasterizer::setClip(const RectF& clip)
{
    _clip = clip;
}


void Rasterizer::resetClip()
{
    setClip( Gfx::RectF(Gfx::PointF(0, 0), 
                         Gfx::SizeF(_image->width(), _image->height())) );
}


void Rasterizer::setFont(const Font& font)
{
    _text->setFont(font);
}


void Rasterizer::drawLine(const PointF& from, const  PointF& to)
{
    if (!_canvas)
        return;

    updateClip();
    _canvas->drawLine(toSkia(from), toSkia(to), _paintData->pen());
}


void Rasterizer::drawText(const PointF& toF, const String& text)
{
    if (!_canvas)
        return;

    Point to = round(toF);

    updateClip();

    Rect currentClip = round(_paintData->clipRect() );

    if (currentClip.isNull())
        currentClip = Gfx::Rect(Gfx::Point(0, 0), _image->size());

    _text->setClip(currentClip);
    _text->draw(*_image, _painter->pen().color(), to, text, _compositionMode);
}


void Rasterizer::drawText(const PointF& toF, const Pt::String& text, const Transform& trans)
{
    if (!_canvas)
        return;

    Point to = round(toF);
    updateClip();
    Rect currentClip = round( _paintData->clipRect() );

    if(currentClip.isNull())
        currentClip = Gfx::Rect(Gfx::Point(0,0), _image->size());

    _text->setClip(currentClip);
    _text->draw(*_image, _painter->pen().color(), to, text, _compositionMode, trans);
}


FontMetrics Rasterizer::fontMetrics(const String& text) const
{
    return _text->fontMetrics(text);
}


FontMetrics Rasterizer::fontMetrics(const Font& font, const Pt::String& text)
{
    DrawText textRender;
    textRender.setFont(font);

    return textRender.fontMetrics(text);
}


void Rasterizer::drawRect(const RectF& r)
{
    if (!_canvas)
        return;

    updateClip();

    _canvas->drawRect( toSkia(r), _paintData->pen() );
}


void Rasterizer::fillRect(const RectF& r)
{
    if (!_canvas)
        return;

    updateClip();

    _canvas->drawRect( toSkia(r), _paintData->brush() );
}


void Rasterizer::drawEllipse(const PointF& topLeft, const SizeF& size)
{
    if (!_canvas)
        return;

    updateClip();

    _canvas->drawOval( toSkia(topLeft, size), _paintData->pen() );
}


void Rasterizer::fillEllipse(const PointF& topLeft, const SizeF& size)
{
    if (!_canvas)
        return;

    updateClip();

    _canvas->drawOval( toSkia(topLeft, size), _paintData->brush() );
}


void Rasterizer::drawPolyline(const PointF* points, const size_t n)
{
    if (!_canvas)
        return;

    updateClip();

    SkPath path;

    path.moveTo( toSkia(points[0]) );

    for (size_t i = 1; i < n; ++i)
        path.lineTo(toSkia(points[i]));

    if ( equals(points[0], points[n - 1] ))
        path.close();

    _canvas->drawPath( path, _paintData->pen() );
}


void Rasterizer::fillPolygon(const PointF* points, const size_t n)
{
    if (!_canvas)
        return;

    updateClip();

    SkPath path;

    path.moveTo( toSkia(points[0]) );

    for (size_t i = 1; i < n; ++i)
        path.lineTo(toSkia(points[i]));

    path.close();

    _canvas->drawPath(path, _paintData->brush() );
}


void Rasterizer::drawImage(const PointF& to, const Image& img)
{
    RectF imageRect( PointF(0, 0), SizeF( img.width(), img.height()) );
    drawImage(to, img, imageRect);
}


void Rasterizer::drawImage(const PointF& toF, const Image& from, const RectF& fromRectF)
{
    updateClip();

    Rect fromRect = round(fromRectF);
    Point to = round(toF);
    Rect currentClip = round( _paintData->clipRect() );

    if(currentClip.isNull())
        currentClip = Gfx::Rect(Gfx::Point(0,0),_image->size());

    // clip fromRect to fit into the clip/image rect
    Point d = currentClip.topLeft() - to;
    Point fromPos = fromRect.topLeft() + d;

    Rect fromClip(fromPos, currentClip.size());
    fromClip = fromRect.intersect(fromClip);

    if (fromClip.isNull())
        return;

    // account for smaller fromRect
    Point toClip = to + (fromClip.topLeft() - fromRect.topLeft());

    _image->format().copy(_image->view(), toClip, from.view(), fromClip, _compositionMode);
}


void Rasterizer::drawPath(const Gfx::Path& path, float smoothness)
{
    if (!_canvas)
        return;

    updateClip();

    SkPath skPath = toSkia(path);
    _canvas->drawPath( skPath, _paintData->pen() );
}


void Rasterizer::fillPath(const Path& path, float smoothness)
{
    if (!_canvas)
        return;

    updateClip();
    SkPath skPath = toSkia(path);
    _canvas->drawPath( skPath, _paintData->brush() );
}


void Rasterizer::drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    updateClip();
}


void Rasterizer::fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    updateClip();
}


void Rasterizer::drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    updateClip();
}


void Rasterizer::fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    updateClip();
}


void Rasterizer::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    updateClip();
}


Image Rasterizer::toImage() const
{
    if( ! _image )
        return Image();
    
    return *_image;
}


void Rasterizer::setFontDir(const Pt::System::Path& path)
{
    FreeType::instance().setFontDir(path);
}


std::string Rasterizer::defaultFont()
{
    return  FreeType::instance().defaultFont();
}


void Rasterizer::setDefaultFont(const std::string& f)
{
    FreeType::instance().setDefaultFont(f);
}


std::vector<std::string> Rasterizer::fontNames()
{
    return FreeType::instance().fontNames();
}


SkPath Rasterizer::toSkia(const Gfx::Path& p)
{
    SkPath skp;

    std::size_t s = p.size();

    for (std::size_t n = 0; n < s; n++)
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
