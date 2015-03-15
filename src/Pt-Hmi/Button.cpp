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

#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/ButtonView.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

class PointingDevice;
class GfxOutput;

Button::Button(ButtonModel& model, ButtonView& view)
: Label(model, view)
, _pressed(false)
, _timeout(false)
, _pressCounter(0)
{
	_doublePressTimer.timeout() += Pt::slot(*this, &Button::onDoublePressedTimeout);
	_doublePressTimer.setActive(Pt::Hmi::Application::instance().loop());
	buttonModel().ButtonState.Changed += Pt::slot(*this, &Button::onButtonStateChanged);
	bindMnemonicToWidget(this);
}

Button::~Button()
{
}

void Button::onDoublePressedTimeout()
{
	_timeout = true;
	_pressCounter = 0;
	_doublePressTimer.stop();
}

void Button::onCheckedAction(bool state)
{
	CheckedAction.send(this, state);
}

void Button::onPressedAction()
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

void Button::onMnemonic()
{
	if(!buttonModel().Enabled.get())
	{
		Label::onMnemonic();
		return;
	}

	if(!buttonModel().Visible.get())
	{
		Label::onMnemonic();
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
	
	Label::onMnemonic();		
	invalidate();	
}

void Button::onDoublePressedAction()
{
	DoublePressedAction.send(this);
	_doublePressTimer.stop();
}

void Button::onModelChanged(bool created, const Model& model)
{
	Label::onModelChanged(created, model);

	if( created)
		buttonModel().ButtonState.Changed += Pt::slot(*this, &Button::onButtonStateChanged);
}

void Button::onButtonStateChanged( const Property<DeviceButton::State>& prop )
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

void Button::onKeyInput(const KeyEvent& ev)
{	
	
	if(!buttonModel().Enabled.get())
	{
		Label::onKeyInput(ev);
		return;
	}

	if(!buttonModel().Visible.get())
	{
		Label::onKeyInput(ev);
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
	
	Label::onKeyInput(ev);
	
	if(genOutput)
		invalidate();
}

void Button::onPointerInput(const PointingEvent& ev)
{	
	
	Pt::Gfx::PointF point = toClient(Pt::Gfx::PointF(ev.x(), ev.y()));

	if(!buttonModel().Enabled.get())
	{
		Label::onPointerInput(ev);
		return;
	}

	if(!buttonModel().Visible.get())
	{
		Label::onPointerInput(ev);
		return;
	}

	if(!buttonModel().contains(point))
	{
		buttonModel().Armed = false;
		Label::onPointerInput(ev);
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
		Label::onPointerInput(ev);
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

	Label::onPointerInput(ev);
	
	if(genOutput)
		invalidate();
}

}}
