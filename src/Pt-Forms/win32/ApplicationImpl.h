/* Copyright (C) 2013 Marc Boris Duerner
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan
   
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

#include "win32/Selector.h"

#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/KeyEvent.h>
#include <Pt/Forms/Cursor.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Timespan.h>

#include <vector>

#include <windows.h>

namespace Pt {

namespace Forms {

class Window;
class Visual;

class Selector : public System::Selector
{
  public:
      Selector();
        
      ~Selector();

  protected:
      virtual DWORD waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout);
};

class ApplicationImpl : public Pt::System::EventLoop
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();

        Window* findWindow(HWND h);

        void setCursor(const Cursor* cursor);

        void setFontDir(const Pt::System::Path& dir);

        void setDefaultFont(const std::string& fontName);

        Pt::Timespan inactivityTime() const;

        void sendKeyEvent(const KeyEvent& ev);

        void sendMouseEvent(const MouseEvent& ev);

        void nextEvent();

    public:
        virtual Pt::System::Selector& selector()
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
        bool waitNext();

        bool processMessage(HWND hwnd, UINT msg, 
                            WPARAM wparam, LPARAM lparam);
  
        static long CALLBACK wndProc(HWND hwnd, UINT message, 
                                     WPARAM wparam, LPARAM lparam);

        void onPaint(Window& w, HWND hwnd);
        
        void onResize(Window& w, WPARAM wparam, LPARAM lparam);

        void onMouse(Window& w, unsigned int msg,  WPARAM wparam, LPARAM lparam);
        
        void onKey(Window& w, UINT vkey, UINT scanCode, bool isPress);
        
        void onMove(Window& w, HWND hwnd, LPARAM lparam);
 
        bool onClose(Window& w);
        
        void onActivate(Window& w, bool f);
        
        void onEnable(Window& w, bool e);
        
        void onShow(Window& w, bool s);

    private:
        System::Mutex                    _mutex;
        System::TimerQueue               _timerQueue;
        System::EventQueue               _eventQueue;
        Pt::Forms::Selector                _selector;
        std::vector<System::Selectable*> _avail;
        HINSTANCE                        _instanceHandle;
        ULONG_PTR                        _gdiplusToken;
        MouseEvent                       _mouseEvent;
        KeyEvent                         _keyEvent;
        HWND                             _pointerWindow;
        HCURSOR                          _defaultCursorHandle;
        HCURSOR                          _cursorHandle;
        const Cursor*                    _currentCursor;
};

} // namespace

} // namespace

#endif
