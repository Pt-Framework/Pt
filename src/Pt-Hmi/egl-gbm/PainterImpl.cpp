 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
    Copyright (C) 2015 Ilja Maier
  
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
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/System/Logger.h>

PT_LOG_DEFINE("Pt.Hmi.Painter")

namespace Pt {

namespace Hmi {

PainterImpl::PainterImpl( PaintSurfaceImpl* surface )
: _surface( surface )
, _renderMode(Gfx::RenderMode::NoAlpha )
{	
  PT_LOG_DEBUG("PainterImpl");
}


PainterImpl::~PainterImpl()
{
}

void PainterImpl::drawLine( const Gfx::PointF& from, const Gfx::PointF& to )
{    
  PT_LOG_DEBUG("drawLine");
}


void PainterImpl::drawText( const Gfx::PointF& to, const Pt::String& text )
{	  
  PT_LOG_DEBUG("drawText");
}


void PainterImpl::drawEllipse( const Gfx::PointF& topLeft, const Gfx::SizeF& size )
{
  PT_LOG_DEBUG("drawEllipse");
}


void PainterImpl::drawRect( const Gfx::RectF& rect )
{
  PT_LOG_DEBUG("drawRect");
}


void PainterImpl::drawPolyline( const Gfx::PointF* pt, const size_t pointCount )
{
  PT_LOG_DEBUG("drawPolyline");
}


void PainterImpl::fillRect( const Gfx::RectF& rect )
{ 
  PT_LOG_DEBUG("fillRect");
}


void PainterImpl::fillEllipse( const Gfx::PointF& topLeft, const Gfx::SizeF& size )
{
  PT_LOG_DEBUG("fillEllipse");
}


void PainterImpl::fillPolygon( const Gfx::PointF* pt, const size_t pointCount )
{  
  PT_LOG_DEBUG("fillPolygon");
}


void PainterImpl::drawSurface( const Gfx::PointF& to, const PaintSurface& pm )
{
  PT_LOG_DEBUG("drawSurface");
}
		

void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::Image& image )
{  
  PT_LOG_DEBUG("drawImage");
}


void PainterImpl::flush()
{	
  PT_LOG_DEBUG("flush");
}


Gfx::FontMetrics PainterImpl::fontMetrics( Pt::String text ) const
{  
  return Gfx::ImagePainter::fontMetrics( _font, text);
}


Gfx::FontMetrics PainterImpl::fontMetrics( const Gfx::Font& font, Pt::String text )
{
  return Gfx::ImagePainter::fontMetrics( font, text );
}


void PainterImpl::clear( const Gfx::Color& color )
{
  PT_LOG_DEBUG("clear");
}

} // namespace

} // namespace
