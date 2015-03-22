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
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Gfx/Point.h>

namespace Pt{
namespace Hmi{

Button::Button(ButtonModel* model)
: Label(model)
, _pressed(false)
, _timeout(false)
, _pressCounter(0)
, _buttonModel(model)
{
	_buttonModel->BorderStyle.set(Hmi::BorderStyleType::Single);
	_doublePressTimer.timeout() += Pt::slot(*this, &Button::onDoublePressedTimeout);
	_doublePressTimer.setActive(Pt::Hmi::Application::instance().loop());
	_buttonModel->ButtonState.Changed += Pt::slot(*this, &Button::onButtonStateChanged);
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

void Button::onPressedAction()
{
	_buttonModel->Clicked.send();
	_pressCounter++;
	
	if(_pressCounter == 1)
		_doublePressTimer.start(_buttonModel->DoublePressTimeInMs.get());

	if( _pressCounter == 2)	
	{
		_pressCounter = 0;
		_buttonModel->DoubleClicked.send();
	}
}

void Button::onMnemonic()
{
	if(!_buttonModel->Enabled.get())
	{
		Label::onMnemonic();
		return;
	}

	if(!_buttonModel->Visible.get())
	{
		Label::onMnemonic();
		return;
	}

	switch(_buttonModel->ButtonType.get())
	{
		case ButtonType::Press:
		{
			onPressedAction();
		}
		break;

		case ButtonType::Toggle:
		{
			if(_buttonModel->ButtonState.get() == DeviceButton::Pressed)
				_buttonModel->ButtonState = DeviceButton::Released;
			else
				_buttonModel->ButtonState = DeviceButton::Pressed;
		}			
		break;
	}
	
	Label::onMnemonic();		
	invalidate();	
}

void Button::onDoublePressedAction()
{
	_buttonModel->DoubleClicked.send();
	_doublePressTimer.stop();
}

void Button::onButtonStateChanged( const Property<DeviceButton::State>& prop )
{
	switch( _buttonModel->ButtonType.get())
	{
		case Pt::Hmi::ButtonType::Press:
		
			switch( _buttonModel->ButtonState.get())
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
			_buttonModel->Checked.send((_buttonModel->ButtonState.get() == Pt::Hmi::DeviceButton::Pressed));
		break;
	}
}

void Button::onKeyInput(const KeyEvent& ev)
{		
	if(!_buttonModel->Enabled.get())
	{
		Label::onKeyInput(ev);
		return;
	}

	if(!_buttonModel->Visible.get())
	{
		Label::onKeyInput(ev);
		return;
	}
	bool genOutput = false;

	switch(_buttonModel->ButtonType.get())
	{
		case ButtonType::Press:
		{
			if(ev.toUTF8String() == _buttonModel->FocusedActionKey.get() && _buttonModel->Focused.get())	
			{
				_buttonModel->ButtonState = (ev.state() == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;
				genOutput = true;
			}
			else if(ev.shortCutKey() == _buttonModel->ActionKey.get())
			{
				_buttonModel->ButtonState = (ev.state() == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;				
				_buttonModel->Focused = false;
				_buttonModel->Focused = true;
				genOutput = true;
			}
			else
			{
				if(_buttonModel->ButtonState.get() != DeviceButton::Released)
				{
					_buttonModel->ButtonState = DeviceButton::Released;
					genOutput = true;
				}
			}
		}
		break;

		case ButtonType::Toggle:
		{
			if(ev.toUTF8String() == _buttonModel->FocusedActionKey.get() && _buttonModel->Focused.get())		
			{
				if((ev.state() == KeyEvent::KeyDown))
				{
					if(_buttonModel->ButtonState.get() == DeviceButton::Pressed)
						_buttonModel->ButtonState = DeviceButton::Released;
					else
						_buttonModel->ButtonState = DeviceButton::Pressed;

					genOutput = true;
				}
			}
			else if(ev.shortCutKey() == _buttonModel->ActionKey.get())
			{			
				if((ev.state() == KeyEvent::KeyDown))
				{
					_buttonModel->ButtonState = (_buttonModel->ButtonState.get() == DeviceButton::Pressed) ? DeviceButton::Released : DeviceButton::Pressed;											
					_buttonModel->Focused = false;
					_buttonModel->Focused = true;
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

	if(!_buttonModel->Enabled.get())
	{
		Label::onPointerInput(ev);
		return;
	}

	if(!_buttonModel->Visible.get())
	{
		Label::onPointerInput(ev);
		return;
	}

	if(!_buttonModel->contains(point))
	{
		_buttonModel->Armed = false;
		Label::onPointerInput(ev);
		return;
	}

	bool genOutput = false;

	if(!_buttonModel->Armed.get())
	{
		_buttonModel->Armed = true;
		genOutput = true;
	}

	if( ev.buttons().size() == 0)
	{
		Label::onPointerInput(ev);
		return;
	}	

	switch(_buttonModel->ButtonType.get())
	{
		case ButtonType::Press:
		{
			if(!_buttonModel->Focused.get())
			{
				genOutput = true;						
				_buttonModel->Focused = false;
				_buttonModel->Focused = true;					
			}

			switch(ev.buttons()[0].state())
			{
				case DeviceButton::Pressed:
				{		

					if(_buttonModel->ButtonState.get() != DeviceButton::Pressed)
					{
						genOutput = true;						
						_buttonModel->ButtonState = DeviceButton::Pressed;
					}
				}
				break;
			
				case DeviceButton::Released:			
				{

					if(_buttonModel->ButtonState.get() != DeviceButton::Released)
					{
						_buttonModel->ButtonState = DeviceButton::Released;						
						genOutput = true;
					}
				}
				break;
			}
		}
		break;

		case ButtonType::Toggle:
		{
			if(!_buttonModel->Focused.get())	
			{
				_buttonModel->Focused = false;
				_buttonModel->Focused = true;
				genOutput = true;
			}

			if(ev.buttons()[0].state() == DeviceButton::Pressed )
			{
				genOutput = true;
				if(_buttonModel->ButtonState.get() == DeviceButton::Pressed)
					_buttonModel->ButtonState = DeviceButton::Released;
				else
					_buttonModel->ButtonState = DeviceButton::Pressed;
			}
		}				
		break;
	}

	Label::onPointerInput(ev);
	
	if(genOutput)
		invalidate();
}


void Button::onRender()
{	

	if(!_buttonModel->Visible.get())
		return;	
	 
	if(!_buttonModel->Enabled.get())
	{
		_buttonModel->ForeColor.set(Pt::Gfx::ARgbColor(0,100,100,100));
		Label::onRender();
		return;
	}			

	_buttonModel->ForeColor.set(Pt::Gfx::ARgbColor(0,0,0,0));
	_buttonModel->HighLight.set(_buttonModel->ButtonState.get() == DeviceButton::Pressed);

	Label::onRender();
	
	if(_buttonModel->ButtonState.get() == DeviceButton::Pressed)
		return;

	Pt::Hmi::Painter& localPainter = paintSurface().painter();
	Pt::Gfx::SizeF size = _buttonModel->Size.get();
       
	if(_buttonModel->Armed.get() || _buttonModel->Focused.get())
	{
		Pt::Gfx::SizeF size = _buttonModel->Size.get();
		size.addHeight(-5);
		size.addWidth(-5);

		Pt::Gfx::ARgbColor armedColor(0,160,160,160);
		 
		Pt::Gfx::Pen pen(1, armedColor, Pt::Gfx::Pen::DashStyle);
		
		localPainter.setPen(pen);		
		Pt::Gfx::RectF rect(Pt::Gfx::PointF(2,2), size);
		localPainter.drawRect(rect);		
	}		
}

}}
