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
#ifndef PT_HMI_PAINTERIMPL_H
#define PT_HMI_PAINTERIMPL_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Ui/ImagePainter.h>
#include <Pt/Ui/FontMetrics.h>
#include <Pt/Ui/RenderPath.h>

namespace Pt {
namespace Hmi {

class PaintSurface;

class PainterImpl
{
  public:
    PainterImpl( PaintSurfaceImpl* surface );
		
    virtual ~PainterImpl();  			

    void drawLine( const Ui::PointF& from, const Ui::PointF& to );
    
    void drawPolyline( const Ui::PointF* points, const size_t pointCount );    

    void drawText( const Ui::PointF& to, const Pt::String& Text );

    void drawRect( const Ui::RectF& rectangle );

    void drawEllipse( const Ui::PointF& topLeft, const Ui::SizeF& size );

    void drawSurface( const Ui::PointF& to, const PaintSurface& surface, const Ui::Region& pmRegion );

    void drawSurface( const Ui::PointF& to, const PaintSurface& surface );
		
    void drawImage( const Ui::PointF& to, const Ui::Image& image );

    void drawPath( const Ui::RenderPath& path );

    void fillRect( const Ui::RectF& rectangle );    

    void fillEllipse( const Ui::PointF& topLeft, const Ui::SizeF& size );

    void fillPolygon( const Ui::PointF* points, const size_t pointCount );
    
    void flush();
        
	int depth() const
	{
		return 0;
	}

  public:
    void setPen( const Ui::Pen& pen )
    {
	    _pen = pen;
    }

    const Ui::Pen& pen() const
    {
	    return _pen;
    }

    void setBrush( const Ui::Brush& brush )
    {
	    _brush = brush ;
    }

    const Ui::Brush& brush() const
    {
	    return _brush;
    }

    void setFont( const Ui::Font& font )
    {
      _font = font;	
    }

    const Ui::Font& font() const
    {
	    return _font;
    }

    Ui::FontMetrics fontMetrics() const;
    
    Ui::FontMetrics fontMetrics( Pt::String text ) const;
    
    const std::list<std::string>& fontFamilyNames();


    void setRenderMode( Ui::RenderMode::Type mode )
    {
      _renderMode =  mode;
    }

    void setSurface( PaintSurface& s )
    {
	    _surface = s.impl();
    }

    void addFontName( const std::string& fontName )
    {
	    //Todo: add font name.
    }

  private:
	  PaintSurfaceImpl*   	_surface;	
	  Ui::RenderMode::Type 	_renderMode;
	  Ui::Font 				_font;
	  Ui::Pen   			_pen;
 	  Ui::Brush 			_brush;	  
};

}}

#endif
