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
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Pen.h>
#include <Pt/Ui/Rect.h>

namespace Pt{
namespace Hmi{

Panel::Panel()
: PT_HMI_INIT_PROPERTY_VALUE(PanelBorderStyle,BorderStyle::Single)
, PT_HMI_INIT_PROPERTY_VALUE(PanelBorderWidth,1)
, PT_HMI_INIT_PROPERTY_VALUE(PanelBorderRoundEdge,false)
, PT_HMI_INIT_PROPERTY_VALUE(BorderColor, Ui::Color::fromRgb8(178,178,178))
{
  AcceptFocus = false;
  Name.set("Panel");
  invalidate();
}


Panel::~Panel()
{
}


void Panel::onRender(PaintSurface& paintSurface)
{	
	Widget::onRender(paintSurface);

	int corner = 0;

	if(PanelBorderRoundEdge.get())
		corner = 2;
    
  if( PanelBorderWidth.get() == 0 )
    return;

  const Ui::SizeF size = paintSurface.originSize();
  const Ui::PointF pos = Ui::PointF(0,0);

	size_t border =  (size_t) PanelBorderWidth.get();	

	Ui::SizeF  clientSize(size.width() - PanelBorderWidth.get()/2, size.height() - PanelBorderWidth.get()/2);	
	Ui::RectF  clientRect(Ui::PointF( PanelBorderWidth.get()/2, PanelBorderWidth.get()/2), clientSize);
	
	Pt::Hmi::Painter& localPainter = paintSurface.painter();
						
	switch( PanelBorderStyle.get() )
	{
		case BorderStyle::Single:
		{			
			std::vector<Ui::PointF> points1(5);
			std::vector<Ui::PointF> points2(5);

			//P0
			points1[0].setX(corner);
			points1[0].setY(clientRect.height());

			//P1
			points1[1].setX(0);
			points1[1].setY(clientRect.height() - corner);

			//P2
			points1[2].setX(0);
			points1[2].setY(corner);

			//P3
			points1[3].setX(corner);
			points1[3].setY(0);

			//P4
			points1[4].setX(clientRect.width() - corner);
			points1[4].setY(0);
			
			//---
			//P0
			points2[0].setX(clientRect.width() - corner);
			points2[0].setY(0);

			//P1
			points2[1].setX(clientRect.width());
			points2[1].setY(corner);

			//P2
			points2[2].setX(clientRect.width());
			points2[2].setY(clientRect.height() - corner);

			//P3
			points2[3].setX(clientRect.width() - corner);
			points2[3].setY(clientRect.height());

			//P4
			points2[4].setX(corner);
			points2[4].setY(clientRect.height());
		
			Ui::Pen pen(border, BorderColor.get());
			localPainter.setPen(pen);
				
			localPainter.drawPolyline(&points1[0], points1.size());								
			localPainter.drawPolyline(&points2[0], points2.size());
		}

		break;
			
		case BorderStyle::Widget:
		{			
			std::vector<Ui::PointF> points1(5);
			std::vector<Ui::PointF> points2(5);

			//P0
			points1[0].setX(corner);
			points1[0].setY(clientRect.height());

			//P1
			points1[1].setX(0);
			points1[1].setY(clientRect.height() - corner);

			//P2
			points1[2].setX(0);
			points1[2].setY(corner);

			//P3
			points1[3].setX(corner);
			points1[3].setY( 0);

			//P4
			points1[4].setX(clientRect.width() - corner);
			points1[4].setY( 0);
			
			//---
			//P0
			points2[0].setX(clientRect.width() - corner);
			points2[0].setY( 0);

			//P1
			points2[1].setX(clientRect.width());
			points2[1].setY(corner);

			//P2
			points2[2].setX(clientRect.width());
			points2[2].setY(clientRect.height() - corner);

			//P3
			points2[3].setX(clientRect.width() - corner);
			points2[3].setY(clientRect.height());

			//P4
			points2[4].setX(corner);
			points2[4].setY(clientRect.height());


			Ui::Pen pen(border, BorderColor.get() );
			localPainter.setPen(pen);
				
			localPainter.drawPolyline(&points2[0], points2.size());
                
			Ui::Pen pen2(border,  BorderColor.get());
			localPainter.setPen(pen2);
                
			localPainter.drawPolyline(&points1[0], points1.size());
            
		}
		break;

		case BorderStyle::Border3D:
		{
			std::vector<Ui::PointF> points1(3);
			std::vector<Ui::PointF> points2(3);

			points1[0].setX(0);
			points1[0].setY(clientRect.height());

			points1[1].setX(0);
			points1[1].setY(0);
				
			points1[2].setX(clientRect.width());
			points1[2].setY(0);


			points2[0].setX(clientRect.width());
			points2[0].setY(0);

			points2[1].setX(clientRect.width());
			points2[1].setY(clientRect.height());

			points2[2].setX(0);
			points2[2].setY(clientRect.height());

		
			Ui::Pen pen(border, Ui::Color(255/255.0,255/255.0,255/255.0));
			localPainter.setPen(pen);
				
			localPainter.drawPolyline(&points1[0], points1.size());
								
			Ui::Pen pen2(border, DisabledColor.get() );
			localPainter.setPen(pen2);

			localPainter.drawPolyline(&points2[0], points2.size());			
		}
		break;	
		default:
		break;			
	}	
}

}}
