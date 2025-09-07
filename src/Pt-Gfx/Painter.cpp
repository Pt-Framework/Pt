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
#include <Pt/Gfx/PaintLayer.h>

namespace Pt {

namespace Gfx {

Painter::Painter()
: _surface(0)
, _paintContext(0)
{
}


Painter::Painter(PaintSurface& surface)
: _surface(0)
, _paintContext(0)

{
    begin(surface);
}


Painter::Painter(PaintLayer& layer)
: _surface(0)
, _paintContext(0)

{
    begin(layer);
}


Painter::~Painter()
{
    // NOTE: finish() is also possible
    //finish();
    
    if( _surface )
    {
        _surface->detachPainter(*this);
        _surface = 0;
    }

    delete _paintContext;
}


void Painter::begin(PaintSurface& surface)
{
    if(_surface == &surface)
        return;

    finish();

    surface.attachPainter(*this);
    _surface = &surface;
   
    PaintContext* reuse = _paintContext;
    _paintContext = surface.getContext(reuse);
   
    if(reuse != _paintContext)
    {
        delete reuse;

        // initialize new paint context
        if(_paintContext)
        {
            _paintContext->setCompositionMode( _paint.compositionMode() );
            _paintContext->setPen( _paint.pen() );
            _paintContext->setBrush( _paint.brush() );
            _paintContext->setFont( _paint.font() );
            _paintContext->setPath( _paint.path() );

            const Gfx::RectF* clip = _paint.clip();
            if( ! clip )
                _paintContext->resetClip();
            else
                _paintContext->setClip(*clip);
        }
    }

    if(_paintContext)
        _paintContext->beginPaint(_paint);
}


void Painter::begin(PaintLayer& layer)
{
    PaintSurface* surface = layer.surface();
    if(surface)
        begin(*surface);
}


void Painter::finish()
{
    if(_paintContext)
        _paintContext->finishPaint();

    if( _surface )
    {
        _surface->detachPainter(*this);
        _surface = 0;
    }
}


void Painter::onDetachSurface(PaintSurface& surface)
{
    if(_surface)
        _surface = 0;
}


const Gfx::ImageFormat& Painter::format() const
{
    if(_paintContext)
        return _paintContext->format();

    return ImageFormat::argb32();
}


const Scaling& Painter::scaling() const
{
    if(_paintContext)
        return _paintContext->scaling();

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


const RectF* Painter::clip() const
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


void Painter::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( _paint.pen().size() == 0 )
        return;

    if(_paintContext)
        _paintContext->drawLine(from, to);
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


void Painter::beginPath()
{
    _paint.setPath( Gfx::Path() );

    if(_paintContext)
        _paintContext->beginPath();
}


void Painter::moveTo(const PointF& to)
{
    _paint.path().moveTo(to);

    if(_paintContext)
        _paintContext->moveTo(to);
}


void Painter::lineTo(const PointF& to)
{
    _paint.path().lineTo(to);

    if(_paintContext)
        _paintContext->lineTo(to);
}


void Painter::curveTo(const PointF& cp, const PointF& to)
{
    _paint.path().curveTo(cp, to);

    if(_paintContext)
        _paintContext->curveTo(cp, to);
}


void Painter::curveTo(const PointF& cp1, const PointF& cp2, const PointF& to)
{
    _paint.path().curveTo(cp1, cp2, to);

    if(_paintContext)
        _paintContext->curveTo(cp1, cp2, to);
}


void Painter::closePath()
{
    _paint.path().close();

    if(_paintContext)
        _paintContext->closePath();
}


void Painter::setPath(const Path& path)
{
    _paint.setPath(path);

    if(_paintContext)
        _paintContext->setPath(path);
}


void Painter::drawPath()
{
    if(_paintContext)
        _paintContext->drawPath( _paint.path() );
}


void Painter::fillPath()
{
    if(_paintContext)
        _paintContext->fillPath( _paint.path() );
}


Gfx::TextMetrics Painter::textMetrics(const Pt::String& text) const
{
    if(_paintContext)
        return _paintContext->textMetrics(text);

    return Gfx::TextMetrics();
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if(_paintContext)
        _paintContext->drawText(to, text);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text, 
                       const Gfx::Transform& transform)
{
    if(_paintContext)
        _paintContext->drawText(to, text, &transform);
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
        _paintContext->drawImage(to, image, &imageRect);
}


void Painter::drawLayer(const Gfx::PointF& to, 
                        const PaintLayer& layer)
{
    if(_paintContext)
    {
        bool isCompatible = _paintContext->drawLayer(to, layer);
        if( ! isCompatible )
        {
            PaintSurface* surface = _surface;
            layer.draw(*surface, _paint, to);
            begin(*surface);
        }
    }
}


void Painter::drawLayer(const Gfx::PointF& to, 
                        const PaintLayer& layer, 
                        const Gfx::RectF& rect)
{
    if(_paintContext)
    {
        bool isCompatible = _paintContext->drawLayer(to, layer, &rect);
        if( ! isCompatible )
        {
            PaintSurface* surface = _surface;
            layer.draw(*surface, _paint, to, &rect);
            begin(*surface);
        }
    }
}

} // namespace

} // namespace
