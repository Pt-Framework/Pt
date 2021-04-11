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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, 
   Boston, MA 02110-1301 USA
*/

#include "ApplicationImpl.h"
#include "MainWindowImpl.h"
#include "PixmapSurfaceImpl.h"
#include "KeyMap.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/WindowStateEvent.h>
#include <Pt/Hmi/Window.h>
#include <Pt/System/IOError.h>
#include <Pt/String.h>
#include <Pt/Types.h>

using std::max;
using std::min;
#include <WindowsX.h>
#include <Gdiplus.h>

#pragma comment (lib, "gdiplus.lib")

namespace {

HBITMAP createImage888(const Pt::uint8_t* data, size_t width, size_t height)
{
    HDC hDC        = ::GetDC(NULL);
    HDC hMainDC    = ::CreateCompatibleDC(hDC); 

    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

    bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER); // Size of this struct.
    bitmapInfo.bmiHeader.biWidth       = width;             // Bitmap width.
    bitmapInfo.bmiHeader.biHeight      = -(Pt::ssize_t)height;  // Bitmap height. Negative value = top-down image.
    bitmapInfo.bmiHeader.biPlanes      = 1;                 // Always 1.
    bitmapInfo.bmiHeader.biBitCount    = 32;                // We internally use a 32-bit bitmap.
    bitmapInfo.bmiHeader.biCompression = BI_RGB;            // Uncompressed (top-down) RGB bitmap.
    bitmapInfo.bmiHeader.biSizeImage   = 0;                 // 0 = automatic for BI_RGB-images.
    bitmapInfo.bmiHeader.biClrUsed     = 0;                 // 0 = No color table.
    bitmapInfo.bmiHeader.biClrImportant= 0;                 // 0 = No color table.

    VOID* imageBits;
    HBITMAP bitmap = CreateDIBSection(hMainDC, &bitmapInfo, DIB_RGB_COLORS, &imageBits, NULL, 0);

    memcpy(imageBits, data, width * height * 3);

    ::DeleteDC(hMainDC);
    ::ReleaseDC(NULL,hDC);

    return bitmap;
}

}

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

