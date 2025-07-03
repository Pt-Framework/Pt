/* Copyright (C) 2020 Marc Boris Duerner 
  
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
#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Algorithm.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// CanvasBase
///////////////////////////////////////////////////////////////////////

CanvasBase::CanvasBase()
{
}


CanvasBase::~CanvasBase()
{
}


const Gfx::ImageFormat& CanvasBase::format() const
{
    return onGetFormat();
}


const Gfx::SizeF& CanvasBase::size() const
{
    return onGetSize();
}


const Scaling& CanvasBase::scaling() const
{
    return onGetScaling();
}


Gfx::PaintContext* CanvasBase::getPaint(Gfx::PaintContext* context)
{
    return onGetPaint(context);
}

///////////////////////////////////////////////////////////////////////
// Canvas
///////////////////////////////////////////////////////////////////////

Canvas::Canvas(PaintSurface& surface)
: _surface(surface)
, _paint(0)
, _active(0)
{
}


Canvas::~Canvas()
{
    if(_paint)
    {
        _paint->detachCanvas(*this);
        _paint = 0;
    }
}


void Canvas::onDetachPaint(PaintContext& canvas)
{
    if(_paint)
    {
        onReleasePaint();
        _paint = 0;
    }
}


Gfx::PaintContext* Canvas::onGetPaint(Gfx::PaintContext* reuse)
{
    if(_paint)
    {
        onReleasePaint();
        _paint->detachCanvas(*this);
        _paint = 0;
    }

    if(reuse)
    {
        if( reuse->scaling() != scaling() )
            reuse = 0;
    }

    if(reuse)
    {
        bool isReused = onSetPaint(reuse);
        if(isReused)
            _paint = reuse;
    }

    if( ! _paint )
        _paint = onCreatePaint();
    
    _paint->attachCanvas(*this);
    return _paint;
}


void Canvas::drawLine(const PointF& from, const PointF& to)
{   
    onDrawLine(from, to);
}


void Canvas::drawPolyline(const Gfx::Polyline& line)
{
    onDrawPolyline(line);
}


void Canvas::fillPolygon(const Gfx::Polyline& line)
{
    onFillPolygon(line);
}


void Canvas::drawRect(const Gfx::RectF& rect)
{
    onDrawRect(rect);
}


void Canvas::fillRect(const Gfx::RectF& rect)
{
    onFillRect(rect);
}


void Canvas::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    onDrawEllipse(topLeft, size);
}


void Canvas::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    onFillEllipse(topLeft, size);
}


void Canvas::drawPath(const Gfx::Path& path)
{
    onDrawPath(path, 1.0f);
}


void Canvas::fillPath(const Gfx::Path& path)
{
    onFillPath(path, 1.0f);
}


TextMetrics Canvas::textMetrics(const Pt::String& text) const
{
    return onGetTextMetrics(text);
}


void Canvas::drawText(const PointF& to, const Pt::String& text, 
                      const Transform* transform)
{
    onDrawText(to, text, transform);
}


void Canvas::drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image, 
                       const Gfx::RectF* rect)
{
    if( image.format() == format() )
    {
        onDrawImage(to, image, rect);
        return;
    }

    Pt::Gfx::Image dest( format(), image.width(), image.height() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest, rect);
}


bool Canvas::drawLayer(const Gfx::PointF& to,
                       const Gfx::PaintLayer& layer,
                       const Gfx::RectF* rect)
{
    return onDrawLayer(to, layer, rect);
}

} // namespace

} // namespace
