/* Copyright (C) 2013 Marc Boris Dürner
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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, 
   Boston, MA 02110-1301 USA
*/

#include "ApplicationImpl.h"
#include "MainWindowImpl.h"
#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/System/IOError.h>
#include <WindowsX.h>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////////////
// Selector
/////////////////////////////////////////////////////////////////////////////

Selector::Selector()
{
}

Selector::~Selector()
{
}

DWORD Selector::waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout)
{	
    DWORD result = MsgWaitForMultipleObjects(numHandles, (HANDLE *)handles, false, msecs, QS_ALLEVENTS);

    if(result == WAIT_FAILED)
        throw Pt::System::IOError( PT_ERROR_MSG("WaitForMultipleObjects failed") );

    if( result == WAIT_TIMEOUT)
    {
        isTimeout = true;
        return 0;
    }

    DWORD offset = result - WAIT_OBJECT_0;

    if(offset == numHandles)
    {
	    MSG msg;

	    while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
	    {
			    TranslateMessage(&msg);
			    DispatchMessage(&msg);
	    }	
    }

    return offset;
}

/////////////////////////////////////////////////////////////////////////////
// ApplicationImpl
/////////////////////////////////////////////////////////////////////////////

ApplicationImpl::ApplicationImpl()
: Pt::System::EventLoop()
, _mouseEvent(0)
, _keyEvent(0)
{
#ifndef _DEBUG  
	FreeConsole();
#endif

	_instanceHandle = (HINSTANCE)GetModuleHandle(NULL);

	registerWindowClasses();
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::registerWindowClasses()
{
	std::string topLevelWindow = "Pt-Hmi";

	WNDCLASS topWindowClass;

	topWindowClass.style         = CS_HREDRAW | CS_VREDRAW;
	topWindowClass.lpfnWndProc   = (WNDPROC)ApplicationImpl::wndProc;
	topWindowClass.cbClsExtra    = 0;
	topWindowClass.cbWndExtra    = 0;
	topWindowClass.hInstance     = _instanceHandle;
	topWindowClass.hIcon         = NULL;
	topWindowClass.hCursor       = NULL;
	topWindowClass.hbrBackground = NULL;
	topWindowClass.lpszMenuName  = NULL;
	topWindowClass.lpszClassName = topLevelWindow.c_str();

	RegisterClass(&topWindowClass);
}


void ApplicationImpl::unregisterWindowClasses()
{
    UnregisterClass("Pt-Hmi", _instanceHandle);
}


long CALLBACK ApplicationImpl::wndProc(HWND hwnd, unsigned int msg, unsigned int wparam, long lparam)
{
    Pt::Hmi::Application& app = Pt::Hmi::Application::instance();
    app.impl()->processMessage(hwnd, msg, wparam, lparam);

    return DefWindowProc(hwnd, msg, wparam, lparam);
}


void ApplicationImpl::nextEvent()
{
	  waitNext();
}


void ApplicationImpl::onAttachSelectable(System::Selectable& s)
{ 
    _selector.attach(s); 
}


void ApplicationImpl::onDetachSelectable(System::Selectable& s)
{ 
    _selector.detach(s); 
}


void ApplicationImpl::onCancel(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);

    std::vector<System::Selectable*>::iterator it = _avail.begin();
    while(it != _avail.end())
    {
        if(*it == &s)
            it = _avail.erase(it);
        else
            ++it;
    }
}


void ApplicationImpl::onReady(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);
    _avail.push_back(&s);
}


void ApplicationImpl::onRun()
{
    while( this->waitNext() )
        ;
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
    _selector.wake(); 
}


void ApplicationImpl::onProcessEvents()
{
    _eventQueue.processEvents( this->eventReceived() );
}


void ApplicationImpl::onAttachTimer(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
}


void ApplicationImpl::onDetachTimer(System::Timer& timer )
{ 
    _timerQueue.removeTimer(timer); 
}


bool ApplicationImpl::waitNext()
{
    size_t timeout = _timerQueue.processTimers();

    // check all selectables that did not require waiting
    while( true )
    {
        Pt::System::MutexLock lock(_mutex);

        if( _avail.empty() )
            break;

        timeout = 0;
        System::Selectable* s = _avail.back();
        _avail.pop_back();
        lock.unlock();

        s->run();
    }

    bool isActive = true;
    
    if( _selector.waitForWake(timeout) )
        isActive = _eventQueue.processEvents( this->eventReceived() );

    return isActive;
}


