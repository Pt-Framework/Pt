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

///////////////////////////////////////////////////////////////////////
// Paint
///////////////////////////////////////////////////////////////////////

Paint::Paint()
{
}


Paint::~Paint()
{
}


const Gfx::CompositionMode& Paint::compositionMode() const
{
    return _compositionMode;
}


void Paint::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


const RectF& Paint::clip() const
{
    return _clip;
}


void Paint::setClip(const Gfx::RectF& clip)
{
    _clip = clip;
}


void Paint::resetClip()
{
    // TODO: RECT-NULL
    _clip = Gfx::RectF();
}


const Gfx::Pen& Paint::pen() const
{
    return _pen;
}


void Paint::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
}


const Gfx::Brush& Paint::brush() const
{
    return _brush;
}


void Paint::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


const Gfx::Font& Paint::font() const
{
    return _font;
}


void Paint::setFont(const Gfx::Font& font)
{
    _font = font;

}

///////////////////////////////////////////////////////////////////////
// PaintContextPtr
///////////////////////////////////////////////////////////////////////

PaintContextPtr::PaintContextPtr()
: _canvas(0)
, _paint(0)
{ 
}


PaintContextPtr::PaintContextPtr(Canvas& canvas, PaintContext* paint)
: _canvas(&canvas)
, _paint(paint)
{ 
    _canvas->attachPaintContext(*this);
}


PaintContextPtr::PaintContextPtr(MoveRef ref)
: _canvas(ref.canvas)
, _paint(ref.paint) 
{ 
    _canvas->attachPaintContext(*this);
}


PaintContextPtr::~PaintContextPtr()
{
    if(_canvas)
    {
        _canvas->detachPaintContext(*this);
        _canvas = 0;
    }

    delete _paint;
}


PaintContextPtr& PaintContextPtr::operator =(MoveRef ref)
{
    if(_canvas)
    {
        if(_paint)
            _paint->reset();

        _canvas->detachPaintContext(*this);
        _canvas = 0;               
    }

    _canvas = ref.canvas;

    if(_canvas)
        _canvas->attachPaintContext(*this);
    
    if(_paint != ref.paint) 
        delete _paint;

    _paint = ref.paint;

    return *this;
}


PaintContextPtr::operator MoveRef()
{
    Canvas* canvas = _canvas;

    if(_canvas)
    {
        _canvas->releasePaintContext();
        _canvas = 0;
    }

    PaintContext* paint = _paint;
    _paint = 0;

    return MoveRef(canvas, paint);
}


PaintContext* PaintContextPtr::release()
{
    PaintContext* paint = _paint;

    if(_paint)
    {
        _paint->reset();
        _paint = 0;
    }

    if(_canvas)
    {
        _canvas->detachPaintContext(*this);
        _canvas = 0;
    }

    return paint;
}


void PaintContextPtr::reset()
{
    if(_canvas)
    {
        if(_paint)
            _paint->reset();
                
        _canvas->detachPaintContext(*this);
        _canvas = 0;               
    }
}


void PaintContextPtr::onDetachCanvas(Canvas& canvas)
{
    if(_canvas)
    {
        if(_paint)
            _paint->reset();
        
        _canvas = 0;
    }
}

///////////////////////////////////////////////////////////////////////
// PaintContext
///////////////////////////////////////////////////////////////////////

PaintContext::PaintContext()
{
}


PaintContext::~PaintContext()
{
}


void PaintContext::setPaint(const Paint& paint)
{
    onBeginPaint(paint);
}


void PaintContext::begin(const Paint& paint)
{
    onBeginPaint(paint);
}


void PaintContext::finish()
{
    onFinishPaint();

    double unbounded = std::numeric_limits<double>::max();

    _region.clear();
    _region.setSize( SizeF(unbounded, unbounded) );
}


void PaintContext::reset()
{
    finish();

    onResetPaint();
}


const RectF& PaintContext::region() const
{
    return _region;
}


const PointF& PaintContext::origin() const
{
    return _region.topLeft();
}


void PaintContext::setRegion(const RectF& r)
{
    _region = r;
}


void PaintContext::setCompositionMode(const Gfx::CompositionMode& mode)
{
    onSetCompositionMode(mode);
}


void PaintContext::setPen(const Pen& pen)
{
    onSetPen(pen);
}


void PaintContext::setBrush(const Brush& brush)
{
    onSetBrush(brush);
}


void PaintContext::setFont(const Gfx::Font& font)
{
    onSetFont(font);
}


void PaintContext::setClip(const RectF& rect)
{ 
    Gfx::RectF clip = rect;
    clip.shift( origin().x(), origin().y() );

    onSetClip(clip);
}


void PaintContext::resetClip()
{
    onResetClip();
}


void PaintContext::drawLine(const PointF& from, const PointF& to)
{   
    Gfx::Line line(*this, from, to);
    onDrawLine(line); 
}


void PaintContext::drawRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    onDrawRect(r);
}


void PaintContext::fillRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    onFillRect(r);
}


void PaintContext::drawPolyline(const Gfx::PointF* ps, const size_t n)
{
    Polyline line(*this, ps, n);
    onDrawPolyline(line);
}


void PaintContext::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    Polyline line(*this, ps, n);
    onFillPolygon(line);
}


void PaintContext::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();
    onDrawEllipse(p, size);
}


void PaintContext::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();
    onFillEllipse(p, size);
}


FontMetrics PaintContext::fontMetrics(const Pt::String& text) const
{
    return onGetFontMetrics(text);
}


void PaintContext::drawText(const PointF& to, const Pt::String& text)
{
    Pt::Gfx::PointF p = to + origin();
    onDrawText(p, text);
}


void PaintContext::drawText(const PointF& to, const Pt::String& text, const Transform& tf)
{
    Pt::Gfx::PointF p = to + origin(); 
    onDrawText(p, text, tf);
}


void PaintContext::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image)
{
    Pt::Gfx::PointF p = to + origin(); 
    onDrawImage(p, image);
}


void PaintContext::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image, 
                          const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();
    onDrawImage(p, image, rect);
}


void PaintContext::drawSurface(const Gfx::PointF& to, 
                            const Gfx::PaintSurface& surface)
{
    Pt::Gfx::PointF p = to + origin();
    onDrawSurface(p, surface);
}


void PaintContext::drawSurface(const Gfx::PointF& to,
                            const Gfx::PaintSurface& surface,
                            const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();
    onDrawSurface(p, surface, rect);
}

} // namespace

} // namespace
