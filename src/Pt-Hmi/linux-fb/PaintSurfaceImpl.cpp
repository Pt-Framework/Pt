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
#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Hmi/Application.h>

namespace Pt {

namespace Hmi {

PaintSurfaceImpl::PaintSurfaceImpl()
: _size(10,10)
{
}


PaintSurfaceImpl::~PaintSurfaceImpl()
{
}


void PaintSurfaceImpl::resize(const Gfx::SizeF& size)
{
    _size = size;
    _pipeline.clear();
}


Gfx::SizeF PaintSurfaceImpl::size() const
{
    return _size;
}


std::list<std::string> PaintSurfaceImpl::fontFamilyNames()
{
    std::list<std::string> fonts;
    return fonts;
}

/////////////////////////////////////////////////////////////////////
// PaintRegionImpl
/////////////////////////////////////////////////////////////////////

PaintRegionImpl::PaintRegionImpl()
: _surface(0)
{
}


PaintRegionImpl::~PaintRegionImpl()
{
}


void PaintRegionImpl::set(PaintSurface& surface, const Gfx::RectF& area)
{
    _surface = &surface;
    _area = area;
}


const Gfx::SizeF& PaintRegionImpl::size() const
{
    return _area.size();
}


void PaintRegionImpl::setPen(const Gfx::Pen& pen)
{
    _surface->impl()->setPen(pen);
}


void PaintRegionImpl::setBrush(const Gfx::Brush& brush)
{
    _surface->impl()->setBrush(brush);
}


void PaintRegionImpl::setFont(const Gfx::Font& font)
{
    _surface->impl()->setFont(font);
}


Gfx::FontMetrics PaintRegionImpl::fontMetrics(const Pt::String& text) const
{
    return _surface->impl()->fontMetrics(text);
}


void PaintRegionImpl::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
{
    _surface->impl()->drawLine(fromF + _area.topLeft(),
                               toF + _area.topLeft() );
}


void PaintRegionImpl::drawText(const Gfx::PointF& toF, const Pt::String& text)
{
    _surface->impl()->drawText(toF + _area.topLeft(), text);
}


void PaintRegionImpl::drawRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    _surface->impl()->drawRect(rect);
}


void PaintRegionImpl::fillRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    _surface->impl()->fillRect(rect);
}


void PaintRegionImpl::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->impl()->drawEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegionImpl::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->impl()->fillEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegionImpl::drawPolyline(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> translated(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
        Gfx::PointF pt = points[i] + _area.topLeft();
        translated[i] = pt;
    }

    _surface->impl()->drawPolyline(points, pointCount);
}


void PaintRegionImpl::fillPolygon(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> translated(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
        Gfx::PointF pt = points[i] + _area.topLeft();
        translated[i] = pt;
    }

    _surface->impl()->fillPolygon(&translated[0], pointCount);
}


void PaintRegionImpl::drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface)
{
    _surface->impl()->drawSurface(toF + _area.topLeft(), surface);
}


void PaintRegionImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    _surface->impl()->drawImage(toF + _area.topLeft(), image);
}


void PixmapSurfaceImpl::flush()
{
    _surface->impl()->flush();
}

/////////////////////////////////////////////////////////////////////
// PixmapSurfaceImpl
/////////////////////////////////////////////////////////////////////

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _size(10,10)
{
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
}


void PixmapSurfaceImpl::clear()
{
    _pipeline.clear();
}


void PixmapSurfaceImpl::resize(const Gfx::SizeF& size)
{
    _size = size;
    _pipeline.clear();
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
}


void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
    _font = font; 
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    return Gfx::ImagePainter::fontMetrics( _font, text);
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
{
    pipeline().stroke( _pen, from , to );
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& toF, const Pt::String& text)
{
    pipeline().text( to, _pen, _font, text);
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rectF)
{
    pipeline().stroke( _pen, rect ); 
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rectF)
{
    pipeline().fill( _brush, rect )
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    pipeline().ellipse( _pen, topLeft, size );
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    pipeline().fillEllipse( _brush, topLeft, size );
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    pipeline().stroke( _pen, pt, pointCount );
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    pipeline().fill( _brush, pt, pointCount );
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface)
{
    Gfx::GraphicsPipeline pipeline( surface.pipeline() );
    pipeline.translate( to.x(), to.y() );  

    pipeline().addPipeline(Gfx::RectF(to, surface._size), pipeline);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    pipeline().image( to, image )
}


void PixmapSurfaceImpl::flush()
{
    Gfx::ImagePainter painter( Application::instance().mainScreen().impl()->image() );

    pipeline().render( painter );
    pipeline().clear();
}

} // namespace

} // namespace
