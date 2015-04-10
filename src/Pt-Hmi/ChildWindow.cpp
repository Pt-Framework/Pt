/* Copyright (C) 2015 Marc Boris Duerner 
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software*/
#include <Pt/Hmi/ChildWindow.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>

namespace Pt{
namespace Hmi{

ChildWindow::ChildWindow()
: PT_HMI_INIT_PROPERTY_VALUE(BorderWidth,5)
, PT_HMI_INIT_PROPERTY_VALUE(BorderColor, Pt::Gfx::ARgbColor(178,178,178))
, PT_HMI_INIT_PROPERTY_VALUE(FocusedColor, Pt::Gfx::ARgbColor(255,108,108))
{
	Widget::BackColor = Pt::Gfx::ARgbColor(0, 255, 255, 255);
	Visible = true;
}


ChildWindow::~ChildWindow()
{
//Todo: remove from window manager
}


void ChildWindow::onRender()
{	
	if(!Visible.get())
		return;

	Widget::onRender();	
   
  const double&			border =  BorderWidth.get();	
	Gfx::SizeF				size = paintSurface().size();	
	Pt::Gfx::RectF		rect(Pt::Gfx::PointF(border/2.0, border/2.0), Pt::Gfx::SizeF( size.width() + 1 - border, size.height() + 1- border) ); 
	Pt::Hmi::Painter& localPainter = paintSurface().painter();
						
	switch(WindowBorder.get())
	{
		case WindowBorder::Sizeable:		
		case WindowBorder::Dialog:
		case WindowBorder::Fixed:
		case WindowBorder::Tool:
		case WindowBorder::ToolSizeable:
		{				

			const Pt::Gfx::ARgbColor& color = Focused.get() ? FocusedColor.get() : BorderColor.get() ;
			Pt::Gfx::Pen pen((size_t) border, color, Pt::Gfx::Pen::SolidStyle, Pt::Gfx::Pen::RoundCap, Pt::Gfx::Pen::MiterJoin);
			localPainter.setPen(pen);
				
			localPainter.drawRect(rect);
		}
		break;
						
		default:
		break;			
	}	
}


void ChildWindow::onInvalidate()
{
	Window::onInvalidate();

	if( _winParent != 0 )
	_winParent->invalidate();
	
}

}}
