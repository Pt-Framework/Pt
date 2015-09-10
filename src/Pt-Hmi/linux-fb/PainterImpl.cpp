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
#include <Pt/Hmi/SurfacePathInfo.h>
#include <Pt/Ui/ImagePathInfo.h>
#include <Pt/Ui/LinePathInfo.h>
#include <Pt/Ui/TextPathInfo.h>
#include <Pt/Ui/EllipsePathInfo.h>
#include <Pt/Ui/RectPathInfo.h>
#include <Pt/Ui/PolylinePathInfo.h>
#include <Pt/Ui/FillRectPathInfo.h>
#include <Pt/Ui/FillEllipsePathInfo.h>
#include <Pt/Ui/FillPolygonPathInfo.h>
#include "ScreenImpl.h"

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl( PaintSurfaceImpl* surface )
: _painter( Application::instance().mainScreen().impl()->image() )
, _surface( surface )
{	
}


PainterImpl::~PainterImpl()
{
}


void PainterImpl::drawPixel( const Ui::PointF& to )
{   
  _surface->path().add( new Ui::LinePathInfo( pen(), to, to) );
}


void PainterImpl::drawLine( const Ui::PointF& from, const Ui::PointF& to )
{    
  _surface->path().add( new Ui::LinePathInfo( pen(), to, to ) );
}


void PainterImpl::drawText( const Ui::PointF& to, const Pt::String& text, const Ui::Color* outline )
{	 
  _surface->path().add( new Ui::TextPathInfo( pen(), to, font(),  text , outline ) );
}


void PainterImpl::drawText( const Ui::PointF& to, const Pt::String& text )
{	  
  _surface->path().add( new Ui::TextPathInfo( pen(), to, font(),  text, 0 ) );
}


void PainterImpl::drawEllipse( const Ui::PointF& topLeft, const Ui::SizeF& size )
{
  _surface->path().add( new Ui::EllipsePathInfo( pen(), topLeft, size ) );	
}


void PainterImpl::drawRect( const Ui::RectF& rectangle )
{
  _surface->path().add( new Ui::RectPathInfo( pen(), rectangle) );
}


void PainterImpl::drawPolyline( const Ui::PointF* points, const size_t pointCount )
{
  _surface->path().add( new Ui::PolylinePathInfo( pen(), points, pointCount ) );
}


void PainterImpl::fillRect( const Ui::RectF& rectangle )
{
 
  _surface->path().add( new Ui::FillRectPathInfo( brush(), rectangle ) );
}


void PainterImpl::fillEllipse( const Ui::PointF& topLeft, const Ui::SizeF& size )
{
  _surface->path().add( new Ui::FillEllipsePathInfo( brush(), topLeft, size ) );
}


void PainterImpl::fillPolygon( const Ui::PointF* points, const size_t pointCount )
{
  _surface->path().add( new Ui::FillPolygonPathInfo( brush(), points, pointCount ) );
}


void PainterImpl::drawSurface( const Ui::PointF& to, PaintSurface& pm, const Ui::Region& pmRegion )
{
  _surface->path().add( new SurfacePathInfo( pm, to, &pmRegion ) );
}


void PainterImpl::drawSurface( const Ui::PointF& to, PaintSurface& pm )
{
  _surface->path().add( new SurfacePathInfo( pm, to, 0 ) );
}
		

void PainterImpl::drawImage(const Ui::PointF& to, const Ui::Image& image )
{
  _surface->path().add( new Ui::ImagePathInfo( image, to, 0 ) );
}


void PainterImpl::drawImage( const Ui::PointF& to, const Ui::Image& image, const Ui::Region& imageRegion )
{
	_surface->path.add( new Ui::ImagePathInfo( image, to, &imageRegion ) );
}


void PainterImpl::flush()
{
  const Ui::RenderPath& path = _surface->path();

  for( size_t i = 0; i < path.size(); ++i )
  {
    switch( path.at(i)->operation() )
    {
      case Ui::RenderPathInfo::DrawLine:
      {
        const Ui::LinePathInfo* info = (const Ui::LinePathInfo*)path[i];
        _painter.setPen( info->pen() );
        _painter.drawLine( info->from() , info->to() );
      }
      break;
      

      case Ui::RenderPathInfo::DrawText:
      {
        const Ui::TextPathInfo* info = (const Ui::TextPathInfo*) path[i];
        _painter.setPen( info->pen() );

        if( info->outline() != 0 )
          _painter.drawText(info->to(), info->text(), info->outline() );
        else
          _painter.drawText(info->to(), info->text());
      }
      break;

      
      case Ui::RenderPathInfo::DrawEllipse:
      {
        const Ui::EllipsePathInfo* info = (const Ui::EllipsePathInfo*) path[i];
        
        _painter.setPen( info->pen() );
        _painter.drawEllipse( info->topLeft(), info->size() );
      }
      break;


      case Ui::RenderPathInfo::DrawRect:
      {
        const Ui::RectPathInfo* info = (const Ui::RectPathInfo*) path[i];
        _painter.setPen( info->pen() );
        _painter.drawRect( info->rect() );
      }
      break;


      case Ui::RenderPathInfo::DrawPolyline:
      {
         const Ui::PolylinePathInfo* info = (const Ui::PolylinePathInfo*)path[i];
         _painter.setPen( info->pen() );
         _painter.drawPolyline( &info->points()[0], info->points().size() );
      }
      break;
          
      
      case Ui::RenderPathInfo::DrawPixel:
      {
         const Ui::LinePathInfo* info = (const Ui::LinePathInfo*)path[i];
         _painter.setPen( info->pen() );
         _painter.drawPixel( info->to() );
      }
      break;
      

      case Ui::RenderPathInfo::DrawImage:
      {
				const Ui::ImagePathInfo* info =  (const Ui::ImagePathInfo*) path[i];
        _painter.drawImage(info->to(), info->image(), info->region() );
      }
      break;


      case Ui::RenderPathInfo::FillRect:
			{
				const Ui::FillRectPathInfo* info = (const Ui::FillRectPathInfo*) path[i];
				_painter.setBrush( info->brush() );
				_painter.fillRect( info->rect() ); 
			}
      break;


      case Ui::RenderPathInfo::FillEllipse:
			{
				const Ui::FillEllipsePathInfo* info = (const Ui::FillEllipsePathInfo*) path[i];
				_painter.setBrush( info->brush() );
				_painter.fillEllipse( info->topLeft(), info->size() );
			}
      break;


      case Ui::RenderPathInfo::FillPolygon:
			{
				const Ui::FillPolygonPathInfo* info = (const Ui::FillPolygonPathInfo*) path[i];
				_painter.setBrush( info->brush() );
				_painter.fillPolygon( &info->points()[0], info->points().size() );
			}
      break;


      case Ui::RenderPathInfo::DrawSurface:
      {
        SurfacePathInfo* info = (SurfacePathInfo*) path[i];
        
        const Ui::SizeF  size( info->surface().size().width(), info->surface().size().height() );

        const Ui::PointF absolutPos     = origin() + info->to();
				PainterImpl*     surfacePainter = info->surface().painter().impl();

        surfacePainter->setOrigin( absolutPos );
        surfacePainter->setClip( Ui::RectF( absolutPos, size ) );        
        surfacePainter->flush();  
      }
      break;
    }
  }

  _surface->path().clear();
}
	
}}
