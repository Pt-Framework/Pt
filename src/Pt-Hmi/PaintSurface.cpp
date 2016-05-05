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

#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Widget.h>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////
// PaintSurface
/////////////////////////////////////////////////////////////////////

PaintSurface::PaintSurface()
{
}


PaintSurface::~PaintSurface()
{
}

/////////////////////////////////////////////////////////////////////
// PaintRegion
/////////////////////////////////////////////////////////////////////

PaintRegion::PaintRegion()
: _impl(new PaintRegionImpl)
, _surface(0)
{
}


PaintRegion::PaintRegion(PaintSurface& surface, const Gfx::RectF& rect)
: _impl(new PaintRegionImpl)
{        
    set(surface, rect);   
}


PaintRegion::~PaintRegion()
{
    if( _surface) 
        _surface->impl()->setClip( Gfx::RectF(Gfx::PointF(0,0), _surface->impl()->size() )) ;

    delete _impl;
}


void PaintRegion::set(PaintSurface& surface, const Gfx::RectF& rect)
{ 
    _surface = &surface;
    _impl->set(surface, rect); 
    _surface->impl()->setClip(rect);

}


PaintSurfaceImpl* PaintRegion::impl()
{ 
    return _impl; 
}


const PaintSurfaceImpl* PaintRegion::impl() const 
{ 
    return _impl; 
}

/////////////////////////////////////////////////////////////////////
// PixmapSurface
/////////////////////////////////////////////////////////////////////

PixmapSurface::PixmapSurface()
: _impl( new PixmapSurfaceImpl())
{
}


PixmapSurface::~PixmapSurface()
{
    delete _impl;
}


void PixmapSurface::resize(const Gfx::SizeF& size)
{
    _impl->resize(size);
}


Gfx::SizeF PixmapSurface::size() const
{
    return _impl->size();
}


void PixmapSurface::clear()
{
    _impl->clear();
}


PaintSurfaceImpl* PixmapSurface::impl()
{ 
    return _impl; 
}

const PaintSurfaceImpl* PixmapSurface::impl() const 
{ 
    return _impl; 
}


PixmapSurfaceImpl* PixmapSurface::pixmapImpl() const
{ 
    return _impl; 
}

} // namespace

} // namespace
