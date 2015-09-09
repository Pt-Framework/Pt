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
#include <Pt/Hmi/LinePath.h>
#include <Pt/Hmi/TextPath.h>
#include <Pt/Hmi/EllipsePath.h>
#include <Pt/Hmi/RectPath.h>
#include <Pt/Hmi/PolylinePath.h>
#include <Pt/Hmi/FillRectPath.h>
#include <Pt/Hmi/FillEllipsePath.h>
#include <Pt/Hmi/FillPolygonPath.h>
#include <Pt/Hmi/SurfacePath.h>
#include <Pt/Hmi/ImagePath.h>
#include "ScreenImpl.h"

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _painter( Application::instance().mainScreen().impl()->image())
, _surface(surface)
{	
}

PainterImpl::~PainterImpl()
{
}

void PainterImpl::setPen(const Ui::Pen& pen)
{
	_pen = pen;
}

const Ui::Pen& PainterImpl::pen() const
{
	return _pen;
}

void PainterImpl::setBrush(const Ui::Brush& brush)
{
	_brush = brush;
}

const Ui::Brush& PainterImpl::brush() const
{
	return _brush;
}

void PainterImpl::setFont(const Ui::Font& font)
{
  _painter.setFont( font );
	_font = font;
}

const Ui::Font& PainterImpl::font() const
{
	return _font;
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
  _surface->addPath( new LinePath( _pen, fromOrigin(to), fromOrigin(to) ) );
}


void PainterImpl::drawLine(const Ui::PointF& from, const Ui::PointF& to)
{    
  _surface->addPath( new LinePath( _pen, fromOrigin(to), fromOrigin(to) ) );
}


void PainterImpl::drawText( const Ui::PointF& to, const Pt::String& text, const Ui::Color* outline )
{	 
  _surface->addPath( new TextPath(_pen, fromOrigin(to), _font,  text , outline) );
}


void PainterImpl::drawText(const Ui::PointF& to, const Pt::String& text)
{	  
  _surface->addPath( new TextPath(_pen, fromOrigin(to), _font,  text, 0) );
}


void PainterImpl::drawEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size)
{
  _surface->addPath( new EllipsePath(_pen, fromOrigin( topLeft ), size) );	
}


void PainterImpl::drawRect(const Ui::RectF& rectangle)
{
  _surface->addPath( new RectPath( _pen, rectangle) );
}


void PainterImpl::drawPolyline(const Ui::PointF* points, const size_t pointCount)
{
	std::vector<Ui::PointF> convPoints(points, points + pointCount);

	for( size_t i = 0; i < convPoints.size(); ++i)
		convPoints[i] = fromOrigin(points[i]);

  _surface->addPath( new PolylinePath(_pen, &convPoints[0], convPoints.size() ) );
}


void PainterImpl::fillRect(const Ui::RectF& rectangle)
{
  Ui::RectF orgRect = fromOrigin( rectangle );
  _surface->addPath( new FillRectPath(_brush, orgRect) );
}


void PainterImpl::fillEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size)
{
  _surface->addPath( new FillEllipsePath( _brush,fromOrigin( topLeft ), size ) );
}


void PainterImpl::fillPolygon(const Ui::PointF* points, const size_t pointCount)
{
	std::vector<Ui::PointF> convPoints(points, points + pointCount);

	for( size_t i = 0; i < convPoints.size(); ++i)
		convPoints[i] = fromOrigin(points[i] );

  _surface->addPath( new FillPolygonPath( _brush, &convPoints[0], convPoints.size() );
}


void PainterImpl::drawSurface(const Ui::PointF& to, PaintSurface& pm, const Ui::Region& pmRegion)
{
  _surface->addPath( new SurfacePath( pm, fromOrigin(to), &pmRegion ) );
}


void PainterImpl::drawSurface( const Ui::PointF& to, PaintSurface& pm)
{
  _surface->addPath( new SurfacePath( pm, fromOrigin(to), 0) );
}
		

void PainterImpl::drawImage(const Ui::PointF& to, const Ui::Image& image)
{
  _surface->addPath( new ImagePath( image, fromOrigin(to), 0) );
}

void PainterImpl::drawImage(const Ui::PointF& to, const Ui::Image& image, const Ui::Region& imageRegion)
{
	_surface->addPath( new ImagePath( image, fromOrigin(to), &imageRegion) );
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


void PainterImpl::flush()
{
	const std::vector<RenderPath*>& paths = _surface->path();
	Ui::Region myClip = _painter.clip();

  for( size_t i = 0; i < paths.size(); ++i )
  {
    switch( paths[i]->operation() )
    {
      case RenderPath::DrawLine:
      {
         LinePath* path = (LinePath*)paths[i];
         _painter.setPen( path->pen() );
         _painter.drawLine( path->from() , path->to() );
      }
      break;
      
      case RenderPath::DrawPolyline:
      {
         PolylinePath* path = (PolylinePath*)paths[i];
         _painter.setPen( path->pen() );
         _painter.drawPolyline( &path->points()[0], path->points().size() );
      }
      break;
      
      case RenderPath::DrawText:
      {
        TextPath* path = (TextPath*) paths[i];
        _painter.setPen( path->pen() );

        if( path->outline() != 0 )
          _painter.drawText(path->to(), path->text(), path->outline() );
        else
          _painter.drawText(path->to(), path->text());
      }
      break;
      
      case RenderPath::DrawRect:
      {
        RectPath* path = (RectPath*) paths[i];
        _painter.setPen( path->pen() );
        _painter.drawRect( path->rect() );
      }
      break;
      
      case RenderPath::DrawPixel:
      {
         LinePath* path = (LinePath*)paths[i];
         _painter.setPen( path->pen() );
         _painter.drawPixel( path->to() );
      }
      break;
      
      case RenderPath::DrawEllipse:
      {
        EllipsePath* path = (EllipsePath*) paths[i];
        
        _painter.setPen( path->pen() );
        _painter.drawEllipse( path->topLeft(), path->size() );
      }
      break;
      
      case RenderPath::DrawSurface:
      {
        SurfacePath* path = (SurfacePath*) paths[i];
				_painter.setClip( Ui::Region( Ui::Point(path->to().x(),path->to().x()) , Ui::Size(path->surface().size().width(), path->surface().size().height() ) );
        path->surface().painter().impl()->flush();        
				_painter.setClip( myClip );
      }
      break;

      case RenderPath::DrawImage:
      {
				ImagePath* path =  (ImagePath*) paths[i];
        _painter.drawImage(path->to(), path->image(), path->region() );
      }
      break;

      case RenderPath::FillRect:
			{
				FillRectPath* path = (FillRectPath*) paths[i];
				_painter.setBrush( path->brush() );
				_painter.fillRect( path->rect() ); 
			}
      break;

      case RenderPath::FillEllipse:
			{
				FillEllipsePath* path = (FillEllipsePath*) paths[i];
				_painter.setBrush( path->brush() );
				_painter.fillEllipse( path->topLeft(), path->size() );
			}

      break;

      case RenderPath::FillPolygon:
			{
				FillPolygonPath* path = (FillPolygonPath*) paths[i];
				_painter.setBrush( path->brush() );
				_painter.fillPolygon( &path->points()[0], path->points().size() );
			}
      break;
    }
  }
}
	
}}
