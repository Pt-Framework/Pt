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
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/View.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowRenderer.h>
#include <Pt/Hmi/Application.h>

#include <iostream>

namespace Pt{
namespace Hmi{

WindowController::WindowController(WindowModel& m, WindowRenderer& r,  View* out)
: WidgetController(m, r)
, _windowParent(0)
{	
	if( out != 0)
		Controller::addOutputDevice(out);

	ClosedAction += Pt::slot(*this, &WindowController::onClosed);
	ClosingAction += Pt::slot(*this, &WindowController::onClosing);
	Pt::Hmi::Application& app = Pt::Hmi::Application::instance();

	app.systemEvent() += Pt::slot(*this, &Controller::devicePointerInput);
	app.systemEvent() += Pt::slot(*this, &Controller::deviceKeyInput);

	windowModel().Size.Changed += Pt::slot(*this, &WindowController::onSizeChanged);
}

WindowController::~WindowController()
{
}

WidgetController* WindowController::mainWidget()
{
	if( Controller::children().size() != 0)
		return dynamic_cast<WidgetController*>( Controller::children()[0]);

	return 0;
}

const WidgetController* WindowController::mainWidget() const
{
	if( Controller::children().size() != 0)
		return dynamic_cast<WidgetController*>( Controller::children()[0]);

	return 0;
}

void WindowController::invalidate()
{
	render();
	output();
}

void WindowController::onKeyInput(const KeyEvent& ev)
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

			invalidate();
		}
	}

	if(ev.toUTF8String() ==  m.FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && ev.shift())
	{
		if( m.Focused.get())
		{
			if(!moveFocusPrev())
				m.Focused = true;

			invalidate();
		}
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyKeyInput(ev);
}

void WindowController::onPointerInput(const PointingEvent& ev)
{
	WindowModel& m = windowModel();

	if(!m.Enabled.get())
		return;
	
	m.Pointer2DStatus = ev;
	m.CursorT.get().setCursor(Pt::Hmi::Cursors::Default);

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyPointerInput(ev);	
}

void WindowController::onSizeChanged(const Property<Pt::Gfx::SizeF>& prop)
{
	WidgetModel& m = windowModel();
	
	m.paintSurface()->resize(m.Size.get());	
	
	if(m.Visible.get())
		invalidate();
}

void WindowController::onModelChanged(bool created,const Model& model)
{
	if( created)
	{
		WidgetModel& m = windowModel();

		m.Size.Changed += Pt::slot(*this, &WindowController::onSizeChanged);					
		m.Size.Changed.send(m.Size);	
	}
}

void WindowController::onClosing(Controller* sender, bool& canClose)
{	
	if(!windowModel().Enabled.get())
		return;

	canClose = windowModel().CanClose.get();
}

void WindowController::onClosed(Controller* sender)
{
    if(!windowModel().Closed.get())
        windowModel().Closed = true;
}

void WindowController::close()
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
