/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2005-2007 Aloysius Indrayanto                           *
 *   Copyright (C) 2014-2007 Laurentiu-Gheorghe Crisan                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/FontMetrics.h>
#include <iostream>

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _font("sans-serif")
, _surface(surface)
{
}


PainterImpl::~PainterImpl()
{
}


void PainterImpl::begin()
{
}



void PainterImpl::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
}


const Gfx::Pen& PainterImpl::pen() const
{
    return _pen;
}


void PainterImpl::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


const Gfx::Brush& PainterImpl::brush() const
{
    return _brush;
}


const Gfx::Font& PainterImpl::font() const
{
    return _font;
}


void PainterImpl::setFont(const Gfx::Font& font)
{
    _font = font;
}


Gfx::FontMetrics PainterImpl::fontMetrics() const
{
    return Gfx::FontMetrics(0, 0, 0, 0);
}


Gfx::FontMetrics PainterImpl::fontMetrics(const Pt::String& text) const
{
    return Gfx::FontMetrics(0, 0, 0, 0);
}


const std::list<std::string>& PainterImpl::fontFamilyNames()
{
    static const std::list<std::string> _fontList;
    return _fontList;
}

void PainterImpl::drawPixel(const Gfx::PointF& to)
{

}

void PainterImpl::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{    
}


void PainterImpl::drawRect(const Gfx::RectF& rect)
{

}


void PainterImpl::drawText(const Gfx::PointF& to, const Pt::String& text)
{

}


void PainterImpl::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{

}


void PainterImpl::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{

}


void PainterImpl::fillRect(const Gfx::RectF& rect)
{

}


void PainterImpl::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{

}


void PainterImpl::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{

}

void PainterImpl::drawSurface(const Gfx::PointF& to, Surface& pm)
{

}

void PainterImpl::drawSurface(const Gfx::PointF& to, PaintSurface& pm, const Gfx::Region& pmRegion)
{

}

void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image)
{
    this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}


void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image, const Gfx::Region& imageRegion)
{

}

}}