Window* ApplicationImpl::findWindow(HWND hwnd)
{
    std::vector<Window*>& windows = Application::instance().screen().windows();

    for(size_t i = 0; i < windows.size(); ++i)
    {
        Window* w = windows[i];
        MainWindowImpl* impl = static_cast<MainWindowImpl*>( w->impl() );

        if( impl->hwnd() == hwnd )
            return w;
    }
    
    return 0;
}


bool ApplicationImpl::processMessage(HWND hwnd, unsigned int msg, 
                                     WPARAM wparam, LPARAM lparam)
{
    bool handled = false;

    Window* w = findWindow(hwnd);
    if( ! w)
        return handled;

    switch(msg)
    {
        case WM_MOUSEMOVE:
        {
          TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
          TrackMouseEvent(&tme);              
          onMouse(*w, msg, wparam, lparam);
          handled = true;
          break;
        }
        
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        {          
           onMouse(*w, msg, wparam, lparam);
           handled = true;
           break;
        }
        
        case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);

            ScrollEvent sev; //( w->vid() );
            sev.set(0, delta/120.0);

            commitEvent(sev);
            handled = true;
            break;
        }
        
        case WM_KEYDOWN:
        case WM_KEYUP:    
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:    
        {
            UINT vkey = wparam;
            UINT scanCode = ((lparam >> 16) & 0xFF);
            bool isPress = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            //UINT repeatCount = lparam & 0xFFFF;

            onKey(*w, vkey, scanCode, isPress);
            handled = true;
            break;
        }

        case WM_SHOWWINDOW:
        {
            onShow(*w, wparam != 0);
            handled = true;
            break;
        }
        
        case WM_PAINT:
        {
            onPaint(*w, hwnd);
            handled = true;
            break;
        }
        
        case WM_SIZE:
        {
            onResize(*w, wparam, lparam);
            handled = true;
            break;
        }

        case WM_MOVE:
        {
            onMove(*w, hwnd, lparam);
            handled = true;
            break;
        }

        case WM_DESTROY:
        {            
            handled = true;
            break;
        }

        case WM_CLOSE:
        {
            onClose(*w);
            handled = true;
            break;
        }

        case WM_ACTIVATE:
        {
            onActivate(*w, wparam != 0);
            handled = true;
            break;
        }
                
        case WM_ENABLE:
            onEnable(*w, wparam != 0);
            break;

        case WM_KILLFOCUS:
        {
            onActivate(*w,  false);
            handled = true;
            break;
        }        

        case WM_GETMINMAXINFO:
        {
            double minWidth = w->minimumSize().width();
            double minHeight = w->minimumSize().height();
            double maxWidth = w->maximumSize().width();
            double maxHeight = w->maximumSize().height();

            POINT min = { static_cast<LONG>(minWidth), 
                          static_cast<LONG>(minHeight) };
            POINT max = { static_cast<LONG>(maxWidth), 
                          static_cast<LONG>(maxHeight) };

            MINMAXINFO* mmi = (MINMAXINFO*)lparam;
            mmi->ptMaxTrackSize = max; 
            mmi->ptMinTrackSize = min;
            handled = true;  
            break;
        }

        case WM_MOUSELEAVE:
            handled = true;  
            _mouseEvent.clear();
            Application::instance().screen().setCursor(0);
            Application::instance().screen().setPointerWindow(0);
            break;        
    }

    return handled;
}


void ApplicationImpl::onShow(Window& w,  bool v)
{
    ShowEvent sev(w.vid(), v);
    commitEvent( sev );

    w.update();
}


void ApplicationImpl::onClose(Window& w)
{
    CloseEvent ev( w.vid() );
    commitEvent(ev);
}


void ApplicationImpl::onActivate(Window& w, bool a)
{
    ActivateEvent aev( w.vid(), a );
    commitEvent(aev);

    w.update();
}


void ApplicationImpl::onEnable(Window& w, bool e)
{
    EnableEvent eev( w.vid(), e );
    commitEvent( eev );

    w.update();
}


