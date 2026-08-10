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

#include <Pt/Forms/Application.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>
#include <Pt/DateTime.h>
#include <Pt/Utf8Codec.h>

#include <emscripten.h>
#include <emscripten/threading.h>
#include <pthread.h>

PT_LOG_DEFINE("Pt.Forms.Application")

namespace Pt {

namespace Forms {

ApplicationImpl::ApplicationImpl()
: _exiting(false)
, _wakePending(false)
, _lastActivityTime( Pt::System::Clock::getSystemTime() )
{
    // registered with an explicit target thread: under -sPROXY_TO_PTHREAD=1 the
    // implicit EM_CALLBACK_THREAD_CONTEXT_CALLING_THREAD auto-detection does not
    // reliably back-proxy DOM events to this worker (events are silently handled
    // on the browser main thread instead, where this module's code is not loaded)
    pthread_t self = pthread_self();

    emscripten_set_keydown_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &ApplicationImpl::onKeyDown, self);
    emscripten_set_keyup_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &ApplicationImpl::onKeyUp, self);

    emscripten_set_mousedown_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &ApplicationImpl::onMouseEvent, self);
    emscripten_set_mouseup_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &ApplicationImpl::onMouseEvent, self);
    emscripten_set_mousemove_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &ApplicationImpl::onMouseEvent, self);
}


ApplicationImpl::~ApplicationImpl()
{
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, 0);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, 0);

    emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, 0);
    emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, 0);
    emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, 0);
}


void ApplicationImpl::setCursor(const Cursor* cursor)
{
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
    this->waitNext();
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


bool ApplicationImpl::waitNext()
{
    std::size_t timeout = _timerQueue.processTimers();

    // never wait indefinitely: a worker thread blocked in pthread_cond_wait
    // cannot service browser input events proxied to it under -sPROXY_TO_PTHREAD=1
    if(timeout == System::EventLoop::WaitInfinite)
        timeout = 16;

    emscripten_current_thread_process_queued_calls();

    {
        System::MutexLock lock(_wakeMutex);

        // a proxied callback above may have already called wake() for this tick;
        // only block if that didn't happen, else the signal would be missed
        if( ! _wakePending)
            _wakeCondition.wait( _wakeMutex, static_cast<unsigned int>(timeout) );

        _wakePending = false;
    }

    this->processEvents();

    return ! _exiting;
}


void ApplicationImpl::onRun()
{
    while( this->waitNext() )
        ;
}


void ApplicationImpl::run()
{
    this->onRun();
}



void ApplicationImpl::onExit()
{
    _exiting = true;

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
    System::MutexLock lock(_wakeMutex);
    _wakePending = true;
    _wakeCondition.signal();
}


void ApplicationImpl::onProcessEvents()
{
    _eventQueue.processEvents( this->eventReceived() );

    ScreenImpl* screen = Application::instance().screen().impl();
    if( screen && screen->commitPending() )
        screen->commitFrame();
}


void ApplicationImpl::dispatchKeyEvent(const EmscriptenKeyboardEvent& e, bool press)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();

    Screen& screen = Application::instance().screen();

    Key::Modifiers modifiers;

    if(e.shiftKey)
        modifiers.add(Key::Shift);

    if(e.ctrlKey)
        modifiers.add(Key::Control);

    if(e.altKey)
        modifiers.add(Key::Alt);

    if(e.metaKey)
        modifiers.add(Key::Meta);

    Key key( modifiers, toKeycode(e.code) );

    if(press)
        _keyEvent.setPress( key, Pt::Char() );
    else
        _keyEvent.setRelease( key, Pt::Char() );

    _keyEvent.setWidget(&screen);
    this->commitEvent(_keyEvent);

    //
    // NOTE: produces a fake key event with a character since keydown
    //       does not report composed characters
    //
    if(press)
    {
        Pt::String text = Pt::Utf8Codec::decode( e.key, std::strlen(e.key) );
        if(text.size() == 1)
        {
            Key textKey;

            KeyEvent textEvent(screen);
            textEvent.setPress(textKey, text[0]);
            this->commitEvent(textEvent);
        }
    }
}


void ApplicationImpl::dispatchMouseEvent(const EmscriptenMouseEvent& e, int eventType)
{
    _lastActivityTime = Pt::System::Clock::getSystemTime();

    Screen& screen = Application::instance().screen();
    double scaling = Application::instance().scaleFactor();

    Gfx::PointF pos(e.targetX, e.targetY);
    pos = pos / scaling;

    _mev.setWidget(&screen);
    _mev.setPosition(pos);

    if(eventType == EMSCRIPTEN_EVENT_MOUSEMOVE)
    {
        _mev.setMove();
    }
    else
    {
        MouseEvent::Button button = e.button == 0 ? MouseEvent::Left :
                                    e.button == 2 ? MouseEvent::Right :
                                    MouseEvent::Middle;

        if(eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
            _mev.setPress(button);
        else
            _mev.setRelease(button);
    }

    this->commitEvent(_mev);
}


EM_BOOL ApplicationImpl::onKeyDown(int /*eventType*/, const EmscriptenKeyboardEvent* e, void* userData)
{
    ApplicationImpl* app = static_cast<ApplicationImpl*>(userData);
    app->dispatchKeyEvent(*e, true);
    return true;
}


EM_BOOL ApplicationImpl::onKeyUp(int /*eventType*/, const EmscriptenKeyboardEvent* e, void* userData)
{
    ApplicationImpl* app = static_cast<ApplicationImpl*>(userData);
    app->dispatchKeyEvent(*e, false);
    return true;
}


EM_BOOL ApplicationImpl::onMouseEvent(int eventType, const EmscriptenMouseEvent* e, void* userData)
{
    ApplicationImpl* app = static_cast<ApplicationImpl*>(userData);
    app->dispatchMouseEvent(*e, eventType);
    return true;
}


void ApplicationImpl::onAttachTimer(System::Timer& timer)
{
    _timerQueue.addTimer(timer);
    wake();
}


void ApplicationImpl::onDetachTimer(System::Timer& timer)
{
    _timerQueue.removeTimer(timer);
    wake();
}

} // namespace

} // namespace
