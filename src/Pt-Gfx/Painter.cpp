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

#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintSurface.h>

namespace Pt {

namespace Gfx {

Painter::Painter()
: _surface(0)
, _paint(0)
{
}


Painter::Painter(PaintSurface& surface)
: _surface(0)
, _paint(0)
{
    begin(surface);
}


Painter::~Painter()
{
    if( _surface )
    {
        _surface->detachPainter(*this);
        _surface = 0;
    }

    if(_paint) 
        _paint->onDetachPainter(*this);

    delete _paint;
}


void Painter::begin(PaintSurface& surface)
{
    finish();

    surface.attachPainter(*this);
    _surface = &surface;

    PaintData* paint = surface.getPaint(_paint);
    if(_paint != paint)
    {
        delete _paint;
        _paint = 0;
    }
    
    if(paint)
        paint->begin(*this);
}


void Painter::finish()
{
    if(_paint)
        _paint->finish();

    if( _surface )
    {
        _surface->detachPainter(*this);
        onDetachSurface(*_surface);
    }
}


void Painter::onDetachSurface(PaintSurface& surface)
{
    if(_surface)
    {
        _scaling = Scaling();
        _surface = 0;
    }
}


void Painter::attachPaint(PaintData& paint)
{
    if(_paint)
    {
        _paint->onDetachPainter(*this);
        _paint = 0;
    }
    
    _paint = &paint;
}


void Painter::detachPaint(PaintData& paint)
{
    if(_paint)
        _paint = 0;
}


const Gfx::ImageFormat& Painter::format() const
{
    return _paint ? _paint->format() : ImageFormat::argb32();
}


const Scaling& Painter::scaling() const
{
    return _paint ? _paint->scaling() : _scaling;
}


const Gfx::CompositionMode& Painter::compositionMode() const
{
    return _compositionMode;
}


void Painter::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;

    if(_paint)
        _paint->setCompositionMode(_compositionMode);
}


void Painter::setPen(const Gfx::Pen& pen)
{
    _pen = pen;

    if(_paint)
        _paint->setPen(pen);
}


const Gfx::Pen& Painter::pen() const
{
    return _pen;
}


void Painter::setClip(const Gfx::RectF& clip)
{
    _clip = clip;

    if(_paint)
    {
        if( _clip.isNull() )
            _paint->resetClip();
        else
            _paint->setClip(_clip);
    }
}


void Painter::resetClip()
{
    // TODO: RECT-NULL
    _clip = Gfx::RectF();

    if(_paint)
        _paint->resetClip();
}


void Painter::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;

    if(_paint)
        _paint->setBrush(_brush);
}


const Gfx::Brush& Painter::brush() const
{
    return _brush;
}


void Painter::setFont(const Gfx::Font& font)
{
    _font = font;

    if(_paint)
        _paint->setFont(_font);
}


const Gfx::Font& Painter::font() const
{
    return _font;
}


Gfx::FontMetrics Painter::fontMetrics(const Pt::String& text) const
{
    if(_paint)
        return _paint->fontMetrics(text);

    return Gfx::FontMetrics();
}


void Painter::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( _pen.size() == 0 )
        return;

    if(_paint)
        _paint->drawLine(from, to);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if(_paint)
        _paint->drawText(to, text);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text, 
                       const Gfx::Transform& trans)
{
    if(_paint)
        _paint->drawText(to, text, trans);
}


void Painter::drawRect(const Gfx::RectF& rect)
{
    if(_paint)
        _paint->drawRect(rect);
}


void Painter::fillRect(const Gfx::RectF& rect)
{
    if(_paint)
        _paint->fillRect(rect);
}


void Painter::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_paint)
        _paint->drawEllipse(topLeft, size);
}


void Painter::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_paint)
        _paint->fillEllipse(topLeft, size);
}


void Painter::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    if (_pen.size() == 0)
        return;

    if(_paint)
        _paint->drawPolyline(points, pointCount);
}


void Painter::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    if(_paint)
        _paint->fillPolygon(points, pointCount);
}


void Painter::drawSurface(const Gfx::PointF& toF, 
                          const PaintSurface& surface)
{
    if(_paint)
        _paint->drawSurface(toF, surface);
}


void Painter::drawSurface(const Gfx::PointF& toF, 
                          const PaintSurface& surface, 
                          const Gfx::RectF& surfaceRect)
{
    if(_paint)
        _paint->drawSurface(toF, surface, surfaceRect);
}


void Painter::drawImage(const Gfx::PointF& to, 
                        const Gfx::Image& image)
{
    if(_paint)
        _paint->drawImage(to, image);
}


void Painter::drawImage(const Gfx::PointF& to,
                        const Gfx::Image& image, 
                        const Gfx::RectF& imageRect)
{
    if(_paint)
        _paint->drawImage(to, image, imageRect);
}


void Painter::drawPath(const Gfx::Path& path, float smoothness)
{
}


void Painter::fillPath(const Path& path, float smoothness)
{
}


void Painter::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}

} // namespace

} // namespace
