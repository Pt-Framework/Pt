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
#include <Pt/Hmi/WindowView.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/WidgetView.h>
#include <iostream>

namespace Pt{
namespace Hmi{

Window::Window(WindowModel& m, WindowView& view)
: Widget(m, view)
, _windowParent(0)
{	
	view.setController(*this);
	Controller::addOutput(&view);

	ClosedAction += Pt::slot(*this, &Window::onClosed);
	ClosingAction += Pt::slot(*this, &Window::onClosing);
	Pt::Hmi::Application& app = Pt::Hmi::Application::instance();

	app.systemEvent() += Pt::slot(*this, &Controller::devicePointerInput);
	app.systemEvent() += Pt::slot(*this, &Controller::deviceKeyInput);

	windowModel().Size.Changed += Pt::slot(*this, &Window::onSizeChanged);		
}

Window::~Window()
{
}

Widget* Window::mainWidget()
{
	if( Controller::children().size() != 0)
		return dynamic_cast<Widget*>( Controller::children()[0]);

	return 0;
}

const Widget* Window::mainWidget() const
{
	if( Controller::children().size() != 0)
		return dynamic_cast<Widget*>( Controller::children()[0]);

	return 0;
}


void Window::onKeyInput(const KeyEvent& ev)
{ 
	WindowModel& m = windowModel();

	if(!m.Enabled.get())
		return;

	m.KeyStatus = ev;

	if(ev.toUTF8String() == m.FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && !ev.shift())
	{
		if( m.Focused.get())
		{
			if(!moveFocusNext())
				m.Focused = true;

			output();
		}
	}

	if(ev.toUTF8String() ==  m.FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && ev.shift())
	{
		if( m.Focused.get())
		{
			if(!moveFocusPrev())
				m.Focused = true;

			output();
		}
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyKeyInput(ev);
}

void Window::onPointerInput(const PointingEvent& ev)
{
	WindowModel& m = windowModel();

	if(!m.Enabled.get())
		return;
	
	m.Pointer2DStatus = ev;
	m.CursorT.get().setCursor(Pt::Hmi::Cursors::Default);

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyPointerInput(ev);	
}

void Window::onSizeChanged(const Property<Pt::Gfx::SizeF>& prop)
{
	WidgetModel& m = windowModel();
	WidgetView& view = widgetView();
	
	view.paintSurface().resize(m.Size.get());	
	
	if(m.Visible.get())
		output();
}

void Window::onModelChanged(bool created,const Model& model)
{
	if( created)
	{
		WidgetModel& m = windowModel();

		m.Size.Changed += Pt::slot(*this, &Window::onSizeChanged);					
		m.Size.Changed.send(m.Size);	
	}
}

void Window::onClosing(Controller* sender, bool& canClose)
{	
	if(!windowModel().Enabled.get())
		return;

	canClose = windowModel().CanClose.get();
}

void Window::onClosed(Controller* sender)
{
    if(!windowModel().Closed.get())
        windowModel().Closed = true;
}

void Window::close()
{
    //Can close??
    bool canClose = false;
    ClosingAction.send(this, canClose);
    
    if(canClose)
    {
      //Set the closed flag
      windowModel().Closed = true;
        
      //Let the system window to close its self.
			output();
    }
}

}}
