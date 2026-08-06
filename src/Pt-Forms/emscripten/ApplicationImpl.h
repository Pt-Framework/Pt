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

#ifndef Pt_Forms_ApplicationImpl_h
#define Pt_Forms_ApplicationImpl_h

#include "posix/Selector.h"

#include <Pt/Forms/Widget.h>
#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/KeyEvent.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Condition.h>
#include <Pt/DateTime.h>
#include <Pt/Timespan.h>

#include <emscripten/html5.h>

#include <thread>

namespace Pt {

namespace Forms {

class GraphicsBackend;

class Selector : public System::Selector
{
    public:
        Selector()
        {
        }

        virtual ~Selector()
        {
        }

        virtual void cancel(System::IOHandle& h)
        {
        }

        // used by IONotifier
        virtual void beginWait(System::IOHandle* h, int flags)
        {
        }

        // used by IONotifier
        virtual int endWait(System::IOHandle* h)
        {
            return 0;
        }

        virtual void beginRead(System::IOHandle* h)
        {
        }

        virtual void endRead(System::IOHandle* h)
        {
        }

        virtual void beginWrite(System::IOHandle* h)
        {
        }

        virtual void endWrite(System::IOHandle* h)
        {
        }

        virtual bool isReadable(System::IOHandle* h)
        {
            return false;
        }

        virtual bool isWritable(System::IOHandle* h)
        {
            return false;
        }

        virtual bool isError(System::IOHandle* h)
        {
            return false;
        }

        // used by IONotifier
        virtual bool isReady(System::IOHandle* h)
        {
            return false;
        }
};

class ApplicationImpl : public Pt::System::EventLoop
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();

        void setCursor(const Cursor* cursor );

        Pt::Timespan inactivityTime() const;

        void sendKeyEvent(const KeyEvent& ev);

        void sendMouseEvent(const MouseEvent& ev);
    
        void nextEvent();

        GraphicsBackend* queryBackend()
        {
            return 0;
        }

        virtual System::Selector& selector()
        { 
          return _selector; 
        }

    protected:
        virtual void onAttachSelectable(System::Selectable&);

        virtual void onDetachSelectable(System::Selectable&);

        virtual void onCancel(System::Selectable& s);

        virtual void onReady(System::Selectable& s);

        virtual void onRun();

        virtual void onExit();

        virtual void onCommitEvent(const Pt::Event& event);

        virtual void onQueueEvent(const Pt::Event& event);

        virtual void onWake();

        virtual void onProcessEvents();

        virtual void onAttachTimer(System::Timer& timer);

        virtual void onDetachTimer(System::Timer& timer);

    private:
        // single wait/dispatch step, shared by onRun() and nextEvent()
        bool waitNext();

        // entry point of the real OS thread that runs the blocking event loop
        void run();

        void dispatchKeyEvent(const EmscriptenKeyboardEvent& e, bool press);

        void dispatchMouseEvent(const EmscriptenMouseEvent& e, int eventType);

        static EM_BOOL onKeyDown(int eventType, const EmscriptenKeyboardEvent* e, void* userData);

        static EM_BOOL onKeyUp(int eventType, const EmscriptenKeyboardEvent* e, void* userData);

        static EM_BOOL onMouseEvent(int eventType, const EmscriptenMouseEvent* e, void* userData);

    private:
        Selector               _selector;
        System::EventQueue     _eventQueue;
        System::Mutex          _wakeMutex;
        System::Condition      _wakeCondition;
        System::TimerQueue     _timerQueue;
        bool                   _exiting;
        Pt::DateTime           _lastActivityTime;
        MouseEvent             _mev;
        KeyEvent               _keyEvent;
};

} // namespace

} // namespace

#endif

