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

#include <Pt/System/EventLoop.h>
#include <Pt/String.h>

#include <linux/input.h>
#include <linux/kd.h>
#include <linux/keyboard.h>

namespace Pt {

namespace Forms {

InputDevice::InputDevice(const char* deviceName)
: _ioh(*this)
, _loop(0)
, _screenWidth(0)
, _screenHeight(0)
, _shift(false)
, _control(false)
, _alt(false)
, _meta(false)
, _keyEvent()
, _mouseEvent()
, _touchCount(0)
, _touchEvent()
{
    _ioh.fd = ::open(deviceName, O_RDONLY|O_NONBLOCK);

    if( _ioh.fd < 0 )
        throw Pt::System::AccessFailed(deviceName);
}


InputDevice::InputDevice()
: _ioh(*this)
, _loop(0)
, _screenWidth(0)
, _screenHeight(0)
, _shift(false)
, _control(false)
, _alt(false)
, _meta(false)
, _keyEvent()
, _mouseEvent()
, _touchCount(0)
, _touchEvent()
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


void InputDevice::begin()
{      
    if( ! _loop )
        throw std::logic_error("input device not active");

    Pt::System::Selector& selector = _loop->selector();
    selector.beginRead(&_ioh);
}


void InputDevice::onAttach(System::EventLoop& loop)
{ 
    _loop = &loop;
}


void InputDevice::onDetach(System::EventLoop& loop)
{ 
    _loop = 0; 
}


void InputDevice::onCancel()
{ 
    throw std::logic_error("not implemented"); 
}


bool InputDevice::onRun()
{
    size_t count = 0;
    struct input_event ev;
    size_t evSize = sizeof(struct input_event);
    
    for(;;)
    {
      ssize_t n = ::read(_ioh.fd, &ev, evSize);
      if(n <= 0)
        break;

      ++count;

      //std::clog << "\ninput on fd: " << _ioh.fd << " #" << count
      //          << " type: " << ev.type << std::endl;

      switch (ev.type)
      {
          case EV_KEY:
          {
              onKey(ev);
              break;
          }

          case EV_REL:
          {
              onRelative(ev);
              break;
          }

          case EV_ABS:
          {
              onAbsolute(ev);
              break;
          }

          case EV_SYN:
          {
              if(_touchCount > 0)
              {
                  //std::clog << "EV_SYN " << ev.code << " " << ev.value << std::endl;

                  _eventReady.send(_touchEvent);
                  _touchCount = 0;

                  if( _touchEvent.isPress() )
                      _touchEvent.setMove();
              }

              break;
          }
      }
    }
    
    return count > 0;
}


void InputDevice::onRelative(const input_event& ev)
{
    //std::clog << "EV_REL " << ev.code << " " << ev.value << std::endl;

    _mouseEvent.setMove();

    if(ev.code == REL_X)
        _mouseEvent.setX( _mouseEvent.x() + static_cast<double>(ev.value) );
    else if(ev.code == REL_Y)
        _mouseEvent.setY( _mouseEvent.y() + static_cast<double>(ev.value) );

    if( _mouseEvent.x() < 0 )
        _mouseEvent.setX( 0);

    if( _mouseEvent.x() >= _screenWidth )
        _mouseEvent.setX( _screenWidth - 1 );

    if( _mouseEvent.y() < 0 )
        _mouseEvent.setY( 0);

    if( _mouseEvent.y() >= _screenHeight )
        _mouseEvent.setY( _screenHeight - 1 );

    _eventReady.send(_mouseEvent);
}


void InputDevice::onAbsolute(const input_event& ev)
{
    // event sequence for touch events
    //EV_KEY 330 1
    //EV_ABS_X
    //EV_ABS_Y
    //EV_SYN
    //
    //EV_ABS_X
    //EV_SYN
    //
    //EV_ABS_X
    //EV_SYN
    //
    //EV_KEY 330 0
    //EV_SYN

    switch(ev.code)
    {
        case ABS_MT_SLOT:
        case ABS_MT_TRACKING_ID:
        
        case ABS_MT_POSITION_Y:
            //std::clog << "ABS_MT_POSITION_Y " << ev.value << std::endl;
            return;
        
        case ABS_MT_POSITION_X:
            //std::clog << "ABS_MT_POSITION_X " << ev.value << std::endl;
            return;
        
        case ABS_MT_PRESSURE:
            //std::clog << "ABS_MT_PRESSURE " << ev.value << std::endl;
            return;

        case ABS_X:
            //std::clog << "ABS_X " << ev.value << std::endl;
            
            _touchEvent.setX( static_cast<double>(ev.value) );
            _touchCount++;
            break;

        case ABS_Y:
            //std::clog << "ABS_Y " << ev.value << std::endl;
           
            _touchEvent.setY( static_cast<double>(ev.value) );
            _touchCount++;
            break;

        case ABS_PRESSURE:
            //std::clog << "ABS_PRESSURE " << ev.value << std::endl;
            return;

        default:
            //std::clog << "UNKNOWN EV CODE " << ev.code << " " <<  ev.value << std::endl;
            return;
    }
}


void InputDevice::onKey(const input_event& ev)
{
    //std::clog << "EV_KEY on fd: " << _ioh.fd << " " 
    //          << ev.code << " " << ev.value << std::endl;
    
    if( ev.code == 272)    
    {
        if(ev.value == 0)
            _mouseEvent.setRelease(MouseEvent::Left);
        else
            _mouseEvent.setPress(MouseEvent::Left);

        _eventReady.send(_mouseEvent); 
        return;
    }

    if( ev.code == 273 )    
    {
        if(ev.value == 0)
            _mouseEvent.setRelease(MouseEvent::Right);
        else
            _mouseEvent.setPress(MouseEvent::Right);

        _eventReady.send(_mouseEvent); 
        return;
    }

    if(ev.code == 330) // BTN_TOUCH
    {
        if(ev.value == 0)
        {
            _touchCount++;
            _touchEvent.setRelease();  
        }
        else
        {
            _touchCount++;
            _touchEvent.setPress();
        }

        return;
    }

    Pt::uint32_t keyCode = Key::NoKey;
    Pt::Char ch;

    switch(ev.code)
    {
        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
            if(ev.value == 1)
            {
                _shift = true;
                keyCode = Key::ShiftKey;
            }
            else if (ev.value == 0)
                _shift = false;

            break; 
        
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            if(ev.value == 1)
            {
                _control = true;
                keyCode = Key::ControlKey;
            }
            else if (ev.value == 0)
                _control = false;

            break; 

        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            if(ev.value == 1)
            {
                _alt = true;
                keyCode = Key::AltKey;
            }
            else if (ev.value == 0)
                _alt = false;

            break; 
        
        case KEY_LEFTMETA:
        case KEY_RIGHTMETA:
            if(ev.value == 1)
            {
                _meta = true;
                keyCode = Key::MetaKey;
            }
            else if (ev.value == 0)
                _meta = false;

            break;

        case KEY_TAB:
            ch = 15;
            keyCode = Key::Tab;
            break;

        case KEY_SPACE:
            ch = 32;
            keyCode = Key::Space;
            break;
        
        default:
        {
            struct kbentry ke;
            ke.kb_table = 0;
            ke.kb_index = ev.code;

            ::ioctl(STDIN_FILENO, KDGKBENT, (unsigned long)&ke);

            unsigned typ = KTYP(ke.kb_value);
            unsigned value = KVAL(ke.kb_value);

            if(typ == KT_LETTER || typ == KT_LATIN)
            {
                ch = value;
                keyCode = Pt::toupper(ch).value();
            }
            else if(typ == KT_PAD && value < 10)
            {
                // TODO: does this mean numpad?
                keyCode = Key::NumPad0 + value;                
                ch = keyCode;
            }

            break;
        }
    }

    Key::Modifiers modifiers;

    if(_shift)
        modifiers.add(Key::Shift);

    if(_control)
        modifiers.add(Key::Control);

    if(_alt)
        modifiers.add(Key::Alt);

    if(_meta)
        modifiers.add(Key::Meta);

    Key key(modifiers, keyCode);

    if(ev.value == 1)
        _keyEvent.setPress(key, ch);
    else
        _keyEvent.setRelease(key, ch);

    _eventReady.send(_keyEvent);
}

} // namespace

} // namespace
