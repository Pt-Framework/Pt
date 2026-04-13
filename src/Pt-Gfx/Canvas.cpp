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

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/PaintSurface.h>

namespace Pt {

namespace Gfx {

Canvas::Canvas()
: _surface(0)
, _active(0)
, _dirty(0)
{
}


Canvas::~Canvas()
{
    if(_surface)
    {
        _surface->onDetachCanvas(*this);
        _surface = 0;
        _active = 0;
    }
}


void Canvas::attachSurface(PaintSurface& surface)
{
    finishPaint();
    
    _surface = &surface;
}


void Canvas::detachSurface(PaintSurface& surface)
{
    if(_surface == &surface)
    {
        onFinishPaint();
        _surface = 0;
        _active = 0;
    }
}


const PointF& Canvas::origin() const
{
    return _region.topLeft();
}


const RectF& Canvas::region() const
{
    return _region;
}


void Canvas::setRegion(const RectF& r)
{
    _region = r;
    updateTransform();
}


const Scaling& Canvas::scaling() const
{
    return _scaling;
}


void Canvas::setScaling(const Scaling& scaling)
{
    _scaling = scaling;
    updateTransform();
}


const Gfx::ImageFormat& Canvas::format() const
{
    if(_surface)
        return _surface->format();

    return ImageFormat::argb32();
}


const Gfx::Transform& Canvas::transform() const
{
    return _tx;
}


void Canvas::setTransform(const Gfx::Transform& tx)
{
    _userTx = tx;
    updateTransform();
}


void Canvas::resetTransform()
{
    _userTx = Transform();
    updateTransform();
}


void Canvas::beginPaint(const Gfx::Paint& paint)
{
    if(_surface)
    {       
        _active = _surface;
        onBeginPaint(paint);
        _dirty = DirtyAll;
    }
}


void Canvas::finishPaint()
{
    if(_surface)
    {
        onFinishPaint();

        _surface->onDetachCanvas(*this);
        _surface = 0;
        _active = 0;
        _dirty = 0;
    }
}


bool Canvas::isActive() const
{
    return _active != 0;
}


void Canvas::setCompositionMode(const Gfx::CompositionMode& mode)
{
    onSetCompositionMode(mode);

    if(_active)
        _dirty |= DirtyComposition;
}


void Canvas::setPen(const Pen& pen)
{
    onSetPen(pen);

    if(_active)
        _dirty |= DirtyPen;
}


void Canvas::setBrush(const Brush& brush)
{
    onSetBrush(brush);

    if(_active)
        _dirty |= DirtyBrush;
}


void Canvas::setFont(const Gfx::Font& font)
{
    onSetFont(font);

    if(_active)
        _dirty |= DirtyFont;
}


void Canvas::setClip(const RectF& rect)
{
    onSetClip(&rect);

    if(_active)
        _dirty |= DirtyClip;
}


void Canvas::resetClip()
{
    onSetClip(0);

    if(_active)
        _dirty |= DirtyClip;
}


void Canvas::drawLine(const PointF& from, const PointF& to)
{   
    if(_active)
    {
        applyState();
        onDrawLine(from, to);
    }
}


void Canvas::drawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if(_active)
    {
        applyState();
        onDrawPolyline(pts, n);
    }
}


void Canvas::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if(_active)
    {
        applyState();
        onFillPolygon(ps, n);
    }
}


void Canvas::drawRect(const Gfx::RectF& rect)
{
    if(_active)
    {
        applyState();
        onDrawRect(rect);
    }
}

        
void Canvas::fillRect(const Gfx::RectF& rect)
{
    if(_active)
    {
        applyState();
        onFillRect(rect);
    }
}


void Canvas::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_active)
    {
        applyState();
        onDrawEllipse(topLeft, size);
    }
}


void Canvas::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_active)
    {
        applyState();
        onFillEllipse(topLeft, size);
    }
}


void Canvas::setPath(const Path& path)
{
    onSetPath(path);
}


void Canvas::drawPath()
{
    if(_active)
    {
        applyState();
        onDrawPath();
    }
}


void Canvas::fillPath()
{
    if(_active)
    {
        applyState();
        onFillPath();
    }
}


void Canvas::drawPath(const Path& path)
{
    if(_active)
    {
        applyState();
        onDrawPath(path);
    }
}


void Canvas::fillPath(const Path& path)
{
    if(_active)
    {
        applyState();
        onFillPath(path);
    }
}


const FontMetrics& Canvas::fontMetrics() const
{
    return onGetFontMetrics();
}


TextMetrics Canvas::textMetrics(const Pt::String& text) const
{
    if(_active)
        return onGetTextMetrics(text);

    return TextMetrics();
}


void Canvas::drawText(const PointF& to, const Pt::String& text, 
                      const Transform* tform)
{
    if(_active)
    {
        applyState();
        onDrawText(to, text, tform);
    }
}


void Canvas::drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image, 
                       const Gfx::RectF* rect)
{
    if(_active)
    {
        applyState();

        if( image.format() == _active->format() )
        {
            onDrawImage(to, image, rect);
        }
        else
        {
            // TODO Gfx: line-wise conversion in onDrawImage
            Image conv( image.width(), image.height(), _active->format() );
            copyView(image, conv);
            onDrawImage(to, conv, rect);
        }
    }
}


void Canvas::invalidate(unsigned flags)
{
    _dirty |= flags;
}


void Canvas::applyState()
{
    if( ! _dirty)
        return;

    if(_dirty & DirtyTransform)   { onApplyTransform();       _dirty &= ~DirtyTransform; }
    if(_dirty & DirtyComposition) { onApplyCompositionMode();  _dirty &= ~DirtyComposition; }
    if(_dirty & DirtyClip)        { onApplyClip();             _dirty &= ~DirtyClip; }
    if(_dirty & DirtyPen)         { onApplyPen();              _dirty &= ~DirtyPen; }
    if(_dirty & DirtyBrush)       { onApplyBrush();            _dirty &= ~DirtyBrush; }
    if(_dirty & DirtyFont)        { onApplyFont();             _dirty &= ~DirtyFont; }
}


void Canvas::updateTransform()
{
    Gfx::Transform viewTx;
    viewTx.scale( _scaling.scaleFactor(), _scaling.scaleFactor() );
    viewTx.translate( _region.x(), _region.y() );

    _viewTx = viewTx;
    _tx = _viewTx * _userTx;

    onSetTransform(_tx);

    if(_active)
        _dirty |= DirtyTransform;
}

} // namespace

} // namespace
