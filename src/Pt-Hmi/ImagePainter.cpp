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
#include <Pt/Api.h>
#include <Pt/Hmi/ImagePainter.h>
#include <Pt/Hmi/ImagePaintSurface.h>
#include <Pt/Hmi/Application.h>

namespace Pt {
namespace Hmi {

ImagePainter::ImagePainter(ImagePaintSurface& surface)
: _painter(surface.image())
{	
}

ImagePainter::~ImagePainter()
{
}

Pt::Gfx::Point ImagePainter::fromUnit(const Pt::Gfx::PointF& value)
{
	return Pt::Hmi::Application::instance().fromUnit(value);
}

Pt::Gfx::Size ImagePainter::fromUnit(const Pt::Gfx::SizeF& value)
{
	return Pt::Hmi::Application::instance().fromUnit(value);
}

Pt::Gfx::Rect ImagePainter::fromUnit(const Pt::Gfx::RectF& value)
{
	return Pt::Hmi::Application::instance().fromUnit(value);
}

void ImagePainter::fromUnit(const Pt::Gfx::PointF* points, size_t count, std::vector<Pt::Gfx::Point>& out)
{
	out.clear();

	for( size_t i = 0; i < count; ++i)
		out.push_back(fromUnit(points[i]));
}

void ImagePainter::setPen(const Gfx::Pen& pen)
{
	_painter.setPen(pen);
}

const Gfx::Pen& ImagePainter::pen() const
{
	return _painter.pen();
}

void ImagePainter::setBrush(const Gfx::Brush& brush)
{
	_painter.setBrush(brush);
}

const Gfx::Brush& ImagePainter::brush() const
{
	return _painter.brush();
}

void ImagePainter::setFont(const Gfx::Font& font)
{
	_painter.setFont(font);
}

const Gfx::Font& ImagePainter::font() const
{
	return _painter.font();
}

Gfx::FontMetrics ImagePainter::fontMetrics() const
{
	return _painter.fontMetrics();
}

Gfx::FontMetrics ImagePainter::fontMetrics(Pt::String text) const
{
	return _painter.fontMetrics(text);
}

const std::list<std::string>& ImagePainter::fontFamilyNames()
{
	return _painter.fontFamilyNames();
}

int ImagePainter::depth() const
{
	return 0;
}

void ImagePainter::drawPixel(const Pt::Gfx::PointF& to)
{
	_painter.drawPixel(fromUnit(to));
}

void ImagePainter::drawLine(const Pt::Gfx::PointF& from, const Pt::Gfx::PointF& to)
{
	_painter.drawLine(fromUnit(from), fromUnit(to));
}

void ImagePainter::drawText( const Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline )
{
	Pt::Gfx::FontMetrics metrics =  fontMetrics(text);
	Gfx::Point  point = fromUnit(to);
	Gfx::Point newTo = Pt::Gfx::Point(point.x(), point.y() + metrics.descent());
	_painter.drawText(newTo, text, outline);
}

void ImagePainter::drawText(const Pt::Gfx::PointF& to, const Pt::String& text)
{	
	Pt::Gfx::FontMetrics metrics =  fontMetrics(text);
	Gfx::Point  point = fromUnit(to);
	Gfx::Point newTo = Pt::Gfx::Point(point.x(), point.y() + metrics.ascent());
	_painter.drawText(newTo, text);
}

void ImagePainter::drawRect(const Pt::Gfx::RectF& rectangle)
{
	_painter.drawRect(fromUnit(rectangle));
}

void ImagePainter::fillRect(const Pt::Gfx::RectF& rectangle)
{
	_painter.fillRect(fromUnit(rectangle));
}

void ImagePainter::drawEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size)
{
	_painter.drawEllipse(fromUnit(topLeft), fromUnit(size));
}

void ImagePainter::fillEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size)
{
	_painter.fillEllipse(fromUnit(topLeft), fromUnit(size));
}

void ImagePainter::drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount)
{
	std::vector<Pt::Gfx::Point> transPoints;

	fromUnit(points, pointCount, transPoints);

	_painter.drawPolyline(&transPoints[0], transPoints.size());		
}

void ImagePainter::fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount)
{
	std::vector<Pt::Gfx::Point> transPoints;

	fromUnit(points, pointCount, transPoints);

	_painter.fillPolygon(&transPoints[0], transPoints.size());
}

void ImagePainter::drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm, const Pt::Gfx::Region& pmRegion)
{
	_painter.drawImage(fromUnit(to), pm.toImage(), pmRegion);
}

void ImagePainter::drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm)
{
	_painter.drawImage(fromUnit(to), pm.toImage());
}
		
void ImagePainter::drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image)
{
	_painter.drawImage(fromUnit(to), image);
}

void ImagePainter::drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion)
{
	_painter.drawImage(fromUnit(to), image, imageRegion);
}

void ImagePainter::addFontName(const std::string& fontName)
{
	
}

	
}}
