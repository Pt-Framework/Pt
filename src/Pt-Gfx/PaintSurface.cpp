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

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/PaintRegion.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Paint.h>

#include <algorithm>

namespace Pt {

namespace Gfx {

PaintSurface::PaintSurface()
: _painter(0)
{
}


PaintSurface::~PaintSurface()
{
    if(_painter)
        _painter->onDetachSurface(*this);

    typedef std::vector<PaintRegion*>::iterator RegionIterator;
    for( RegionIterator it = _regions.begin(); it != _regions.end(); ++it )
    {
        (*it)->onDetachSurface(this);
    } 
}


void PaintSurface::attachRegion(PaintRegion& region)
{
    _regions.push_back(&region);
}


void PaintSurface::detachRegion(PaintRegion& region)
{
    typedef std::vector<PaintRegion*>::iterator RegionIterator;

    RegionIterator rit = std::find(_regions.begin(), _regions.end(), &region);
    if( rit != _regions.end() )
    {
        _regions.erase(rit);
    }
}


void PaintSurface::onReset()
{
    if(_painter)
        _painter->finish();

    typedef std::vector<PaintRegion*>::iterator RegionIterator;
    for( RegionIterator it = _regions.begin(); it != _regions.end(); ++it )
    {
        (*it)->onReset();
    }
}

//
// TODO: virtual canvas() function instead of format() and non-virtual
//       accessors in Canvas base class
//

const Canvas* PaintSurface::canvas() const
{
    return onGetCanvas();
}


const Gfx::ImageFormat& PaintSurface::format() const
{
    return onGetFormat();
}


const Gfx::SizeF& PaintSurface::size() const
{
    return onGetSize();
}


const Scaling& PaintSurface::scaling() const
{
    return onGetScaling();
}


bool PaintSurface::beginPaint(PaintContext* context, const Gfx::Paint& paint)
{
    return onBeginPaint(context, paint);
}


PaintContextPtr PaintSurface::beginPaint(const Gfx::Paint& paint)
{
    return onBeginPaint(paint);
}


Image PaintSurface::toImage() const
{
    return onGetImage();
}


void PaintSurface::attachPainter(Painter& painter)
{
    if(_painter)
    {
        _painter->onDetachSurface(*this);
        _painter = 0;
    }

    _painter = &painter;
}


void PaintSurface::detachPainter(Painter& painter)
{
    if(_painter)
        _painter = 0;
}

} // namespace

} // namespace
