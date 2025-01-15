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

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>
#include <Pt/DateTime.h>

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
        if(ev.type == SDL_WINDOWEVENT)
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
          SDL_MouseButtonEvent* mev = reinterpret_cast<SDL_MouseButtonEvent*>(&ev);

          double scaling = Application::instance().scaleFactor();

          Gfx::PointF pos(mev->x, mev->y);
          pos = pos / scaling;

          _mev.setVisual(&screen);
          _mev.setPosition(pos);
          
          if( ev.type == SDL_MOUSEBUTTONDOWN )
            _mev.setPress(MouseEvent::Left);
          else
            _mev.setRelease(MouseEvent::Left);

          Application::instance().processEvent(_mev);
        }
        else if(ev.type == SDL_MOUSEMOTION)
        {
          SDL_MouseMotionEvent* mev = reinterpret_cast<SDL_MouseMotionEvent*>(&ev);
          
          double scaling = Application::instance().scaleFactor();

          Gfx::PointF pos(mev->x, mev->y);
          pos = pos / scaling;

          _mev.setVisual(&screen);
          _mev.setPosition(pos);
          _mev.setMove();

          Application::instance().processEvent(_mev);
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
