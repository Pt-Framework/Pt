/* Copyright (C) 2013 Marc Boris Duerner 
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/


#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include "WindowImpl.h"
#include <Pt/Hmi/PositionEvent.h>
#include <Pt/Gfx/Size.h>

namespace Pt{
namespace Hmi{

Window::Window()
: _impl(new WindowImpl(this, &paintSurface() ) )
, PT_HMI_INIT_PROPERTY_VALUE(MinimumSize,Pt::Gfx::SizeF(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(MaximumSize,Pt::Gfx::SizeF(65535,65535))
, PT_HMI_INIT_PROPERTY_VALUE(WindowStartPostion, WindowStartPositionType::Manual)
, PT_HMI_INIT_PROPERTY_VALUE(WindowState, WindowStateType::Normal)
, PT_HMI_INIT_PROPERTY_VALUE(ShowInTaskbar,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowTitle,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMinimizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMaximizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowSysMenu,true)	
, PT_HMI_INIT_PROPERTY_VALUE(Caption,"")
, PT_HMI_INIT_PROPERTY_VALUE(Border,WindowBorderType::Sizeable)
, PT_HMI_INIT_PROPERTY_VALUE(Icon, Pt::Gfx::ARgbImage(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(Closed,false)
, PT_HMI_INIT_PROPERTY_VALUE(CanClose,true)
, PT_HMI_INIT_PROPERTY_VALUE(TopMost, false)
, PT_HMI_INIT_PROPERTY_VALUE(FocuseMoveKey, "\t")
{
	Visible.set(false);
	Position.set(Pt::Gfx::PointF(20,20));
	Size.set(Pt::Gfx::SizeF(200,200));
	Focused.set(true);
		
	Size.Changed += Pt::slot(*this, &Window::onSizeChanged);					
	Size.Changed.send(Size);	
	Closed.Changed += Pt::slot(*this, &Window::onClosed);

	_impl->windowEvent() += Pt::slot(*this, &Widget::pointerInput);
	_impl->windowEvent() += Pt::slot(*this, &Widget::keyInput);	
	_impl->windowEvent() += Pt::slot(*this, &Window::resizeEvent);
	_impl->windowEvent() += Pt::slot(*this, &Window::positionEvent);	
}

Window::~Window()
{
}

bool Window::focusNextChild(int index)
{
	index++;
	
	for( ; index < (int)children().size(); ++index)
	{
		Widget* child = children()[index];
		
		if(!child->AcceptFocus.get())
			continue;

		child->Focused = true;
		return true;
	}

	return false;
}


void Window::onKeyInput(const KeyEvent& ev)
{ 
	if(!Enabled.get())
		return;

	KeyStatus = ev;

	if(ev.toUTF8String() == FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && !ev.shift())
	{
		if( Focused.get())
		{
			if(!moveFocusNext())
				Focused = true;

			render();
		}
	}

	if(ev.toUTF8String() ==  FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && ev.shift())
	{
		if( Focused.get())
		{
			if(!moveFocusPrev())
				Focused = true;

			render();
		}
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->keyInput(ev);
}

void Window::onPointerInput(const PointingEvent& ev)
{
	if(!Enabled.get())
		return;
	
	Pointer2DStatus = ev;
	CursorT.get().setCursor(Pt::Hmi::Cursors::Default);

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->pointerInput(ev);	
}


bool Window::moveFocusNext()
{
	if(children().size() == 0)
		return false;

	const int index = getFocusedChild();

	if( index == -1)
		return focusNextChild(index);
	
	Widget* child = children()[index];
		
	if(!child->AcceptFocus.get())
	{
		child->Focused = true;
		return true;
	}

	Focused = false;

	return focusNextChild(index);
}

Widget* Window::mainWidget()
{
	if(children().size() != 0)
		return children()[0];

	return 0;
}

const Widget* Window::mainWidget() const
{	
	if(children().size() != 0)
		return children()[0];

	return 0;
}


void Window::onSizeChanged(const Property<Pt::Gfx::SizeF>& prop)
{
	paintSurface().resize(Size.get());	
	
	if(Visible.get())
		render();
}


void Window::onClosed(const Property<bool> & closed)
{	
	close();
}

bool Window::close()
{
	if(!CanClose.get())
		return false;
    
  //Set the closed flag
	if(!Closed.get())
	{
		Closed.set(true);
        
		//Let the system window to close its self.
		render();
	}

	return true;
}


void Window::resizeEvent(const ResizeEvent& ev)
{
	const Pt::Gfx::SizeF& curSize = Size.get();

	if( curSize == ev.size() ) 
		return;

	Size = ev.size();
}


void Window::positionEvent(const PositionEvent& ev)
{
	const Pt::Gfx::PointF& curPos = Position.get();

	if( curPos == ev.position() ) 
		return;

	Position = ev.position();
}


void Window::onInvalidate()
{
	render();
	_impl->render();
}

}}
