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
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Algorithm.h>

#include <algorithm>

namespace Pt {

namespace Gfx {

Canvas::Canvas()
: _paint(0)
{
}


Canvas::~Canvas()
{
    if(_paint)
        _paint->onDetach(*this);
}


const Gfx::ImageFormat& Canvas::format() const
{
    return onGetFormat();
}


const Gfx::SizeF& Canvas::size() const
{
    return onSize();
}


const Scaling& Canvas::scaling() const
{
    return onGetScaling();
}


void Canvas::attachPaint(PaintData& paint)
{
    if(_paint)
    {
        _paint->finish();
        _paint = 0;
    }

    _paint = &paint;
}


void Canvas::detachPaint()
{
    if(_paint)
    {
        onFinish();

        _paint = 0;
    }
}


void Canvas::drawSurface(const Gfx::PointF& to, 
                         const Gfx::PaintSurface& surface)
{
    Pt::Gfx::Image image = surface.toImage();
    if( image.format() == format() )
    {
        drawImage(to, image);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest);
}


void Canvas::drawSurface(const Gfx::PointF& to,
                         const Gfx::PaintSurface& surface,
                         const Gfx::RectF& rect)
{
    Pt::Gfx::Image image = surface.toImage();
    if( image.format() == format() )
    {
        drawImage(to, image, rect);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest, rect);
}

///////////////////////////////////////////////////////////////////////
// PaintSurface
///////////////////////////////////////////////////////////////////////

PaintSurface::PaintSurface()
: _painter(0)
{
}


PaintSurface::~PaintSurface()
{
    if(_painter)
        _painter->onDetach(*this);

    typedef std::vector<PaintSurface*>::iterator RegionIterator;
    for( RegionIterator it = _regions.begin(); it != _regions.end(); ++it )
    {
        (*it)->onDestroy(this);
    } 
}


void PaintSurface::attachRegion(PaintSurface& region)
{
    _regions.push_back(&region);
}


void PaintSurface::detachRegion(PaintSurface& region)
{
    typedef std::vector<PaintSurface*>::iterator RegionIterator;

    RegionIterator rit = std::find(_regions.begin(), _regions.end(), &region);
    if( rit != _regions.end() )
    {
        _regions.erase(rit);
    }
}


void PaintSurface::onDestroy(PaintSurface* region)
{
    if(_painter)
        _painter->finish();

    typedef std::vector<PaintSurface*>::iterator RegionIterator;
    for( RegionIterator it = _regions.begin(); it != _regions.end(); ++it )
    {
        (*it)->onReset();
    }
}


void PaintSurface::onReset()
{
    if(_painter)
        _painter->finish();

    typedef std::vector<PaintSurface*>::iterator RegionIterator;
    for( RegionIterator it = _regions.begin(); it != _regions.end(); ++it )
    {
        (*it)->onReset();
    }
}


const Gfx::ImageFormat& PaintSurface::format() const
{
    return onGetFormat();
}


const Gfx::SizeF& PaintSurface::size() const
{
    return onSize();
}


Painter* PaintSurface::painter()
{
    return _painter;
}


void PaintSurface::attachPainter(Painter& painter)
{
    if(_painter)
    {
        _painter->finish();
        _painter = 0;
    }

    _painter = &painter;
}


void PaintSurface::detachPainter()
{
    if(_painter)
    {
        //onFinish();

        _painter = 0;
    }
}


PaintData* PaintSurface::getPaint(PaintData* paint)
{
    return onGetPaint(paint);
}


Canvas* PaintSurface::canvas()
{
    return onGetCanvas();
}


RectF PaintSurface::region() const
{
    return onGetRegion();
}


RectF PaintSurface::onGetRegion() const
{
    return RectF( size() );
}

} // namespace

} // namespace
