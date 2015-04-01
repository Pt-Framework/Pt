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
: PT_HMI_INIT_PROPERTY_VALUE(MinimumSize,Pt::Gfx::SizeF(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(MaximumSize,Pt::Gfx::SizeF(std::numeric_limits<Pt::uint16_t>::max() ,std::numeric_limits<Pt::uint16_t>::max()))
, PT_HMI_INIT_PROPERTY_VALUE(StartPostion, WindowStartPosition::Manual)
, PT_HMI_INIT_PROPERTY_VALUE(State, WindowState::Normal)
, PT_HMI_INIT_PROPERTY_VALUE(ShowTitle,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMinimizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMaximizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowSysMenu,true)	
, PT_HMI_INIT_PROPERTY_VALUE(Caption,"")
, PT_HMI_INIT_PROPERTY_VALUE(WindowBorder,WindowBorder::Sizeable)
, PT_HMI_INIT_PROPERTY_VALUE(Icon, Pt::Gfx::ARgbImage(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(Closed,false)
, PT_HMI_INIT_PROPERTY_VALUE(CanClose,true)
, PT_HMI_INIT_PROPERTY_VALUE(FocuseMoveKey, "\t")
, PT_HMI_INIT_PROPERTY_VALUE(FirstShow,true)  
, PT_HMI_INIT_PROPERTY_VALUE(BorderWidth,5)
, PT_HMI_INIT_PROPERTY_VALUE(BorderColor, Pt::Gfx::ARgbColor(178,178,178))
, PT_HMI_INIT_PROPERTY_VALUE(FocusedColor, Pt::Gfx::ARgbColor(108,108,108))
{
	Widget::BackColor = Pt::Gfx::ARgbColor(0, 255, 255, 255);
}


ChildWindow::~ChildWindow()
{
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
	render();		
}

void ChildWindow::handleKeyInput(const KeyEvent& ev)
{
	onKeyInput(ev);
}


void ChildWindow::handlePointerInput( const Pt::Hmi::PointingEvent& mouseEvent )
{
	onPointerInput(mouseEvent);
}


}}