void ApplicationImpl::onKey(Window& w, UINT vkey, UINT scanCode, bool isPress)
{
    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);

    wchar_t wc = 0;
    ToUnicode(vkey, scanCode, (BYTE*)keyboardState, &wc, 1, 0);    

    bool shift = (keyboardState[VK_SHIFT] & 0x80) == 0x80;
    bool control = (keyboardState[VK_CONTROL] & 0x80) == 0x80;
    bool alt = (keyboardState[VK_MENU] & 0x80) == 0x80;
    bool rwin = (keyboardState[VK_RWIN] & 0x80) == 0x80;
    bool lwin = (keyboardState[VK_LWIN] & 0x80) == 0x80;

    Key::Modifiers modifiers;
    if(shift)
        modifiers |= Key::Shift;

    if(control)
        modifiers |= Key::Control;

    if(alt)
        modifiers |= Key::Alt;

    if(rwin || lwin)
        modifiers |= Key::Meta;

    Key::Code keyCode = static_cast<Key::Code>(vkey);
    Key key(modifiers, keyCode);

    if(isPress)
        _keyEvent.setPress(key, wc);
    else
        _keyEvent.setRelease(key, wc); 

    _keyEvent.setId( w.vid() );
    commitEvent(_keyEvent);
}


void ApplicationImpl::onMouse(Window& w, unsigned int msg, WPARAM wparam, LPARAM lparam)
{    
    int xPos = GET_X_LPARAM(lparam); 
    int yPos = GET_Y_LPARAM(lparam); 

    switch(msg)
    {
        case WM_LBUTTONDOWN:
            _mouseEvent.setPress(MouseEvent::Left);
        break;
        
        case WM_LBUTTONUP:        
            _mouseEvent.setRelease(MouseEvent::Left);
        break;
                            
        case WM_MBUTTONDOWN:
            _mouseEvent.setPress(MouseEvent::Middle);
        break;
        
        case WM_MBUTTONUP:
            _mouseEvent.setRelease(MouseEvent::Middle);
        break;   
        
        case WM_RBUTTONDOWN:        
            _mouseEvent.setPress(MouseEvent::Right);
        break;
        
        case WM_RBUTTONUP:
            _mouseEvent.setRelease(MouseEvent::Right);
        break; 

        case WM_MOUSEMOVE:
            _mouseEvent.setMove();
        break;
    }
  
    Application::instance().screen().setPointerWindow(&w);

    Gfx::PointF p = Application::instance().screen().toUnit( Gfx::Point(xPos, yPos) );
    _mouseEvent.setX( p.x() );
    _mouseEvent.setY( p.y() );            
    _mouseEvent.setId( w.vid() );
    commitEvent(_mouseEvent);
}


void ApplicationImpl::onMove(Window& w, HWND hwnd, LPARAM lParam)
{ 
    RECT  info;
    GetWindowRect(hwnd, &info);
    int xPos = info.left;
    int yPos = info.top;

    MoveEvent ev(w.vid(), Gfx::PointF(xPos, yPos) );
    commitEvent( ev );          
}


void ApplicationImpl::onResize(Window& w, WPARAM wParam, LPARAM lParam)
{   
    WindowState::Type state = WindowState::Normal;

    switch(wParam)
    {
        case SIZE_MAXHIDE:
        case SIZE_MAXSHOW:
            break;

        case SIZE_MAXIMIZED:
            state = WindowState::Maximized;
            break;

        case SIZE_MINIMIZED:
            state = WindowState::Minimized;
            break;
 
        default:
        case SIZE_RESTORED:
            state = WindowState::Normal;
            break;
    }   

    int width  = LOWORD(lParam);
    int height = HIWORD(lParam); 

    Gfx::SizeF to(width, height);
          
    ResizeEvent rev(w.vid(), to);
    commitEvent(rev);
           
    Gfx::RectF updateRect(Gfx::PointF(0,0), to);
    w.update(updateRect);
            
    // windows starts a nested message loop during resizing, so the events
    // required for painting need to be processed manually
    processEvents();
}


void ApplicationImpl::onPaint(Window& w, HWND hwnd)
{
    RECT info;
    GetClientRect(hwnd, &info);      
      
    PAINTSTRUCT ps;
    HDC windowContext = BeginPaint(hwnd, &ps);

    // TODO: rect optimization
    Gfx::RectF updateRect(Gfx::PointF(0,0), w.size());

    HDC bitmapContext = w.surface().pixmapImpl()->deviceContext();
    BitBlt(windowContext, 0, 0, info.right, info.bottom, bitmapContext, 0, 0, SRCCOPY);    
    
    EndPaint(hwnd, &ps);    
}

} // namespace

} // namespace