DWORD Selector::waitFor(DWORD numHandles, const HANDLE *handles, 
                        DWORD msecs, bool& isTimeout)
{    
    DWORD result = MsgWaitForMultipleObjects(numHandles, (HANDLE*)handles, 
                                             FALSE, msecs, QS_ALLINPUT);

    if(result == WAIT_FAILED)
        throw Pt::System::IOError("WaitForMultipleObjects");

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
, _instanceHandle(NULL)
, _gdiplusToken(0)
, _mouseEvent(0)
, _keyEvent(0)
, _pointerInWindow(false)
, _cursorHandle(0)
, _currentCursor(0)
, _onScroll(false)
{
#ifdef NDEBUG  
    FreeConsole();
#endif

    SetProcessDPIAware();

    _instanceHandle = (HINSTANCE) GetModuleHandle(NULL);

    Gdiplus::GdiplusStartupInput startupInput;
    Gdiplus::GdiplusStartup(&_gdiplusToken, &startupInput, NULL);

    WNDCLASS winClass;
    winClass.style         = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc   = (WNDPROC)ApplicationImpl::wndProc;
    winClass.cbClsExtra    = 0;
    winClass.cbWndExtra    = 0;
    winClass.hInstance     = _instanceHandle;
    winClass.hIcon         = NULL;
    winClass.hCursor       = NULL;
    winClass.hbrBackground = NULL;
    winClass.lpszMenuName  = NULL;
    winClass.lpszClassName = "Pt-Hmi";
    RegisterClass(&winClass);
}


ApplicationImpl::~ApplicationImpl()
{
    if( _cursorHandle != 0 )
       DestroyCursor( _cursorHandle );

    Gdiplus::GdiplusShutdown(_gdiplusToken);

    UnregisterClass("Pt-Hmi", _instanceHandle);
}


void ApplicationImpl::setCursor(const Cursor* cursor)
{      
    if( _currentCursor == cursor )
        return;

    _currentCursor = cursor;

    if( cursor == 0 )
        return;

   if( _cursorHandle != 0 )            
       DestroyCursor( _cursorHandle );

    if( cursor->empty() )
    {
        SetCursor(0);
        return;
    }

    HBITMAP andMask = createImage888( &cursor->andRgb888()[0], cursor->width(),  cursor->height() );
    HBITMAP xorMask = createImage888( &cursor->xorRgb888()[0], cursor->width(),  cursor->height() );

    ICONINFO iconInfo;
    iconInfo.fIcon = false; 
    iconInfo.xHotspot = cursor->xHotspot();
    iconInfo.yHotspot = cursor->yHotspot();
    iconInfo.hbmColor = xorMask;
    iconInfo.hbmMask  = andMask;

    _cursorHandle = CreateIconIndirect(&iconInfo);

    if( _cursorHandle != 0 )
      SetCursor( _cursorHandle );    

    DeleteObject( andMask );
    DeleteObject( xorMask );
}


void ApplicationImpl::setDefaultFont(const std::string& fontName)
{
    PixmapSurfaceImpl::setDefaultFont(fontName);
}


Pt::Timespan ApplicationImpl::inactivityTime() const
{
	LASTINPUTINFO info = { 0 };
	info.cbSize = sizeof(info);
	GetLastInputInfo(&info);

  // GetTickCount() overflows like GetLastInputInfo() 
  DWORD ticks = GetTickCount();
  Pt::int64_t msecs = ticks - info.dwTime;
  
  return Pt::Timespan(msecs * 1000);
}	


void ApplicationImpl::grabPointer(Window& grabber)
{
    grabber.mainWindow().impl()->grabPointer();
}


void ApplicationImpl::releasePointer(Window& grabber)
{
    ReleaseCapture();
}


void ApplicationImpl::grabPointer(Widget& grabber)
{
    Window* w = grabber.window();
    if( ! w )
        return;

    w->mainWindow().impl()->grabPointer();
}


void ApplicationImpl::releasePointer(Widget& grabber)
{
    ReleaseCapture();
}


void ApplicationImpl::sendKeyEvent(const KeyEvent& ev)
{
    HWND hwnd = GetActiveWindow();
    if( ! hwnd )
        return;

    Window* window = findWindow(hwnd);
    if( ! window )
        return;

    KeyEvent kev = ev;
    kev.setId( window->vid() );
    commitEvent(kev);
}


void ApplicationImpl::sendMouseEvent(const MouseEvent& ev)
{
    POINT p = { Pt::lround( ev.position().x() ), 
                Pt::lround( ev.position().y() ) };

    HWND h = WindowFromPoint(p);

    Window* w = findWindow(h);
    if( ! w )
        return;

    // screen to window coordinates
    Gfx::PointF pos = w->fromScreen( ev.position() );

    MouseEvent mev = ev;
    mev.setPosition(pos);
    mev.setId( w->vid() );
    
    if( ! _pointerInWindow )
    {
        Application::instance().setPointerWindow(w);
        _pointerInWindow = true;
    }

    // TODO: call Application::processMouseEvent which returns true if the
    //       event was consumed. If it returns false and the event was not
    //       consumed call ApplicationImpl::dispatchMouseEvent

    Application::instance().processMouseEvent(mev);
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
    //_selector.waitForWake(0);
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


long CALLBACK ApplicationImpl::wndProc(HWND hwnd, UINT msg, 
                                       WPARAM wparam, LPARAM lparam)
{
    Pt::Hmi::Application& app = Pt::Hmi::Application::instance();

    bool handled = app.impl()->processMessage(hwnd, msg, wparam, lparam);
    if( ! handled )
        return DefWindowProc(hwnd, msg, wparam, lparam);

    if(msg == WM_ERASEBKGND)
        return TRUE;

    return handled ? 0 : 1;
}


Window* ApplicationImpl::findWindow(HWND hwnd)
{
    const std::vector<Window*>& windows = Application::instance().screen().windows();

    for(size_t i = 0; i < windows.size(); ++i)
    {
        Window* w = windows[i];

        if( ! w->impl() )
            continue;
        
        if( w->impl()->hwnd() == hwnd )
            return w;
    }
    
    return 0;
}


bool ApplicationImpl::processMessage(HWND hwnd, UINT msg, 
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

            ScrollEvent sev( w->vid() );
            sev.set(ScrollEvent::Vertical, (delta/WHEEL_DELTA)*20);

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
        
        case WM_ERASEBKGND:
        {
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
            handled = onClose(*w);
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

            MINMAXINFO* mmi = (MINMAXINFO*)lparam;
            mmi->ptMaxTrackSize.x = static_cast<LONG>(maxWidth);
            mmi->ptMaxTrackSize.y = static_cast<LONG>(maxHeight); 
            mmi->ptMinTrackSize.x = static_cast<LONG>(minWidth);
            mmi->ptMinTrackSize.y = static_cast<LONG>(minHeight);
            handled = true;  
            break;
        }

        case WM_MOUSELEAVE:
            handled = true;  
            //_mouseEvent.clear();
            Application::instance().setCursor(0);
            Application::instance().setPointerWindow(0);
            _pointerInWindow = false;
            break;
    }

    return handled;
}


void ApplicationImpl::onShow(Window& w,  bool v)
{
    ShowEvent sev(w.vid(), v);
    commitEvent( sev );

    w.invalidate();
}


bool ApplicationImpl::onClose(Window& w)
{  
    Pt::uint64_t id =  w.vid();

    CloseEvent ev(id);
    w.processEvent(ev);

    const Visual* v = Application::instance().findVisual(id);
    if( ! v )
        return true;
        
    bool isClosed = ! w.isClosed();
    return isClosed;
}


void ApplicationImpl::onActivate(Window& w, bool a)
{
    ActivateEvent aev( w.vid(), a );
    commitEvent(aev);
}


void ApplicationImpl::onEnable(Window& w, bool e)
{
    EnableEvent eev( w.vid(), e );
    commitEvent( eev );

    w.invalidate();
}


void ApplicationImpl::onKey(Window& w, UINT vkey, UINT scanCode, bool isPress)
{
    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);

    //std::clog << "KEY: " << std::hex << vkey << std::endl;

    wchar_t wc = 0;
    ToUnicode(vkey, scanCode, (BYTE*)keyboardState, &wc, 1, 0);    

    bool shift = (keyboardState[VK_SHIFT] & 0x80) == 0x80;
    bool control = (keyboardState[VK_CONTROL] & 0x80) == 0x80;
    bool alt = (keyboardState[VK_MENU] & 0x80) == 0x80;
    bool rwin = (keyboardState[VK_RWIN] & 0x80) == 0x80;
    bool lwin = (keyboardState[VK_LWIN] & 0x80) == 0x80;

    Key::Modifiers modifiers;
    if(shift)
        modifiers.add(Key::Shift);

    if(control)
        modifiers.add(Key::Control);

    if(alt)
        modifiers.add(Key::Alt);

    if(rwin || lwin)
        modifiers.add(Key::Meta);

    Pt::uint32_t keyCode = Key::NoKey;
    if(vkey < keyMapSize)
    {
        keyCode = keyMap[vkey];
        if(keyCode == 0)
        {
            Pt::Char ch = static_cast<Pt::uint32_t>(wc);
            keyCode = toupper(ch).value();
        }
    }

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
    
    const double scaling = w.scaleFactor();

    Gfx::PointF pos(Gfx::PointF(xPos / scaling, 
                                yPos / scaling));
    
    _mouseEvent.setPosition(pos);
    _mouseEvent.setId( w.vid() );
    
    if( ! _pointerInWindow )
    {
        Application::instance().setPointerWindow(&w);
        _pointerInWindow = true;
    }

    // TODO: call Application::processMouseEvent which returns true if the
    //       event was consumed. If it returns false and the event was not
    //       consumed call ApplicationImpl::dispatchMouseEvent

    Application::instance().processMouseEvent(_mouseEvent);
}


