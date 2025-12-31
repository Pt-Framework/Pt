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
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/PaintContext.h>

namespace Pt {

namespace Gfx {

Painter::Painter()
: _surface(0)
, _context(0)
, _canvas(0)
{
}


Painter::Painter(PaintSurface& surface)
: _surface(0)
, _context(0)
, _canvas(0)
{
    begin(surface);
}


Painter::Painter(PaintContext& canvas)
: _surface(0)
, _context(0)
, _canvas(0)
{
    begin(canvas);
}


Painter::~Painter()
{
    finish();

    delete _canvas;
}


void Painter::begin(PaintSurface& surface)
{
    if( ! _context && _surface == &surface )
        return;

    finish();

    onBeginPaint(surface);
}


void Painter::begin(PaintContext& context)
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
    Canvas* reuse = _canvas;
    Canvas* canvas = surface.getCanvas(reuse);
   
    surface.attachPainter(*this);
    _surface = &surface;

    onBeginPaint(*canvas);
}


void Painter::onBeginPaint(Canvas& canvas)
{
    Canvas* reuse = _canvas;
    _canvas = &canvas;
   
    if(_canvas != reuse)
    {
        delete reuse;

        // initialize new canvas
        if(_canvas)
        {
            _canvas->setCompositionMode( _paint.compositionMode() );
            _canvas->setPen( _paint.pen() );
            _canvas->setBrush( _paint.brush() );
            _canvas->setFont( _paint.font() );
            _canvas->setPath( _paint.path() );

            const Gfx::RectF* clip = _paint.clip();
            if( ! clip )
                _canvas->resetClip();
            else
                _canvas->setClip(*clip);
        }
    }

    if(_canvas)
        _canvas->beginPaint(_paint);
}


void Painter::finish()
{
    if(_canvas)
        _canvas->finishPaint();

    if( ! _context && _surface )
    {
        _surface->finish();
    }

    if(_surface)
    {
        _surface->detachPainter(*this);
        _surface = 0;
    }

    if(_context)
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


void Painter::onDetachContext(PaintContext& context)
{
    finish();

    if(_context)
        _context = 0;
}


const Gfx::ImageFormat& Painter::format() const
{
    if(_canvas)
        return _canvas->format();

    return ImageFormat::argb32();
}


const Scaling& Painter::scaling() const
{
    if(_canvas)
        return _canvas->scaling();

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

    if(_canvas)
        _canvas->setCompositionMode(mode);
}


const Gfx::Pen& Painter::pen() const
{
    return _paint.pen();
}


void Painter::setPen(const Gfx::Pen& pen)
{
    _paint.setPen(pen);

    if(_canvas)
        _canvas->setPen(pen);
}


const Gfx::Brush& Painter::brush() const
{
    return _paint.brush();
}


void Painter::setBrush(const Gfx::Brush& brush)
{
    _paint.setBrush(brush);

    if(_canvas)
        _canvas->setBrush(brush);
}


const Gfx::Font& Painter::font() const
{
    return _paint.font();
}


void Painter::setFont(const Gfx::Font& font)
{
    _paint.setFont(font);

    if(_canvas)
        _canvas->setFont(font);
}


const RectF* Painter::clip() const
{
    return _paint.clip();
}


void Painter::setClip(const Gfx::RectF& clip)
{
    _paint.setClip(clip);

    if(_canvas)
    {
        if( clip.isNull() )
            _canvas->resetClip();
        else
            _canvas->setClip(clip);
    }
}


void Painter::resetClip()
{
    _paint.resetClip();

    if(_canvas)
        _canvas->resetClip();
}


void Painter::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( _paint.pen().size() == 0 )
        return;

    if(_canvas)
        _canvas->drawLine(from, to);
}


void Painter::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    if( _paint.pen().size() == 0 )
        return;

    if(_canvas)
        _canvas->drawPolyline(points, pointCount);
}


void Painter::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    if(_canvas)
        _canvas->fillPolygon(points, pointCount);
}


void Painter::drawRect(const Gfx::RectF& rect)
{
    if(_canvas)
        _canvas->drawRect(rect);
}


void Painter::fillRect(const Gfx::RectF& rect)
{
    if(_canvas)
        _canvas->fillRect(rect);
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
    if(_canvas)
        _canvas->drawEllipse(topLeft, size);
}


void Painter::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_canvas)
        _canvas->fillEllipse(topLeft, size);
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

    if(_canvas)
        _canvas->setPath(path);
}


void Painter::drawPath()
{
    if(_canvas)
        _canvas->drawPath();
}


void Painter::fillPath()
{
    if(_canvas)
        _canvas->fillPath();
}


void Painter::drawPath(const Path& path)
{
    if(_canvas)
        _canvas->drawPath(path);
}


void Painter::fillPath(const Path& path)
{
    if(_canvas)
        _canvas->fillPath(path);
}


Gfx::TextMetrics Painter::textMetrics(const Pt::String& text) const
{
    if(_canvas)
        return _canvas->textMetrics(text);

    return Gfx::TextMetrics();
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if(_canvas)
        _canvas->drawText(to, text);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text, 
                       const Gfx::Transform& transform)
{
    if(_canvas)
        _canvas->drawText(to, text, &transform);
}


void Painter::drawImage(const Gfx::PointF& to, 
                        const Gfx::Image& image)
{
    if(_canvas)
        _canvas->drawImage(to, image);
}


void Painter::drawImage(const Gfx::PointF& to,
                        const Gfx::Image& image, 
                        const Gfx::RectF& imageRect)
{
    if(_canvas)
        _canvas->drawImage(to, image, &imageRect);
}

} // namespace

} // namespace
