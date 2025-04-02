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

#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"

#include <Pt/Forms/Painter.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Forms/Application.h>
#include <Pt/Gfx/ImagePainter.h>


namespace Pt {

namespace Forms {

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _surface(surface)
, _origin(0.0f, 0.0f)
, _pen(1)
, _brush( Gfx::Color(0, 0, 0, 0) )
, _font( PaintSurfaceImpl::defaultFont() )
{
}


PainterImpl::~PainterImpl()
{
}


void PainterImpl::setSurface(PaintSurface& surface)
{
    _surface = surface.impl();
    
    _surface->setBrush(_brush);
    _surface->setFont(_font);
    _surface->setPen(_pen);  
}


void PainterImpl::setRenderMode(Gfx::RenderMode::Type)
{
}


void PainterImpl::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
    _surface->setPen(_pen);
}


const Gfx::Pen& PainterImpl::pen() const
{
    return _pen;
}


void PainterImpl::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
    _surface->setBrush(_brush);
}


const Gfx::Brush& PainterImpl::brush() const
{
    return _brush;
}


void PainterImpl::setFont(const Gfx::Font& font)
{
    if (font == _font) 
        return;

    _font = font;
    _surface->setFont(_font);
}


const Gfx::Font& PainterImpl::font() const
{
    return _font;
}


Gfx::FontMetrics PainterImpl::fontMetrics(const Pt::String& text) const
{
    return _surface->fontMetrics(text);
}


Gfx::FontMetrics PainterImpl::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{   
    return Gfx::ImagePainter::fontMetrics(font, text);
}


std::list<std::string> PainterImpl::fontFamilyNames()
{
    return PaintSurfaceImpl::fontFamilyNames();
}


void PainterImpl::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
{
    if (_pen.size() == 0) 
        return;

    _surface->drawLine(fromF, toF);
}


void PainterImpl::drawText(const Gfx::PointF& toF, const Pt::String& text)
{
    _surface->drawText(toF, text);
}


void PainterImpl::drawRect(const Gfx::RectF& rectF)
{
    _surface->drawRect(rectF);
}


void PainterImpl::fillRect(const Gfx::RectF& rectF)
{
    _surface->fillRect(rectF);
}


void PainterImpl::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->drawEllipse(topLeftF, sizeF);
}


void PainterImpl::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->fillEllipse(topLeftF, sizeF);
}


void PainterImpl::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    if (_pen.size() == 0)
       return;

    _surface->drawPolyline(points, pointCount);
}


void PainterImpl::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    _surface->fillPolygon(points, pointCount);
}


void PainterImpl::drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface)
{
    _surface->drawSurface(toF, surface);
}


void PainterImpl::drawSurface(const Gfx::PointF& toF, 
                              const PixmapSurface& pm, 
                              const Gfx::RectF& pmRect)
{
    _surface->drawSurface(toF, pm, pmRect);
}


void PainterImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    _surface->drawImage(toF, image);
}


void PainterImpl::clear(const Gfx::Color& color)
{
    Gfx::RectF rect(Gfx::PointF(0,0), _surface->size() );   
    setBrush( Gfx::Brush(color) );
    fillRect(rect);
}


void PainterImpl::flush()
{	
}

} // namespace

} // namespace