void ApplicationImpl::onMove(Window& w, HWND hwnd, LPARAM lParam)
{ 
    RECT info;
    GetWindowRect(hwnd, &info);
    
    int x = info.left;
    int y = info.top;

    Gfx::PointF pos(x, y);
    pos = w.toLogical(pos);

    MoveEvent ev(w.vid(), pos);
    commitEvent( ev );
}


void ApplicationImpl::onResize(Window& w, WPARAM wParam, LPARAM lParam)
{   
    Window::State wstate = w.state();

    switch(wParam)
    {
        case SIZE_MAXHIDE:
        case SIZE_MAXSHOW:
            break;

        case SIZE_MAXIMIZED:
            wstate = Window::Maximized;
            break;

        case SIZE_MINIMIZED:
            wstate = Window::Minimized;
            break;

        case SIZE_RESTORED:
            wstate = Window::Normal;
            break;

        default:
            break;
    }

    if(w.state() != wstate)
    {
        WindowStateEvent wse(w.vid(), wstate);
        commitEvent(wse);
    }

    int width  = LOWORD(lParam);
    int height = HIWORD(lParam);

    Gfx::SizeF to(width, height);
    to = w.toLogical(to);

    ResizeEvent rev(w.vid(), to);
    w.processEvent(rev);

    Gfx::RectF updateRect(Gfx::PointF(0, 0), to);
    w.repaint(updateRect);
}


void ApplicationImpl::onPaint(Window& w, HWND hwnd)
{
    RECT updateRect;
    GetUpdateRect(hwnd, &updateRect, FALSE);

    const Gfx::RectF r(updateRect.left, updateRect.right, updateRect.top, updateRect.bottom);

    PaintEvent ev(w.vid(), r);
    w.processEvent(ev);

    PAINTSTRUCT ps;
    HDC windowContext = BeginPaint(hwnd, &ps);

    HDC bitmapContext = w.surface().pixmapImpl()->deviceContext();
    BitBlt(windowContext, updateRect.left, updateRect.top, 
           updateRect.right - updateRect.left, updateRect.bottom - updateRect.top,
           bitmapContext, updateRect.left, updateRect.top, SRCCOPY);

    EndPaint(hwnd, &ps);
}

} // namespace

} // namespace
