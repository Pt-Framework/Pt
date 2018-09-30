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
#include "PainterImpl.h"
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/System/FileInfo.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>
#include <sys/kd.h>

namespace Pt {

namespace Hmi {

ApplicationImpl::ApplicationImpl()
: _lastMouse(0)
{           
    showConsole(false);
    
    _inputDevices.reserve(10);

    for(size_t i = 0; i < 10; ++i)
    {
        System::Path deviceName("/dev/input/event");

        std::ostringstream oss;
        oss << i;
        deviceName += oss.str().c_str();
            
        if( Pt::System::FileInfo::exists(deviceName) )
        {
            InputDevice* device = new InputDevice( deviceName.toLocal().c_str() );
            device->setScreenLimit( _frameBuffer.size() );
            device->setActive(*this);
            device->begin();
            device->eventReady() += Pt::slot(*this, &ApplicationImpl::onMouseEvent);
            device->eventReady() += Pt::slot(*this, &ApplicationImpl::onTouchEvent);

            _inputDevices.push_back(device);
            std::clog << "using: " << deviceName.toLocal() << std::endl;
        }
    }
}


ApplicationImpl::~ApplicationImpl()
{
    std::vector<InputDevice*>::iterator it;
    for(it = _inputDevices.begin(); it != _inputDevices.end(); ++it)
    {
        delete *it;
    }

    showConsole(true);
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
    PainterImpl::setFontDir(dir);
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


void ApplicationImpl::sendKeyEvent(const KeyEvent& ev)
{
    Application::instance().screen().impl()->dispatchKeyEvent(ev);
}


void ApplicationImpl::sendMouseEvent(const MouseEvent& ev)
{
    onMouseEvent(ev);
}


void ApplicationImpl::onMouseEvent(const MouseEvent& ev)
{
    MouseEvent mev = ev;
    mev.setId( Application::instance().screen().vid() );

    ScreenImpl* screen = Application::instance().screen().impl();
    
    Gfx::PointF pos = screen->screenPosition( ev.position() );
    mev.setPosition(pos);

    screen->drawCursor(mev);

    _lastMouse = mev;

    // TODO: call Application::processMouseEvent which returns true if the
    //       event was consumed. If it returns false and the event was not
    //       consumed call ApplicationImpl::dispatchMouseEvent

    Application::instance().processMouseEvent(mev);
}


void ApplicationImpl::onTouchEvent(const TouchEvent& ev)
{
    TouchEvent tev = ev;
    tev.setId( Application::instance().screen().vid() );

    ScreenImpl* screen = Application::instance().screen().impl();

    Gfx::PointF pos = screen->screenPosition( ev.position() );
    tev.setPosition(pos);

    Application::instance().processTouchEvent(tev);
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
