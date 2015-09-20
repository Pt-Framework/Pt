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
#include <Pt/Ui/ClipLine.h>
#include <Pt/Ui/ClipPolygon.h>
#include "ScreenImpl.h"

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl( PaintSurfaceImpl* surface )
: _surface( surface )
, _renderMode( Ui::RenderMode::NoAlpha )
{	
}


PainterImpl::~PainterImpl()
{
}

void PainterImpl::drawLine( const Ui::PointF& f, const Ui::PointF& t )
{    
  Ui::ClipLine clipper;

   Ui::PointF from(f);
   Ui::PointF to(t);

  clipper.clip( from, to, 0, _surface->size().width(),  0, _surface->size().height() );

  _surface->path().stroke( _pen, from , to );
}


void PainterImpl::drawText( const Ui::PointF& to, const Pt::String& text )
{	  
  Ui::FontMetrics metrics = fontMetrics(text);

  Ui::RectF rect(to, Ui::SizeF( metrics.width(), metrics.height() ) );

  _surface->path().drawText( _pen, _font, text, rect );
}


void PainterImpl::drawEllipse( const Ui::PointF& topLeft, const Ui::SizeF& size )
{
//Todo: draw ellipse op
}


void PainterImpl::drawRect( const Ui::RectF& rect )
{
  Ui::ClipPolygon clipper;

  std::vector<Ui::PointF> points;

  points.push_back( rect.topLeft() );
  points.push_back( rect.topRight() );
  points.push_back( rect.bottomRight() );
  points.push_back( rect.bottomLeft() );

  clipper.clip( points, Ui::RectF( Ui::PointF(0,0), Ui::SizeF( _surface->size().width(), _surface->size().height() )  ) );

  if( !points.empty() )
  {
    points.push_back( points[0] );
    _surface->path().stroke( _pen, &points[0], points.size() );
  }
}


void PainterImpl::drawPolyline( const Ui::PointF* pt, const size_t pointCount )
{
  if( pointCount < 2)
      return;

  std::vector<Ui::PointF> points;
  
  Ui::PointF p2;  

  for( size_t i = 1; i < pointCount; ++i )
  {
    Ui::PointF p1 = pt[i - 1];
    p2 = pt[i];    

    Ui::ClipLine clipper;
    clipper.clip(p1, p2, 0,  _surface->size().width(), 0,  _surface->size().height()  );
    points.push_back( p1 );
  }

   points.push_back( p2 );

   _surface->path().stroke( _pen, &points[0], points.size() );
}


void PainterImpl::fillRect( const Ui::RectF& rect )
{ 
  Ui::ClipPolygon clipper;

  std::vector<Ui::PointF> points;

  points.push_back( rect.topLeft() );
  points.push_back( rect.topRight() );
  points.push_back( rect.bottomRight() );
  points.push_back( rect.bottomLeft() );

  clipper.clip( points, Ui::RectF( Ui::PointF(0,0), Ui::SizeF( _surface->size().width(), _surface->size().height() )  ) );

  _surface->path().fill( _brush, &points[0], points.size() );
}


void PainterImpl::fillEllipse( const Ui::PointF& topLeft, const Ui::SizeF& size )
{
 //Todo;  
}


void PainterImpl::fillPolygon( const Ui::PointF* pt, const size_t pointCount )
{
  Ui::ClipPolygon clipper;

  std::vector<Ui::PointF> points( pt, pt + pointCount );

  clipper.clip( points, Ui::RectF( Ui::PointF(0,0), Ui::SizeF( _surface->size().width(), _surface->size().height() )  ) );

  _surface->path().fill( _brush, &points[0], points.size() );
}


void PainterImpl::drawSurface( const Ui::PointF& to, const PaintSurface& pm )
{
  Ui::RenderPath path( pm.impl()->path() );
  
  path.translate( to.x(), to.y() );

  path.clip( Ui::RectF( Ui::PointF(0,0), _surface->size() ) );

  _surface->path().addPath( path );
}
		

void PainterImpl::drawImage(const Ui::PointF& to, const Ui::Image& image )
{
  //Todo:
/*
  std::vector<Ui::PointF> imgRect;

  imgRect.push_back( to );
  imgRect.push_back( Ui::PointF( to.x() + image.width(), to.y() ) );
  imgRect.push_back( Ui::PointF( to.x() + image.width(), to.y()  + image.height()) );
  imgRect.push_back( Ui::PointF( to.x(), to.y()  + image.height()) );

  Ui::ClipPolygon clipper;

  clipper.clip( imgRect, Ui::RectF( Ui::PointF(0,0), _surface->size() )  );
  
  _surface->path().drawImage( to, image);
  */
}


void PainterImpl::drawPath( const Ui::RenderPath& path )
{
  _surface->path().addPath( path );
}


void PainterImpl::flush()
{	
  Ui::ImagePainter painter( Application::instance().mainScreen().impl()->image() );

  painter.drawPath( _surface->path() );  
	_surface->path().clear();
}


Ui::FontMetrics PainterImpl::fontMetrics() const
{
  Ui::ImagePainter painter( Application::instance().mainScreen().impl()->image() );
	
  painter.setFont( _font );

  return painter.fontMetrics();
}


Ui::FontMetrics PainterImpl::fontMetrics( Pt::String text ) const
{
  Ui::ImagePainter painter( Application::instance().mainScreen().impl()->image() );
	
  painter.setFont( _font );

  return painter.fontMetrics(text);
}

	
const std::list<std::string>& PainterImpl::fontFamilyNames()
{
   Ui::ImagePainter painter( Application::instance().mainScreen().impl()->image() );
  return painter.fontFamilyNames();
}


}}
