/*
 * Copyright (C) 2006 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"
#include "Pt/Types.h"
#include <Pt/System/Clock.h>
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gfx/Region.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Gfx/Rgb888Color.h"
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/NativePaintSurface.h>
#include <iostream>
#include <algorithm>

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _surface(surface)
, _pen(Gfx::Pen(1))
, _brush(Gfx::Brush(Gfx::ARgbColor(0, 0, 0)))
, _font(Gfx::Font(determinePlatformDefaultFontName()))
{
}


PainterImpl::~PainterImpl()
{
}


void PainterImpl::drawText( const Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline )
{
    drawText(to, text);
}

void PainterImpl::setPen(const Gfx::Pen& pen)
{
    if (pen == _pen) 
	{
        return;
    }

    _pen = pen;
    updatePen();
}



const Gfx::Pen& PainterImpl::pen() const
{
    return _pen;
}


void PainterImpl::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
    updateBrush();
}


void PainterImpl::updateBrush()
{
  
}


void PainterImpl::updatePen()
{
  
}

const Gfx::Brush& PainterImpl::brush() const
{
    return _brush;
}

void PainterImpl::setFont(const Gfx::Font& font)
{
    if (font == _font) 
	{
        return;
    }

    _font = font;
    updateFont();
}


void PainterImpl::updateFont()
{
   	//Todo:
}

std::string PainterImpl::determinePlatformDefaultFontName()
{
	//Todo:
  return "";
}


const Gfx::Font& PainterImpl::font() const
{
    return _font;
}


Gfx::FontMetrics PainterImpl::fontMetrics() const
{
  Gfx::FontMetrics metric;
  //Todo:
  return metric;
}


Gfx::FontMetrics PainterImpl::fontMetrics(Pt::String text) const
{

     //Todo:
  Gfx::FontMetrics metric;
  //Todo:
  return metric;
     
}


void PainterImpl::addFontName(const std::string& fontName)
{
    _fontNamesList.push_back(fontName);
}

const std::list<std::string>& PainterImpl::fontFamilyNames()
{
    return _fontNamesList;
}


int PainterImpl::depth() const
{
   return 0;
}


void PainterImpl::drawPixel(const Pt::Gfx::PointF& toF)
{
    Pt::Gfx::Point to = Application::instance().fromUnit(toF);

    Gfx::Rgb888Color col;

    assign( col, _pen.color() );
    //Todo:
}


void PainterImpl::drawLine(const Pt::Gfx::PointF& fromF, const  Pt::Gfx::PointF& toF)
{
	if (_pen.size() == 0) 
		return; 

	Pt::Gfx::Point from = Application::instance().fromUnit(fromF);
	Pt::Gfx::Point to = Application::instance().fromUnit(toF);


//Todo:
}

void PainterImpl::drawText(const Pt::Gfx::PointF& toF, const Pt::String& text)
{
	Pt::Gfx::Point to = Application::instance().fromUnit(toF);
//Todo:
}

void PainterImpl::fillRect(const Pt::Gfx::RectF& rectF)
{
	Pt::Gfx::Rect rect = Application::instance().fromUnit(rectF);
//Todo:
}

void PainterImpl::drawRect(const Pt::Gfx::RectF& rectF)
{
	Pt::Gfx::Rect rect = Application::instance().fromUnit(rectF);
//Todo:
}


void PainterImpl::drawEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
	Pt::Gfx::Point topLeft = Application::instance().fromUnit(topLeftF);
	Pt::Gfx::Size size = Application::instance().fromUnit(sizeF);
//Todo:	
}


void PainterImpl::fillEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
	Pt::Gfx::Point topLeft = Application::instance().fromUnit(topLeftF);
	Pt::Gfx::Size size = Application::instance().fromUnit(sizeF);
//Todo:
}


void PainterImpl::drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount)
{
    if (_pen.size() == 0)
       return;
//Todo:
}


void PainterImpl::fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount)
{
//Todo:
}


void PainterImpl::drawSurface(const Pt::Gfx::PointF& toF, PaintSurface& surface, const  Pt::Gfx::Region& pixmapRegion)
{
	NativePaintSurface* nativeSurface = dynamic_cast<NativePaintSurface*>(&surface);

	Pt::Gfx::Point to = Application::instance().fromUnit(toF);
//Todo:	
}

void PainterImpl::drawSurface(const Pt::Gfx::PointF& toF, PaintSurface& surface)
{
	NativePaintSurface* nativeSurface = dynamic_cast<NativePaintSurface*>(&surface);
	
	if(nativeSurface == 0)
		throw std::exception();
//Todo:		
}

void PainterImpl::drawImage(const Pt::Gfx::PointF& toF, const Gfx::ARgbImage& image)
{
	Pt::Gfx::Point to = Application::instance().fromUnit(toF);

//Todo:
}

void PainterImpl::drawImage(const Pt::Gfx::PointF& toF, const Gfx::ARgbImage& image, const  Pt::Gfx::Region& imageRegion)
{
	Pt::Gfx::Point to = Application::instance().fromUnit(toF);
//Todo:	
}

void PainterImpl::drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height)
{
//Todo:
}


void PainterImpl::drawCompatibleImage(size_t x, size_t y, size_t depth, const char* data, size_t width, size_t height)
{
//Todo:
}

}}

