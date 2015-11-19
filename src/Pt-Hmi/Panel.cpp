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
#include <Pt/Hmi/Panel.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Rect.h>

namespace Pt{
namespace Hmi{

Panel::Panel()
: _panelBorderStyle(BorderStyle::Single)
, _panelBorderWidth(1)
, _panelBorderRoundEdge(false)
, _borderColor(Gfx::Color::fromRgb8(178,178,178))
{
  setAcceptFocus(false);  
}


Panel::~Panel()
{
}


void Panel::onRender(PaintSurface& paintSurface)
{	
	Widget::onRender(paintSurface);

	int corner = 0;

	if(_panelBorderRoundEdge)
		corner = 2;
    
  if( _panelBorderWidth == 0 )
    return;

  const Gfx::SizeF  size = clientSize();
  const Gfx::PointF pos  = clientPos();

	size_t border =  (size_t) _panelBorderWidth;	

	Gfx::SizeF  clientSize(size.width() - _panelBorderWidth/2, size.height() - _panelBorderWidth/2);	
	Gfx::RectF  clientRect(Gfx::PointF( _panelBorderWidth/2, _panelBorderWidth/2), clientSize);
	
	Pt::Hmi::Painter& painter = paintSurface.painter();
						
	switch( _panelBorderStyle )
	{
		case BorderStyle::Single:
		{			
			std::vector<Gfx::PointF> points1(5);
			std::vector<Gfx::PointF> points2(5);

			//P0
			points1[0].setX(pos.x() + corner);
			points1[0].setY(pos.y() + clientRect.height());

			//P1
			points1[1].setX(pos.x());
			points1[1].setY(pos.y() + clientRect.height() - corner);

			//P2
			points1[2].setX(pos.x());
			points1[2].setY(pos.y() + corner);

			//P3
			points1[3].setX(pos.x() + corner);
			points1[3].setY(pos.y());

			//P4
			points1[4].setX(pos.x() + clientRect.width() - corner);
			points1[4].setY(pos.y() );
			
			//---
			//P0
			points2[0].setX(pos.x() + clientRect.width() - corner);
			points2[0].setY(pos.y());

			//P1
			points2[1].setX(pos.x() + clientRect.width());
			points2[1].setY(pos.y() + corner);

			//P2
			points2[2].setX(pos.x() + clientRect.width());
			points2[2].setY(pos.y() + clientRect.height() - corner);

			//P3
			points2[3].setX(pos.x() + clientRect.width() - corner);
			points2[3].setY(pos.y() + clientRect.height());

			//P4
			points2[4].setX(pos.x() + corner);
			points2[4].setY(pos.y() + clientRect.height());
		
			Gfx::Pen pen(border, _borderColor);
			painter.setPen(pen);
			
			painter.drawPolyline(&points1[0], points1.size());								
			painter.drawPolyline(&points2[0], points2.size());
		}

		break;
			
		case BorderStyle::Widget:
		{			
			std::vector<Gfx::PointF> points1(5);
			std::vector<Gfx::PointF> points2(5);

			//P0
			points1[0].setX(pos.x() + corner);
			points1[0].setY(pos.y() + clientRect.height());

			//P1
			points1[1].setX(pos.x());
			points1[1].setY(pos.y() + clientRect.height() - corner);

			//P2
			points1[2].setX(pos.x());
			points1[2].setY(pos.y() + corner);

			//P3
			points1[3].setX(pos.x() + corner);
			points1[3].setY(pos.y());

			//P4
			points1[4].setX(pos.x() + clientRect.width() - corner);
			points1[4].setY(pos.y());
			
			//---
			//P0
			points2[0].setX(pos.x() + clientRect.width() - corner);
			points2[0].setY(pos.y());

			//P1
			points2[1].setX(pos.x() + clientRect.width());
			points2[1].setY(pos.y() + corner);

			//P2
			points2[2].setX(pos.x() + clientRect.width());
			points2[2].setY(pos.y() + clientRect.height() - corner);

			//P3
			points2[3].setX(pos.x() + clientRect.width() - corner);
			points2[3].setY(pos.y() + clientRect.height());

			//P4
			points2[4].setX(pos.x() + corner);
			points2[4].setY(pos.y() + clientRect.height());


			Gfx::Pen pen(border, _borderColor);
			painter.setPen(pen);
				
			painter.drawPolyline(&points2[0], points2.size());
                
			Gfx::Pen pen2(border,  _borderColor);
			painter.setPen(pen2);
                
			painter.drawPolyline(&points1[0], points1.size());
            
		}
		break;

		case BorderStyle::Border3D:
		{
			std::vector<Gfx::PointF> points1(3);
			std::vector<Gfx::PointF> points2(3);

			points1[0].setX(pos.x());
			points1[0].setY(pos.y() +clientRect.height());

			points1[1].setX(pos.x());
			points1[1].setY(pos.y());
				
			points1[2].setX(pos.x() + clientRect.width());
			points1[2].setY(pos.y());


			points2[0].setX(pos.x() + clientRect.width());
			points2[0].setY(pos.y());

			points2[1].setX(pos.x() + clientRect.width());
			points2[1].setY(pos.y() + clientRect.height());

			points2[2].setX(pos.x());
			points2[2].setY(pos.y() + clientRect.height());

		
			Gfx::Pen pen(border,Gfx::Color(255/255.0,255/255.0,255/255.0));
			painter.setPen(pen);
				
			painter.drawPolyline(&points1[0], points1.size());
								
			Gfx::Pen pen2(border, _borderColor );
			painter.setPen(pen2);

			painter.drawPolyline(&points2[0], points2.size());			
		}
		break;	
		default:
		break;			
	}	
}

}}
