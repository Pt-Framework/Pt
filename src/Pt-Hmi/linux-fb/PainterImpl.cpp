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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
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
, _surface(surface)
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
	_painter.drawPixel(fromOrigin(to));
}

void PainterImpl::drawLine(const Ui::PointF& from, const Ui::PointF& to)
{
	_painter.drawLine(fromOrigin(from), fromOrigin(to));
}

void PainterImpl::drawText( const Ui::PointF& to, const Pt::String& text, const Ui::Color* outline )
{
	Ui::FontMetrics metrics =  fontMetrics(text);
	_painter.drawText(fromOrigin(to), text, outline);
}

void PainterImpl::drawText(const Ui::PointF& to, const Pt::String& text)
{	
	Ui::FontMetrics metrics =  fontMetrics(text);	
	_painter.drawText(fromOrigin(to), text);
}

void PainterImpl::drawRect(const Ui::RectF& rectangle)
{
	_painter.drawRect(fromOrigin(rectangle));
}

void PainterImpl::fillRect(const Ui::RectF& rectangle)
{
	_painter.fillRect(fromOrigin(rectangle));
}

void PainterImpl::drawEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size)
{
	_painter.drawEllipse(fromOrigin(topLeft), size);
}

void PainterImpl::fillEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size)
{
	_painter.fillEllipse(topLeft, size);
}

void PainterImpl::drawPolyline(const Ui::PointF* points, const size_t pointCount)
{
	std::vector<Ui::PointF> convPoints(points, points + pointCount);

	for( size_t i = 0; i < convPoints.size(); ++i)
		convPoints[i] = fromOrigin(points[i]);

	_painter.drawPolyline(&convPoints[0], pointCount);		
}


void PainterImpl::fillPolygon(const Ui::PointF* points, const size_t pointCount)
{
	std::vector<Ui::PointF> convPoints(points, points + pointCount);

	for( size_t i = 0; i < convPoints.size(); ++i)
		convPoints[i] = fromOrigin(points[i] );

	_painter.fillPolygon(&convPoints[0],pointCount);
}

void PainterImpl::drawSurface(const Ui::PointF& to, PaintSurface& pm, const Ui::Region& pmRegion)
{
	_painter.drawImage(fromOrigin(to), pm.impl()->image(), pmRegion);
}

void PainterImpl::drawSurface( const Ui::PointF& to, PaintSurface& surface)
{
	if( surface.originPos() != Ui::PointF(0,0) || surface.originSize() != surface.size() )
	{
		//Todo: optimize this
		Ui::Point pos((size_t)surface.originPos().x(),(size_t)surface.originPos().y());
		Ui::Size  size( (size_t)surface.originSize().width(), (size_t)surface.originSize().height() );

		Ui::Region regionIn(pos, size );
		Ui::Image subImage = surface.impl()->image().subImage(regionIn);
		_painter.drawImage(fromOrigin(to), subImage );
	}
	else
	{
		_painter.drawImage(fromOrigin(to), surface.impl()->image() );
	}
}
		
void PainterImpl::drawImage(const Ui::PointF& to, const Ui::Image& image)
{
	_painter.drawImage(fromOrigin(to), image);
}

void PainterImpl::drawImage(const Ui::PointF& to, const Ui::Image& image, const Ui::Region& imageRegion)
{
	_painter.drawImage(fromOrigin(to), image, imageRegion);
}

void PainterImpl::addFontName(const std::string& fontName)
{
	
}

Ui::PointF PainterImpl::fromOrigin(const Ui::PointF& p)
{
  return Ui::PointF(_surface->originPos().x() + p.x(), _surface->originPos().y() + p.y());
}


Ui::RectF PainterImpl::fromOrigin(const Ui::RectF& p)
{
	Ui::PointF pos( _surface->originPos().x() + p.left(), _surface->originPos().y() + p.top() );
	Ui::SizeF size( p.size() );
	const Ui::SizeF& orgSize = _surface->originSize();

	if( size.width() > orgSize.width() )
		size.setWidth( orgSize.width() );

	if( size.height() > orgSize.height() )
		size.setHeight( orgSize.height() );
	
  return Ui::RectF( pos, size);
}

void PainterImpl::setSurface(PaintSurface& s)
{
	_surface = s.impl();
}

	
}}
