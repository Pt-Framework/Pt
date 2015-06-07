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

void PainterImpl::setPen(const Ui::Pen& pen)
{
	_painter.setPen(pen);
}

const Ui::Pen& PainterImpl::pen() const
{
	return _painter.pen();
}

void PainterImpl::setBrush(const Ui::Brush& brush)
{
	_painter.setBrush(brush);
}

const Ui::Brush& PainterImpl::brush() const
{
	return _painter.brush();
}

void PainterImpl::setFont(const Ui::Font& font)
{
	_painter.setFont(font);
}

const Ui::Font& PainterImpl::font() const
{
	return _painter.font();
}

Ui::FontMetrics PainterImpl::fontMetrics() const
{
	return _painter.fontMetrics();
}

Ui::FontMetrics PainterImpl::fontMetrics(Pt::String text) const
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

void PainterImpl::drawPixel(const Ui::PointF& to)
{
	_painter.drawPixel(to);
}

void PainterImpl::drawLine(const Ui::PointF& from, const Ui::PointF& to)
{
	_painter.drawLine(from, to);
}

void PainterImpl::drawText( const Ui::PointF& to, const Pt::String& text, const Ui::Color* outline )
{
	Ui::FontMetrics metrics =  fontMetrics(text);
	_painter.drawText(to, text, outline);
}

void PainterImpl::drawText(const Ui::PointF& to, const Pt::String& text)
{	
	Ui::FontMetrics metrics =  fontMetrics(text);	
	_painter.drawText(to, text);
}

void PainterImpl::drawRect(const Ui::RectF& rectangle)
{
	_painter.drawRect(rectangle);
}

void PainterImpl::fillRect(const Ui::RectF& rectangle)
{
	_painter.fillRect(rectangle);
}

void PainterImpl::drawEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size)
{
	_painter.drawEllipse(topLeft, size);
}

void PainterImpl::fillEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size)
{
	_painter.fillEllipse(topLeft, size);
}

void PainterImpl::drawPolyline(const Ui::PointF* points, const size_t pointCount)
{
	_painter.drawPolyline(points, pointCount);		
}

void PainterImpl::fillPolygon(const Ui::PointF* points, const size_t pointCount)
{
	_painter.fillPolygon(points,pointCount);
}

void PainterImpl::drawSurface(const Ui::PointF& to, PaintSurface& pm, const Ui::Region& pmRegion)
{
	_painter.drawImage(to, pm.impl()->image(), pmRegion);
}

void PainterImpl::drawSurface(const Ui::PointF& to, PaintSurface& pm)
{
	_painter.drawImage(to, pm.impl()->image());
}
		
void PainterImpl::drawImage(const Ui::PointF& to, const Ui::Image& image)
{
	_painter.drawImage(to, image);
}

void PainterImpl::drawImage(const Ui::PointF& to, const Ui::Image& image, const Ui::Region& imageRegion)
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
