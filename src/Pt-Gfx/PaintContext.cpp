/* Copyright (C) 2015-2024 Laurentiu-Gheorghe Crisan

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

#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/Canvas.h>

#include <limits>

namespace Pt {

namespace Gfx {

PaintContext::PaintContext()
: _canvas(0)
, _active(0)
, _hasClip(false)
{
}


PaintContext::~PaintContext()
{
    if(_canvas)
    {
        _canvas->onDetachPaint(*this);
        _canvas = 0;
        _active = 0;
    }
}


void PaintContext::attachCanvas(Canvas& canvas)
{
    resetPaint();
    
    const double unbounded = std::numeric_limits<double>::max();
    _region.setSize( SizeF(unbounded, unbounded) );

    _scaling = canvas.scaling();

    Gfx::Transform tx;
    tx.scale( _scaling.scaleFactor(), _scaling.scaleFactor() );
    _tx = tx;

    _canvas = &canvas;
}


void PaintContext::detachCanvas(Canvas& canvas)
{
    _region.clear();

    if(_canvas)
    {
        if(_active)
            onFinishPaint();
        
        onResetPaint();
        _canvas = 0;
        _active = 0;
    }
}


const PointF& PaintContext::origin() const
{
    return _region.topLeft();
}


const RectF& PaintContext::region() const
{
    return _region;
}


void PaintContext::setRegion(const RectF& r)
{
    Gfx::Transform tx;
    tx.translate( r.x(), r.y() );
    tx.scale( _scaling.scaleFactor(), _scaling.scaleFactor() );
    
    _region = r;
    _tx = tx;
}


const Scaling& PaintContext::scaling() const
{
    return _scaling;
}


const Gfx::Transform& PaintContext::transform() const
{
    return _tx;
}


void PaintContext::beginPaint(const Gfx::Paint& paint)
{
    if(_canvas)
    {
        _canvas->onCompositionModeChanged();
        _canvas->onPenChanged();
        _canvas->onBrushChanged();
        _canvas->onFontChanged();

        if( ! _hasClip )
        {
            onSetClip(0);
        }
        else
        {
            Gfx::RectF clip = _clip;
            clip.shift( origin().x(), origin().y() );
            onSetClip(&clip);
        }

        _canvas->onClipChanged();
        
        _active = _canvas;
        onBeginPaint(paint);
    }
}


void PaintContext::finishPaint()
{
    if(_active)
    {
        onFinishPaint();
        _active = 0;
    }
}


bool PaintContext::isActive() const
{
    return _active != 0;
}


void PaintContext::resetPaint()
{
    finishPaint();

    _region.clear();
    
    Gfx::Transform tx;
    tx.scale( _scaling.scaleFactor(), _scaling.scaleFactor() );
    _tx = tx;

    if(_canvas)
    {
        _canvas->onDetachPaint(*this);

        onResetPaint();
        _canvas = 0;
        _active = 0;
    }
}


void PaintContext::setCompositionMode(const Gfx::CompositionMode& mode)
{
    onSetCompositionMode(mode);

    if(_active)
    {
        _active->onCompositionModeChanged();
    }
}


void PaintContext::setPen(const Pen& pen)
{
    onSetPen(pen);

    if(_active)
    {
        _active->onPenChanged();
    }
}


void PaintContext::setBrush(const Brush& brush)
{
    onSetBrush(brush);

    if(_active)
    {
        _active->onBrushChanged();
    }
}


void PaintContext::setFont(const Gfx::Font& font)
{
    onSetFont(font);

    if(_active)
    {
        _active->onFontChanged();
    }
}


void PaintContext::setClip(const RectF& rect)
{ 
    if(_active)
    {
        Gfx::RectF clip = rect;
        clip.shift( origin().x(), origin().y() );
        onSetClip(&clip);

        _active->onClipChanged();
    }

    _clip = rect;
    _hasClip = true;
}


void PaintContext::resetClip()
{
    if(_active)
    {
        onSetClip(0);
        _active->onClipChanged();
    }

    _clip.clear();
    _hasClip = false;
}


void PaintContext::drawLine(const PointF& from, const PointF& to)
{   
    if( ! _active )
        return;

    Gfx::PointF p0 = from + origin();
    Gfx::PointF p1 = to + origin();

    p0 = scaling().toPhysical(p0);
    p1 = scaling().toPhysical(p1);

    _active->drawLine(p0, p1);
}


void PaintContext::drawPolyline(const Gfx::PointF* ps, const size_t n)
{
    Polyline line(*this, ps, n);
    
    if(_active)
        _active->drawPolyline(line);
}


void PaintContext::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    Polyline line(*this, ps, n);
    
    if(_active)
        _active->fillPolygon(line);
}


void PaintContext::drawRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    r.shift( origin().x(), origin().y() ); 
    
    r = scaling().toPhysical(r); 

    if(_active)
        _active->drawRect(r);
}


void PaintContext::fillRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    r.shift( origin().x(), 
              origin().y() );

    r = scaling().toPhysical(r);

    if(_active)
        _active->fillRect(r);
}


void PaintContext::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();
    p = _scaling.toPhysical(p);
    
    Gfx::SizeF s = _scaling.toPhysical(size);

    if(_active)
        _active->drawEllipse(p, s);
}


void PaintContext::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();
    p = _scaling.toPhysical(p);
    
    Gfx::SizeF s = _scaling.toPhysical(size);

    if(_active)
        _active->fillEllipse(p, s);
}


void PaintContext::setPath(const Path& path)
{
    onSetPath(path);
}


void PaintContext::drawPath()
{
    onDrawPath();
}

void PaintContext::fillPath()
{
    onFillPath();
}


TextMetrics PaintContext::textMetrics(const Pt::String& text) const
{
    if(_active)
        return _active->textMetrics(text);

    return TextMetrics();
}


void PaintContext::drawText(const PointF& to, const Pt::String& text, 
                            const Transform* transform)
{
    if(_active)
    {
        Pt::Gfx::PointF p = to + origin();
        p = scaling().toPhysical(p);

        Gfx::Transform t = transform ? *transform : Gfx::Transform();
        double scaleFactor = scaling().scaleFactor();
        t.scale(scaleFactor, scaleFactor);

        _active->drawText(p, text, &t);
    }
}


void PaintContext::drawImage(const Gfx::PointF& to, 
                             const Gfx::Image& image, 
                             const Gfx::RectF* rect)
{
    Pt::Gfx::PointF p = to + origin();

    if(_active)
    {
        _active->drawImage(p, image, rect);
    }
}


bool PaintContext::drawLayer(const Gfx::PointF& to,
                             const Gfx::PaintLayer& layer,
                             const Gfx::RectF* rect)
{
    if(_active)
    {
        Pt::Gfx::PointF p = to + origin();
        return _active->drawLayer(p, layer, rect);
    }

    return true;
}

} // namespace

} // namespace
