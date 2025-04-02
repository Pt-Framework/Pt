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

#include "MouseDevice.h"

#include <Pt/Forms/ScrollEvent.h>
#include <Pt/System/EventLoop.h>
#include <Pt/String.h>

#include <linux/input.h>
#include <linux/kd.h>
#include <linux/keyboard.h>

namespace Pt {

namespace Forms {

MouseDevice::MouseDevice(const char* deviceName)
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
, _touchMove(0)
, _touchEvent()
{
    _ioh.fd = ::open(deviceName, O_RDONLY|O_NONBLOCK);

    if( _ioh.fd < 0 )
        throw Pt::System::AccessFailed(deviceName);

    input_absinfo info = {0};
    ioctl(_ioh.fd, EVIOCGABS(ABS_X), &info);
    _mouseResWidth = info.maximum;
        
    info.maximum = 0;
    ioctl(_ioh.fd, EVIOCGABS(ABS_Y), &info);
    _mouseResHeigh = info.maximum;   
}


MouseDevice::MouseDevice()
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
, _touchMove(0)
, _touchEvent()
{
    _ioh.fd = -1;
}


MouseDevice::~MouseDevice()
{
    try
    {
        this->close();
    }
    catch(...)
    {}
}


void MouseDevice::open(const char* deviceName)
{
    close();

    _ioh.fd = ::open(deviceName, O_RDONLY|O_NONBLOCK);

    if( _ioh.fd < 0 )
        throw Pt::System::AccessFailed(deviceName);
}


void MouseDevice::close()
{ 
    if(_ioh.fd != -1)
    {
        ::close(_ioh.fd); 
        _ioh.fd = -1;
    }
}


void MouseDevice::begin()
{      
    if( ! _loop )
        throw std::logic_error("input device not active");

    Pt::System::Selector& selector = _loop->selector();
    selector.beginRead(&_ioh);
}


void MouseDevice::onAttach(System::EventLoop& loop)
{ 
    _loop = &loop;
}


void MouseDevice::onDetach(System::EventLoop& loop)
{ 
    _loop = 0; 
}


void MouseDevice::onCancel()
{ 
    throw std::logic_error("not implemented"); 
}


bool MouseDevice::onRun()
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
      }
    }
    
    return count > 0;
}


void MouseDevice::onRelative(const input_event& ev)
{
    //std::clog << "EV_REL " << ev.code << " " << ev.value << std::endl;

    if (ev.code == REL_WHEEL)
    {
        int delta = ev.value;

        ScrollEvent sev;
        sev.set(ScrollEvent::Vertical, delta * 20);
        _eventReady.send(sev);
        return;
    }

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


void MouseDevice::onAbsolute(const input_event& ev)
{
    //std::clog << "EV_ABS " << ev.code << " " << ev.value << std::endl;

    _mouseEvent.setMove();

    if(ev.code == ABS_X)
        _mouseEvent.setX( static_cast<double>(ev.value) * _screenWidth / _mouseResWidth);
    else if(ev.code == ABS_Y)
        _mouseEvent.setY( static_cast<double>(ev.value) * _screenHeight / _mouseResHeigh );    

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


void MouseDevice::onKey(const input_event& ev)
{
    //std::clog << "EV_KEY on fd: " << _ioh.fd << " " << ev.code << " " << ev.value << std::endl;
    
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
}

} // namespace

} // namespace
