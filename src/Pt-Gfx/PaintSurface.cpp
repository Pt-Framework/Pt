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
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Algorithm.h>

#include <algorithm>

namespace Pt {

namespace Gfx {

PaintSurface::PaintSurface()
: _canvas(0)
, _painter(0)
{
}


PaintSurface::~PaintSurface()
{
    if(_painter)
    {
        _painter->onDetachSurface(*this);
    }

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


const PaintInfo& PaintSurface::info() const
{
    if(_canvas)
        return _canvas->info();

    return onGetPaintInfo();
}


//const Scaling& PaintSurface::scaling() const
//{
//    return info().scaling();
//}


const ImageFormat& PaintSurface::format() const
{
    return info().format();
}


Canvas* PaintSurface::canvas()
{
    return _canvas;
}


const Canvas* PaintSurface::canvas() const
{
    return _canvas;
}


void PaintSurface::setCanvas(Canvas* canvas)
{
    _canvas = canvas;
}


PaintContext* PaintSurface::getPaint(PaintContext* reuse)
{
    if(_canvas)
        return _canvas->getPaint(reuse);

    return onGetPaint(reuse);   
}


PaintContext* PaintSurface::onGetPaint(PaintContext* context)
{
    return 0;
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

///////////////////////////////////////////////////////////////////////
// PaintLayer
///////////////////////////////////////////////////////////////////////

PaintLayer::PaintLayer()
{
}


PaintLayer::~PaintLayer()
{
}


Image PaintLayer::toImage() const
{
    return onGetImage();
}


void PaintLayer::onDraw(PaintContext& paint, 
                        const Gfx::PointF& to,
                        const Gfx::RectF* rect) const
{
    Pt::Gfx::Image image = toImage();

    if( ! rect )
    {
        paint.drawImage(to, image);
        return;
    }

    Gfx::RectF imageRect = info().scaling().toPhysical(*rect);
    paint.drawImage(to, image, imageRect);
}

} // namespace

} // namespace
