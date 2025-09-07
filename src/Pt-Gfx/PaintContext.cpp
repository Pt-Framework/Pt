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
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Algorithm.h>

namespace Pt {

namespace Gfx {

PaintContext::PaintContext()
: _surface(0)
, _active(0)
{
}


PaintContext::~PaintContext()
{
    if(_surface)
    {
        _surface->onDetachContext(*this);
        _surface = 0;
        _active = 0;
    }
}


void PaintContext::attachSurface(PaintSurface& surface)
{
    finishPaint();
    
    _surface = &surface;
}


void PaintContext::detachSurface(PaintSurface& surface)
{
    if(_surface)
    {
        onResetPaint();
        _surface = 0;
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


void PaintContext::setScaling(const Scaling& scaling)
{
    Gfx::Transform tx;
    tx.translate( _region.x(), _region.y() );
    tx.scale( scaling.scaleFactor(), scaling.scaleFactor() );

    _scaling = scaling;
    _tx = tx;
}


const Gfx::Transform& PaintContext::transform() const
{
    return _tx;
}


const Gfx::ImageFormat& PaintContext::format() const
{
    if(_surface)
        return _surface->format();

    return ImageFormat::argb32();
}


bool PaintContext::isActive() const
{
    return _active != 0;
}


void PaintContext::beginPaint(const Gfx::Paint& paint)
{
    if(_surface)
    {       
        _active = _surface;
        onBeginPaint(paint);

        // TODO: apply in draw* functions and keep a state which attributes
        //       are dirty and need to be applied

        onApplyCompositionMode( paint.compositionMode() );
        onApplyPen( paint.pen() );
        onApplyBrush( paint.brush() );
        onApplyFont( paint.font() );
        onApplyClip( paint.clip() );
    }
}


void PaintContext::finishPaint()
{
    if(_surface)
    {
        _surface->onDetachContext(*this);

        onResetPaint();
        _surface = 0;
        _active = 0;
    }
}


void PaintContext::onBeginPaint(const Gfx::Paint& paint)
{
}


void PaintContext::onResetPaint()
{
}


void PaintContext::setCompositionMode(const Gfx::CompositionMode& mode)
{
    onSetCompositionMode(mode);

    if(_active)
    {
        onApplyCompositionMode(mode);
    }
}


void PaintContext::setPen(const Pen& pen)
{
    onSetPen(pen);

    if(_active)
    {
        onApplyPen(pen);
    }
}


void PaintContext::setBrush(const Brush& brush)
{
    onSetBrush(brush);

    if(_active)
    {
        onApplyBrush(brush);
    }
}


void PaintContext::setFont(const Gfx::Font& font)
{
    onSetFont(font);

    if(_active)
    {
        onApplyFont(font);
    }
}


void PaintContext::setClip(const RectF& rect)
{
    onSetClip(&rect);

    if(_active)
    {
        onApplyClip(&rect);
    }
}


void PaintContext::resetClip()
{
    onSetClip(0);

    if(_active)
    {
        onApplyClip(0);
    }
}


void PaintContext::drawLine(const PointF& from, const PointF& to)
{   
    if( _active )
        onDrawLine(from, to);
}


void PaintContext::drawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if(_active)
        onDrawPolyline(pts, n);
}


void PaintContext::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if(_active)
        onFillPolygon(ps, n);
}


void PaintContext::drawRect(const Gfx::RectF& rect)
{
    if(_active)
        onDrawRect(rect);
}

        
void PaintContext::fillRect(const Gfx::RectF& rect)
{
    if(_active)
        onFillRect(rect);
}


void PaintContext::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_active)
        onDrawEllipse(topLeft, size);
}


void PaintContext::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_active)
        onFillEllipse(topLeft, size);
}


void PaintContext::beginPath()
{
    if(_active)
        onBeginPath();
}


void PaintContext::moveTo(const PointF& to)
{
    if(_active)
        onMoveTo(to);
}


void PaintContext::lineTo(const PointF& to)
{
    if(_active)
        onLineTo(to);
}


void PaintContext::curveTo(const PointF &cp, const PointF& to)
{
    if(_active)
        onCurveTo(cp, to);
}


void PaintContext::curveTo(const PointF &cp1, const PointF &cp2, 
                           const PointF& to)
{
    if(_active)
        onCurveTo(cp1, cp2, to);
}


void PaintContext::closePath()
{
    if(_active)
        onClosePath();
}


void PaintContext::setPath(const Path& path)
{
    if(_active)
        onSetPath(path);
}


void PaintContext::drawPath(const Path& path)
{
    if(_active)
        onDrawPath(path);
}


void PaintContext::fillPath(const Path& path)
{
    if(_active)
        onFillPath(path);
}


TextMetrics PaintContext::textMetrics(const Pt::String& text) const
{
    if(_active)
        return onGetTextMetrics(text);

    return TextMetrics();
}


void PaintContext::drawText(const PointF& to, const Pt::String& text, 
                            const Transform* tform)
{
    if(_active)
        onDrawText(to, text, tform);
}


void PaintContext::drawImage(const Gfx::PointF& to, 
                             const Gfx::Image& image, 
                             const Gfx::RectF* rect)
{
    if(_active)
    {
        if( image.format() == _active->format() )
        {
            onDrawImage(to, image, rect);
        }
        else
        {
            Pt::Gfx::Image dest( _active->format(), image.width(), image.height() );
            Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
            onDrawImage(to, dest, rect);
        }
    }
}


bool PaintContext::drawLayer(const Gfx::PointF& to,
                             const Gfx::PaintLayer& layer,
                             const Gfx::RectF* rect)
{
    if(_active)
    {
        onDrawLayer(to, layer, rect);
    }

    return true;
}

} // namespace

} // namespace
