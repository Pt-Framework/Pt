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
#include "ScreenImpl.h"

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl( PaintSurfaceImpl* surface )
: _surface( surface )
, _renderMode(Gfx::RenderMode::NoAlpha )
{	
}


PainterImpl::~PainterImpl()
{
}

void PainterImpl::drawLine( const Gfx::PointF& from, const Gfx::PointF& to )
{    
  _surface->path().stroke( _pen, from , to );
}


void PainterImpl::drawText( const Gfx::PointF& to, const Pt::String& text )
{	  
  _surface->path().text( _pen, _font, text);
}


void PainterImpl::drawEllipse( const Gfx::PointF& topLeft, const Gfx::SizeF& size )
{
 _surface->path().ellipse( topLeft, size );
}


void PainterImpl::drawRect( const Gfx::RectF& rect )
{
  _surface->path().stroke( _pen, rect );  
}


void PainterImpl::drawPolyline( const Gfx::PointF* pt, const size_t pointCount )
{
  _surface->path().stroke( _pen, pt, pointCount );  
}


void PainterImpl::fillRect( const Gfx::RectF& rect )
{ 
  _surface->path().fill( _brush, rect );
}


void PainterImpl::fillEllipse( const Gfx::PointF& topLeft, const Gfx::SizeF& size )
{
  _surface->path().fillEllipse( _brush, topLeft, size );
}


void PainterImpl::fillPolygon( const Gfx::PointF* pt, const size_t pointCount )
{  
  _surface->path().fill( _brush, pt, pointCount );
}


void PainterImpl::drawSurface( const Gfx::PointF& to, const PaintSurface& pm )
{
 Gfx::RenderPath path( pm.impl()->path() );
  
  path.translate( to.x(), to.y() );  

  _surface->path().path(Gfx::RectF( to, pm.size() ),  path );
}
		

void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::Image& image )
{  
  _surface->path().image( to, image );
}


void PainterImpl::drawPath( const Gfx::RenderPath& path )
{
  _surface->path().path(Gfx::RectF(Gfx::PointF(0,0), _surface->size() ),  path );
}


void PainterImpl::flush()
{	
 Gfx::ImagePainter painter( Application::instance().mainScreen().impl()->image() );

  painter.drawPath( _surface->path() );  
	_surface->path().clear();
}


Gfx::FontMetrics PainterImpl::fontMetrics( Pt::String text ) const
{  
  return Gfx::ImagePainter::fontMetrics( _font, text);
}


void PainterImpl::clear( const Gfx::Color& color )
{      
 _surface->path().clear();

  setBrush(Gfx::Brush(color) );
  fillRect(Gfx::RectF(Gfx::PointF( 0,0 ), _surface->size() ) ); 
}


}}
