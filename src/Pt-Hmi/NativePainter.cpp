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
#include <Pt/Hmi/NativePainter.h>
#include <Pt/Hmi/NativePaintSurface.h>
#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"

namespace Pt {
namespace Hmi {

NativePainter::NativePainter(NativePaintSurface& surface)
: _impl(new PainterImpl(surface.impl()))
{	
}

NativePainter::~NativePainter()
{
	delete _impl;
}

void NativePainter::setPen(const Gfx::Pen& pen)
{
	_impl->setPen(pen);
}

const Gfx::Pen& NativePainter::pen() const
{
	return _impl->pen();
}

void NativePainter::setBrush(const Gfx::Brush& brush)
{
	_impl->setBrush(brush);
}

const Gfx::Brush& NativePainter::brush() const
{
	return _impl->brush();
}

void NativePainter::setFont(const Gfx::Font& font)
{
	_impl->setFont(font);
}

const Gfx::Font& NativePainter::font() const
{
	return _impl->font();
}

Gfx::FontMetrics NativePainter::fontMetrics() const
{
	return _impl->fontMetrics();
}

Gfx::FontMetrics NativePainter::fontMetrics(Pt::String Text) const
{
	return _impl->fontMetrics(Text);
}

const std::list<std::string>& NativePainter::fontFamilyNames()
{
	return _impl->fontFamilyNames();
}

int NativePainter::depth() const
{
	return _impl->depth();
}

void NativePainter::drawPixel(const Pt::Gfx::PointF& to)
{
	_impl->drawPixel(to);
}

void NativePainter::drawLine(const Pt::Gfx::PointF& from, const Pt::Gfx::PointF& to)
{
	_impl->drawLine(from,to);
}

void NativePainter::drawText( const Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline )
{
	_impl->drawText( to,text, outline);
}

void NativePainter::drawText(const Pt::Gfx::PointF& to, const Pt::String& Text)
{
	_impl->drawText(to, Text);
}

void NativePainter::drawRect(const Pt::Gfx::RectF& rectangle)
{
	_impl->drawRect(rectangle);
}

void NativePainter::fillRect(const Pt::Gfx::RectF& rectangle)
{
	_impl->fillRect(rectangle);
}

void NativePainter::drawEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size)
{
	_impl->drawEllipse(topLeft, size);
}

void NativePainter::fillEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size)
{
	_impl->fillEllipse(topLeft, size);
}

void NativePainter::drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount)
{
	_impl->drawPolyline(points, pointCount);
}

void NativePainter::fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount)
{
	_impl->fillPolygon(points, pointCount);
}

void NativePainter::drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm, const Pt::Gfx::Region& pmRegion)
{
	NativePaintSurface* surface = dynamic_cast<NativePaintSurface*>(&pm);
	
	if( surface != 0)
		_impl->drawSurface(to, pm, pmRegion);
	else
		drawImage(to, pm.toImage(), pmRegion);
}

void NativePainter::drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm)
{
	NativePaintSurface* surface = dynamic_cast<NativePaintSurface*>(&pm);
	
	if( surface != 0)
		_impl->drawSurface(to, pm);
	else
		drawImage(to, pm.toImage());
}	
	
void NativePainter::drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image)
{
	_impl->drawImage(to, image);
}

void NativePainter::drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion)
{
	_impl->drawImage(to, image, imageRegion);
}

void NativePainter::addFontName(const std::string& fontName)
{
	_impl->addFontName(fontName);
}

}}
