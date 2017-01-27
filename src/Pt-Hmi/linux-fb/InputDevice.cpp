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

namespace Hmi {

InputDevice::InputDevice(const char* deviceName)
: _ioh(*this)
, _loop(0)
, _shift(false)
, _control(false)
, _alt(false)
, _meta(false)
, _keyEvent(0)
, _mouseEvent(0)
, _touchMove(0)
, _touchEvent(0)
{
    _ioh.fd = ::open(deviceName, O_RDONLY|O_NONBLOCK);

    if( _ioh.fd < 0 )
        throw Pt::System::AccessFailed(deviceName);
}


InputDevice::InputDevice()
: _ioh(*this)
, _loop(0)
, _shift(false)
, _control(false)
, _alt(false)
, _meta(false)
, _keyEvent(0)
, _mouseEvent(0)
, _touchMove(0)
, _touchEvent(0)
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
    struct input_event evts[64];

    int bytes = ::read(_ioh.fd, evts, sizeof(struct input_event) * 64);

    if( bytes < (int) sizeof(struct input_event) )
    {
        return false;
    }

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
        }
    }

    return true;
}


void InputDevice::onRelative(const input_event& ev)
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

    _eventReady.send(_mouseEvent);
}


void InputDevice::onAbsolute(const input_event& ev)
{
    //std::clog << "EV_ABS " << ev.code << " " << ev.value << std::endl;

    switch(ev.code)
    {
        case ABS_MT_SLOT:
        case ABS_MT_TRACKING_ID:
        case ABS_MT_POSITION_Y:
        case ABS_MT_POSITION_X:
        case ABS_MT_PRESSURE:
          return;

        case ABS_X:
            _touchEvent.setX(  (static_cast<double>(ev.value)));
            _touchMove++;
        break;

        case ABS_Y:
           _touchEvent.setY( static_cast<double>(ev.value));
            _touchMove++;
        break;

        case ABS_PRESSURE:
        return;

        default:
           return;
    }

    if(_touchMove > 1)
    {
        if(_touchMove == 2)
            _touchEvent.setPress();
        else
            _touchEvent.setMove();

        _eventReady.send(_touchEvent);
    }
}


void InputDevice::onKey(const input_event& ev)
{
    //std::clog << "EV_KEY " << ev.code << " " << ev.value << std::endl;
    
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

    if(ev.code == 330)
    {
        if( ev.value == 0  )
        {
            if(_touchMove == 0)
            {
                _touchEvent.setPress();
                _eventReady.send(_touchEvent);
            }

            _touchMove = 0;
            _touchEvent.setRelease();
            _eventReady.send(_touchEvent);
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
            else
                _shift = false;

            break; 
        
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            if(ev.value == 1)
            {
                _control = true;
                keyCode = Key::ControlKey;
            }
            else
                _control = false;

            break; 

        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            if(ev.value == 1)
            {
                _alt = true;
                keyCode = Key::AltKey;
            }
            else
                _alt = false;

            break; 
        
        case KEY_LEFTMETA:
        case KEY_RIGHTMETA:
            if(ev.value == 1)
            {
                _meta = true;
                keyCode = Key::MetaKey;
            }
            else
                _meta = false;

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
