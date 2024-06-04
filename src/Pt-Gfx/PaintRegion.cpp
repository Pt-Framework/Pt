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

#include <Pt/Gfx/PaintRegion.h>

namespace Pt {

namespace Gfx {

PaintRegion::PaintRegion()
: _surface(0)
{
}


PaintRegion::PaintRegion(PaintSurface& surface, const Gfx::RectF& rect)
: _surface(0)
{           
    _surface = &surface;
    _area = rect; 
}


PaintRegion::~PaintRegion()
{
    if(_surface)
        _surface->detachRegion(*this);
}


void PaintRegion::attach(PaintSurface& surface, const Gfx::RectF& rect)
{
    if(_surface)
        detach();

    _surface = &surface;
    _area = rect;

    surface.attachRegion(*this);
}


void PaintRegion::detach()
{
    onReset();

    if(_surface)
        _surface->detachRegion(*this);

    _surface = 0;
    _area.clear();
}


PaintSurface* PaintRegion::surface() const
{
    return _surface;
}


const Gfx::RectF& PaintRegion::area() const
{
    return _area;
}


void PaintRegion::move(const Gfx::PointF& pos)
{
    onReset();
    
    _area.setOrigin(pos);
}


void PaintRegion::resize(const Gfx::SizeF& size)
{
    onReset();
    
    _area.setSize(size);
}


const Gfx::SizeF& PaintRegion::onSize() const
{
    return _area.size();
}


double PaintRegion::onScaleFactor() const
{
    if(_surface)
        return _surface->scaleFactor();

    return 1.0;
}


RectF PaintRegion::onGetRegion() const
{
    if(_surface)
    {
        RectF region = _surface->region();
        region.shift( _area.topLeft().x(),
                      _area.topLeft().y() );

        region.setSize( _area.size() );

        return region;
    }

    return _area;
}


Canvas* PaintRegion::onGetCanvas()
{
    Canvas* canvas = 0;

    if(_surface)
    {
        canvas = _surface->onGetCanvas();
    }

    return canvas;
}


PaintData* PaintRegion::onGetPaint(PaintData* p)
{
    if(_surface)
        return _surface->onGetPaint(p);

    return 0;
}


void PaintRegion::onDestroy(PaintSurface* region)
{
    _surface = 0;
    _area.clear();

    PaintSurface::onDestroy(region);
}


void PaintRegion::onReset()
{
    PaintSurface::onReset();
}


const Gfx::ImageFormat& PaintRegion::onGetFormat() const
{
    return _surface ? _surface->format()
                    : Gfx::ImageFormat::argb32();
}


const Gfx::Scaling& PaintRegion::onGetScaling() const
{
    return _surface->scaling();
}


Image PaintRegion::toImage() const
{
    return _surface ? _surface->toImage()
                    : Image();
}

} // namespace

} // namespace
