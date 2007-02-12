/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <linux/input.h>

#include "ApplicationImpl.h"
#include <Pt/System/MutexLock.h>
#include <iostream>
#include <cerrno>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)


namespace Pt {

namespace Gui {

InputHandler::InputHandler()
: _exit(false)
, _highestFd(0)
, _fd1(-1)
, _fd2(-1)
, _fd3(-1)
{
    FD_ZERO(&_fds);

    _fd1 = open("/dev/input/event0", O_RDONLY|O_NONBLOCK);
    if( _fd1 >= 0 )
    {
        _highestFd = std::max(_highestFd, _fd1);
    }

    _fd2 = open("/dev/input/event1", O_RDONLY|O_NONBLOCK);
    if( _fd2 >= 0 )
    {
        _highestFd = std::max(_highestFd, _fd2);
    }

    _fd3 = open("/dev/input/event2", O_RDONLY|O_NONBLOCK);
    if( _fd3 >= 0 )
    {
        _highestFd = std::max(_highestFd, _fd3);
    }

    if(_highestFd == 0)
        throw std::runtime_error("Could not open device /dev/input/eventX" + PT_SOURCEINFO);
}


InputHandler::~InputHandler()
{
    if(_fd1 > 0)
        close(_fd1);

    if(_fd2 > 0)
        close(_fd2);

    if(_fd3 > 0)
        close(_fd3);
}


void InputHandler::run()
{
    struct input_event ev[64];
    const int msec = 200;

    while( !_exit )
    {
        int bytes = read(_fd1, ev, sizeof(struct input_event) * 64);
        this->handleEvents(ev, bytes);

        bytes = read(_fd2, ev, sizeof(struct input_event) * 64);
        this->handleEvents(ev, bytes);

        bytes = read(_fd3, ev, sizeof(struct input_event) * 64);
        this->handleEvents(ev, bytes);

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(_fd1, &rfds);
        FD_SET(_fd2, &rfds);
        FD_SET(_fd3, &rfds);

        struct timeval tv;
        tv.tv_sec = msec / 1000;
        tv.tv_usec = (msec % 1000) * 1000;

        int ret = ::select(_highestFd + 1, &rfds, 0, 0, &tv);

        if(ret == 0)
            continue;

        if(ret == -1)
        {
            if(errno == EINTR)
                continue;

            throw std::runtime_error("Could not select on socket" + PT_SOURCEINFO);
        }

        if( FD_ISSET(_fd1, &_fds) )
            bytes = read(_fd1, ev, sizeof(struct input_event) * 64);

        if( FD_ISSET(_fd2, &_fds) )
            bytes = read(_fd2, ev, sizeof(struct input_event) * 64);

        if( FD_ISSET(_fd3, &_fds) )
            bytes = read(_fd3, ev, sizeof(struct input_event) * 64);

        this->handleEvents(ev, bytes);
    }
}


void InputHandler::handleEvents(input_event* events, int bytes)
{
    if( bytes < (int) sizeof(struct input_event) )
    {
        return;
    }

    for( unsigned i = 0; i < bytes / sizeof(input_event); i++ )
    {
        if (events[i].type == EV_KEY)
        {
            keyEvent.send( events[i].code, events[i].value );
        }
    }
}




LfbEventLoop::LfbEventLoop()
{
    connect(_input.keyEvent, *this, &LfbEventLoop::handleKeyEvent);
    _input.start();
}


LfbEventLoop::~LfbEventLoop()
{
    _input.stop();
    _input.wait();
}


void LfbEventLoop::registerWidget(Widget& widget)
{
    Pt::System::MutexLock lock(_mutex);
    _widgets.push_back(&widget);
}


void LfbEventLoop::unregisterWidget(Widget& widget)
{
    Pt::System::MutexLock lock(_mutex);
    _widgets.remove(&widget);
}


void LfbEventLoop::handleKeyEvent(int keycode, int value)
{
    Pt::System::MutexLock lock(_mutex);
    if( _widgets.empty() )
        return;

    Widget* widget = _widgets.front();

    KeyEvent::Type type = KeyEvent::Press;
    if(value == 0)
        type = KeyEvent::Release;

    KeyEvent::KeyCode code = KeyEvent::Void;
    switch( keycode ) {
        case KEY_UP:         code = KeyEvent::Up; break;
        case KEY_LEFT:       code = KeyEvent::Left; break;
        case KEY_RIGHT:      code = KeyEvent::Right; break;
        case KEY_DOWN:       code = KeyEvent::Down; break;
        case KEY_ESC:        code = KeyEvent::Escape; break;
        case KEY_PAGEDOWN:   code = KeyEvent::PageDown; break;
        case KEY_PAGEUP:     code = KeyEvent::PageUp; break;
        case KEY_SPACE:      code = KeyEvent::Space; break;
        case KEY_ENTER:      code = KeyEvent::Enter; break;
        case KEY_LEFTSHIFT:  code = KeyEvent::ShiftL; break;
        case KEY_RIGHTSHIFT: code = KeyEvent::ShiftR; break;
        case KEY_LEFTALT:    code = KeyEvent::AltL; break;
        case KEY_RIGHTALT:   code = KeyEvent::AltR; break;
    }

    KeyEvent kev(*widget, type, code, 0);
    this->commitEvent(kev);
}




Screen::Screen()
: _fd(-1)
, _buffer(0)
, _bufferSize(0)
{
    // Open the frame buffer device
    _fd = open ("/dev/fb0", O_RDWR);
    if(_fd < 0)
        throw std::runtime_error("Could not open framebuffer device" + PT_SOURCEINFO);

/*
    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

    _screenInfo.bits_per_pixel = 16;
    _screenInfo.xres           = 640;
    _screenInfo.yres           = 480;

    if( 0 > ioctl(_fd, FBIOPUT_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOPUT_VSCREENINFO failed" + PT_SOURCEINFO);
*/

    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

    // Get the fixed state
    if( ioctl(_fd, FBIOGET_FSCREENINFO, &_fixedInfo) < 0 )
        throw std::runtime_error("FBIOGET_FSCREENINFO failed" + PT_SOURCEINFO);

    //_fixedInfo.type;   // 0 -> Packed pixels
                         // 1 -> Non interleaved planes
                         // 2 -> Interleaved planes
                         // 3 -> Text/attributes
                         // 4 -> EGA/VGA planes

    //_fixedInfo.visual; // 0 -> Mono (1=black, 0=white)
                         // 1 -> Mono (1=white, 0=black)
                         // 2 -> True color
                         // 3 -> Pseudo color (like atari)
                         // 4 -> Direct color
                         // 5 -> Pseudo color readonly

    // Memory map the display
    unsigned _pitch = _screenInfo.xres * _screenInfo.bits_per_pixel / 8;
    _bufferSize     = _pitch * _screenInfo.yres;
    _buffer         =  mmap(NULL, _bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
}


Screen::~Screen()
{
    LfbEventLoop::instance().exit();

    if(_buffer)
        munmap(_buffer, _bufferSize);

    if(_fd > 0)
        close(_fd);
}



ApplicationImpl::ApplicationImpl(Application& app)
{
    Screen::instance();
    connect(LfbEventLoop::instance().event, app.event);
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::commitEvent(const Pt::Event& event)
{
    LfbEventLoop::instance().commitEvent(event);
}


void ApplicationImpl::queueEvent(const Pt::Event& event)
{
    LfbEventLoop::instance().queueEvent(event);
}


void ApplicationImpl::processEvents()
{
    LfbEventLoop::instance().processEvents();
}


int ApplicationImpl::run()
{
    return LfbEventLoop::instance().run();
}


void ApplicationImpl::wake()
{
    LfbEventLoop::instance().wake();
}


void ApplicationImpl::exit()
{
    LfbEventLoop::instance().exit();
}

} // namespace Gui

} // namespace Pt

