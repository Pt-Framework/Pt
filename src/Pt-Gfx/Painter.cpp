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
#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/PaintSurface.h>

namespace Pt {

namespace Gfx {

Painter::Painter()
: _surface(0)
, _context(0)
, _paintCanvas(0)
{
}


Painter::Painter(PaintSurface& surface)
: _surface(0)
, _context(0)
, _paintCanvas(0)
{
    begin(surface);
}


Painter::Painter(RenderContext& canvas)
: _surface(0)
, _context(0)
, _paintCanvas(0)
{
    begin(canvas);
}


Painter::~Painter()
{
    finish();

    delete _paintCanvas;
}


void Painter::begin(PaintSurface& surface)
{
    if( ! _context && _surface == &surface )
        return;

    finish();

    onBeginPaint(surface);
}


void Painter::begin(RenderContext& context)
{
    PaintSurface* surface = context.surface();
  
    if(_context == &context && _surface == surface)
        return;

    finish();

    context.attachPainter(*this);
    _context = &context;

    if(surface)
        onBeginPaint(*surface);
}


void Painter::onBeginPaint(PaintSurface& surface)
{
    surface.attachPainter(*this);
    _surface = &surface;
   
    Canvas* reuse = _paintCanvas;
    _paintCanvas = surface.getCanvas(reuse);
   
    if(reuse != _paintCanvas)
    {
        delete reuse;

        // initialize new paint context
        if(_paintCanvas)
        {
            _paintCanvas->setCompositionMode( _paint.compositionMode() );
            _paintCanvas->setPen( _paint.pen() );
            _paintCanvas->setBrush( _paint.brush() );
            _paintCanvas->setFont( _paint.font() );
            _paintCanvas->setPath( _paint.path() );

            const Gfx::RectF* clip = _paint.clip();
            if( ! clip )
                _paintCanvas->resetClip();
            else
                _paintCanvas->setClip(*clip);
        }
    }

    if(_paintCanvas)
        _paintCanvas->beginPaint(_paint);
}


void Painter::finish()
{
    if(_paintCanvas)
        _paintCanvas->finishPaint();

    if( ! _context && _surface)
    {
        _surface->finish();
    }

    if( _surface )
    {
        _surface->detachPainter(*this);
        _surface = 0;
    }

    if( _context )
    {
        _context->detachPainter(*this);
        _context = 0;
    }
}


void Painter::onDetachSurface(PaintSurface& surface)
{
    if(_surface)
        _surface = 0;
}


void Painter::onDetachContext(RenderContext& context)
{
    finish();

    if(_context)
        _context = 0;
}


const Gfx::ImageFormat& Painter::format() const
{
    if(_paintCanvas)
        return _paintCanvas->format();

    return ImageFormat::argb32();
}


const Scaling& Painter::scaling() const
{
    if(_paintCanvas)
        return _paintCanvas->scaling();

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

    if(_paintCanvas)
        _paintCanvas->setCompositionMode(mode);
}


const Gfx::Pen& Painter::pen() const
{
    return _paint.pen();
}


void Painter::setPen(const Gfx::Pen& pen)
{
    _paint.setPen(pen);

    if(_paintCanvas)
        _paintCanvas->setPen(pen);
}


const Gfx::Brush& Painter::brush() const
{
    return _paint.brush();
}


void Painter::setBrush(const Gfx::Brush& brush)
{
    _paint.setBrush(brush);

    if(_paintCanvas)
        _paintCanvas->setBrush(brush);
}


const Gfx::Font& Painter::font() const
{
    return _paint.font();
}


void Painter::setFont(const Gfx::Font& font)
{
    _paint.setFont(font);

    if(_paintCanvas)
        _paintCanvas->setFont(font);
}


const RectF* Painter::clip() const
{
    return _paint.clip();
}


void Painter::setClip(const Gfx::RectF& clip)
{
    _paint.setClip(clip);

    if(_paintCanvas)
    {
        if( clip.isNull() )
            _paintCanvas->resetClip();
        else
            _paintCanvas->setClip(clip);
    }
}


void Painter::resetClip()
{
    _paint.resetClip();

    if(_paintCanvas)
        _paintCanvas->resetClip();
}


void Painter::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( _paint.pen().size() == 0 )
        return;

    if(_paintCanvas)
        _paintCanvas->drawLine(from, to);
}


void Painter::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    if( _paint.pen().size() == 0 )
        return;

    if(_paintCanvas)
        _paintCanvas->drawPolyline(points, pointCount);
}


void Painter::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    if(_paintCanvas)
        _paintCanvas->fillPolygon(points, pointCount);
}


void Painter::drawRect(const Gfx::RectF& rect)
{
    if(_paintCanvas)
        _paintCanvas->drawRect(rect);
}


void Painter::fillRect(const Gfx::RectF& rect)
{
    if(_paintCanvas)
        _paintCanvas->fillRect(rect);
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
    if(_paintCanvas)
        _paintCanvas->drawEllipse(topLeft, size);
}


void Painter::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_paintCanvas)
        _paintCanvas->fillEllipse(topLeft, size);
}


void Painter::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


const Gfx::Path& Painter::path() const
{
    return _paint.path();
}


void Painter::setPath(const Path& path)
{
    _paint.setPath(path);

    if(_paintCanvas)
        _paintCanvas->setPath(path);
}


void Painter::drawPath()
{
    if(_paintCanvas)
        _paintCanvas->drawPath();
}


void Painter::fillPath()
{
    if(_paintCanvas)
        _paintCanvas->fillPath();
}


void Painter::drawPath(const Path& path)
{
    if(_paintCanvas)
        _paintCanvas->drawPath(path);
}


void Painter::fillPath(const Path& path)
{
    if(_paintCanvas)
        _paintCanvas->fillPath(path);
}


Gfx::TextMetrics Painter::textMetrics(const Pt::String& text) const
{
    if(_paintCanvas)
        return _paintCanvas->textMetrics(text);

    return Gfx::TextMetrics();
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if(_paintCanvas)
        _paintCanvas->drawText(to, text);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text, 
                       const Gfx::Transform& transform)
{
    if(_paintCanvas)
        _paintCanvas->drawText(to, text, &transform);
}


void Painter::drawImage(const Gfx::PointF& to, 
                        const Gfx::Image& image)
{
    if(_paintCanvas)
        _paintCanvas->drawImage(to, image);
}


void Painter::drawImage(const Gfx::PointF& to,
                        const Gfx::Image& image, 
                        const Gfx::RectF& imageRect)
{
    if(_paintCanvas)
        _paintCanvas->drawImage(to, image, &imageRect);
}

} // namespace

} // namespace
