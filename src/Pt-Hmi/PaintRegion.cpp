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

#include <Pt/Hmi/PaintRegion.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

PaintRegion::PaintRegion()
: _surface(0)
{
}


PaintRegion::PaintRegion(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
: _surface(&surface)
, _area(rect)
{
}


PaintRegion::~PaintRegion()
{
}


void PaintRegion::reset(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    if(_surface)
        reset();

    _surface = &surface;
    _area = rect;
}


void PaintRegion::reset()
{
    onReset();

    _surface = 0;
    _area.clear(); 
}


const Gfx::Canvas* PaintRegion::onGetCanvas() const
{
    return _surface ? _surface->canvas() : 0;
}


const Gfx::ImageFormat& PaintRegion::onGetFormat() const
{
    return _surface ? _surface->format()
                    : Gfx::ImageFormat::argb32();
}


const Gfx::SizeF& PaintRegion::onGetSize() const
{
    return _surface ? _surface->size() : _area.size();
}


const Gfx::Scaling& PaintRegion::onGetScaling() const
{
    return _surface ? _surface->scaling() : _scaling;
}


Gfx::PaintContext* PaintRegion::onBeginPaint(Gfx::PaintContext* context) 
{
    if( ! _surface )
        return 0;

    Gfx::PaintContext* paintContext = _surface->beginPaint(context);
    if( ! paintContext )
        return paintContext;

    Gfx::RectF r = paintContext->region();
    r.shift( _area.topLeft().x(),
             _area.topLeft().y() );
    r.setSize( _area.size() );

    paintContext->setRegion(r);
    return paintContext;
}


void PaintRegion::onDraw(Gfx::PaintContext& paint, 
                         const Gfx::PointF& to) const
{
    if(_surface)
        _surface->draw(paint, to, _area);
}


void PaintRegion::onDraw(Gfx::PaintContext& paint, 
                         const Gfx::PointF& to, 
                         const Gfx::RectF& rect) const
{
    Gfx::RectF r = rect;
    r.shift( _area.topLeft().x(),
             _area.topLeft().y() );
    
    if(_surface)
        _surface->draw(paint, to, r);
}

} // namespace

} // namespace
