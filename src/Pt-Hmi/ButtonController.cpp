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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/Hmi/ButtonController.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/ButtonRenderer.h>
#include <Pt/Hmi/PointingDevice.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

class PointingDevice;
class GfxOutput;

ButtonController::ButtonController(ButtonModel& model, ButtonRenderer& renderer)
: LabelController(model, renderer)
, _pressed(false)
, _timeout(false)
, _pressCounter(0)
{
	_doublePressTimer.timeout() += Pt::slot(*this, &ButtonController::onDoublePressedTimeout);
	_doublePressTimer.setActive(Pt::Hmi::Application::instance().loop());
	buttonModel().ButtonState.PropertyChanged += Pt::slot(*this,&ButtonController::onButtonStateChanged);
	bindMnemonicToWidget(this);
}

ButtonController::~ButtonController()
{
}

void ButtonController::onDoublePressedTimeout()
{
	_timeout = true;
	_pressCounter = 0;
	_doublePressTimer.stop();
}

void ButtonController::onCheckedAction(bool state)
{
	CheckedAction.send(this, state);
}

void ButtonController::onPressedAction()
{
	PressedAction.send(this);
	_pressCounter++;
	
	if(_pressCounter == 1)
		_doublePressTimer.start(buttonModel().DoublePressTimeInMs.get());

	if( _pressCounter == 2)	
	{
		_pressCounter = 0;
		onDoublePressedAction();
	}
}

void ButtonController::onMnemonic()
{
	if(!buttonModel().Enable.get())
	{
		LabelController::onMnemonic();
		return;
	}

	if(!buttonModel().Visible.get())
	{
		LabelController::onMnemonic();
		return;
	}

	switch(buttonModel().ButtonType.get())
	{
		case ButtonType::Press:
		{
			onPressedAction();
		}
		break;

		case ButtonType::Toggle:
		{
			if(buttonModel().ButtonState.get() == DeviceButton::Pressed)
				buttonModel().ButtonState = DeviceButton::Released;
			else
				buttonModel().ButtonState = DeviceButton::Pressed;
		}			
		break;
	}
	
	LabelController::onMnemonic();		
	invalidate();	
}

void ButtonController::onDoublePressedAction()
{
	DoublePressedAction.send(this);
	_doublePressTimer.stop();
}

void ButtonController::onModelChanged(bool created, const PropertyBase* prop)
{
	LabelController::onModelChanged(created, prop);

	if( created)
		buttonModel().ButtonState.PropertyChanged += Pt::slot(*this, &ButtonController::onButtonStateChanged);
}

void ButtonController::onButtonStateChanged(const void* sender, const PropertyBase& prop)
{
	switch( buttonModel().ButtonType.get())
	{
		case Pt::Hmi::ButtonType::Press:
		
			switch( buttonModel().ButtonState.get())
			{
				case Pt::Hmi::DeviceButton::Pressed:
					_pressed = true;
				break;

				case Pt::Hmi::DeviceButton::Released:				
					if(_pressed)
					{
						_pressed = false;
						onPressedAction();									
					}
				break;
			}
		break;

		case Pt::Hmi::ButtonType::Toggle:
			onCheckedAction(buttonModel().ButtonState.get() == Pt::Hmi::DeviceButton::Pressed);
		break;
	}
}

void ButtonController::onKeyInput(const KeyEvent& ev)
{	
	
	if(!buttonModel().Enable.get())
	{
		LabelController::onKeyInput(ev);
		return;
	}

	if(!buttonModel().Visible.get())
	{
		LabelController::onKeyInput(ev);
		return;
	}
	bool genOutput = false;

	switch(buttonModel().ButtonType.get())
	{
		case ButtonType::Press:
		{
			if(ev.toUTF8String() == buttonModel().FocusedActionKey.get() && buttonModel().Focused.get())	
			{
				buttonModel().ButtonState = (ev.state() == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;
				genOutput = true;
			}
			else if(ev.shortCutKey() == buttonModel().ActionKey.get())
			{
				buttonModel().ButtonState = (ev.state() == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;				
				buttonModel().Focused = false;
				buttonModel().Focused = true;
				genOutput = true;
			}
			else
			{
				if(buttonModel().ButtonState.get() != DeviceButton::Released)
				{
					buttonModel().ButtonState = DeviceButton::Released;
					genOutput = true;
				}
			}
		}
		break;

		case ButtonType::Toggle:
		{
			if(ev.toUTF8String() == buttonModel().FocusedActionKey.get() && buttonModel().Focused.get())		
			{
				if((ev.state() == KeyEvent::KeyDown))
				{
					if(buttonModel().ButtonState.get() == DeviceButton::Pressed)
						buttonModel().ButtonState = DeviceButton::Released;
					else
						buttonModel().ButtonState = DeviceButton::Pressed;

					genOutput = true;
				}
			}
			else if(ev.shortCutKey() == buttonModel().ActionKey.get())
			{			
				if((ev.state() == KeyEvent::KeyDown))
				{
					buttonModel().ButtonState = (buttonModel().ButtonState.get() == DeviceButton::Pressed) ? DeviceButton::Released : DeviceButton::Pressed;											
					buttonModel().Focused = false;
					buttonModel().Focused = true;
					genOutput = true;
				}
			}
		}			
		break;
	}
	
	LabelController::onKeyInput(ev);
	
	if(genOutput)
		invalidate();
}

void ButtonController::onPointerInput(const PointingEvent& ev)
{	
	
	Pt::Gfx::PointF point = toClient(Pt::Gfx::PointF(ev.x(), ev.y()));

	if(!buttonModel().Enable.get())
	{
		LabelController::onPointerInput(ev);
		return;
	}

	if(!buttonModel().Visible.get())
	{
		LabelController::onPointerInput(ev);
		return;
	}

	if(!buttonModel().contains(point))
	{
		buttonModel().Armed = false;
		LabelController::onPointerInput(ev);
		return;
	}

	bool genOutput = false;

	if(!buttonModel().Armed.get())
	{
		buttonModel().Armed = true;
		genOutput = true;
	}

	if( ev.buttons().size() == 0)
	{
		LabelController::onPointerInput(ev);
		return;
	}	

	switch(buttonModel().ButtonType.get())
	{
		case ButtonType::Press:
		{
			if(!buttonModel().Focused.get())
			{
				genOutput = true;						
				buttonModel().Focused = false;
				buttonModel().Focused = true;					
			}

			switch(ev.buttons()[0].state())
			{
				case DeviceButton::Pressed:
				{		

					if(buttonModel().ButtonState.get() != DeviceButton::Pressed)
					{
						genOutput = true;						
						buttonModel().ButtonState = DeviceButton::Pressed;
					}
				}
				break;
			
				case DeviceButton::Released:			
				{

					if(buttonModel().ButtonState.get() != DeviceButton::Released)
					{
						buttonModel().ButtonState = DeviceButton::Released;						
						genOutput = true;
					}
				}
				break;
			}
		}
		break;

		case ButtonType::Toggle:
		{
			if(!buttonModel().Focused.get())	
			{
				buttonModel().Focused = false;
				buttonModel().Focused = true;
				genOutput = true;
			}

			if(ev.buttons()[0].state() == DeviceButton::Pressed )
			{
				genOutput = true;
				if(buttonModel().ButtonState.get() == DeviceButton::Pressed)
					buttonModel().ButtonState = DeviceButton::Released;
				else
					buttonModel().ButtonState = DeviceButton::Pressed;
			}
		}				
		break;
	}

	LabelController::onPointerInput(ev);
	
	if(genOutput)
		invalidate();
}

}}
