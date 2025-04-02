/*
  Copyright (C) 2015 Marc Boris Duerner 
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include "InputDevice.h"

#include <Pt/Forms/Application.h>

#include <linux/input.h>
#include <linux/kd.h>
#include <linux/keyboard.h>

namespace Pt {

namespace Forms {

InputDevice::InputDevice(const char* deviceName)
: _ioh(*this)
, _loop(0)
, _leftAlt(false)
, _rightAlt(false)
, _touchMove(0)
{
    _ioh.fd = ::open(deviceName, O_RDONLY|O_NONBLOCK);

    if( _ioh.fd < 0 )
        throw Pt::System::AccessFailed(deviceName);
}


InputDevice::InputDevice()
: _ioh(*this)
, _loop(0)
{
    _ioh.fd = -1;
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


void InputDevice::open(const char* deviceName)
{
    close();

    _ioh.fd = ::open(deviceName, O_RDONLY|O_NONBLOCK);

    if( _ioh.fd < 0 )
        throw Pt::System::AccessFailed(deviceName);
}


void InputDevice::close()
{ 
    if(_ioh.fd != -1)
    {
        ::close(_ioh.fd); 
        _ioh.fd = -1;
    }
}


bool InputDevice::onRun()
{
    struct input_event evts[64];

    int bytes = ::read(_ioh.fd, evts, sizeof(struct input_event) * 64);

    if( bytes < (int) sizeof(struct input_event) )
    {
        return false;
    }

    bool hasPointerEvent = false;

    // event sequence for touch events
    //EV_KEY 330 1
    //EV_ABS_X
    //EV_ABS_Y
    //EV_ABS_X
    //EV_ABS_X
    //EV_KEY 330 0

    for( unsigned i = 0; i < bytes / sizeof(input_event); i++ )
    {
        struct input_event& ev = evts[i];

        switch (ev.type)
        {
            case EV_KEY:
            {
                //std::clog << "EV_KEY " << ev.code << " " << ev.value << std::endl;
                if( ev.code == 272)    
                {
                    if( ev.value == 0  )
                        _mouseEvent.setRelease(MouseEvent::Left);
                    else
                        _mouseEvent.setPress(MouseEvent::Left);

                    _eventReady.send(_mouseEvent); 
                    hasPointerEvent = false;                                               
                    break;
                }

                if( ev.code == 273 )    
                {
                    if( ev.value == 0  )
                        _mouseEvent.setRelease(MouseEvent::Right);
                    else
                        _mouseEvent.setPress(MouseEvent::Right);

                    _eventReady.send(_mouseEvent); 
                    hasPointerEvent = false;                                            
                    break;
                }

                if(ev.code == 330)    
                {
                    if( ev.value == 0  )
                    {
                        _touchMove = 0;
                        _mouseEvent.setRelease(MouseEvent::Left);
                        _eventReady.send(_mouseEvent);
                    }
                                                                   
                    break;
                }

                Key::Code keyCode;

                switch(ev.code)
                {
                    case KEY_LEFTALT:
                        if(ev.value == 1)
                        {
                            _leftAlt = true;
                            keyCode = Key::LMenu;
                        }
                        else
                            _leftAlt = false;

                        break;    

                    case KEY_RIGHTALT:
                        if(ev.value == 1)
                        {
                            _rightAlt = true;
                            keyCode = Key::RMenu;
                        }
                        else
                            _rightAlt = false;

                        break;    

                    case KEY_LEFTCTRL:
                    case KEY_RIGHTCTRL:
                        break;

                    case KEY_LEFTSHIFT:
                    case KEY_RIGHTSHIFT: 
                        break;  

                    default:
                    {
                        struct kbentry ke;
                        ke.kb_table = 0;
                        ke.kb_index = ev.code;

                        ::ioctl(STDIN_FILENO, KDGKBENT, (unsigned long)&ke);

                        unsigned typ = KTYP(ke.kb_value);
                        unsigned value = KVAL(ke.kb_value);

                        if(typ == KT_LETTER|| typ == KT_LATIN)
                        {
                            keyCode = static_cast<Key::Code>(value);
                        }
                        else if(typ == KT_PAD && value < 10)
                        {
                            keyCode = static_cast<Key::Code>(0x30 + value);
                        }

                        break;
                    }
                }

                Key::Modifiers modifiers;

                if(_leftAlt || _rightAlt)
                    modifiers |= Key::Alt;

                Key key(modifiers, keyCode);
                    
                // TODO: 
                Pt::Char ch = keyCode;

                if(ev.value == 1)
                    _keyEvent.setPress(key, ch);
                else
                    _keyEvent.setRelease(key, ch);

                _eventReady.send(_keyEvent);
                break;
            }

            case EV_REL:
            {
                //std::clog << "EV_REL" << ev.value << std::endl;
                _mouseEvent.setMove();

                if(ev.code == REL_X)
                    _mouseEvent.setX( _mouseEvent.x() + static_cast<double>(ev.value) );
                else if(ev.code == REL_Y)
                    _mouseEvent.setY( _mouseEvent.y() + static_cast<double>(ev.value) );

                if( _mouseEvent.x() < 0 )
                    _mouseEvent.setX( 0);

                if( _mouseEvent.x() >= _screenSize.width() )
                    _mouseEvent.setX( _screenSize.width() - 1 );

                if( _mouseEvent.y() < 0 )
                    _mouseEvent.setY( 0);

                if( _mouseEvent.y() >= _screenSize.height() )
                    _mouseEvent.setY( _screenSize.height() - 1 );

                hasPointerEvent = true;
                break;
            }

            case EV_ABS:
            {
                //std::clog << "EV_ABS" << std::endl;
                
                Gfx::SizeF screenSize = Application::instance().mainScreen().size();
                double scaleX =  screenSize.width() / 800.0;
                double scaleY =  screenSize.height() / 480.0;

                switch(ev.code)
                {
                    case ABS_MT_SLOT:                
                    case ABS_MT_TRACKING_ID:

                    case ABS_X:
                    case ABS_MT_POSITION_X:
                        _mouseEvent.setX( static_cast<double>(ev.value)*scaleX  );
                        _touchMove++;
                        break;

                    case ABS_Y:
                    case ABS_MT_POSITION_Y:
                        _mouseEvent.setY( static_cast<double>(ev.value)*scaleY );
                        _touchMove++;
                        break;

                    case ABS_PRESSURE:
                    case ABS_MT_PRESSURE:                  
                        break;

                    default:
                        break;
                }
                
                if( _touchMove == 2 )
                {
                    _mouseEvent.setPress(MouseEvent::Left);
                    _eventReady.send(_mouseEvent);
                    
                    _mouseEvent.setMove();
                }

                break;
            }
        }
    }

    if(hasPointerEvent || _touchMove > 2)
    {
        //std::clog << "mouse: " << _mouseEvent.isPressed(MouseEvent::Left) << " "
        //          << _mouseEvent.x() << " " << _mouseEvent.y() << std::endl;
        _eventReady.send(_mouseEvent);
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
