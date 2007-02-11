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


namespace Pt {

namespace Gui {


KeyboardHandler::KeyboardHandler()
: _exit(false)
, _fd(-1)
{
    _fd = open("/dev/input/event0", O_RDONLY);
    if( _fd < 0 )
    {
        throw std::runtime_error("Could not open keyboard device /dev/input/eventX" + PT_SOURCEINFO);
    }
}


KeyboardHandler::~KeyboardHandler()
{
    if(_fd > 0)
        close(_fd);
}


void KeyboardHandler::run()
{
    struct input_event ev[64];

    // TODO: make non-blocking and use select
    while( !_exit )
    {
        int rd = read(_fd, ev, sizeof(struct input_event) * 64);

        if (rd < (int) sizeof(struct input_event))
        {
            continue;
        }

        for (unsigned i = 0; i < rd / sizeof(struct input_event); i++)
        {
            if (ev[i].type == EV_KEY)
            {
                 keyEvent.send(ev[i].code, ev[i].value);
            }
        }
    }
}




LfbEventLoop::LfbEventLoop()
{
    connect(_keyboard.keyEvent, *this, &LfbEventLoop::handleKeyEvent);
    _keyboard.start();
}


LfbEventLoop::~LfbEventLoop()
{
    _keyboard.stop();
    _keyboard.wait();
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
        case KEY_UP:        code = KeyEvent::Up; break;
        case KEY_LEFT:      code = KeyEvent::Left; break;
        case KEY_RIGHT:     code = KeyEvent::Right; break;
        case KEY_DOWN:      code = KeyEvent::Down; break;
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
    //if( ioctl(_fd, FBIOGET_FSCREENINFO, &_fixedInfo) < 0 )
    //    throw std::runtime_error("FBIOGET_FSCREENINFO failed" + PT_SOURCEINFO);

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

