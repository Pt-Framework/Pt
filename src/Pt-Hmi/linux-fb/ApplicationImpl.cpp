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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
    MA 02110-1301    USA
*/

#include "ApplicationImpl.h"
#include "ScreenImpl.h"

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Screen.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/System/Logger.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/Clock.h>
#include <Pt/DateTime.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>
#include <sys/kd.h>

PT_LOG_DEFINE("Pt.Hmi.Application")

namespace Pt {

namespace Hmi {

ApplicationImpl::ApplicationImpl()
: _mouseDevice(0)
, _lastActivityTime( Pt::System::Clock::getSystemTime() )
, _lastMouse(0)
{           
    showConsole(false);
    
    std::string keyboard = Pt::System::Application::getEnvVar("PT_KEYBOARD_DEVICE");
    if( ! keyboard.empty() )
      openInputDevice(keyboard);

    std::string mouse = Pt::System::Application::getEnvVar("PT_MOUSE_DEVICE");
    std::string mouseTransform = Pt::System::Application::getEnvVar("PT_MOUSE_TRANSFORM");

    try
    {
        _mouseDevice = new MouseDevice(mouse.c_str());
        _mouseDevice->setScreenLimit(_frameBuffer.size());
        _mouseDevice->setActive(*this);
        _mouseDevice->begin();
        _mouseDevice->eventReady() += Pt::slot(*this, &ApplicationImpl::onMouseEvent);
        std::clog << "using mouse: " << mouse << std::endl;
    }
    catch(const std::exception& ex)
    {
        delete _mouseDevice;
        _mouseDevice = 0;
        std::clog << "skipping mouse device: " << mouse << std::endl;
    }

    std::string touch = Pt::System::Application::getEnvVar("PT_TOUCH_DEVICE");
    if( ! touch.empty() )
      openInputDevice(touch);

    std::string touchTransform = Pt::System::Application::getEnvVar("PT_TOUCH_TRANSFORM");
    if( ! touchTransform.empty() )
    {
      std::istringstream iss(touchTransform);
      float m11, m12, m21, m22, dx, dy;
      iss >> m11 >> m12 >> dx >> m21 >> m22 >> dy;

      _touchTransform.set(m11, m12, m21, m22, dx, dy);
    }
}


ApplicationImpl::~ApplicationImpl()
{
    std::vector<InputDevice*>::iterator it;
    for(it = _inputDevices.begin(); it != _inputDevices.end(); ++it)
    {
        delete *it;
    }

    delete _mouseDevice;

    showConsole(true);
} 


void ApplicationImpl::openInputDevice(const std::string& deviceName)
{
    try
    {
        InputDevice* device = new InputDevice(deviceName.c_str());
        device->setScreenLimit(_frameBuffer.size());
        device->setActive(*this);
        device->begin();
        device->eventReady() += Pt::slot(*this, &ApplicationImpl::onKeyEvent);
        device->eventReady() += Pt::slot(*this, &ApplicationImpl::onMouseEvent);
        device->eventReady() += Pt::slot(*this, &ApplicationImpl::onTouchEvent);

        _inputDevices.push_back(device);
        std::clog << "using input device: " << deviceName << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::clog << "skipping input device: " << deviceName<< std::endl;
    }
}


void ApplicationImpl::setCursor(const Cursor* cursor)
{
    if( &_cursor == cursor )
        return;

    if(cursor == 0)
        return;

    if( cursor->empty() )
    {
        _cursor.clear();
        return;
    }

    _cursor = *cursor;
}


void ApplicationImpl::setFontDir(const Pt::System::Path& dir)
{
    Gfx::ImageSurface::setFontDir(dir);
}


void ApplicationImpl::setDefaultFont(const std::string& fontName)
{
    Gfx::ImageSurface::setDefaultFont(fontName);
}


void ApplicationImpl::grabPointer(Window& grabber)
{
}


void ApplicationImpl::releasePointer(Window& grabber)
{
    // TODO: if mouse is not enabled unset pointer widget
    //Application::instance().setPointerWidget(0);

    // send mouse move event with current button state
    // so widget under the cursor gets an enter event 
    _lastMouse.setMove(); 
    _lastMouse.setId( Application::instance().screen().vid() );

    Application::instance().processMouseEvent(_lastMouse);
}


void ApplicationImpl::grabPointer(Widget& grabber)
{
}


void ApplicationImpl::releasePointer(Widget& grabber)
{
    // TODO: if mouse is not enabled unset pointer widget
    //Application::instance().setPointerWidget(0);

    // send mouse move event with current button state
    // so widget under the cursor gets an enter event 
    _lastMouse.setMove();
    _lastMouse.setId( Application::instance().screen().vid() );

    Application::instance().processMouseEvent(_lastMouse);
}


Pt::Timespan ApplicationImpl::inactivityTime() const
{
    Pt::DateTime now = Pt::System::Clock::getSystemTime();
    Pt::Timespan inactivity = now - _lastActivityTime;
    return inactivity;
}	


void ApplicationImpl::sendKeyEvent(const KeyEvent& ev)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();

    Application::instance().screen().impl()->dispatchKeyEvent(ev);
}


void ApplicationImpl::sendMouseEvent(const MouseEvent& ev)
{
    onMouseEvent(ev);
}


void ApplicationImpl::onMouseEvent(const MouseEvent& ev)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();
   
    MouseEvent mev = ev;
    mev.setId( Application::instance().screen().vid() );

    ScreenImpl* screen = Application::instance().screen().impl();
    screen->drawCursor(ev);
    
    double scaling = Application::instance().scaleFactor();

    Gfx::PointF pos(ev.position().x() / scaling, 
                    ev.position().y() / scaling);
    mev.setPosition(pos);

    _lastMouse = mev;

    // TODO: call Application::processMouseEvent which returns true if the
    //       event was consumed. If it returns false and the event was not
    //       consumed call ScreenImpl::dispatchMouseEvent

    Application::instance().processMouseEvent(mev);
}


void ApplicationImpl::onTouchEvent(const TouchEvent& ev)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();

    Pt::Gfx::PointF pos = _touchTransform * ev.position();

    double scaling = Application::instance().scaleFactor();
    Gfx::PointF scaledPos(pos.x() / scaling,pos.y() / scaling);

    TouchEvent tev = ev;
    tev.setId( Application::instance().screen().vid() );
    tev.setPosition(scaledPos);

    //std::clog << "raw touch: " << ev.position().x() << ", " 
    //                           << ev.position().y() << std::endl;
    //std::clog << "transformed touch: " << pos.x() << ", " 
    //                                   << pos.y() << std::endl;

    Application::instance().processTouchEvent(tev);
}


void ApplicationImpl::onKeyEvent(const KeyEvent& ev)
{
    //TODO: VID???
    _lastActivityTime = Pt::System::Clock::getSystemTime();

    Application::instance().screen().impl()->dispatchKeyEvent(ev);
}


void ApplicationImpl::nextEvent()
{
    MainLoop::waitNext();
}


void ApplicationImpl::showConsole(bool s)
{
    std::string terminal;
    std::ifstream ifs("/sys/class/tty/tty0/active");
    ifs >> terminal;
    terminal = "/dev/" + terminal;

    int fd = open(terminal.c_str(), O_RDWR);
    
    if( ! s )
          ioctl( fd, KDSETMODE, KD_GRAPHICS );
    else
          ioctl( fd, KDSETMODE, KD_TEXT );

    close( fd );
}

} // namespace

} // namespace
