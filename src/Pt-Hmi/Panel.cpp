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
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

Panel::Panel()
: _backgroundColor(Gfx::Color::fromRgb8(237,237,237))
, _foregroundColor( Gfx::Color::fromRgb8(0,0,0) )
, _backgroundImage()
, _backgroundImageLayout( NoLayout )
, _borderStyle(Single)
, _borderWidth(1)
, _borderRound(false)
, _borderColor(Gfx::Color::fromRgb8(178,178,178))
{
  setAcceptFocus(false);  
}


Panel::~Panel()
{
}


void Panel::onUpdate()
{
    if( _surface.size() != size() )
        _surface.resize( size() );

    Widget::onUpdate();
}


void Panel::onLayout(LayoutItem::Iterator begin, LayoutItem::Iterator end)
{
    Widget::onLayout(begin, end);
    
    //if( _surface.size() != size() )
    //    _surface.resize( size() );
}


void Panel::onRender(const Gfx::PointF& pos, PaintSurface& parentSurface)
{
    if( ! isValid() )
    {
        _surface.clear();
        onPaint(_surface);
    }

    Gfx::PointF offset(0, 0);
    Widget::onRender(offset, _surface);

    parentSurface.painter().drawSurface(pos, _surface);
}


void Panel::onPaint(PaintSurface& surface)
{	
    const Gfx::SizeF& size = surface.size();

    if( size.width() < 0 || size.height() < 0)
        return; 

    const Gfx::Image& backImage = backgroundImage();
    Pt::Hmi::Painter& painter = surface.painter();
    
    Gfx::Brush brush( backgroundColor() );
    painter.setBrush(brush);  
    
    Gfx::PointF origin(0, 0);
    Gfx::RectF rect( origin, size );              
    painter.fillRect(rect);

    if( ! backImage.empty() )
    {
        switch( backgroundImageLayout() )
        {              
            default:  
            case NoLayout:
            {
                painter.drawImage( Pt::Gfx::PointF(0,0), backImage );
            }
            break;
            
            case Tile:
            {
                for( double x = 0; x < size.width();  x += backImage.width() )
                {
                    for( double y = 0; y < size.height();  y += backImage.height() )
                        painter.drawImage(Gfx::PointF(x,y), backImage);
                }
            }
            break;

            case Center:
            {
                const double x = size.width()/2  - backImage.width()/2;
                const double y = size.height()/2  - backImage.height()/2;
                painter.drawImage(Gfx::PointF(x,y), backImage);
            }
            break;
            
            case Strech:
            {
                Gfx::Image strech = backImage.blockScale(Gfx::Size((int) size.width(), (int)size.height() ) );
                painter.drawImage( origin, strech );
            }
            break;

            case Zoom:
            {
                const double factor = size.width()/(double)backImage.width();
                Pt::Gfx::Size newSize( ( size_t)( backImage.width()*factor), (size_t)(backImage.height()*factor));

                Gfx::Image strech = backImage.blockScale(newSize);
                painter.drawImage(origin, strech);
            }
            break;
        }
    }

    if( _borderWidth <= 0 )
      return;

	  double corner = _borderRound ? 2.0 : 0;
    size_t border = static_cast<size_t>(_borderWidth);	

	  Gfx::SizeF borderSize(size.width() - _borderWidth/2, size.height() - _borderWidth/2);	
    Gfx::PointF borderPos(_borderWidth/2, _borderWidth/2);
	  Gfx::RectF borderRect(borderPos, borderSize);
						
	  switch( _borderStyle )
	  {
        default:
		    case Single:
		    {			
			      std::vector<Gfx::PointF> points1(5);
			      std::vector<Gfx::PointF> points2(5);

			      //P0
			      points1[0].setX(corner);
			      points1[0].setY(borderRect.height());

			      //P1
			      points1[1].setX(0);
			      points1[1].setY(borderRect.height() - corner);

			      //P2
			      points1[2].setX(0);
			      points1[2].setY(corner);

			      //P3
			      points1[3].setX(corner);
			      points1[3].setY(0);

			      //P4
			      points1[4].setX(borderRect.width() - corner);
			      points1[4].setY(0);
			
			      //---
			      //P0
			      points2[0].setX(borderRect.width() - corner);
			      points2[0].setY(0);

			      //P1
			      points2[1].setX(borderRect.width());
			      points2[1].setY(corner);

			      //P2
			      points2[2].setX(borderRect.width());
			      points2[2].setY(borderRect.height() - corner);

			      //P3
			      points2[3].setX(borderRect.width() - corner);
			      points2[3].setY(borderRect.height());

			      //P4
			      points2[4].setX(corner);
			      points2[4].setY(borderRect.height());
		
			      Gfx::Pen pen(border, _borderColor);
			      painter.setPen(pen);
			
			      painter.drawPolyline(&points1[0], points1.size());								
			      painter.drawPolyline(&points2[0], points2.size());
		    }

		    break;

		    case Border3D:
		    {
			      std::vector<Gfx::PointF> points1(3);
			      points1[0].setX(0);
			      points1[0].setY(borderRect.height());

			      points1[1].setX(0);
			      points1[1].setY(0);
				
			      points1[2].setX(0 + borderRect.width());
			      points1[2].setY(0);

			      Gfx::Pen pen(border, _borderColor);
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

			      Gfx::Pen pen2( border, color );
			      painter.setPen(pen2);
			      painter.drawPolyline(&points2[0], points2.size());			
		    }
		    break;			
	  }	
}

}}
