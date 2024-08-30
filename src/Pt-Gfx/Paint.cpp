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
: _paint(0)
{ 
}


PaintContextPtr::PaintContextPtr(PaintContext* paint)
: _paint(paint)
{ 
}


PaintContextPtr::PaintContextPtr(MoveRef ref)
: _paint(ref.paint) 
{ 
}


PaintContextPtr::~PaintContextPtr()
{
    delete _paint;
}


PaintContextPtr& PaintContextPtr::operator =(MoveRef ref)
{  
    if(_paint != ref.paint) 
        delete _paint;

    _paint = ref.paint;

    return *this;
}


PaintContextPtr::operator MoveRef()
{
    PaintContext* paint = _paint;
    _paint = 0;

    return MoveRef(paint);
}

///////////////////////////////////////////////////////////////////////
// PaintContext
///////////////////////////////////////////////////////////////////////

PaintContext::PaintContext()
: _canvas(0)
{
}


PaintContext::~PaintContext()
{
    if(_canvas)
    {
        _canvas->detachPaint(*this);
        _canvas = 0;
    }
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


const Scaling& PaintContext::scaling() const
{
    return _scaling;
}


void PaintContext::onDetachCanvas(Canvas& canvas)
{
    if(_canvas)
    {
        _canvas = 0;
    }
}


void PaintContext::setCanvas(Canvas& canvas)
{
    if(_canvas == &canvas)
        return;

    if(_canvas)
    {        
        _canvas->detachPaint(*this);
        _canvas = 0;
    }

    canvas.attachPaint(*this);
    _canvas = &canvas;

    _scaling = canvas.scaling();
}


void PaintContext::reset()
{
    double unbounded = std::numeric_limits<double>::max();

    _region.clear();
    _region.setSize( SizeF(unbounded, unbounded) );

    if(_canvas)
    {        
        _canvas->detachPaint(*this);
        _canvas = 0;
    }
}


void PaintContext::setCompositionMode(const Gfx::CompositionMode& mode)
{
    onSetCompositionMode(mode);

    if(_canvas)
        _canvas->setCompositionMode(mode);
}


void PaintContext::setPen(const Pen& pen)
{
    onSetPen(pen);

    // notify that pen has changed
    if(_canvas)
        _canvas->setPen(pen);
}


void PaintContext::setBrush(const Brush& brush)
{
    onSetBrush(brush);

    if(_canvas)
        _canvas->setBrush(brush);
}


void PaintContext::setFont(const Gfx::Font& font)
{
    onSetFont(font);

    if(_canvas)
        _canvas->setFont(font);
}


void PaintContext::setClip(const RectF& rect)
{ 
    Gfx::RectF clip = rect;
    clip.shift( origin().x(), origin().y() );

    onSetClip(clip);

    if(_canvas)
        _canvas->setClip(clip);
}


void PaintContext::resetClip()
{
    onResetClip();

    if(_canvas)
        _canvas->resetClip();
}


void PaintContext::drawLine(const PointF& from, const PointF& to)
{   
    Gfx::Line line(*this, from, to);
    if(_canvas)
        _canvas->drawLine(line);
}


void PaintContext::drawRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    if(_canvas)
        _canvas->drawRect(r);
}


void PaintContext::fillRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    if(_canvas)
        _canvas->fillRect(r);
}


void PaintContext::drawPolyline(const Gfx::PointF* ps, const size_t n)
{
    Polyline line(*this, ps, n);
    if(_canvas)
        _canvas->drawPolyline(line);
}


void PaintContext::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    Polyline line(*this, ps, n);
    if(_canvas)
        _canvas->fillPolygon(line);
}


void PaintContext::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();
    if(_canvas)
        _canvas->drawEllipse(p, size);
}


void PaintContext::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();
    if(_canvas)
        _canvas->fillEllipse(p, size);
}


FontMetrics PaintContext::fontMetrics(const Pt::String& text) const
{
    if(_canvas)
        return _canvas->fontMetrics(text);

    return FontMetrics();
}


void PaintContext::drawText(const PointF& to, const Pt::String& text)
{
    Pt::Gfx::PointF p = to + origin();
    if(_canvas)
        _canvas->drawText(p, text);
}


void PaintContext::drawText(const PointF& to, const Pt::String& text, const Transform& tf)
{
    Pt::Gfx::PointF p = to + origin(); 
    if(_canvas)
        _canvas->drawText(p, text, tf);
}


void PaintContext::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image)
{
    Pt::Gfx::PointF p = to + origin(); 
    if(_canvas)
        _canvas->drawImage(p, image);
}


void PaintContext::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image, 
                          const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();
    if(_canvas)
        _canvas->drawImage(p, image, rect);
}


void PaintContext::drawSurface(const Gfx::PointF& to, 
                            const Gfx::PaintSurface& surface)
{
    Pt::Gfx::PointF p = to + origin();
    if(_canvas)
        _canvas->drawSurface(p, surface);
}


void PaintContext::drawSurface(const Gfx::PointF& to,
                            const Gfx::PaintSurface& surface,
                            const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();
    if(_canvas)
        _canvas->drawSurface(p, surface, rect);
}

} // namespace

} // namespace
