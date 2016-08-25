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
#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Widget.h>

namespace Pt {
namespace Hmi {

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


void PaintRegion::drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect)
{
    _surface->drawImage(to + _area.topLeft(), image, imgRect);
}


void PaintRegion::drawPicture(const Gfx::PointF& to, const Picture& pic)
{
    _surface->drawPicture(to + _area.topLeft(), pic);
}


void PaintRegion::setClip( const Gfx::RectF& clip)
{
    _surface->setClip( Gfx::RectF( clip.topLeft() +  _area.topLeft(), clip.size()));
}

const Gfx::RectF& PaintRegion::clip() const
{
    return _surface->clip();
}


void PaintRegion::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _surface->setCompositionMode(mode);
}


const Gfx::CompositionMode& PaintRegion::compositionMode() const
{
    return _surface->compositionMode();
}

} // namespace

} 
