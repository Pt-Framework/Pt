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
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Widget.h>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////
// PaintSurfaceImpl
/////////////////////////////////////////////////////////////////////

PaintSurfaceImpl::PaintSurfaceImpl()
{
}


PaintSurfaceImpl::~PaintSurfaceImpl()
{
}

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
: _surface(0)
{
}


PaintRegion::PaintRegion(PaintSurface& surface, const Gfx::RectF& rect)
: _surface(0)
{        
    set(surface, rect);   
}


PaintRegion::~PaintRegion()
{
    if( _surface) 
        _surface->setClip( Gfx::RectF(Gfx::PointF(0,0), _surface->size() )) ;
}


void PaintRegion::set(PaintSurface& surface, const Gfx::RectF& area)
{
    _surface = &surface;
    _area = area;
}


const Gfx::SizeF& PaintRegion::size() const
{
    return _area.size();
}


void PaintRegion::setPen(const Gfx::Pen& pen)
{
    _surface->setPen(pen);
}


void PaintRegion::setBrush(const Gfx::Brush& brush)
{
    _surface->setBrush(brush);
}


void PaintRegion::setFont(const Gfx::Font& font)
{
    _surface->setFont(font);
}


Gfx::FontMetrics PaintRegion::fontMetrics(const Pt::String& text) const
{
    return _surface->fontMetrics(text);
}


void PaintRegion::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
{
    _surface->drawLine(fromF + _area.topLeft(),
                       toF + _area.topLeft() );
}


void PaintRegion::drawText(const Gfx::PointF& toF, const Pt::String& text)
{
    _surface->drawText(toF + _area.topLeft(), text);
}


void PaintRegion::drawRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    _surface->drawRect(rect);
}


void PaintRegion::fillRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    _surface->fillRect(rect);
}


void PaintRegion::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->drawEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegion::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->fillEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegion::drawPolyline(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> translated(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
        Gfx::PointF pt = points[i] + _area.topLeft();
        Gfx::Point p = Application::instance().screen().fromUnit(pt);
        translated[i].setX( p.x() );
        translated[i].setY( p.y() );
    }

    _surface->drawPolyline(&translated[0], pointCount);
}


void PaintRegion::fillPolygon(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> translated(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
        Gfx::PointF pt = points[i] + _area.topLeft();
        Gfx::Point p = Application::instance().screen().fromUnit(pt);
        translated[i].setX( p.x() );
        translated[i].setY( p.y() );
    }

    _surface->fillPolygon(&translated[0], pointCount);
}


void PaintRegion::drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface)
{
    _surface->drawSurface(toF + _area.topLeft(), surface);
}


void PaintRegion::drawSurface(const Gfx::PointF& toF, 
                                  const PixmapSurface& pm,
                                  const Gfx::RectF& pmRect)
{
    _surface->drawSurface(toF + _area.topLeft(), pm, pmRect);
}


void PaintRegion::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    _surface->drawImage(toF + _area.topLeft(), image);
}


void PaintRegion::setClip( const Gfx::RectF& clip)
{
    _surface->setClip( Gfx::RectF( clip.topLeft() +  _area.topLeft(), clip.size()));
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


void PixmapSurface::clear(const Gfx::Color& )
{
    // TODO
    _impl->clear();
}


const Gfx::SizeF& PixmapSurface::size() const
{
    return _impl->size();
}


void PixmapSurface::setPen(const Gfx::Pen& pen)
{
    _impl->setPen(pen);
}


void PixmapSurface::setBrush(const Gfx::Brush& brush)
{
    _impl->setBrush(brush);
}


void PixmapSurface::setFont(const Gfx::Font& font)
{
    _impl->setFont(font);
}


Gfx::FontMetrics PixmapSurface::fontMetrics(const Pt::String& text) const
{
    return _impl->fontMetrics(text);
}


void PixmapSurface::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    _impl->drawLine(from, to);
}


void PixmapSurface::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    _impl->drawText(to, text);
}


void PixmapSurface::drawRect(const Gfx::RectF& r)
{
    _impl->drawRect(r);
}


void PixmapSurface::fillRect(const Gfx::RectF& r)
{
    _impl->fillRect(r);
}


void PixmapSurface::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _impl->drawEllipse(topLeft, size);
}


void PixmapSurface::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _impl->fillEllipse(topLeft, size);
}


void PixmapSurface::drawPolyline(const Gfx::PointF* points, size_t pointCount)
{
    _impl->drawPolyline(points, pointCount);
}


void PixmapSurface::fillPolygon(const Gfx::PointF* points, size_t pointCount)
{
    _impl->fillPolygon(points, pointCount);
}


void PixmapSurface::drawSurface(const Gfx::PointF& to, const PixmapSurface& surface)
{
    _impl->drawSurface(to, surface);
}


void PixmapSurface::drawSurface(const Gfx::PointF& to, 
                                  const PixmapSurface& pm,
                                  const Gfx::RectF& pmRect)
{
    _impl->drawSurface(to, pm, pmRect);
}


void PixmapSurface::drawImage(const Gfx::PointF& to, const Gfx::Image& image)
{
    _impl->drawImage(to, image);
}


void PixmapSurface::setClip( const Gfx::RectF& clip)
{
    _impl->setClip(clip);
}


PixmapSurfaceImpl* PixmapSurface::pixmapImpl() const
{ 
    return _impl; 
}

} // namespace

} // namespace
