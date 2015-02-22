/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
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
#include <Pt/Api.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"

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

Gfx::FontMetrics Painter::fontMetrics() const
{
	return _impl->fontMetrics();
}

Gfx::FontMetrics Painter::fontMetrics(Pt::String Text) const
{
	return _impl->fontMetrics(Text);
}

const std::list<std::string>& Painter::fontFamilyNames()
{
	return _impl->fontFamilyNames();
}

int Painter::depth() const
{
	return _impl->depth();
}

void Painter::drawPixel(const Pt::Gfx::PointF& to)
{
	_impl->drawPixel(to);
}

void Painter::drawLine(const Pt::Gfx::PointF& from, const Pt::Gfx::PointF& to)
{
	_impl->drawLine(from,to);
}

void Painter::drawText( const Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline )
{
	_impl->drawText( to,text, outline);
}

void Painter::drawText(const Pt::Gfx::PointF& to, const Pt::String& Text)
{
	_impl->drawText(to, Text);
}

void Painter::drawRect(const Pt::Gfx::RectF& rectangle)
{
	_impl->drawRect(rectangle);
}

void Painter::fillRect(const Pt::Gfx::RectF& rectangle)
{
	_impl->fillRect(rectangle);
}

void Painter::drawEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size)
{
	_impl->drawEllipse(topLeft, size);
}

void Painter::fillEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size)
{
	_impl->fillEllipse(topLeft, size);
}

void Painter::drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount)
{
	_impl->drawPolyline(points, pointCount);
}

void Painter::fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount)
{
	_impl->fillPolygon(points, pointCount);
}

void Painter::drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm, const Pt::Gfx::Region& pmRegion)
{
	_impl->drawSurface(to, pm, pmRegion);
}

void Painter::drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm)
{
	_impl->drawSurface(to, pm);
}	
	
void Painter::drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image)
{
	_impl->drawImage(to, image);
}

void Painter::drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion)
{
	_impl->drawImage(to, image, imageRegion);
}

void Painter::addFontName(const std::string& fontName)
{
	_impl->addFontName(fontName);
}

void Painter::setSurface(PaintSurface& surface)
{    
    _impl->setSurface(surface);
}

}}
