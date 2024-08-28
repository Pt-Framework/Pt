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
#include <Pt/Gfx/Canvas.h>

namespace Pt {

namespace Gfx {

Painter::Painter()
: _surface(0)
{
}


Painter::Painter(PaintSurface& surface)
: _surface(0)
, _ref(0)
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
}


void Painter::begin(PaintSurface& surface)
{
    finish();

    surface.attachPainter(*this);
    _surface = &surface;
   
    PaintContext* reuse = _paintContext.get();
    _paintContext = surface.beginPaint(reuse);
          
    if(_paintContext)
    {
        _paintContext->setPainter(*this);
        _paintContext->beginPaint(_paint);
    }
}


void Painter::finish()
{
    if(_paintContext)
    {
        _paintContext->finishPaint();
        _paintContext->releaseCanvas();
    }

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
        _surface = 0;
    }
}


const Gfx::ImageFormat& Painter::format() const
{
    return _surface ? _surface->format() 
                    : ImageFormat::argb32();
}


const Scaling& Painter::scaling() const
{
    return _scaling;
}


const Paint& Painter::paint() const
{
    return _paint;
}


const Gfx::CompositionMode& Painter::compositionMode() const
{
    return _paint.compositionMode();
}


void Painter::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _paint.setCompositionMode(mode);

    if(_paintContext)
        _paintContext->setCompositionMode(mode);
}


const RectF& Painter::clip() const
{
    return _paint.clip();
}


void Painter::setClip(const Gfx::RectF& clip)
{
    _paint.setClip(clip);

    if(_paintContext)
    {
        if( clip.isNull() )
            _paintContext->resetClip();
        else
            _paintContext->setClip(clip);
    }
}


void Painter::resetClip()
{
    _paint.resetClip();

    if(_paintContext)
        _paintContext->resetClip();
}


const Gfx::Pen& Painter::pen() const
{
    return _paint.pen();
}


void Painter::setPen(const Gfx::Pen& pen)
{
    _paint.setPen(pen);

    if(_paintContext)
        _paintContext->setPen(pen);
}


const Gfx::Brush& Painter::brush() const
{
    return _paint.brush();
}


void Painter::setBrush(const Gfx::Brush& brush)
{
    _paint.setBrush(brush);

    if(_paintContext)
        _paintContext->setBrush(brush);
}


const Gfx::Font& Painter::font() const
{
    return _paint.font();
}


void Painter::setFont(const Gfx::Font& font)
{
    _paint.setFont(font);

    if(_paintContext)
        _paintContext->setFont(font);
}


Gfx::FontMetrics Painter::fontMetrics(const Pt::String& text) const
{
    if(_paintContext)
        return _paintContext->fontMetrics(text);

    return Gfx::FontMetrics();
}


void Painter::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( _paint.pen().size() == 0 )
        return;

    if(_paintContext)
        _paintContext->drawLine(from, to);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if(_paintContext)
        _paintContext->drawText(to, text);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text, 
                       const Gfx::Transform& trans)
{
    if(_paintContext)
        _paintContext->drawText(to, text, trans);
}


void Painter::drawRect(const Gfx::RectF& rect)
{
    if(_paintContext)
        _paintContext->drawRect(rect);
}


void Painter::fillRect(const Gfx::RectF& rect)
{
    if(_paintContext)
        _paintContext->fillRect(rect);
}


void Painter::drawCircle(const PointF& topLeft, double diameter)
{
    drawEllipse(topLeft, SizeF(diameter, diameter));
}


void Painter::fillCircle(const PointF& topLeft, double diameter)
{
    fillEllipse(topLeft, SizeF(diameter, diameter));
}


void Painter::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_paintContext)
        _paintContext->drawEllipse(topLeft, size);
}


void Painter::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_paintContext)
        _paintContext->fillEllipse(topLeft, size);
}


void Painter::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    if( _paint.pen().size() == 0 )
        return;

    if(_paintContext)
        _paintContext->drawPolyline(points, pointCount);
}


void Painter::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    if(_paintContext)
        _paintContext->fillPolygon(points, pointCount);
}


void Painter::drawSurface(const Gfx::PointF& toF, 
                          const PaintSurface& surface)
{
    if(_paintContext)
        _paintContext->drawSurface(toF, surface);
}


void Painter::drawSurface(const Gfx::PointF& toF, 
                          const PaintSurface& surface, 
                          const Gfx::RectF& surfaceRect)
{
    if(_paintContext)
        _paintContext->drawSurface(toF, surface, surfaceRect);
}


void Painter::drawImage(const Gfx::PointF& to, 
                        const Gfx::Image& image)
{
    if(_paintContext)
        _paintContext->drawImage(to, image);
}


void Painter::drawImage(const Gfx::PointF& to,
                        const Gfx::Image& image, 
                        const Gfx::RectF& imageRect)
{
    if(_paintContext)
        _paintContext->drawImage(to, image, imageRect);
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
