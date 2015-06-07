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
#include <Pt/Hmi/Application.h>
#include <Pt/Ui/Pen.h>
#include <Pt/Ui/Color.h>
#include <Pt/Ui/Point.h>

namespace Pt{
namespace Hmi{

Button::Button()
: _pressed(false)
, _timeout(false)
, _pressCounter(0)
, PT_HMI_INIT_PROPERTY_VALUE(ButtonState,Pt::Hmi::DeviceButton::Released)
, PT_HMI_INIT_PROPERTY_VALUE(Armed,false)
, PT_HMI_INIT_PROPERTY_VALUE(ButtonType,Pt::Hmi::ButtonType::Press)
, PT_HMI_INIT_PROPERTY_VALUE(DoublePressTimeInMs,1500)
, PT_HMI_INIT_PROPERTY_VALUE(Image,Ui::Image(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(ImageAlign, Align::MidleLeft)
, _lastPointerState( DeviceButton::Released )
{
  BackColor.set(Ui::Color(245/255.0,245/255.0,245/255.0));
	PanelBorderStyle.set(BorderStyle::Widget);
	PanelBorderWidth.set(1);
	Caption.set("Button");	
  Name.set("Button");
	AutoSize.set(false);
	TextAlign.set(Align::MidleCenter);
	AcceptFocus.set(true);
	PanelBorderRoundEdge.set(true);
	PanelBorderStyle.set(BorderStyle::Single);
	
	//Todo: change property to Property
	ButtonState.changed() += Pt::slot(*this, &Button::onButtonStateChanged);
	
	_doublePressTimer.timeout() += Pt::slot(*this, &Button::onDoublePressedTimeout);
	_doublePressTimer.setActive(Pt::Hmi::Application::instance().loop());	
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
	_pressCounter++;
	
	if(_pressCounter == 1)
		_doublePressTimer.start(DoublePressTimeInMs.get());

	if( _pressCounter == 2)	
	{
		_pressCounter = 0;
		DoubleClicked.send();
	}

	Clicked.send();
}

void Button::onMnemonic()
{
	if(!Enabled.get())
	{
		Label::onMnemonic();
		return;
	}

	if(!Visible.get())
	{
		Label::onMnemonic();
		return;
	}

	switch(ButtonType.get())
	{
		case ButtonType::Press:
		{
			onPressedAction();
		}
		break;

		case ButtonType::Toggle:
		{
			if(ButtonState.get() == DeviceButton::Pressed)
				ButtonState = DeviceButton::Released;
			else
				ButtonState = DeviceButton::Pressed;
		}			
		break;
	}
	
  HighLight = ButtonState.get() == DeviceButton::Pressed ;
	Label::onMnemonic();	    		
	invalidate();	
}

void Button::onDoublePressedAction()
{
	DoubleClicked.send();
	_doublePressTimer.stop();
}

void Button::onButtonStateChanged( const DeviceButton::State& prop )
{
	switch( ButtonType.get())
	{
		case Pt::Hmi::ButtonType::Press:
		
			switch( ButtonState.get())
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
			Checked.send((ButtonState.get() == Pt::Hmi::DeviceButton::Pressed));
		break;
	}
}

void Button::onActionKey( KeyEvent::KeyState state )
{
	switch( ButtonType.get() )
	{
		case ButtonType::Press:
		{
			ButtonState = ( state == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;
		}
		break;
		
		case ButtonType::Toggle:
		{
			if((state == KeyEvent::KeyDown))
			{
				if(ButtonState.get() == DeviceButton::Pressed)
					ButtonState = DeviceButton::Released;
				else
					ButtonState = DeviceButton::Pressed;
			}
		}
		break;
	}

	HighLight = ButtonState.get() == DeviceButton::Pressed ;
	invalidate();
}

void Button::onShortcutKey( KeyEvent::KeyState state )
{
	switch( ButtonType.get() )
	{
		case ButtonType::Press:
		{
			ButtonState = (state == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;				
			setFocus( true );

		}
		break;

		case ButtonType::Toggle:
		{
			if( state == KeyEvent::KeyDown )
			{
				ButtonState = (ButtonState.get() == DeviceButton::Pressed) ? DeviceButton::Released : DeviceButton::Pressed;											
				setFocus( true );
			}
		}
		break;
	}
	
	HighLight = ButtonState.get() == DeviceButton::Pressed ;
	invalidate();			
}

void Button::onKeyInput(const KeyEvent& ev)
{		
	if( !Enabled.get() )
		return;

	if( !Visible.get() )
		return;

	Label::onKeyInput(ev);

	if(ButtonType.get() == ButtonType::Press && ButtonState.get() != DeviceButton::Released)
	{
		ButtonState = DeviceButton::Released;
        HighLight = ButtonState.get() == DeviceButton::Pressed;
		invalidate();
	}		
}

void Button::onPointerInput(const PointerEvent& ev)
{    
	Ui::PointF point = toClient(Ui::PointF(ev.x(), ev.y()));
    
	Label::onPointerInput(ev);
		    
	if( !Enabled.get() )
	{
		_lastPointerState = ev.buttons()[0].state();
		return;
	}

	if( !Visible.get() )
	{
		_lastPointerState = ev.buttons()[0].state();
		return;
	}
    
	if( !contains(point) )
	{
		if( Armed.get() )
			Armed = false;

		_lastPointerState = ev.buttons()[0].state();

		return;			
	}
    
	bool genOutput = false;

	if(!Armed.get())
	{
		Armed = true;
		genOutput = true;
	}

	if( _lastPointerState != ev.buttons()[0].state() )		
	{	
		switch(ButtonType.get())
		{
			case ButtonType::Press:
			{
				if(!isFocused())
				{
					genOutput = true;						
					setFocus(true);															
				}

				switch(ev.buttons()[0].state())
				{
					case DeviceButton::Pressed:
					{		

						if(ButtonState.get() != DeviceButton::Pressed)
						{
							genOutput = true;						
							ButtonState = DeviceButton::Pressed;
						}
					}
					break;
			
					case DeviceButton::Released:			
					{

						if(ButtonState.get() != DeviceButton::Released)
						{
							genOutput = true;
							ButtonState = DeviceButton::Released;												
						}
					}
					break;
				}
			}
			break;

			case ButtonType::Toggle:
			{
				if(!isFocused())	
				{
					setFocus(true);			
					genOutput = true;
				}

				if(ev.buttons()[0].state() == DeviceButton::Pressed )
				{
					genOutput = true;

					if(ButtonState.get() == DeviceButton::Pressed)
						ButtonState = DeviceButton::Released;
					else
						ButtonState = DeviceButton::Pressed;
				}
			}				
			break;
		}
	}

	_lastPointerState = ev.buttons()[0].state();

  if( genOutput )
  {
  	HighLight = ButtonState.get() == DeviceButton::Pressed ;
		invalidate();
  }
}


void Button::onRender()
{	
	if( !Visible.get() )
		return;	
	 
	if( !Enabled.get() )
	{
		Label::onRender();
		return;
	}			

	Label::onRender();
	
	if( ButtonState.get() == DeviceButton::Pressed )
		return;

	Pt::Hmi::Painter& localPainter = paintSurface().painter();
	Ui::SizeF    size = paintSurface().size();
       
	if( Armed.get() || isFocused() )
	{
		size.addHeight(-5);
		size.addWidth(-5);

		Ui::Color armedColor(0,160/255.0,160/255.0,160/255.0);
		 
		Ui::Pen pen(1, armedColor, Ui::Pen::DashStyle);
		
		localPainter.setPen(pen);		
		Ui::RectF rect(Ui::PointF(2,2), size);
		localPainter.drawRect(rect);		
	}		
}

}}
