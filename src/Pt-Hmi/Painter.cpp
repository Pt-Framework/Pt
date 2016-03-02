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
  MA  02110-1301  USA
*/

#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>

namespace Pt {

namespace Hmi {

Painter::Painter(PaintSurface& surface)
: _impl(new PainterImpl(surface.impl()))
{    
}


Painter::~Painter()
{
    delete _impl;
}


void Painter::setSurface(PaintSurface& surface)
{    
  _impl->setSurface(surface);
}


void Painter::setOrigin(const Gfx::PointF& p)
{
    _impl->setOrigin(p);
}


void Painter::setClip( const Gfx::RectF& clip )
{
  _impl->setClip( clip );
}
        

const Gfx::RectF& Painter::clip() const
{
  return _impl->clip(); 
}


void Painter::setRenderMode(Gfx::RenderMode::Type mode)
{
    _impl->setRenderMode( mode );
}


void Painter::setPen(const Gfx::Pen& pen)
{
    _impl->setPen(pen);
}


const Gfx::Pen& Painter::pen() const
{
    return _impl->pen();
}


void Painter::setBrush(const Gfx::Brush& brush)
{
    _impl->setBrush(brush);
}


const Gfx::Brush& Painter::brush() const
{
    return _impl->brush();
}


void Painter::setFont(const Gfx::Font& font)
{
    _impl->setFont(font);
}


const Gfx::Font& Painter::font() const
{
    return _impl->font();
}


Gfx::FontMetrics Painter::fontMetrics(const Pt::String& text) const
{
    return _impl->fontMetrics(text);
}


Gfx::FontMetrics Painter::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{
  return PainterImpl::fontMetrics( font, text );
}


void Painter::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    _impl->drawLine(from,to);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& Text)
{
    _impl->drawText(to, Text);
}


void Painter::drawRect(const Gfx::RectF& rect)
{
    _impl->drawRect(rect);
}


void Painter::fillRect(const Gfx::RectF& rectangle)
{
    _impl->fillRect(rectangle);
}


void Painter::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _impl->drawEllipse(topLeft, size);
}


void Painter::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _impl->fillEllipse(topLeft, size);
}


void Painter::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    _impl->drawPolyline(points, pointCount);
}


void Painter::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    _impl->fillPolygon(points, pointCount);
}


void Painter::drawSurface(const Gfx::PointF& to, const PixmapSurface& pm)
{  
    _impl->drawSurface(to, pm);
}    
    

void Painter::drawSurface(const Gfx::PointF& to, 
                          const PixmapSurface& pm, const Gfx::RectF& pmRect)
{  
    _impl->drawSurface(to, pm, pmRect);
} 


void Painter::drawImage(const Gfx::PointF& to, const Gfx::Image& image)
{
    _impl->drawImage(to, image);
}


void Painter::flush()
{
    _impl->flush();
}


void Painter::clear( const Gfx::Color& color)
{
  _impl->clear( color );
}

}

}
