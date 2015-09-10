 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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
#include <linux/input.h>
#include <linux/kd.h>
#include <linux/keyboard.h>

#include "InputDevice.h"
#include "ScreenImpl.h"
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"

namespace Pt {
namespace Hmi {

InputDevice::InputDevice(const char* deviceName)
: _ioh(*this)
, _loop(0)
{
  _ioh.fd = ::open(deviceName, O_RDONLY|O_NONBLOCK);

	if( _ioh.fd < 0 )
		throw Pt::System::AccessFailed(deviceName);
}


InputDevice::~InputDevice()
{
    try
    {
        this->close();
    }
    catch(...)
    {}
}


bool InputDevice::onRun()
{
	struct input_event evts[64];
		

	int bytes = ::read(_ioh.fd, evts, sizeof(struct input_event) * 64);
    
	if( bytes < (int) sizeof(struct input_event) )
	{
			return false;
	}

	for( unsigned i = 0; i < bytes / sizeof(input_event); i++ )
	{
		struct input_event& ev = evts[i];

    switch (ev.type)
    {
			case EV_KEY:
			{

				if( ev.code == 272 )	
				{
					_mouseEvent.buttons()[0].setState( ev.value == 0 ? DeviceButton::Released : DeviceButton::Pressed);
					Application::instance().mainScreen().impl()->eventReceived().send( _mouseEvent );				                				
					break;
				}
				
				if( ev.code == 273 )	
				{
					_mouseEvent.buttons()[2].setState( ev.value == 0 ? DeviceButton::Released : DeviceButton::Pressed);
					Application::instance().mainScreen().impl()->eventReceived().send( _mouseEvent );				                				
					break;
				}

				if(ev.value == 1)
					_keyEvent.setState(KeyEvent::KeyDown);
				else if(ev.value == 0)
					_keyEvent.setState(KeyEvent::KeyUp);
				else
					break;;
			
		        switch(ev.code)
				{
					case KEY_RIGHTALT:
					case KEY_LEFTALT:
						_keyEvent.setAlt(_keyEvent.state() == KeyEvent::KeyDown);
											break;	
		
					case KEY_LEFTCTRL:
					case KEY_RIGHTCTRL:
						_keyEvent.setCtrl(_keyEvent.state() == KeyEvent::KeyDown);
					break;

					case KEY_LEFTSHIFT:
					case KEY_RIGHTSHIFT:
						_keyEvent.setShift(_keyEvent.state() == KeyEvent::KeyDown);  
					break;  

					default:
					{
						struct kbentry ke;
						ke.kb_table = 0;
						ke.kb_index = ev.code;

						::ioctl(STDIN_FILENO, KDGKBENT, (unsigned long)&ke);
					
						unsigned unicode = 0;
						unsigned typ = KTYP(ke.kb_value);
						unsigned value = KVAL(ke.kb_value);
                    
						if(typ == KT_LETTER|| typ == KT_LATIN)
						{
								unicode = value;
						}
						else if(typ == KT_PAD && value < 10)
						{
								unicode = 0x30 + value;
						}
						else
								break;

							_keyEvent.setUnicode(unicode);
						}
					break;
				}

				Application::instance().mainScreen().impl()->eventReceived().send( _keyEvent );				
			}
			break;

      case EV_REL:
			{
				if(ev.code == REL_X)
					_mouseEvent.addX( static_cast<double>(ev.value) );
				else if(ev.code == REL_Y)
					_mouseEvent.addY( static_cast<double>(ev.value) );

				if( _mouseEvent.x() < 0 )
					_mouseEvent.setX( 0);

				if( _mouseEvent.x() >= Application::instance().mainScreen().width() )
					_mouseEvent.setX( Application::instance().mainScreen().width() - 1 );

				if( _mouseEvent.y() < 0 )
					_mouseEvent.setY( 0);

				if( _mouseEvent.y() >= Application::instance().mainScreen().height() )
					_mouseEvent.setY( Application::instance().mainScreen().height() - 1 );

						                
				Application::instance().mainScreen().impl()->eventReceived().send( _mouseEvent );				                				
			}
			break;

			case EV_ABS:
			{
				switch(ev.code)
				{
					case ABS_MT_SLOT:                
					case ABS_MT_TRACKING_ID:
                
					case ABS_X:
					case ABS_MT_POSITION_X:
			            std::cout<<"ABS_X" << ev.value<<std::endl;
						_mouseEvent.setX( static_cast<double>(ev.value) );
					break;
                    
					case ABS_Y:
					case ABS_MT_POSITION_Y:
				          std::cout<<"ABS_Y" << ev.value<<std::endl;
						_mouseEvent.setY( static_cast<double>(ev.value) );
					break;
                    
					case ABS_PRESSURE:
					case ABS_MT_PRESSURE:                  
			            _mouseEvent.buttons()[0].setState( ev.value == 0 ? DeviceButton::Released : DeviceButton::Pressed );						
					break;

					default:
					break;
				}
				

				Application::instance().mainScreen().impl()->eventReceived().send( _mouseEvent );
			}
			break;  
		}		
	}

	return true;
}


void InputDevice::onAttach(System::EventLoop& loop)
{ 
    _loop = &loop;
}


void InputDevice::onDetach(System::EventLoop& loop)
{ 
    _loop = 0; 
}

} // namespace
} // namespace
