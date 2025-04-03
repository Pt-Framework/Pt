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

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/Screen.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/System/Logger.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/Clock.h>
#include <Pt/DateTime.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>
#include <sys/kd.h>
#include <sys/vt.h>

PT_LOG_DEFINE("Pt.Forms.Application")

namespace Pt {

namespace Forms {

ApplicationImpl::ApplicationImpl()
: _mouseDevice(0)
, _lastActivityTime( Pt::System::Clock::getSystemTime() )
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
        _mouseDevice->setScreenLimit(_frameBuffer.width(), _frameBuffer.height());
        _mouseDevice->setActive(*this);
        _mouseDevice->begin();
        _mouseDevice->eventReady() += Pt::slot(*this, &ApplicationImpl::onMouseEvent);
        _mouseDevice->eventReady() += Pt::slot(*this, &ApplicationImpl::onScrollEvent);
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
        device->setScreenLimit( _frameBuffer.width(), _frameBuffer.height() );
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


Pt::Timespan ApplicationImpl::inactivityTime() const
{
    Pt::DateTime now = Pt::System::Clock::getSystemTime();
    Pt::Timespan inactivity = now - _lastActivityTime;
    return inactivity;
}	


void ApplicationImpl::sendKeyEvent(const KeyEvent& ev)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();

    //Application::instance().screen().impl()->dispatchKeyEvent(ev);
}


void ApplicationImpl::sendMouseEvent(const MouseEvent& ev)
{
    onMouseEvent(ev);
}


void ApplicationImpl::onMouseEvent(const MouseEvent& ev)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();
   
    Screen& screen = Application::instance().screen();
    ScreenImpl* screenImpl = screen.impl();
    screenImpl->drawCursor( ev.position() );
    
    double scaling = Application::instance().scaleFactor();
    Gfx::PointF pos = ev.position() / scaling;
    
    MouseEvent mev = ev;
    mev.setWidget(&screen);
    mev.setPosition(pos);

    Application::instance().processEvent(mev);
}


void ApplicationImpl::onScrollEvent(const ScrollEvent& ev)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();

    Screen& screen = Application::instance().screen();

    ScrollEvent sev = ev;
    sev.setWidget(&screen);

    Application::instance().processEvent(sev);
}


void ApplicationImpl::onTouchEvent(const TouchEvent& ev)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();

    Screen& screen = Application::instance().screen();

    double scaling = Application::instance().scaleFactor();
    Pt::Gfx::PointF pos = _touchTransform * ev.position();
    pos /= scaling;
    
    TouchEvent tev = ev;
    tev.setWidget(&screen);
    tev.setPosition(pos);

    Application::instance().processEvent(tev);
}


void ApplicationImpl::onKeyEvent(const KeyEvent& ev)
{
    //TODO: VID???
    _lastActivityTime = Pt::System::Clock::getSystemTime();

    Screen& screen = Application::instance().screen();

    KeyEvent kev = ev;
    kev.setWidget(&screen);
    Application::instance().processEvent(kev);
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

    //static struct termios tm_orig;

    if( ! s )
    {
        // NOTE: the code below did not work for serial consoles
        //       app output like logging was indented

        //struct termios tm;
        //tcgetattr(STDIN_FILENO, &tm);
        //tm_orig = tm;

        //tm.c_iflag = tm.c_oflag = 0;
        //tm.c_cflag &= ~CSIZE;
        //tm.c_cflag |= CS8;
        //tm.c_lflag &= ~(ECHO | ISIG | ICANON);
        //tm.c_cc[VMIN] = 1; /* min data size (byte) */
        //tm.c_cc[VTIME] = 0; /* time out */
        //tcsetattr(STDIN_FILENO, TCSAFLUSH, &tm);

        ioctl( fd, KDSETMODE, KD_GRAPHICS );
    }
    else
    {
        ioctl(fd, KDSETMODE, KD_TEXT);
        
        //tcsetattr(STDIN_FILENO, TCSAFLUSH, &tm_orig);
    }

    close(fd);
}

} // namespace

} // namespace
