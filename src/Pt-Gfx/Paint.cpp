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
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Algorithm.h>

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
#ifndef PT_HMI_CANVAS_PAINT
    if(_canvas)
    {
        _canvas->onDetachPaint(*this);
        _canvas = 0;
    }
#else
    if(_canvas)
    {
        _canvas->detachPaint(*this);
        _canvas = 0;
    }
#endif

    if(_painter)
    {
        _painter->detachPaint(*this);
        _painter = 0;
    }
}

#ifndef PT_HMI_CANVAS_PAINT
void PaintData::attachCanvas(Canvas& canvas)
{
    if(_canvas)
    {
        _canvas->onDetachPaint(*this);
        _canvas = 0;
    }

    _canvas = &canvas;

    const Scaling& scaling = _canvas->scaling();
    if(_scaling != scaling)
    {
        _scaling = scaling;
        _invalid = true;
    }
}


void PaintData::detachCanvas(Canvas& canvas)
{
    if(_canvas)
    {
        _canvas = 0;
    }
}

#else

void PaintData::onDetachCanvas(Canvas& canvas)
{
    if(_canvas)
    {
        // onFinish();
        _canvas = 0;
    }
}


void PaintData::move(Canvas* canvas)
{
    if(_canvas)
    {
        _canvas->detachPaint(*this);
        onDetachCanvas(*_canvas);
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

    return this;
}
#endif


void PaintData::onDetachPainter(Painter& painter)
{
    if(_painter)
    {
        onSetPainter(0);
        _painter = 0;
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
        onSetPainter(_painter);

        _invalid = false;
    }

    onBeginPaint(*_painter);
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
    onSetCompositionMode(mode);
}


void PaintData::setPen(const Pen& pen)
{
    onSetPen(pen);
}


void PaintData::setBrush(const Brush& brush)
{
    onSetBrush(brush);
}


void PaintData::setFont(const Gfx::Font& font)
{
    onSetFont(font);
}


void PaintData::setClip(const RectF& rect)
{ 
    Gfx::RectF clip = rect;
    clip.shift( origin().x(), origin().y() );

    onSetClip(clip);
}


void PaintData::resetClip()
{
    onResetClip();
}


void PaintData::drawLine(const PointF& from, const PointF& to)
{   
    Gfx::Line line(*this, from, to);
    onDrawLine(line); 
}


void PaintData::drawRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    onDrawRect(r);
}


void PaintData::fillRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    onFillRect(r);
}


void PaintData::drawPolyline(const Gfx::PointF* ps, const size_t n)
{
    Polyline line(*this, ps, n);
    onDrawPolyline(line);
}


void PaintData::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    Polyline line(*this, ps, n);
    onFillPolygon(line);
}


void PaintData::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();
    onDrawEllipse(p, size);
}


void PaintData::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();
    onFillEllipse(p, size);
}


FontMetrics PaintData::fontMetrics(const Pt::String& text) const
{
    return onGetFontMetrics(text);
}


void PaintData::drawText(const PointF& to, const Pt::String& text)
{
    Pt::Gfx::PointF p = to + origin();
    onDrawText(p, text);
}


void PaintData::drawText(const PointF& to, const Pt::String& text, const Transform& tf)
{
    Pt::Gfx::PointF p = to + origin(); 
    onDrawText(p, text, tf);
}


void PaintData::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image)
{
    Pt::Gfx::PointF p = to + origin(); 
    onDrawImage(p, image);
}


void PaintData::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image, 
                          const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();
    onDrawImage(p, image, rect);
}


void PaintData::drawSurface(const Gfx::PointF& to, 
                            const Gfx::PaintSurface& surface)
{
    Pt::Gfx::PointF p = to + origin();
    onDrawSurface(p, surface);
}


void PaintData::drawSurface(const Gfx::PointF& to,
                            const Gfx::PaintSurface& surface,
                            const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();
    onDrawSurface(p, surface, rect);
}


void PaintData::onDrawSurface(const Gfx::PointF& to, 
                              const Gfx::PaintSurface& surface)
{
    Pt::Gfx::Image image = surface.toImage();
    if( image.format() == format() )
    {
        drawImage(to, image);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest);
}


void PaintData::onDrawSurface(const Gfx::PointF& to,
                              const Gfx::PaintSurface& surface,
                              const Gfx::RectF& rect)
{
    Pt::Gfx::Image image = surface.toImage();
    if( image.format() == format() )
    {
        drawImage(to, image, rect);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest, rect);
}

} // namespace

} // namespace
