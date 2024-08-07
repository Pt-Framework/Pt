/* Copyright (C) 2015-2024 Marc Boris Duerner
   Copyright (C) 2015-2024 Laurentiu-Gheorghe Crisan

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

#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Canvas.h>

#include <limits>

namespace Pt {

namespace Gfx {

PaintData::PaintData()
: _painter(0)
, _canvas(0)
, _invalid(false)
{
}


PaintData::~PaintData()
{
    if(_canvas)
    {
        _canvas->detachPaint(*this);
        _canvas = 0;
    }

    if(_painter)
    {
        _painter->detachPaint(*this);
        _painter = 0;
    }
}


void PaintData::onDetachCanvas(Canvas& canvas)
{
    if(_canvas)
    {
        _canvas = 0;
    }
}


void PaintData::onDetachPainter(Painter& painter)
{
    if(_painter)
    {
        onSetPainter(0);
        _painter = 0;
    }
}

/*
void PaintData::attachCanvas(Canvas& canvas)
{
    if(_canvas)
    {
        _canvas->onDetachPaint(*this);
        _canvas = 0;
    }

    _canvas = &canvas;
}


void PaintData::detachCanvas(Canvas& painter)
{
    if(_canvas)
    {
        _canvas = 0;
    }
}
*/


void PaintData::setCanvas(Canvas* canvas)
{
    if(_canvas)
    {
        _canvas->detachPaint(*this);
        _canvas = 0;
    }

    if(canvas)
    {
        canvas->attachPaint(*this);
        _canvas = canvas;

        const Scaling& scaling = _canvas->scaling();
        if(_scaling != scaling)
        {
            _scaling = scaling;
            _invalid = true;
        }
    }
}


void PaintData::begin(Painter& painter)
{
    if(_painter != &painter)
    {
        if(_painter)
        {
            _painter->detachPaint(*this);
            _painter = 0;               
        }

        painter.attachPaint(*this);
        _painter = &painter;
        _invalid = true;
    }

    if(_invalid)
    {
        //onSetPen( _painter->pen() );
        onSetCompositionMode( _painter->compositionMode() );
        onSetBrush( _painter->brush() );
        onSetFont( _painter->font() );

        Gfx::RectF clip = _painter->clip();
        if( clip.isNull() )
        {
            onResetClip();
        }
        else
        {
            clip.shift( origin().x(), origin().y() );
            onSetClip(clip);
        }

        onSetPainter(_painter);

        _invalid = false;
    }

    onBeginPaint(_painter);

    if(_painter && _canvas)
    {
        _canvas->setCompositionMode( _painter->compositionMode() );
        //_canvas->setPen( _painter->pen() );
        _canvas->setBrush( _painter->brush() );
        _canvas->setFont( _painter->font() );

        Gfx::RectF clip = _painter->clip();
        if( clip.isNull() )
        {
            _canvas->resetClip();
        }
        else
        {
          clip.shift( origin().x(), origin().y() );
          _canvas->setClip(clip);
        }
    }
}


void PaintData::finish()
{
    double unbounded = std::numeric_limits<double>::max();

    _region.clear();
    _region.setSize( SizeF(unbounded, unbounded) );
}


const RectF& PaintData::region() const
{
    return _region;
}


const PointF& PaintData::origin() const
{
    return _region.topLeft();
}


void PaintData::setRegion(const RectF& r)
{
    _region = r;
}


const ImageFormat& PaintData::format() const
{
    return _canvas ? _canvas->format() : ImageFormat::argb32();
}


const Scaling& PaintData::scaling() const
{
    return _scaling;
}


void PaintData::setCompositionMode(const Gfx::CompositionMode& mode)
{
    if(_canvas)
        _canvas->setCompositionMode(mode);
}


void PaintData::setPen(const Pen& pen)
{
    onSetPen(pen);

    //if(_canvas)
    //    _canvas->setPen(pen);
}


void PaintData::setBrush(const Brush& brush)
{
    onSetBrush(brush);

    if(_canvas)
        _canvas->setBrush(brush);
}


void PaintData::setFont(const Gfx::Font& font)
{
    onSetFont(font);

    if(_canvas)
        _canvas->setFont(font);
}


void PaintData::setClip(const RectF& rect)
{ 
    Gfx::RectF clip = rect;
    clip.shift( origin().x(), origin().y() );

    onSetClip(clip);

    if(_canvas)
        _canvas->setClip(clip);
}


void PaintData::resetClip()
{
    onResetClip();

    if(_canvas)
        _canvas->resetClip();
}


void PaintData::drawLine(const PointF& from, const PointF& to)
{   
    if(_canvas)
    {
        Gfx::Line line(*this, from, to);
        _canvas->drawLine(line);
    }
}


void PaintData::drawRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    if(_canvas)
        _canvas->drawRect(r);
}


void PaintData::fillRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    if(_canvas)
        _canvas->fillRect(r);
}


void PaintData::drawPolyline(const Gfx::PointF* ps, const size_t n)
{
    if(_canvas)
    {
        Polyline line(*this, ps, n);
        _canvas->drawPolyline(line);
    }
}


void PaintData::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if(_canvas)
    {
        Polyline line(*this, ps, n);
        _canvas->fillPolygon(line);
    }
}


void PaintData::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();

    if(_canvas)
        _canvas->drawEllipse(p, size);
}


void PaintData::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();

    if(_canvas)
        _canvas->fillEllipse(p, size);
}


FontMetrics PaintData::fontMetrics(const Pt::String& text) const
{
    if(_canvas)
        return _canvas->fontMetrics(text);

    return FontMetrics();
}


void PaintData::drawText(const PointF& to, const Pt::String& text)
{
    Pt::Gfx::PointF p = to + origin(); 
    if(_canvas)
        _canvas->drawText(p, text);
}


void PaintData::drawText(const PointF& to, const Pt::String& text, const Transform& t)
{
    Pt::Gfx::PointF p = to + origin(); 
    if(_canvas)
        _canvas->drawText(p, text, t);
}


void PaintData::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image)
{
    Pt::Gfx::PointF p = to + origin(); 
    if(_canvas)
        _canvas->drawImage(p, image);
}


void PaintData::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image, 
                          const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();

    if(_canvas)
        _canvas->drawImage(p, image, rect);
}


void PaintData::drawSurface(const Gfx::PointF& to, 
                            const Gfx::PaintSurface& surface)
{
    Pt::Gfx::PointF p = to + origin();
    if(_canvas)
        _canvas->drawSurface(p, surface);
}


void PaintData::drawSurface(const Gfx::PointF& to,
                            const Gfx::PaintSurface& surface,
                            const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();

    if(_canvas)
        _canvas->drawSurface(p, surface, rect);
}

} // namespace

} // namespace
