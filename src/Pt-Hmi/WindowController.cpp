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
#include <Pt/Hmi/PointingDevice.h>
#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/GfxOutput.h>
#include <Pt/Hmi/WindowModel.h>

#include <iostream>

namespace Pt{
namespace Hmi{

WindowController::WindowController(GfxModel* m, Renderer* r,  GfxOutput* out, PointingDevice* in1, InputDevice* in2)
:_windowParent(0)
{	
	if( m != 0)
		Controller::setModel(m);

	if( r != 0)
		Controller::setRenderer(r);

	if( out != 0)
		Controller::addOutputDevice(out);

	if( in1 != 0)
		Controller::addInputDevice(in1);

	if( in2 != 0)
		Controller::addInputDevice(in2);

	ClosedAction += Pt::slot(*this, &WindowController::onClosed);
	ClosingAction += Pt::slot(*this, &WindowController::onClosing);
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
	WindowModel* m = (WindowModel*)gfxModel();

	if(!m->Enable.get())
		return;

	m->KeyStatus = ev;

	if(ev.toUTF8String() == m->FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && !ev.shift())
	{
		if( m->Focused.get())
		{
			if(!moveFocusNext())
				m->Focused = true;

			invalidate();
		}
	}

	if(ev.toUTF8String() ==  m->FocuseMoveKey.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && ev.shift())
	{
		if( m->Focused.get())
		{
			if(!moveFocusPrev())
				m->Focused = true;

			invalidate();
		}
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyKeyInput(ev);
}

void WindowController::onPointerInput(const PointingEvent& ev)
{
	GfxModel* m = gfxModel();

	if(!m->Enable.get())
		return;
	
	m->Pointer2DStatus = ev;
	m->CursorT.get().setCursor(Pt::Hmi::Cursors::Default, this);

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyPointerInput(ev);	
}

void WindowController::onSizeChanged(const void* sender, const PropertyBase& prop)
{
	GfxModel* m = gfxModel();
	
	Pt::Gfx::Size size = m->fromUnit(m->Size.get());

	m->PaintBuffer.resize(size.width(), size.height());	
	
	if(m->Visible.get())
		invalidate();
}

GfxModel* WindowController::gfxModel()
{
	GfxModel* m = dynamic_cast<GfxModel*>(model());

	if( m == 0)
		throw std::logic_error("ERROR: WindowController expect a GFXModel!");

	return m;
}

void WindowController::onModelChanged(bool created)
{
	if( created)
	{
		GfxModel* m = gfxModel();

		m->Size.PropertyChanged += Pt::slot(*this, &WindowController::onSizeChanged);					
		m->Size.PropertyChanged.send(m, m->Size);	
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyModelChanged(created);	
		
}

void WindowController::onClosing(Controller* sender, bool& canClose)
{
	WindowModel* m = dynamic_cast<WindowModel*>(model());

	if( m == 0)
		return;
	
	if(!m->Enable.get())
		return;

	canClose = m->CanClose.get();
}

void WindowController::onClosed(Controller* sender)
{
	WindowModel* m = dynamic_cast<WindowModel*>(model());

	if( m == 0)
		return;

    if(!m->Closed.get())
        m->Closed = true;
}

void WindowController::close()
{
	WindowModel* m = dynamic_cast<WindowModel*>(model());
	
	if( m == 0)
		return;
    
    //Can close??
    bool canClose = false;
    ClosingAction.send(this, canClose);
    
    if(canClose)
    {
        //Set the closed flag
        m->Closed = true;
        
        //Let the system window to close its self.
        invalidate();
    }
}
}}
