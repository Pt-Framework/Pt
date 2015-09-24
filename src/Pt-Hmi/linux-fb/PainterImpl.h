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
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/RenderPath.h>

namespace Pt {
namespace Hmi {

class PaintSurface;

class PainterImpl
{
  public:
    PainterImpl( PaintSurfaceImpl* surface );
		
    virtual ~PainterImpl();  			

    void drawLine( const Gfx::PointF& from, const Gfx::PointF& to );
    
    void drawPolyline( const Gfx::PointF* points, const size_t pointCount );    

    void drawText( const Gfx::PointF& to, const Pt::String& Text );

    void drawRect( const Gfx::RectF& rectangle );

    void drawEllipse( const Gfx::PointF& topLeft, const Gfx::SizeF& size );

    void drawSurface( const Gfx::PointF& to, const PaintSurface& surface );
		
    void drawImage( const Gfx::PointF& to, const Gfx::Image& image );

    void drawPath( const Gfx::RenderPath& path );

    void fillRect( const Gfx::RectF& rectangle );    

    void fillEllipse( const Gfx::PointF& topLeft, const Gfx::SizeF& size );

    void fillPolygon( const Gfx::PointF* points, const size_t pointCount );
    
    void flush();
        
  public:
    void setPen( const Gfx::Pen& pen )
    {
	    _pen = pen;
    }

    const Gfx::Pen& pen() const
    {
	    return _pen;
    }

    void setBrush( const Gfx::Brush& brush )
    {
	    _brush = brush ;
    }

    const Gfx::Brush& brush() const
    {
	    return _brush;
    }

    void setFont( const Gfx::Font& font )
    {
      _font = font;	
    }

    const Gfx::Font& font() const
    {
	    return _font;
    }    
    
     Gfx::FontMetrics fontMetrics( Pt::String text ) const;

     static Gfx::FontMetrics fontMetrics( const Gfx::Font& font, Pt::String text );
    
    const std::list<std::string>& fontFamilyNames();


    void setRenderMode(Gfx::RenderMode::Type mode )
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

    void setClip( const Gfx::RectF& clip )
    {
      _clip = clip;
    }

    const Gfx::RectF& clip() const
    {
      return _clip;
    }

    void clear( const Gfx::Color& color );

  private:
	 PaintSurfaceImpl*   	_surface;	
	 Gfx::RenderMode::Type 	_renderMode;
	 Gfx::Font 				_font;
	 Gfx::Pen   			_pen;
 	 Gfx::Brush 			_brush;	  
	 Gfx::RectF       _clip;
};

}}

#endif
