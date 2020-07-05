/* Copyright (C) 2020 Marc Boris Duerner
    Copyright (C) 2020 Laurentiu-Gheorghe Crisan

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

#include "SkiaPainter.h"
#include <Pt/Gfx/ImagePainter.h>
#include <SkPath.h>
#include <SkDashPathEffect.h>

namespace Pt {
namespace Hmi {


SkiaPainter::SkiaPainter(Gfx::Image& image)
: _image(0)
, _canvas(0)
, _imgPainter(image)
{
    setImage(image);
}

SkiaPainter::~SkiaPainter()
{

}

void SkiaPainter::setImage(Gfx::Image& image)
{
    _image = &image;
    _imgPainter.setImage(image);


#if 1
    SkImageInfo info = SkImageInfo::Make(image.width(), image.height(), kPt_SkColorType, kPremul_SkAlphaType, 0);    
    
    size_t rowBytes = info.minRowBytes();
    _surface = SkSurface::MakeRasterDirect(info, (void*)&image, rowBytes);
#else
    
    SkImageInfo info = SkImageInfo::Make(image.width(), image.height(), kBGRA_8888_SkColorType, kOpaque_SkAlphaType, 0);
    size_t rowBytes = info.minRowBytes();
    _surface = SkSurface::MakeRasterDirect(info, (void*)image.data(), rowBytes);
#endif
    
    _canvas = _surface->getCanvas();

    resetClip();
}

const Gfx::ImageFormat& SkiaPainter::format() const
{
    return _image->format();
}

void SkiaPainter::setCompositionMode(const Gfx::CompositionMode& mode)
{

    _imgPainter.setCompositionMode(mode);
}

const Gfx::CompositionMode& SkiaPainter::compositionMode() const
{
    return _imgPainter.compositionMode();
}

void SkiaPainter::setClip(const Gfx::RectF& clip)
{
    _clip = clip;
    _canvas->clipRect(toSkia(clip), SkClipOp::kMax_EnumValue);
}

void SkiaPainter::resetClip()
{
    setClip(Gfx::RectF(Gfx::PointF(0, 0), Gfx::SizeF(_image->width(), _image->height())));
}

void SkiaPainter::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
    
    _skiaPen = SkPaint();
    _skiaPen.setColor(toSkia(pen.color()));
    _skiaPen.setStyle(SkPaint::kStroke_Style);
    _skiaPen.setAntiAlias(true);
    _skiaPen.setStrokeWidth(pen.size());

    _skiaPen.setStrokeJoin(toSkia(pen.joinStyle()));
    
    _skiaPen.setStrokeCap(toSkia(pen.capStyle()));


    switch (_pen.style())
    {
        default:
        case Pt::Gfx::Pen::Solid:
        {
        }
        break;

        case Pt::Gfx::Pen::Dash:
        {
            if (_pen.capStyle() == Gfx::Pen::RoundCap || _pen.capStyle() == Gfx::Pen::SquareCap)
            {
                SkScalar dashes[] = { 2.0f * pen.size(), 2.0f * pen.size() };
                _skiaPen.setPathEffect(SkDashPathEffect::Make(dashes, 2, 0));
            }
            else
            {
                SkScalar dashes[] = { 3.0f * pen.size(), 1.0f * pen.size() };
                _skiaPen.setPathEffect(SkDashPathEffect::Make(dashes, 2, 0));
            }
        }
        break;

        case Pt::Gfx::Pen::Dot:
        {
            if (_pen.capStyle() == Gfx::Pen::RoundCap ||
                _pen.capStyle() == Gfx::Pen::SquareCap)
            {
                SkScalar dashes[] = { 1.0f, 2.0f * pen.size() };
                _skiaPen.setPathEffect(SkDashPathEffect::Make(dashes, 2, 0));
            }
            else
            {
                SkScalar dashes[] = { 1.0f * pen.size(), 1.0f * pen.size() };

                _skiaPen.setPathEffect(SkDashPathEffect::Make(dashes, 2, 0));
            }            
        }
        break;
    }

    _imgPainter.setPen(_pen);
}


const Gfx::Pen& SkiaPainter::pen() const
{
    return _pen;
}

void SkiaPainter::setBrush(const Gfx::Brush& brush)
{
    _skiaBrush = SkPaint();
    _skiaBrush.setAntiAlias(true);
    _skiaBrush.setStyle(SkPaint::kFill_Style);
    _skiaBrush.setColor(toSkia(brush.color()));
    _brush = brush;


    switch (brush.fillStyle())
    {
        case Gfx::Brush::Solid:
        {
     
        }
        break;

        case Gfx::Brush::Texture:
        {
       
        
        }
        break;

        case Gfx::Brush::Gradient:
        {
            switch (brush.gradient())
            {
                case Gfx::Brush::Linear:
                case Gfx::Brush::Horizontal:
                case Gfx::Brush::Vertical:
                   break;

            case Gfx::Brush::Radial:

                break;
            }
        }
        break;

        default:
            break;
    }
}

const Gfx::Brush& SkiaPainter::brush() const
{
    return _brush;
}

void SkiaPainter::setFont(const Gfx::Font& font)
{
    _font = font;

    _imgPainter.setFont(_font);
}

const Gfx::Font& SkiaPainter::font() const
{
    return _font;
}

Gfx::FontMetrics SkiaPainter::fontMetrics(const Pt::String& text) const
{
    return _imgPainter.fontMetrics(text);
}

void SkiaPainter::drawLine(const Gfx::PointF& from, const  Gfx::PointF& to)
{
    _canvas->drawLine(toSkia(from), toSkia(to), _skiaPen);
}

void SkiaPainter::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    
    _imgPainter.drawText(to, text);
}

void SkiaPainter::drawText(const Gfx::PointF& to, const Pt::String& text, const Gfx::Transform& trans)
{
    _imgPainter.drawText(to, text, trans);
}

void SkiaPainter::drawRect(const Gfx::RectF& rect)
{
    _canvas->drawRect(toSkia(rect), _skiaPen);
}

void SkiaPainter::fillRect(const Gfx::RectF& rect)
{
    _canvas->drawRect(toSkia(rect), _skiaBrush);
}


void SkiaPainter::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _canvas->drawOval(toRect(topLeft, size), _skiaPen);
}


void SkiaPainter::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _canvas->drawOval(toRect(topLeft, size), _skiaBrush);
}

void SkiaPainter::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    SkPath path;

    path.moveTo(toSkia(points[0]));

    for (size_t i = 1; i < pointCount; ++i)
        path.lineTo(toSkia(points[i]));

    if(equals(points[0], points[pointCount-1]))
        path.close();

    _canvas->drawPath(path, _skiaPen);
}

void SkiaPainter::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    SkPath path;

    path.moveTo(toSkia(points[0]));

    for (size_t i = 1; i < pointCount; ++i)
        path.lineTo(toSkia(points[i]));

    path.close();

    _canvas->drawPath(path, _skiaBrush);
}

void SkiaPainter::drawImage(const  Gfx::PointF& to, const Gfx::Image& image)
{
    _imgPainter.drawImage(to, image);
}


void SkiaPainter::drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imageRect)
{
    _imgPainter.drawImage(to, image, imageRect);
}


void SkiaPainter::setFontDir(const System::Path& path)
{
    Gfx::ImagePainter::setFontDir(path);
}

std::string SkiaPainter::defaultFont()
{
    return Gfx::ImagePainter::defaultFont();
}

void SkiaPainter::setDefaultFont(const std::string& name)
{
    Gfx::ImagePainter::setDefaultFont(name);
}

std::vector<std::string> SkiaPainter::fontNames()
{
    return Gfx::ImagePainter::fontNames();
}


Gfx::FontMetrics SkiaPainter::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{
    return Gfx::ImagePainter::fontMetrics(font, text);
}


}}


