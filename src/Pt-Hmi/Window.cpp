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
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/Application.h>
#include "WindowImpl.h"
#include <Pt/Hmi/PositionEvent.h>
#include <Pt/Gfx/Size.h>

namespace Pt{
namespace Hmi{

Window::Window(WindowModel* model)
: Widget(model)
, _windowParent(0)
, _windowModel(model)
, _impl(new WindowImpl(model, &paintSurface() ) )
{						 
		
	_windowModel->Size.Changed += Pt::slot(*this, &Window::onSizeChanged);					
	_windowModel->Size.Changed.send(_windowModel->Size);	
	_windowModel->Closed.Changed += Pt::slot(*this, &Window::onClosed);

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
		WidgetModel* model = child->widgetModel();
		
		if(!model->AcceptFocus.get())
			continue;

		model->Focused = true;
		return true;
	}

	return false;
}


void Window::onKeyInput(const KeyEvent& ev)
{ 
	if(!_windowModel->Enabled.get())
		return;

	_windowModel->KeyStatus = ev;

	if(ev.toUTF8String() == _windowModel->FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && !ev.shift())
	{
		if( _windowModel->Focused.get())
		{
			if(!moveFocusNext())
				_windowModel->Focused = true;

			render();
		}
	}

	if(ev.toUTF8String() ==  _windowModel->FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && ev.shift())
	{
		if( _windowModel->Focused.get())
		{
			if(!moveFocusPrev())
				_windowModel->Focused = true;

			render();
		}
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->keyInput(ev);
}

void Window::onPointerInput(const PointingEvent& ev)
{
	if(!_windowModel->Enabled.get())
		return;
	
	_windowModel->Pointer2DStatus = ev;
	_windowModel->CursorT.get().setCursor(Pt::Hmi::Cursors::Default);

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
	WidgetModel* childModel = child->widgetModel();	
		
	if(!childModel->AcceptFocus.get())
	{
		child->widgetModel()->Focused = true;
		return true;
	}

	_windowModel->Focused = false;

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
	paintSurface().resize(_windowModel->Size.get());	
	
	if(_windowModel->Visible.get())
		render();
}


void Window::onClosed(const Property<bool> & closed)
{	
	close();
}

bool Window::close()
{
	if(!_windowModel->CanClose.get())
		return false;
    
  //Set the closed flag
	if(!_windowModel->Closed.get())
	{
		_windowModel->Closed.set(true);
        
		//Let the system window to close its self.
		render();
	}

	return true;
}


void Window::resizeEvent(const ResizeEvent& ev)
{
	const Pt::Gfx::SizeF& curSize = _windowModel->Size.get();

	if( curSize == ev.size() ) 
		return;

	_windowModel->Size = ev.size();
}


void Window::positionEvent(const PositionEvent& ev)
{
	const Pt::Gfx::PointF& curPos = _windowModel->Position.get();

	if( curPos == ev.position() ) 
		return;

	_windowModel->Position = ev.position();
}


void Window::onInvalidate()
{
	render();
	_impl->render();
}

}}
