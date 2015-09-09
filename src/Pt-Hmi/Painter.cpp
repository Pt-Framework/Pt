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

void Painter::setRenderMode(Ui::RenderMode::Type mode)
{
	_impl->setRenderMode( mode );
}

void Painter::setPen(const Ui::Pen& pen)
{
	_impl->setPen(pen);
}

const Ui::Pen& Painter::pen() const
{
	return _impl->pen();
}

void Painter::setBrush(const Ui::Brush& brush)
{
	_impl->setBrush(brush);
}

const Ui::Brush& Painter::brush() const
{
	return _impl->brush();
}

void Painter::setFont(const Ui::Font& font)
{
	_impl->setFont(font);
}

const Ui::Font& Painter::font() const
{
	return _impl->font();
}

Ui::FontMetrics Painter::fontMetrics() const
{
	return _impl->fontMetrics();
}

Ui::FontMetrics Painter::fontMetrics(Pt::String Text) const
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

void Painter::drawPixel(const Ui::PointF& to)
{
	_impl->drawPixel(to);
}

void Painter::drawLine(const Ui::PointF& from, const Ui::PointF& to)
{
	_impl->drawLine(from,to);
}

void Painter::drawText( const Ui::PointF& to, const Pt::String& text, const Ui::Color* outline )
{
	_impl->drawText( to,text, outline);
}

void Painter::drawText(const Ui::PointF& to, const Pt::String& Text)
{
	_impl->drawText(to, Text);
}

void Painter::drawRect(const Ui::RectF& rectangle)
{
	_impl->drawRect(rectangle);
}

void Painter::fillRect(const Ui::RectF& rectangle)
{
	_impl->fillRect(rectangle);
}

void Painter::drawEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size)
{
	_impl->drawEllipse(topLeft, size);
}

void Painter::fillEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size)
{
	_impl->fillEllipse(topLeft, size);
}

void Painter::drawPolyline(const Ui::PointF* points, const size_t pointCount)
{
	_impl->drawPolyline(points, pointCount);
}

void Painter::fillPolygon(const Ui::PointF* points, const size_t pointCount)
{
	_impl->fillPolygon(points, pointCount);
}

void Painter::drawSurface(const Ui::PointF& to, PaintSurface& pm, const Ui::Region& pmRegion)
{
	_impl->drawSurface(to, pm, pmRegion);
}

void Painter::drawSurface(const Ui::PointF& to, PaintSurface& pm)
{  
	_impl->drawSurface(to, pm);
}	
	
void Painter::drawImage(const Ui::PointF& to, const Ui::Image& image)
{
	_impl->drawImage(to, image);
}

void Painter::drawImage(const Ui::PointF& to, const Ui::Image& image, const Ui::Region& imageRegion)
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

void Painter::flush()
{
	_impl->flush();
}

}}
