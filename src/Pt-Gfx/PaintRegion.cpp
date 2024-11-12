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
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Canvas.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// PaintRegionInfo
///////////////////////////////////////////////////////////////////////

PaintRegionInfo::PaintRegionInfo()
: _surface(0)
, _hasArea(false)
{
}


PaintRegionInfo::~PaintRegionInfo()
{
}


void PaintRegionInfo::reset()
{
    _surface = 0;
    _hasArea = false;
}


void PaintRegionInfo::reset(PaintSurface& surface, const Gfx::RectF* rect)
{
    _surface = &surface;

    if(rect)
    {
        _area = *rect;
        _hasArea = true;
    }
    else
    {
        _area.clear();
        _hasArea = false;
    }
}


const Gfx::PointF& PaintRegionInfo::position() const
{
    return _area.topLeft();
}


void PaintRegionInfo::setPosition(const Gfx::PointF& pos)
{
    _area.setOrigin(pos);
    _hasArea = true;
}


const Gfx::SizeF& PaintRegionInfo::size() const
{
    return _area.size();
}


void PaintRegionInfo::setSize(const Gfx::SizeF& size)
{
    _area.setSize(size);
    _hasArea = true;
}


const RectF* PaintRegionInfo::area() const
{
    return _hasArea ? &_area : 0;
}


const Gfx::ImageFormat& PaintRegionInfo::onGetFormat() const
{
    return _surface && _surface->canvas() ? _surface->canvas()->format() 
                                          : Gfx::ImageFormat::argb32();
}


const Gfx::SizeF& PaintRegionInfo::onGetSize() const
{
    return _surface && _surface->canvas() && ! _hasArea ? _surface->canvas()->size() 
                                                        : _area.size();
}


const Scaling& PaintRegionInfo::onGetScaling() const
{
    return _surface && _surface->canvas() ? _surface->canvas()->scaling() 
                                          : _scaling;
}


PaintContext* PaintRegionInfo::onGetPaint(PaintContext* context) 
{
    if( ! _surface )
        return 0;

    PaintContext* paintContext = _surface->getPaint(context);
    if( ! paintContext )
        return paintContext;

    const RectF* area = this->area();
    if( ! area )
        return paintContext;

    RectF r = paintContext->region();
    r.shift( area->topLeft().x(),
             area->topLeft().y() );
    r.setSize( area->size() );

    paintContext->setRegion(r);
    return paintContext;
}

///////////////////////////////////////////////////////////////////////
// PaintRegion
///////////////////////////////////////////////////////////////////////

PaintRegion::PaintRegion()
: _surface(0)
{
    setCanvas(&_info);
}


PaintRegion::PaintRegion(PaintSurface& surface, const Gfx::RectF& rect)
: _surface(0)
{           
    _info.reset(surface);
    _surface = &surface;

    setCanvas(&_info);
}


PaintRegion::~PaintRegion()
{
    if(_surface)
        _surface->detachRegion(*this);
}


void PaintRegion::onDetachSurface(PaintSurface* region)
{
    _info.reset();
    _surface = 0;

    PaintSurface::onReset();
}


void PaintRegion::attach(PaintSurface& surface)
{
    if(_surface)
        detach();

    _info.reset(surface);
    _surface = &surface;

    surface.attachRegion(*this);
}


void PaintRegion::attach(PaintSurface& surface, const Gfx::RectF& rect)
{
    if(_surface)
        detach();

    _info.reset(surface, &rect);
    _surface = &surface;

    surface.attachRegion(*this);
}


void PaintRegion::detach()
{
    onReset();

    if(_surface)
        _surface->detachRegion(*this);

    _info.reset();
    _surface = 0;
}


PaintSurface* PaintRegion::surface() const
{
    return _surface;
}


const PointF& PaintRegion::position() const
{
    return _info.position();
}


const RectF* PaintRegion::area() const
{
    return _info.area();
}


void PaintRegion::move(const Gfx::PointF& pos)
{
    onReset();
    
    _info.setPosition(pos);
}


void PaintRegion::resize(const Gfx::SizeF& size)
{
    onReset();
    
    _info.setSize(size);
}


void PaintRegion::onReset()
{
    PaintSurface::onReset();
}


//void PaintRegion::onDraw(PaintContext& paint, 
//                         const Gfx::PointF& to) const
//{
//    if(_surface)
//    {
//        const RectF* area = _info.area();
//        if(area)
//            _surface->draw(paint, to, *area);
//        else
//            _surface->draw(paint, to);
//    }
//}
//
//
//void PaintRegion::onDraw(PaintContext& paint, 
//                         const Gfx::PointF& to, 
//                         const Gfx::RectF& rect) const
//{
//    Gfx::RectF r = rect;
//
//    const PointF& pos = position();
//    r.shift( pos.x(), pos.y() );
//    
//    if(_surface)
//        _surface->draw(paint, to, r);
//}

} // namespace

} // namespace
