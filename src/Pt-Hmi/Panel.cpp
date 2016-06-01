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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA  02110-1301  USA
*/

#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Layout.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

Panel::Panel()
: _backgroundColor(Gfx::Color::fromRgb8(237,237,237))
, _foregroundColor( Gfx::Color::fromRgb8(0,0,0) )
, _backgroundImage()
, _backgroundImageLayout( ImageLayout::None )
, _borderStyle(Single)
, _borderWidth(1)
, _borderRound(false)
, _borderColor(Gfx::Color::fromRgb8(178,178,178))
{
    setAcceptsFocus(false);  
}


Panel::~Panel()
{
}


void Panel::onPaintEvent( const PaintEvent& ev )
{
    Widget::onPaintEvent(ev);

    Gfx::PointF winpos = toWindow( Gfx::PointF(0,0) );
    PaintSurface& windowSurface = this->window()->surface();

    Gfx::RectF paintRect(winpos, size());
    PaintRegion region(windowSurface, paintRect);    
    onPaint(region, ev.rect());
}


void Panel::onPaint(PaintSurface& surface, const Gfx::RectF& updateRect)
{	
    const Gfx::SizeF& size = this->size();

    if( size.width() < 0 || size.height() < 0)
        return; 

    Painter painter(surface);

	  Gfx::RectF borderRect(_borderWidth/2, size.width() - _borderWidth,
                          _borderWidth/2, size.height() - _borderWidth);

    double corner = _borderRound ? 2.0 : 0;
		std::vector<Gfx::PointF> outline(9);

    // top left
		outline[0].setX(0);
		outline[0].setY(corner);

		outline[1].setX(corner);
		outline[1].setY(0);

    // top right
		outline[2].setX(borderRect.width() - corner);
		outline[2].setY(0);

		outline[3].setX(borderRect.width());
		outline[3].setY(corner);

    // bottom right
		outline[4].setX(borderRect.width());
		outline[4].setY(borderRect.height() - corner);

		outline[5].setX(borderRect.width() - corner);
		outline[5].setY(borderRect.height());

    // bottom left
		outline[6].setX(corner);
		outline[6].setY(borderRect.height());

		outline[7].setX(0);
		outline[7].setY(borderRect.height() - corner);
            
		outline[8] = outline[0];

    Gfx::Brush brush( backgroundColor() );
    painter.setBrush(brush); 
    painter.fillPolygon(&outline[0], outline.size());

    const Gfx::Image& backImage = backgroundImage();

    if( ! backImage.empty() )
    {
        switch( _backgroundImageLayout.type() )
        {              
            default:  
            case ImageLayout::None:
            {
                painter.drawImage( Pt::Gfx::PointF(0,0), backImage );
            }
            break;
            
            case ImageLayout::Tile:
            {
                for( double x = 0; x < size.width();  x += backImage.width() )
                {
                    for( double y = 0; y < size.height();  y += backImage.height() )
                        painter.drawImage(Gfx::PointF(x,y), backImage);
                }
            }
            break;

            case ImageLayout::Center:
            {
                const double x = size.width()/2  - backImage.width()/2;
                const double y = size.height()/2  - backImage.height()/2;
                painter.drawImage(Gfx::PointF(x, y), backImage);
            }
            break;
            
            case ImageLayout::Strech:
            {
                Gfx::Image strech = backImage.blockScale(Gfx::Size((int) size.width(), (int)size.height() ) );
                painter.drawImage( Pt::Gfx::PointF(0,0), strech );
            }
            break;

            case ImageLayout::Zoom:
            {
                const double factor = size.width()/(double)backImage.width();
                Pt::Gfx::Size newSize( ( size_t)( backImage.width()*factor), (size_t)(backImage.height()*factor));

                Gfx::Image strech = backImage.blockScale(newSize);
                painter.drawImage( Pt::Gfx::PointF(0,0), strech);
            }
            break;
        }
    }

    if( _borderWidth <= 0 )
      return;
						
	  switch( _borderStyle )
	  {
		    case Single:
		    {			
            Gfx::Pen pen(static_cast<size_t>(_borderWidth), _borderColor);
			      painter.setPen(pen);
            painter.drawPolyline(&outline[0], outline.size());
            break;
		    }

		    case Border3D:
		    {
			      std::vector<Gfx::PointF> points1(3);
			      points1[0].setX(0);
			      points1[0].setY(borderRect.height());

			      points1[1].setX(0);
			      points1[1].setY(0);
				
			      points1[2].setX(0 + borderRect.width());
			      points1[2].setY(0);

			      Gfx::Pen pen(static_cast<size_t>(_borderWidth), _borderColor);
			      painter.setPen(pen);
			      painter.drawPolyline(&points1[0], points1.size());

            std::vector<Gfx::PointF> points2(3);
			      points2[0].setX(0 + borderRect.width());
			      points2[0].setY(0);

			      points2[1].setX(0 + borderRect.width());
			      points2[1].setY(0 + borderRect.height());

			      points2[2].setX(0);
			      points2[2].setY(0 + borderRect.height());

            Gfx::Color color(_borderColor.red() * 0.9f, 
                             _borderColor.green() * 0.9f,
                             _borderColor.blue() * 0.9f);

			      Gfx::Pen pen2( static_cast<size_t>(_borderWidth), color );
			      painter.setPen(pen2);
			      painter.drawPolyline(&points2[0], points2.size());	
            break;			
		    }	

        default:
            break;
	  }	
}

} // namespace

} // namespace
