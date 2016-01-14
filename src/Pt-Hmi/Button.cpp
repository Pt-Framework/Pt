/* Copyright (C) 2013 Marc Boris Duerner 
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Point.h>

namespace Pt{
namespace Hmi{

Button::Button()
: _pressed(false)
, _timeout(false)
, _pressCounter(0)
, _hover(false)
, _buttonType(Pt::Hmi::ButtonType::Press)
, _doublePressTimeInMs(1500)
, _image(Gfx::Image(0,0))
, _imageAlign( MidleLeft)
{
  setBackgroundColor(Gfx::Color::fromRgb8(245,245,245));
  setPanelBorderStyle(Custom);
  setPanelBorderWidth(1);
  setAutoSize(false);
  setContentAlignment(MidleCenter);
  setAcceptFocus(true);
  setPanelBorderRoundEdge(true);
  setPanelBorderStyle(Panel::Single);
	
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
		_doublePressTimer.start(_doublePressTimeInMs);

	if( _pressCounter == 2)	
	{
		_pressCounter = 0;
		DoubleClicked.send();
	}

	Clicked.send();
}

void Button::onMnemonic()
{
	switch(_buttonType)
	{
		case ButtonType::Press:
		{
			onPressedAction();
		}
		break;

		case ButtonType::Toggle:
		{
		}			
		break;
	}
	  
	Label::onMnemonic();	    		
	invalidate();	
}

void Button::onDoublePressedAction()
{
	DoubleClicked.send();
	_doublePressTimer.stop();
}



void Button::onActionKey( const KeyEvent& kev )
{
	//switch( _buttonType )
	//{
	//	case ButtonType::Press:
	//	{
	//		_buttonState= ( state == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;
	//	}
	//	break;
	//	
	//	case ButtonType::Toggle:
	//	{
	//		if((state == KeyEvent::KeyDown))
	//		{
	//			if(_buttonState== DeviceButton::Pressed)
	//				_buttonState= DeviceButton::Released;
	//			else
	//				_buttonState= DeviceButton::Pressed;
	//		}
	//	}
	//	break;
	//}

	invalidate();
}

void Button::onShortcutKey( const KeyEvent& kev )
{
	//switch( _buttonType)
	//{
	//	case ButtonType::Press:
	//	{
	//		_buttonState= (state == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;				
	//		setFocus( true );

	//	}
	//	break;

	//	case ButtonType::Toggle:
	//	{
	//		if( state == KeyEvent::KeyDown )
	//		{
	//			_buttonState= (_buttonState== DeviceButton::Pressed) ? DeviceButton::Released : DeviceButton::Pressed;											
	//			setFocus( true );
	//		}
	//	}
	//	break;
	//}
	
	invalidate();			
}


void Button::onKeyEvent(const KeyEvent& ev)
{		
	if( !isEnabled() )
		return;

	if( !visible() )
		return;

	Label::onKeyEvent(ev);

	//if(_buttonType == ButtonType::Press && _buttonState!= DeviceButton::Released)
	//{
	//	_buttonState= DeviceButton::Released;
	//	invalidate();
	//}		
}

void Button::onPointerEvent(const MouseEvent& ev)
{    
	Gfx::PointF point = toClient(Gfx::PointF(ev.x(), ev.y()));  
      
	Label::onPointerEvent(ev);
		    
	//if( !isEnabled() )
	//{
	//	_lastPointerState = ev.buttons()[0].state();
	//	return;
	//}

	//if( !visible() )
	//{
	//	_lastPointerState = ev.buttons()[0].state();
	//	return;
	//}
    
	/*if( !contains(point) )
	{
		if( _hover )
        {
			    _hover =   false;
          invalidate();
        }

		_lastPointerState = ev.buttons()[0].state();

		return;			
	}
    
	bool genOutput = false;

	if(!_hover)
	{
		_hover =   true;    
		genOutput = true;
	}

	if( _lastPointerState != ev.buttons()[0].state() )		
	{	
		switch(_buttonType)
		{
			case ButtonType::Press:
			{
				if(!hasFocus())
				{
					genOutput = true;						
					setFocus(true);															
				}

				switch(ev.buttons()[0].state())
				{
					case DeviceButton::Pressed:
					{		

						if(_buttonState != DeviceButton::Pressed)
						{
							genOutput = true;						
							_buttonState = DeviceButton::Pressed;
						}
					}
					break;
			
					case DeviceButton::Released:			
					{

						if(_buttonState!= DeviceButton::Released)
						{
							genOutput = true;
							_buttonState= DeviceButton::Released;												
						}
					}
					break;
				}
			}
			break;

			case ButtonType::Toggle:
			{
				if(!hasFocus())	
				{
					setFocus(true);			
					genOutput = true;
				}

				if(ev.buttons()[0].state() == DeviceButton::Pressed )
				{
					genOutput = true;

					if(_buttonState== DeviceButton::Pressed)
						_buttonState= DeviceButton::Released;
					else
						_buttonState= DeviceButton::Pressed;
				}
			}				
			break;
		}
	}

	_lastPointerState = ev.buttons()[0].state();

  if( genOutput )
  {
		invalidate();
  }*/
}


void Button::onRender(PaintSurface& paintSurface)
{	
	if( !isEnabled() )
	{
		Label::onRender(paintSurface);
		return;
	}			

	Label::onRender(paintSurface);
	
	//if( _buttonState== DeviceButton::Pressed )
	//	return;

	Pt::Hmi::Painter& painter = paintSurface.painter();
 Gfx::SizeF  size = this->size();
 Gfx::PointF pos  = this->position();
       
	if( _hover || hasFocus() )
	{
		size.addHeight(-4);
		size.addWidth(-4);

		Gfx::Color armedColor(0,160/255.0f,160/255.0f,160/255.0f);
		 
		Gfx::Pen pen(1, armedColor,Gfx::Pen::DashStyle);
		
		painter.setPen(pen);		
		Gfx::RectF rect(pos +Gfx::PointF(2,2), size);
		painter.drawRect(rect);		
	}		
}

}}
