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

#include <Pt/Gfx/PainterBase.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/PaintContext.h>

namespace Pt {

namespace Gfx {

PainterBase::PainterBase()
: _surface(0)
, _context(0)
, _canvas(0)
, _hasClip(false)
{
}


PainterBase::~PainterBase()
{
    finish();

    delete _canvas;
}


void PainterBase::beginPaint(PaintSurface& surface)
{
    if( ! _context && _surface == &surface )
        return;

    finish();

    onBeginPaint(surface);
}


void PainterBase::beginPaint(PaintContext& context)
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


void PainterBase::onBeginPaint(PaintSurface& surface)
{
    Canvas* reuse = _canvas;
    Canvas* canvas = surface.getCanvas(reuse);
   
    surface.attachPainter(*this);
    _surface = &surface;

    if(canvas)
        onBeginPaint(*canvas);
}


void PainterBase::onBeginPaint(Canvas& canvas)
{
    Canvas* reuse = _canvas;
    _canvas = &canvas;
   
    if(_canvas != reuse)
    {
        delete reuse;

        // initialize new canvas
        if(_canvas)
        {
            _canvas->setTransform(_transform);
            _canvas->setCompositionMode( _paint.compositionMode() );
            _canvas->setPen( _paint.pen() );
            _canvas->setBrush( _paint.brush() );
            _canvas->setFont( _paint.font() );
            _canvas->setPath(_path);

            if( ! _hasClip )
                _canvas->resetClip();
            else
                _canvas->setClip(_clip);
        }
    }

    if(_canvas)
        _canvas->beginPaint(_paint);
}


void PainterBase::finish()
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


void PainterBase::onDetachSurface(PaintSurface& surface)
{
    if(_surface)
        _surface = 0;
}


void PainterBase::onDetachContext(PaintContext& context)
{
    finish();

    if(_context)
        _context = 0;
}


const Gfx::ImageFormat& PainterBase::format() const
{
    if(_canvas)
        return _canvas->format();

    return ImageFormat::argb32();
}


const Scaling& PainterBase::scaling() const
{
    if(_canvas)
        return _canvas->scaling();

    return _scaling;
}


const Gfx::CompositionMode& PainterBase::compositionMode() const
{
    return _paint.compositionMode();
}


void PainterBase::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _paint.setCompositionMode(mode);

    if(_canvas)
        _canvas->setCompositionMode(mode);
}


const Gfx::Pen& PainterBase::pen() const
{
    return _paint.pen();
}


void PainterBase::setPen(const Gfx::Pen& pen)
{
    _paint.setPen(pen);

    if(_canvas)
        _canvas->setPen(pen);
}


const Gfx::Brush& PainterBase::brush() const
{
    return _paint.brush();
}


void PainterBase::setBrush(const Gfx::Brush& brush)
{
    _paint.setBrush(brush);

    if(_canvas)
        _canvas->setBrush(brush);
}


const Gfx::Font& PainterBase::font() const
{
    return _paint.font();
}


void PainterBase::setFont(const Gfx::Font& font)
{
    _paint.setFont(font);

    if(_canvas)
        _canvas->setFont(font);
}


const Gfx::Transform& PainterBase::transform() const
{
    return _transform;
}


void PainterBase::setTransform(const Gfx::Transform& tx)
{
    _transform = tx;

    if(_canvas)
        _canvas->setTransform(tx);
}


void PainterBase::resetTransform()
{
    _transform = Transform();

    if(_canvas)
        _canvas->resetTransform();
}


const RectF* PainterBase::clip() const
{
    return _hasClip ? &_clip : 0;
}


void PainterBase::setClip(const Gfx::RectF& clip)
{
    _clip = clip;
    _hasClip = true;

    if(_canvas)
    {
        if( clip.isNull() )
            _canvas->resetClip();
        else
            _canvas->setClip(clip);
    }
}


void PainterBase::resetClip()
{
    _clip = Gfx::RectF();
    _hasClip = false;

    if(_canvas)
        _canvas->resetClip();
}


void PainterBase::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( _paint.pen().size() == 0 )
        return;

    if(_canvas)
        _canvas->drawLine(from, to);
}


void PainterBase::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    if( _paint.pen().size() == 0 )
        return;

    if(_canvas)
        _canvas->drawPolyline(points, pointCount);
}


void PainterBase::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    if(_canvas)
        _canvas->fillPolygon(points, pointCount);
}


void PainterBase::drawRect(const Gfx::RectF& rect)
{
    if(_canvas)
        _canvas->drawRect(rect);
}


void PainterBase::fillRect(const Gfx::RectF& rect)
{
    if(_canvas)
        _canvas->fillRect(rect);
}


void PainterBase::drawCircle(const PointF& topLeft, double diameter)
{
    drawEllipse(topLeft, SizeF(diameter, diameter));
}


void PainterBase::fillCircle(const PointF& topLeft, double diameter)
{
    fillEllipse(topLeft, SizeF(diameter, diameter));
}


void PainterBase::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_canvas)
        _canvas->drawEllipse(topLeft, size);
}


void PainterBase::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_canvas)
        _canvas->fillEllipse(topLeft, size);
}


void PainterBase::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void PainterBase::fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void PainterBase::fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


const Gfx::Path& PainterBase::path() const
{
    return _path;
}


void PainterBase::setPath(const Path& path)
{
    _path = path;

    if(_canvas)
        _canvas->setPath(path);
}


void PainterBase::drawPath()
{
    if(_canvas)
        _canvas->drawPath();
}


void PainterBase::fillPath()
{
    if(_canvas)
        _canvas->fillPath();
}


void PainterBase::drawPath(const Path& path)
{
    if(_canvas)
        _canvas->drawPath(path);
}


void PainterBase::fillPath(const Path& path)
{
    if(_canvas)
        _canvas->fillPath(path);
}


const Gfx::FontMetrics& PainterBase::fontMetrics() const
{
    if(_canvas)
        return _canvas->fontMetrics();

    static const Gfx::FontMetrics empty;
    return empty;
}


Gfx::TextMetrics PainterBase::textMetrics(const Pt::String& text) const
{
    if(_canvas)
        return _canvas->textMetrics(text);

    return Gfx::TextMetrics();
}


void PainterBase::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if(_canvas)
        _canvas->drawText(to, text);
}


void PainterBase::drawText(const Gfx::PointF& to, const Pt::String& text, 
                           const Gfx::Transform& transform)
{
    if(_canvas)
        _canvas->drawText(to, text, &transform);
}


void PainterBase::drawImage(const Gfx::PointF& to, 
                            const Gfx::Image& image)
{
    if(_canvas)
        _canvas->drawImage(to, image);
}


void PainterBase::drawImage(const Gfx::PointF& to,
                            const Gfx::Image& image, 
                            const Gfx::RectF& imageRect)
{
    if(_canvas)
        _canvas->drawImage(to, image, &imageRect);
}

} // namespace

} // namespace
