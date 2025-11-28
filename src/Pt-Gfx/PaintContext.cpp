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

Canvas::Canvas()
: _surface(0)
, _active(0)
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
    if(_surface)
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
    Gfx::Transform tx;
    tx.translate( r.x(), r.y() );
    tx.scale( _scaling.scaleFactor(), _scaling.scaleFactor() );
    
    _region = r;
    _tx = tx;
}


const Scaling& Canvas::scaling() const
{
    return _scaling;
}


void Canvas::setScaling(const Scaling& scaling)
{
    Gfx::Transform tx;
    tx.translate( _region.x(), _region.y() );
    tx.scale( scaling.scaleFactor(), scaling.scaleFactor() );

    _scaling = scaling;
    _tx = tx;
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


void Canvas::beginPaint(const Gfx::Paint& paint)
{
    if(_surface)
    {       
        _active = _surface;
        onBeginPaint(paint);

        // TODO: apply in draw* functions and keep a state which attributes
        //       are dirty and need to be applied

        // TODO: currently scaling/transform is handled in onBeginPaint()
        //       move this to:
        //onApplyTransform();

        onApplyCompositionMode( paint.compositionMode() );
        onApplyPen( paint.pen() );
        onApplyBrush( paint.brush() );
        onApplyFont( paint.font() );
        onApplyClip( paint.clip() );
    }
}


void Canvas::finishPaint()
{
    if(_surface)
    {
        _surface->onDetachCanvas(*this);

        onFinishPaint();
        _surface = 0;
        _active = 0;
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
    {
        onApplyCompositionMode(mode);
    }
}


void Canvas::setPen(const Pen& pen)
{
    onSetPen(pen);

    if(_active)
    {
        onApplyPen(pen);
    }
}


void Canvas::setBrush(const Brush& brush)
{
    onSetBrush(brush);

    if(_active)
    {
        onApplyBrush(brush);
    }
}


void Canvas::setFont(const Gfx::Font& font)
{
    onSetFont(font);

    if(_active)
    {
        onApplyFont(font);
    }
}


void Canvas::setClip(const RectF& rect)
{
    onSetClip(&rect);

    if(_active)
    {
        onApplyClip(&rect);
    }
}


void Canvas::resetClip()
{
    onSetClip(0);

    if(_active)
    {
        onApplyClip(0);
    }
}


void Canvas::drawLine(const PointF& from, const PointF& to)
{   
    if( _active )
        onDrawLine(from, to);
}


void Canvas::drawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if(_active)
        onDrawPolyline(pts, n);
}


void Canvas::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if(_active)
        onFillPolygon(ps, n);
}


void Canvas::drawRect(const Gfx::RectF& rect)
{
    if(_active)
        onDrawRect(rect);
}

        
void Canvas::fillRect(const Gfx::RectF& rect)
{
    if(_active)
        onFillRect(rect);
}


void Canvas::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_active)
        onDrawEllipse(topLeft, size);
}


void Canvas::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_active)
        onFillEllipse(topLeft, size);
}


void Canvas::setPath(const Path& path)
{
    if(_active)
        onSetPath(path);
}


void Canvas::drawPath()
{
    if(_active)
        onDrawPath();
}


void Canvas::fillPath()
{
    if(_active)
        onFillPath();
}


void Canvas::drawPath(const Path& path)
{
    if(_active)
        onDrawPath(path);
}


void Canvas::fillPath(const Path& path)
{
    if(_active)
        onFillPath(path);
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
        onDrawText(to, text, tform);
}


void Canvas::drawImage(const Gfx::PointF& to, 
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

} // namespace

} // namespace
