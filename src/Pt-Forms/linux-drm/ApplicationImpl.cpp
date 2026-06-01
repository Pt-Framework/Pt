/* Copyright (C) 2026 Marc Boris Duerner

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

#include "ApplicationImpl.h"
#include "ScreenImpl.h"

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/Screen.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/System/Logger.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/Clock.h>
#include <Pt/DateTime.h>

#include <sstream>
#include <fstream>
#include <cmath>

#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <sys/vt.h>
#include <unistd.h>

PT_LOG_DEFINE("Pt.Forms.Application")

namespace Pt {

namespace Forms {

ApplicationImpl::ApplicationImpl()
: _drmDisplay(_drmDevice.fd())
, _consoleFd(-1)
, _consoleInGraphicsMode(false)
, _lastActivityTime( Pt::System::Clock::getSystemTime() )
{
    _drmDevice.setActive(*this);
    _drmDevice.begin();

#ifdef PT_FORMS_DRM_VULKAN
    _vulkanDevice.setDrmFd(_drmDevice.fd());
#endif

    std::string keyboard = Pt::System::Application::getEnvVar("PT_KEYBOARD_DEVICE");
    if( ! keyboard.empty() )
        openInputDevice(keyboard);

    std::string mouse = Pt::System::Application::getEnvVar("PT_MOUSE_DEVICE");
    if( ! mouse.empty() )
        openMouseDevice(mouse);

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

    showConsole(false);
}


ApplicationImpl::~ApplicationImpl()
{
    _mouseDevice.reset();
    _inputDevices.clear();

    _drmDisplay.shutdown();
    _drmDevice.shutdown();

    if( _consoleInGraphicsMode )
        showConsole(true);

    closeConsole();
}


void ApplicationImpl::openMouseDevice(const std::string& deviceName)
{
    try
    {
        _mouseDevice = std::make_unique<MouseDevice>(deviceName.c_str());
        _mouseDevice->setScreenLimit(_drmDisplay.width(), _drmDisplay.height());
        _mouseDevice->setActive(*this);
        _mouseDevice->begin();
        _mouseDevice->eventReady() += Pt::slot(*this, &ApplicationImpl::onMouseEvent);
        _mouseDevice->eventReady() += Pt::slot(*this, &ApplicationImpl::onScrollEvent);
        PT_LOG_INFO("using mouse: " << deviceName);
    }
    catch(const std::exception& ex)
    {
        _mouseDevice.reset();
        PT_LOG_WARN("skipping mouse device: " << deviceName);
    }
}


void ApplicationImpl::openInputDevice(const std::string& deviceName)
{
    try
    {
        auto device = std::make_unique<InputDevice>(deviceName.c_str());
        device->setScreenLimit( _drmDisplay.width(), _drmDisplay.height() );
        device->setActive(*this);
        device->begin();
        device->eventReady() += Pt::slot(*this, &ApplicationImpl::onKeyEvent);
        device->eventReady() += Pt::slot(*this, &ApplicationImpl::onMouseEvent);
        device->eventReady() += Pt::slot(*this, &ApplicationImpl::onTouchEvent);

        _inputDevices.push_back( std::move(device) );
        PT_LOG_INFO("using input device: " << deviceName);
    }
    catch(const std::exception& ex)
    {
        PT_LOG_WARN("skipping input device: " << deviceName);
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


Pt::Timespan ApplicationImpl::inactivityTime() const
{
    Pt::DateTime now = Pt::System::Clock::getSystemTime();
    Pt::Timespan inactivity = now - _lastActivityTime;
    return inactivity;
}


void ApplicationImpl::sendKeyEvent(const KeyEvent& ev)
{
    onKeyEvent(ev);
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
    screenImpl->setCursor( ev.position() );

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


bool ApplicationImpl::openConsole()
{
    if( _consoleFd >= 0 )
        return true;

    if( _consoleDevice.empty() )
    {
        std::ifstream ifs("/sys/class/tty/tty0/active");
        if( ! ifs )
        {
            PT_LOG_WARN("could not open /sys/class/tty/tty0/active");
            return false;
        }

        ifs >> _consoleDevice;

        if( _consoleDevice.empty() )
        {
            PT_LOG_WARN("could not determine active terminal");
            return false;
        }

        _consoleDevice = "/dev/" + _consoleDevice;
    }

    _consoleFd = ::open(_consoleDevice.c_str(), O_RDWR | O_CLOEXEC);
    if( _consoleFd < 0 )
    {
        PT_LOG_WARN("could not open terminal: " << _consoleDevice);
        return false;
    }

    return true;
}


void ApplicationImpl::closeConsole()
{
    if( _consoleFd >= 0 )
    {
        ::close(_consoleFd);
        _consoleFd = -1;
    }

    _consoleInGraphicsMode = false;
    _consoleDevice.clear();
}


void ApplicationImpl::showConsole(bool s)
{
    if( ! openConsole() )
        return;

    int mode = s ? KD_TEXT : KD_GRAPHICS;

    if( ioctl(_consoleFd, KDSETMODE, mode) < 0 )
    {
        PT_LOG_WARN("KDSETMODE failed on: " << _consoleDevice);
        return;
    }

    _consoleInGraphicsMode = ! s;
}

} // namespace

} // namespace
