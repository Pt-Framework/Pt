/***************************************************************************
 *   Copyright (C) 2014 Laurentiu-Gheorghe Crisa                           *
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
#include <Pt/Api.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _painter(surface->image())
{	
}

PainterImpl::~PainterImpl()
{
}

void PainterImpl::setPen(const Gfx::Pen& pen)
{
	_painter.setPen(pen);
}

const Gfx::Pen& PainterImpl::pen() const
{
	return _painter.pen();
}

void PainterImpl::setBrush(const Gfx::Brush& brush)
{
	_painter.setBrush(brush);
}

const Gfx::Brush& PainterImpl::brush() const
{
	return _painter.brush();
}

void PainterImpl::setFont(const Gfx::Font& font)
{
	_painter.setFont(font);
}

const Gfx::Font& PainterImpl::font() const
{
	return _painter.font();
}

Gfx::FontMetrics PainterImpl::fontMetrics() const
{
	return _painter.fontMetrics();
}

Gfx::FontMetrics PainterImpl::fontMetrics(Pt::String text) const
{
	return _painter.fontMetrics(text);
}

const std::list<std::string>& PainterImpl::fontFamilyNames()
{
	return _painter.fontFamilyNames();
}

int PainterImpl::depth() const
{
	return 0;
}

void PainterImpl::drawPixel(const Pt::Gfx::PointF& to)
{
	_painter.drawPixel(to);
}

void PainterImpl::drawLine(const Pt::Gfx::PointF& from, const Pt::Gfx::PointF& to)
{
	_painter.drawLine(from, to);
}

void PainterImpl::drawText( const Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline )
{
	Pt::Gfx::FontMetrics metrics =  fontMetrics(text);
	_painter.drawText(to, text, outline);
}

void PainterImpl::drawText(const Pt::Gfx::PointF& to, const Pt::String& text)
{	
	Pt::Gfx::FontMetrics metrics =  fontMetrics(text);	
	_painter.drawText(to, text);
}

void PainterImpl::drawRect(const Pt::Gfx::RectF& rectangle)
{
	_painter.drawRect(rectangle);
}

void PainterImpl::fillRect(const Pt::Gfx::RectF& rectangle)
{
	_painter.fillRect(rectangle);
}

void PainterImpl::drawEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size)
{
	_painter.drawEllipse(topLeft, size);
}

void PainterImpl::fillEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size)
{
	_painter.fillEllipse(topLeft, size);
}

void PainterImpl::drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount)
{
	_painter.drawPolyline(points, pointCount);		
}

void PainterImpl::fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount)
{
	_painter.fillPolygon(points,pointCount);
}

void PainterImpl::drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm, const Pt::Gfx::Region& pmRegion)
{
	_painter.drawImage(to, pm.impl()->image(), pmRegion);
}

void PainterImpl::drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm)
{
	_painter.drawImage(to, pm.impl()->image());
}
		
void PainterImpl::drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image)
{
	_painter.drawImage(to, image);
}

void PainterImpl::drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion)
{
	_painter.drawImage(to, image, imageRegion);
}

void PainterImpl::addFontName(const std::string& fontName)
{
	
}

void PainterImpl::setSurface(PaintSurface& s)
{

}

	
}}
