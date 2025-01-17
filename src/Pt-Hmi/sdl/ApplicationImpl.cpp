/* Copyright (C) 2015-2025 Marc Boris Duerner

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
#include "Keycodes.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>
#include <Pt/DateTime.h>
#include <Pt/Utf8Codec.h>

#include <SDL.h>
#include <emscripten.h>

PT_LOG_DEFINE("Pt.Hmi.Application")

namespace Pt {

namespace Hmi {

ApplicationImpl::ApplicationImpl()
: _lastActivityTime( Pt::System::Clock::getSystemTime() )
{
    SDL_Init(SDL_INIT_VIDEO);
}


ApplicationImpl::~ApplicationImpl()
{
    SDL_Quit();
} 


void ApplicationImpl::setCursor(const Cursor* cursor)
{
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
}


void ApplicationImpl::sendMouseEvent(const MouseEvent& ev)
{
}


void ApplicationImpl::nextEvent()
{
    //MainLoop::waitNext();
}


void ApplicationImpl::onAttachSelectable(System::Selectable&)
{
}


void ApplicationImpl::onDetachSelectable(System::Selectable&)
{
}


void ApplicationImpl::onCancel(System::Selectable& s)
{
}


void ApplicationImpl::onReady(System::Selectable& s)
{
}


void ApplicationImpl::mainLoop(void* arg)
{
    std::clog << emscripten_get_now() << std::endl;
    
    ApplicationImpl* app = static_cast<ApplicationImpl*>(arg); 
    app->processEvents();
}


void ApplicationImpl::onRun()
{
    emscripten_set_main_loop_arg(&ApplicationImpl::mainLoop, this, 0, true);
}


void ApplicationImpl::onExit()
{
    _eventQueue.exit();
    wake();

    std::clog << "emscripten_cancel_main_loop" << std::endl;
    emscripten_cancel_main_loop();
}


void ApplicationImpl::onCommitEvent(const Pt::Event& ev)
{
    _eventQueue.pushEvent(ev); 
    wake();
}


void ApplicationImpl::onQueueEvent(const Pt::Event& ev)
{
    _eventQueue.pushEvent(ev); 
}


void ApplicationImpl::onWake()
{
}


void ApplicationImpl::onProcessEvents()
{
    Screen& screen = Application::instance().screen();

    SDL_Event ev;
    while( SDL_PollEvent(&ev) > 0 )
    {
        if(ev.type == SDL_TEXTINPUT)
        {
            //
            // NOTE: produces a fake key event with a character since SDL
            //       does not report characters in key events
            //

            Pt::String text = Pt::Utf8Codec::decode(ev.text.text, 
                                                    std::strlen(ev.text.text));
            if( ! text.empty() )
            {
                Key key;

                KeyEvent keyEvent(screen);
                keyEvent.setPress(key, text[0]);
                Application::instance().processEvent(keyEvent);
            }
        }
        else if(ev.type == SDL_WINDOWEVENT)
        {
            if(ev.window.event == SDL_WINDOWEVENT_RESIZED ||
                ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
            {
                Gfx::SizeF to(ev.window.data1, ev.window.data2);
                to /= screen.scaleFactor();

                ResizeEvent rev(screen, to);
                Application::instance().processEvent(rev);

                Gfx::RectF updateRect(Gfx::PointF(0, 0), to);
                screen.repaint(updateRect);
            }
        }
        else if(ev.type == SDL_MOUSEBUTTONUP || ev.type == SDL_MOUSEBUTTONDOWN)
        {
            _lastActivityTime = Pt::System::Clock::getSystemTime();
          
            double scaling = Application::instance().scaleFactor();

            Gfx::PointF pos(ev.button.x, ev.button.y);
            pos = pos / scaling;

            _mev.setVisual(&screen);
            _mev.setPosition(pos);
          
            int buttonIdx = ev.button.button;
            MouseEvent::Button button = buttonIdx == 1 ? MouseEvent::Left :
                                        buttonIdx == 3 ? MouseEvent::Right :
                                        MouseEvent::Middle;
          
            if( ev.type == SDL_MOUSEBUTTONDOWN )
              _mev.setPress(button);
            else
              _mev.setRelease(button);

          Application::instance().processEvent(_mev);
        }
        else if(ev.type == SDL_MOUSEMOTION)
        {
            _lastActivityTime = Pt::System::Clock::getSystemTime();
          
            double scaling = Application::instance().scaleFactor();

            Gfx::PointF pos(ev.motion.x, ev.motion.y);
            pos = pos / scaling;

            _mev.setVisual(&screen);
            _mev.setPosition(pos);
            _mev.setMove();

            Application::instance().processEvent(_mev);
        }
        else if(ev.type == SDL_KEYUP  || ev.type == SDL_KEYDOWN )
        {
            _lastActivityTime = Pt::System::Clock::getSystemTime();

            Pt::uint32_t keyCode = toKeycode(ev.key.keysym.sym);
          
            Pt::Char ch;
            Key::Modifiers modifiers;

            if(ev.key.keysym.mod & KMOD_LSHIFT)
                modifiers.add(Key::Shift);
          
            if(ev.key.keysym.mod & KMOD_RSHIFT)
                modifiers.add(Key::Shift);

            if(ev.key.keysym.mod & KMOD_LCTRL )
                modifiers.add(Key::Control);

            if(ev.key.keysym.mod & KMOD_RCTRL )
                modifiers.add(Key::Control);

            if(ev.key.keysym.mod & KMOD_LALT  )
                modifiers.add(Key::Alt);

            if(ev.key.keysym.mod & KMOD_RALT  )
                modifiers.add(Key::Alt);

            if(ev.key.keysym.mod & KMOD_LGUI )
                modifiers.add(Key::Meta);

            if(ev.key.keysym.mod & KMOD_RGUI )
                modifiers.add(Key::Meta);

            Key key(modifiers, keyCode);

            if( ev.type == SDL_KEYDOWN )
                _keyEvent.setPress(key, ch);
            else
                _keyEvent.setRelease(key, ch);

            _keyEvent.setVisual(&screen);

            Application::instance().processEvent(_keyEvent);
        }
    }

    _eventQueue.processEvents( this->eventReceived() );
}


void ApplicationImpl::onAttachTimer(System::Timer& timer)
{
}


void ApplicationImpl::onDetachTimer(System::Timer& timer)
{
}

} // namespace

} // namespace
