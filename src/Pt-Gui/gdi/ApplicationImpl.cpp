/*
 * Copyright (C) 2006-2007 Tobias Mueller
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ApplicationImpl.h"

#include "Pt/Gui/Application.h"
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/CloseEvent.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Gui/MoveEvent.h>
#include <Pt/Gui/MouseMoveEvent.h>
#include <Pt/System/IOError.h>

#include "WidgetImpl.h"
#include "win32.h"

#include <iostream>
#include <commctrl.h>

using namespace std;


namespace Pt {

namespace Gui {


const LPCSTR GDIRegistry::TOP_WINDOW_CLASS_NAME   = "PTVTopWindow";
const LPCSTR GDIRegistry::CHILD_WINDOW_CLASS_NAME = "PTVChildWindow";

const UINT GDIEventLoop::WM_MESSAGE_QUEUE_WAKE_UP = WM_APP + 1;

bool GDIEventLoop::_trackingMouseEvent = false;

Pt::Signal<const Pt::Event&> GDIEventLoop::eventQueueSignal;



GDIRegistry::GDIRegistry()
{
    _instanceHandle = (HINSTANCE)GetModuleHandle(NULL);

    registerWindowClasses();
}


GDIRegistry::~GDIRegistry()
{
    unregisterWindowClasses();
}


void GDIRegistry::registerWidget(HWND windowHandle, Widget& widget)
{
    _windowHandle2Widget.insert(std::make_pair(windowHandle, &widget));
}


void GDIRegistry::unregisterWidget(HWND windowHandle)
{
    _windowHandle2Widget.erase(windowHandle);
}


Widget* GDIRegistry::findWidget(HWND windowHandle)
{
    std::map<HWND, Widget*>::iterator it = _windowHandle2Widget.find(windowHandle);
    if( it == _windowHandle2Widget.end() ) {
        return 0;
    }

    return it->second;
}


HWND GDIRegistry::getFirstHWND()
{
    if (_windowHandle2Widget.empty()) {
        return 0;
    }

    return _windowHandle2Widget.begin()->first;
}


void GDIRegistry::registerWindowClasses()
{
    basic_string<TCHAR> topLevelWindow = win32::fromMultiByte(TOP_WINDOW_CLASS_NAME);
    basic_string<TCHAR> childWindow    = win32::fromMultiByte(CHILD_WINDOW_CLASS_NAME);

    // TODO Add icons to top level windows.

    // Register top-level window class that is used for all top-level-windows.
    WNDCLASS topWindowClass;

    topWindowClass.style         = CS_HREDRAW | CS_VREDRAW;
    topWindowClass.lpfnWndProc   = (WNDPROC)GDIEventLoop::wndProc;
    topWindowClass.cbClsExtra    = 0;
    topWindowClass.cbWndExtra    = 0;
    topWindowClass.hInstance     = _instanceHandle;
    topWindowClass.hIcon         = NULL;
    topWindowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    topWindowClass.hbrBackground = NULL;
    topWindowClass.lpszMenuName  = NULL;
    topWindowClass.lpszClassName = topLevelWindow.c_str();

    RegisterClass(&topWindowClass);


    // Register the child window class that is used for all child-windows aka widgets
    // inside a top-level-window.
    WNDCLASS childWindowClass;

    childWindowClass.style         = CS_HREDRAW | CS_VREDRAW;
    childWindowClass.lpfnWndProc   = (WNDPROC)GDIEventLoop::wndProc;
    childWindowClass.cbClsExtra    = 0;
    childWindowClass.cbWndExtra    = 0;
    childWindowClass.hInstance     = _instanceHandle;
    childWindowClass.hIcon         = NULL;
    childWindowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    childWindowClass.hbrBackground = NULL;
    childWindowClass.lpszMenuName  = NULL;
    childWindowClass.lpszClassName = childWindow.c_str();

    RegisterClass(&childWindowClass);
}


void GDIRegistry::unregisterWindowClasses()
{
    UnregisterClass(win32::fromMultiByte(TOP_WINDOW_CLASS_NAME).c_str(),   _instanceHandle);
    UnregisterClass(win32::fromMultiByte(CHILD_WINDOW_CLASS_NAME).c_str(), _instanceHandle);
}



GDIEventLoop::GDIEventLoop()
: _messageLoopThreadId(0)
{
}


GDIEventLoop::~GDIEventLoop()
{
}


void GDIEventLoop::commitEvent(const Pt::Event& e)
{
//  std::cout << "committing an event" << std::endl;

    queueEvent(e);
    wake();
}


void GDIEventLoop::queueEvent(const Pt::Event& e)
{
    _queueMutex.lock();

    Pt::Event& ev = e.clone(_allocator);
    _eventQueue.push_back(&ev);

    _queueMutex.unlock();
}


void GDIEventLoop::processEvents()
{
    while (true)
    {
        _queueMutex.lock();

        if( _eventQueue.empty() ) {
            _queueMutex.unlock();
            break;
        }

        Pt::Event* ev = _eventQueue.front();
        _eventQueue.remove(ev);

        _queueMutex.unlock();

        eventQueueSignal.send(*ev);
        ev->destroy(_allocator);
    }
}


void GDIEventLoop::wake()
{
    if (_messageLoopThreadId) {
        PostThreadMessage(_messageLoopThreadId, WM_MESSAGE_QUEUE_WAKE_UP, 0, 0);
    }
}


int GDIEventLoop::run()
{
    MSG msg;

    // Save current thread-id to later post messages to this thread's message queue.
    _messageLoopThreadId = GetCurrentThreadId();

    // Process the events which were added to the queue before entring the run method.
    processEvents();
    while (GetMessage(&msg, NULL, 0, 0)) {
        // Deliver our events first.
        processEvents();

        if (msg.message == WM_MESSAGE_QUEUE_WAKE_UP) {
            // The message was just a trigger to process the events in the event queue.
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


int GDIEventLoop::exit()
{
    if (_messageLoopThreadId) {
        // Basically the same as PostQuitMessage(), but posting to a specific thread.
        PostThreadMessage(_messageLoopThreadId, WM_QUIT, 0, 0);
    }

    return 0;
}


long CALLBACK GDIEventLoop::wndProc(HWND hwnd, unsigned int message, unsigned int wParam, long lParam)
{
    return dispatchGDIEvent(hwnd, message, wParam, lParam);
}


LRESULT GDIEventLoop::dispatchGDIEvent(HWND hwnd, unsigned int message, unsigned int wParam, long lParam)
{
    // Translate the GDI message into an event we can understand.
    //std::cout << "dispatch: " << message << std::endl;

    if (message == WM_MESSAGE_QUEUE_WAKE_UP) {
        GDIEventLoop::instance().processEvents();
        return 0;
    }

    Widget* widget = GDIRegistry::instance().findWidget(hwnd);

    if (widget == 0) {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    switch (message)
    {
        case WM_DESTROY:
            processDestroyMessage(*widget);
            break;

        case WM_KEYDOWN:
            processVirtualKeyMessage(*widget, wParam, lParam, KeyEvent::Press);
            break;

        case WM_SYSKEYDOWN:
            processVirtualKeyMessage(*widget, wParam, lParam, KeyEvent::Press);
            break;

        case WM_CHAR:
            processCharacterKeyMessage(*widget, wParam, lParam);
            break;

        case WM_KEYUP:
            processVirtualKeyMessage(*widget, wParam, lParam, KeyEvent::Release);
            break;

        case WM_LBUTTONUP:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::LeftButton, MouseEvent::Release);
            break;

        case WM_LBUTTONDOWN:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::LeftButton, MouseEvent::Press);
            break;

        case WM_LBUTTONDBLCLK:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::LeftButton, MouseEvent::DoubleClick);
            break;

        case WM_RBUTTONUP:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::RightButton, MouseEvent::Release);
            break;

        case WM_RBUTTONDOWN:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::RightButton, MouseEvent::Press);
            break;

        case WM_RBUTTONDBLCLK:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::RightButton, MouseEvent::DoubleClick);
            break;

        case WM_MBUTTONUP:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::MiddleButton, MouseEvent::Release);
            break;

        case WM_MBUTTONDOWN:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::MiddleButton, MouseEvent::Press);
            break;

        case WM_MBUTTONDBLCLK:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::MiddleButton, MouseEvent::DoubleClick);
            break;

        case WM_MOUSEWHEEL:
            processMouseWheelMessage(*widget, wParam, lParam);
            break;

        case WM_MOUSEMOVE:
            processMouseMoveMessage(*widget, wParam, lParam);
            break;

// TODO See processMouseEnter()
#ifndef _WIN32_WCE
        case WM_MOUSELEAVE:
            processMouseLeaveMessage(*widget);
            break;
#endif

        case WM_PAINT:
            processPaintMessage(hwnd, *widget);
            break;

        case WM_MOVE:
            processMoveMessage(*widget, wParam, lParam);
            break;

        case WM_SIZE:
            processSizeMessage(*widget, wParam, lParam);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
            break;
    }

    return 0;
}


void GDIEventLoop::processDestroyMessage(Widget& widget)
{
    //std::cout << __FUNCTION__ << widget.impl().hwnd() <<std::endl;


    CloseEvent closeEvent(widget);
    eventQueueSignal.send(closeEvent);
}

void GDIEventLoop::processVirtualKeyMessage(Widget& widget, int wParam, int lParam, KeyEvent::Type type)
{
    //std::cout << __FUNCTION__ << "  " << wParam << "   extended: " << (lParam & (1 << 24)) << std::endl;


    BOOL isExtendedKey = (lParam & (1 << 24));

    KeyEvent::KeyCode code = KeyEvent::Void;

    // TODO How to distinguish between left and right shift?
    switch (wParam)
    {
        case VK_CONTROL: code = isExtendedKey ? KeyEvent::CtrlR : KeyEvent::CtrlL; break;
        case VK_MENU:    code = isExtendedKey ? KeyEvent::AltL  : KeyEvent::AltR;  break;
        case VK_SHIFT:   code = KeyEvent::ShiftL;      break;
        case VK_F1:      code = KeyEvent::F1;          break;
        case VK_F2:      code = KeyEvent::F2;          break;
        case VK_F3:      code = KeyEvent::F3;          break;
        case VK_F4:      code = KeyEvent::F4;          break;
        case VK_F5:      code = KeyEvent::F5;          break;
        case VK_F6:      code = KeyEvent::F6;          break;
        case VK_F7:      code = KeyEvent::F7;          break;
        case VK_F8:      code = KeyEvent::F8;          break;
        case VK_F9:      code = KeyEvent::F9;          break;
        case VK_F10:     code = KeyEvent::F10;         break;
        case VK_F11:     code = KeyEvent::F11;         break;
        case VK_F12:     code = KeyEvent::F12;         break;
        case VK_F13:     code = KeyEvent::F13;         break;
        case VK_F14:     code = KeyEvent::F14;         break;
        case VK_F15:     code = KeyEvent::F15;         break;
        case VK_F16:     code = KeyEvent::F16;         break;
        case VK_F17:     code = KeyEvent::F17;         break;
        case VK_F18:     code = KeyEvent::F18;         break;
        case VK_F19:     code = KeyEvent::F19;         break;
        case VK_F20:     code = KeyEvent::F20;         break;
        case VK_F21:     code = KeyEvent::F21;         break;
        case VK_F22:     code = KeyEvent::F22;         break;
        case VK_F23:     code = KeyEvent::F23;         break;
        case VK_F24:     code = KeyEvent::F24;         break;
        case VK_RETURN:  code = KeyEvent::Enter;       break;
        case VK_BACK:    code = KeyEvent::Backspace;   break;
        case VK_TAB:     code = KeyEvent::Tab;         break;
        case VK_CANCEL:  code = KeyEvent::Cancel;      break;
        case VK_CLEAR:   code = KeyEvent::Clear;       break;
        case VK_PAUSE:   code = KeyEvent::Pause;       break;
        case VK_CAPITAL: code = KeyEvent::CapsLock;    break;
        case VK_ESCAPE:  code = KeyEvent::Escape;      break;
        case VK_SPACE:   code = KeyEvent::Space;       break;
        case VK_PRIOR:   code = KeyEvent::PageUp;      break;
        case VK_NEXT:    code = KeyEvent::PageDown;    break;
        case VK_END:     code = KeyEvent::End;         break;
        case VK_HOME:    code = KeyEvent::Home;        break;
        case VK_INSERT:  code = KeyEvent::Insert;      break;
        case VK_DELETE:  code = KeyEvent::Delete;      break;
        case VK_LEFT:    code = KeyEvent::Left;        break;
        case VK_RIGHT:   code = KeyEvent::Right;       break;
        case VK_UP:      code = KeyEvent::Up;          break;
        case VK_DOWN:    code = KeyEvent::Down;        break;
        case VK_NUMLOCK: code = KeyEvent::NumLock;     break;
        case VK_SCROLL:  code = KeyEvent::ScrollLock;  break;
        case VK_SNAPSHOT:code = KeyEvent::PrintScreen; break;
        case VK_HELP:    code = KeyEvent::Help;        break;
        case VK_LWIN:    code = KeyEvent::WindowsL;    break;
        case VK_RWIN:    code = KeyEvent::WindowsR;    break;
        case VK_APPS:    code = KeyEvent::ContextMenu; break;
        // TODO More, more!
    }

    KeyEvent keyEvent(widget, type, code, 0);
    eventQueueSignal.send(keyEvent);
}


void GDIEventLoop::processCharacterKeyMessage(Widget& widget, int wParam, int lParam)
{
    //std::cout << __FUNCTION__ << "  " << (char)wParam << std::endl;


    KeyEvent keyEvent(widget, KeyEvent::Press, KeyEvent::Void, (wchar_t)wParam);
    eventQueueSignal.send(keyEvent);
}


void GDIEventLoop::processMouseMoveMessage(Widget& widget, int wParam, int lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    //std::cout << __FUNCTION__ << " x:" << x << "   y:" << y << std::endl;

    if (!_trackingMouseEvent)// || _lastMouseMoveWidget->impl().hwnd() != widget.impl().hwnd())
    {
        processMouseEntered(widget, wParam, lParam);
        _trackingMouseEvent = true;
    }

    unsigned int modifiers = createModifiersFromMouseMessage(wParam);

    MouseMoveEvent mouseMoveEvent(widget, x, y, MouseMoveEvent::Moved, modifiers);
    eventQueueSignal.send(mouseMoveEvent);
}


void GDIEventLoop::processMouseEntered(Widget& widget, int wParam, int lParam)
{
// TODO
// Is not supported by WinCE.
// There are alternative ways of doing enter/leave-handling:
// 1. Use the TrackMouseEvent API and the WM_MOUSELEAVE message (Win98/NT4+)
// 2. By using SetCapture API. When the window first receives a WM_MOUSEMOVE message, set
// the mouse capture. When the mouse leaves the window, Windows will send the window one last
// WM_MOUSEMOVE (the coordinates will be outside the window's client area). You can use this
// fact to detect when the mouse has left a window.
// 3 . By using a Timer. When the mouse enters a window, set a timer going with a small interval
// (10ms, say). When the timer expires, check if the mouse is still in the window. If it is, then
// let the timer keep going. Otherwise, the mouse has left the window, and the timer can be stopped.
// 4 . By using a mouse hook. When the mouse enters a window, install a mouse hook to monitor all
// mouse events. By checking for WM_MOUSEMOVE messages, you can check when the mouse has left a
// window and remove the hook appropriately.
// http://www.catch22.net/tuts/tips.asp#DetectMouseLeave
#ifndef _WIN32_WCE
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    TRACKMOUSEEVENT trackMouseEvent;
    trackMouseEvent.cbSize = sizeof(trackMouseEvent);
    trackMouseEvent.dwFlags = TME_LEAVE;
    trackMouseEvent.hwndTrack = widget.impl().hwnd();
    TrackMouseEvent(&trackMouseEvent);

    unsigned int modifiers = createModifiersFromMouseMessage(wParam);

    MouseMoveEvent mouseMoveEvent(widget, x, y, MouseMoveEvent::Entered, modifiers);
    eventQueueSignal.send(mouseMoveEvent);
#endif
}


void GDIEventLoop::processMouseButtonMessage(Widget& widget, int wParam, int lParam, MouseEvent::Button button, MouseEvent::Action action)
{
    //std::cout << __FUNCTION__ << widget.impl().hwnd() <<std::endl;


    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    unsigned int modifiers = createModifiersFromMouseMessage(wParam);

    MouseEvent mouseEvent(widget, x, y, button, action, modifiers);
    eventQueueSignal.send(mouseEvent);
}


void GDIEventLoop::processMouseWheelMessage(Widget& widget, int wParam, int lParam)
{
    //std::cout << __FUNCTION__ << widget.impl().hwnd() <<std::endl;

    // Mouse position is relative to screen(!), so calculate the position relative to the widget.
    int x = LOWORD(lParam) - widget.region().x();
    int y = HIWORD(lParam) - widget.region().y();

    int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

    unsigned int modifiers = createModifiersFromMouseMessage(wParam);

    MouseEvent mouseEvent(widget, x, y, (zDelta > 0) ? MouseEvent::WheelUp : MouseEvent::WheelDown, MouseEvent::Press, modifiers);
    eventQueueSignal.send(mouseEvent);
}


void GDIEventLoop::processMouseLeaveMessage(Widget& widget)
{
    _trackingMouseEvent = false;

    MouseMoveEvent mouseMoveEvent(widget, 0, 0, MouseMoveEvent::Exited, MouseMoveEvent::NoButton);
    eventQueueSignal.send(mouseMoveEvent);
}


void GDIEventLoop::processPaintMessage(HWND hwnd, Widget& widget)
{
    //std::cout << __FUNCTION__ << widget.impl().hwnd() <<std::endl;

    RECT gdiRectangle;

    GetUpdateRect(hwnd, &gdiRectangle, false);

     Pt::Gfx::Region region(
         Pt::Gfx::Point(gdiRectangle.left, gdiRectangle.top),
         Pt::Gfx::Size(gdiRectangle.right - gdiRectangle.left + 1, gdiRectangle.bottom - gdiRectangle.top + 1)
    );

    PaintEvent paintEvent(widget, region);

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    eventQueueSignal.send(paintEvent); // commitEvent

    EndPaint(hwnd, &ps);
}


void GDIEventLoop::processMoveMessage(Widget& widget, int wParam, int lParam)
{
    //std::cout << __FUNCTION__ <<std::endl;


    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    MoveEvent moveEvent(widget, x, y);
    eventQueueSignal.send(moveEvent);
}


void GDIEventLoop::processSizeMessage(Widget& widget, int wParam, int lParam)
{
    int width  = LOWORD(lParam);
    int height = HIWORD(lParam);

    //std::cout << __FUNCTION__ << "  width: " << width << "   height: " << height << std::endl;

    ResizeEvent::Type resizeType;

    switch (wParam) {
        case SIZE_MAXIMIZED:
            resizeType = ResizeEvent::Maximized;
            break;

        case SIZE_MINIMIZED:
            resizeType = ResizeEvent::Minimized;
            break;

        case SIZE_RESTORED:
            resizeType = ResizeEvent::Restored;
            break;

        default:
            resizeType = ResizeEvent::Resize;
            break;
    }

    ResizeEvent resizeEvent(widget, width, height, resizeType);
    eventQueueSignal.send(resizeEvent);
}

unsigned int GDIEventLoop::createModifiersFromMouseMessage(int wParam)
{
    //std::cout << __FUNCTION__ << std::endl;


    unsigned int modifiers = 0;

    // TODO Alt-Key is missing.
    if (wParam & MK_CONTROL) { modifiers |= MouseEvent::CtrlDown;         }
    if (wParam & MK_SHIFT)   { modifiers |= MouseEvent::ShiftDown;        }
    if (wParam & MK_LBUTTON) { modifiers |= MouseEvent::LeftButtonDown;   }
    if (wParam & MK_MBUTTON) { modifiers |= MouseEvent::MiddleButtonDown; }
    if (wParam & MK_RBUTTON) { modifiers |= MouseEvent::RightButtonDown;  }

    return modifiers;
}




ApplicationImpl::ApplicationImpl(Application& app)
: _impl(0)
{
    connect(GDIEventLoop::eventQueueSignal, app.event);
    //_impl = new MainLoopImpl();
    //connect(_impl->event(), app.event);
}


ApplicationImpl::~ApplicationImpl()
{
    //delete _impl;
}


void ApplicationImpl::commitEvent(const Pt::Event& e)
{
    GDIEventLoop::instance().commitEvent(e);
}


void ApplicationImpl::queueEvent(const Pt::Event& e)
{
    GDIEventLoop::instance().queueEvent(e);
}


int ApplicationImpl::run()
{
    return GDIEventLoop::instance().run();

    //_impl->run();
    //return 0;
}


int ApplicationImpl::exit()
{
    return GDIEventLoop::instance().exit();

    //_impl->exit();
    //return 0;
}

void ApplicationImpl::processEvents()
{
    GDIEventLoop::instance().processEvents();
}


MainLoopImpl* g_loopImpl = 0;


MainLoopImpl::MainLoopImpl()
: Pt::System::MainLoopImpl()
, _trackingMouseEvent(false)
{
    g_loopImpl = this;
    GDIRegistry::instance();
}


MainLoopImpl::MainLoopImpl(Allocator& a)
: Pt::System::MainLoopImpl(a)
, _trackingMouseEvent(false)
{
    g_loopImpl = this;
    GDIRegistry::instance();
}


MainLoopImpl::~MainLoopImpl()
{
}


DWORD MainLoopImpl::waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout)
{
    DWORD result = MsgWaitForMultipleObjects(numHandles, handles, false, msecs, QS_ALLEVENTS);
    if(result == WAIT_FAILED)
    {
        //DWORD err = GetLastError();
        throw Pt::System::IOError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
    }

    if( result == WAIT_TIMEOUT)
    {
        isTimeout = true;
        return 0;
    }

    DWORD offset = result - WAIT_OBJECT_0;

    if(offset == numHandles)
    {
        processMessage();
    }

    return offset;
}


void MainLoopImpl::processMessage()
{
    MSG msg;

    while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


long CALLBACK MainLoopImpl::wndProc(HWND hwnd, unsigned int message, unsigned int wParam, long lParam)
{
    return g_loopImpl->dispatchGDIEvent(hwnd, message, wParam, lParam);
}


LRESULT MainLoopImpl::dispatchGDIEvent(HWND hwnd, unsigned int message, unsigned int wParam, long lParam)
{
    // Translate the GDI message into an event we can understand.

    Widget* widget = GDIRegistry::instance().findWidget(hwnd);

    if (widget == 0) {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    switch (message)
    {
        case WM_DESTROY:
            processDestroyMessage(*widget);
            break;

        case WM_KEYDOWN:
            processVirtualKeyMessage(*widget, wParam, lParam, KeyEvent::Press);
            break;

        case WM_SYSKEYDOWN:
            processVirtualKeyMessage(*widget, wParam, lParam, KeyEvent::Press);
            break;

        case WM_CHAR:
            processCharacterKeyMessage(*widget, wParam, lParam);
            break;

        case WM_KEYUP:
            processVirtualKeyMessage(*widget, wParam, lParam, KeyEvent::Release);
            break;

        case WM_LBUTTONUP:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::LeftButton, MouseEvent::Release);
            break;

        case WM_LBUTTONDOWN:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::LeftButton, MouseEvent::Press);
            break;

        case WM_LBUTTONDBLCLK:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::LeftButton, MouseEvent::DoubleClick);
            break;

        case WM_RBUTTONUP:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::RightButton, MouseEvent::Release);
            break;

        case WM_RBUTTONDOWN:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::RightButton, MouseEvent::Press);
            break;

        case WM_RBUTTONDBLCLK:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::RightButton, MouseEvent::DoubleClick);
            break;

        case WM_MBUTTONUP:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::MiddleButton, MouseEvent::Release);
            break;

        case WM_MBUTTONDOWN:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::MiddleButton, MouseEvent::Press);
            break;

        case WM_MBUTTONDBLCLK:
            processMouseButtonMessage(*widget, wParam, lParam, MouseEvent::MiddleButton, MouseEvent::DoubleClick);
            break;

        case WM_MOUSEWHEEL:
            processMouseWheelMessage(*widget, wParam, lParam);
            break;

        case WM_MOUSEMOVE:
            processMouseMoveMessage(*widget, wParam, lParam);
            break;

// TODO See processMouseEnter()
#ifndef _WIN32_WCE
        case WM_MOUSELEAVE:
            processMouseLeaveMessage(*widget);
            break;
#endif

        case WM_PAINT:
            processPaintMessage(hwnd, *widget);
            break;

        case WM_MOVE:
            processMoveMessage(*widget, wParam, lParam);
            break;

        case WM_SIZE:
            processSizeMessage(*widget, wParam, lParam);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
            break;
    }

    return 0;
}


void MainLoopImpl::processDestroyMessage(Widget& widget)
{
    //std::cout << __FUNCTION__ << widget.impl().hwnd() <<std::endl;


    CloseEvent closeEvent(widget);
    event().send(closeEvent);
}

void MainLoopImpl::processVirtualKeyMessage(Widget& widget, int wParam, int lParam, KeyEvent::Type type)
{
    //std::cout << __FUNCTION__ << "  " << wParam << "   extended: " << (lParam & (1 << 24)) << std::endl;


    BOOL isExtendedKey = (lParam & (1 << 24));

    KeyEvent::KeyCode code = KeyEvent::Void;

    // TODO How to distinguish between left and right shift?
    switch (wParam)
    {
        case VK_CONTROL: code = isExtendedKey ? KeyEvent::CtrlR : KeyEvent::CtrlL; break;
        case VK_MENU:    code = isExtendedKey ? KeyEvent::AltL  : KeyEvent::AltR;  break;
        case VK_SHIFT:   code = KeyEvent::ShiftL;      break;
        case VK_F1:      code = KeyEvent::F1;          break;
        case VK_F2:      code = KeyEvent::F2;          break;
        case VK_F3:      code = KeyEvent::F3;          break;
        case VK_F4:      code = KeyEvent::F4;          break;
        case VK_F5:      code = KeyEvent::F5;          break;
        case VK_F6:      code = KeyEvent::F6;          break;
        case VK_F7:      code = KeyEvent::F7;          break;
        case VK_F8:      code = KeyEvent::F8;          break;
        case VK_F9:      code = KeyEvent::F9;          break;
        case VK_F10:     code = KeyEvent::F10;         break;
        case VK_F11:     code = KeyEvent::F11;         break;
        case VK_F12:     code = KeyEvent::F12;         break;
        case VK_F13:     code = KeyEvent::F13;         break;
        case VK_F14:     code = KeyEvent::F14;         break;
        case VK_F15:     code = KeyEvent::F15;         break;
        case VK_F16:     code = KeyEvent::F16;         break;
        case VK_F17:     code = KeyEvent::F17;         break;
        case VK_F18:     code = KeyEvent::F18;         break;
        case VK_F19:     code = KeyEvent::F19;         break;
        case VK_F20:     code = KeyEvent::F20;         break;
        case VK_F21:     code = KeyEvent::F21;         break;
        case VK_F22:     code = KeyEvent::F22;         break;
        case VK_F23:     code = KeyEvent::F23;         break;
        case VK_F24:     code = KeyEvent::F24;         break;
        case VK_RETURN:  code = KeyEvent::Enter;       break;
        case VK_BACK:    code = KeyEvent::Backspace;   break;
        case VK_TAB:     code = KeyEvent::Tab;         break;
        case VK_CANCEL:  code = KeyEvent::Cancel;      break;
        case VK_CLEAR:   code = KeyEvent::Clear;       break;
        case VK_PAUSE:   code = KeyEvent::Pause;       break;
        case VK_CAPITAL: code = KeyEvent::CapsLock;    break;
        case VK_ESCAPE:  code = KeyEvent::Escape;      break;
        case VK_SPACE:   code = KeyEvent::Space;       break;
        case VK_PRIOR:   code = KeyEvent::PageUp;      break;
        case VK_NEXT:    code = KeyEvent::PageDown;    break;
        case VK_END:     code = KeyEvent::End;         break;
        case VK_HOME:    code = KeyEvent::Home;        break;
        case VK_INSERT:  code = KeyEvent::Insert;      break;
        case VK_DELETE:  code = KeyEvent::Delete;      break;
        case VK_LEFT:    code = KeyEvent::Left;        break;
        case VK_RIGHT:   code = KeyEvent::Right;       break;
        case VK_UP:      code = KeyEvent::Up;          break;
        case VK_DOWN:    code = KeyEvent::Down;        break;
        case VK_NUMLOCK: code = KeyEvent::NumLock;     break;
        case VK_SCROLL:  code = KeyEvent::ScrollLock;  break;
        case VK_SNAPSHOT:code = KeyEvent::PrintScreen; break;
        case VK_HELP:    code = KeyEvent::Help;        break;
        case VK_LWIN:    code = KeyEvent::WindowsL;    break;
        case VK_RWIN:    code = KeyEvent::WindowsR;    break;
        case VK_APPS:    code = KeyEvent::ContextMenu; break;
        // TODO More, more!
    }

    KeyEvent keyEvent(widget, type, code, 0);
    event().send(keyEvent);
}


void MainLoopImpl::processCharacterKeyMessage(Widget& widget, int wParam, int lParam)
{
    //std::cout << __FUNCTION__ << "  " << (char)wParam << std::endl;


    KeyEvent keyEvent(widget, KeyEvent::Press, KeyEvent::Void, (wchar_t)wParam);
    event().send(keyEvent);
}


void MainLoopImpl::processMouseMoveMessage(Widget& widget, int wParam, int lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    //std::cout << __FUNCTION__ << " x:" << x << "   y:" << y << std::endl;

    if (!_trackingMouseEvent)// || _lastMouseMoveWidget->impl().hwnd() != widget.impl().hwnd())
    {
        processMouseEntered(widget, wParam, lParam);
        _trackingMouseEvent = true;
    }

    unsigned int modifiers = createModifiersFromMouseMessage(wParam);

    MouseMoveEvent mouseMoveEvent(widget, x, y, MouseMoveEvent::Moved, modifiers);
    event().send(mouseMoveEvent);
}


void MainLoopImpl::processMouseEntered(Widget& widget, int wParam, int lParam)
{
// TODO
// Is not supported by WinCE.
// There are alternative ways of doing enter/leave-handling:
// 1. Use the TrackMouseEvent API and the WM_MOUSELEAVE message (Win98/NT4+)
// 2. By using SetCapture API. When the window first receives a WM_MOUSEMOVE message, set
// the mouse capture. When the mouse leaves the window, Windows will send the window one last
// WM_MOUSEMOVE (the coordinates will be outside the window's client area). You can use this
// fact to detect when the mouse has left a window.
// 3 . By using a Timer. When the mouse enters a window, set a timer going with a small interval
// (10ms, say). When the timer expires, check if the mouse is still in the window. If it is, then
// let the timer keep going. Otherwise, the mouse has left the window, and the timer can be stopped.
// 4 . By using a mouse hook. When the mouse enters a window, install a mouse hook to monitor all
// mouse events. By checking for WM_MOUSEMOVE messages, you can check when the mouse has left a
// window and remove the hook appropriately.
// http://www.catch22.net/tuts/tips.asp#DetectMouseLeave
#ifndef _WIN32_WCE
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    TRACKMOUSEEVENT trackMouseEvent;
    trackMouseEvent.cbSize = sizeof(trackMouseEvent);
    trackMouseEvent.dwFlags = TME_LEAVE;
    trackMouseEvent.hwndTrack = widget.impl().hwnd();
    TrackMouseEvent(&trackMouseEvent);

    unsigned int modifiers = createModifiersFromMouseMessage(wParam);

    MouseMoveEvent mouseMoveEvent(widget, x, y, MouseMoveEvent::Entered, modifiers);
    event().send(mouseMoveEvent);
#endif
}


void MainLoopImpl::processMouseButtonMessage(Widget& widget, int wParam, int lParam, MouseEvent::Button button, MouseEvent::Action action)
{
    //std::cout << __FUNCTION__ << widget.impl().hwnd() <<std::endl;


    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    unsigned int modifiers = createModifiersFromMouseMessage(wParam);

    MouseEvent mouseEvent(widget, x, y, button, action, modifiers);
    event().send(mouseEvent);
}


void MainLoopImpl::processMouseWheelMessage(Widget& widget, int wParam, int lParam)
{
    //std::cout << __FUNCTION__ << widget.impl().hwnd() <<std::endl;

    // Mouse position is relative to screen(!), so calculate the position relative to the widget.
    int x = LOWORD(lParam) - widget.region().x();
    int y = HIWORD(lParam) - widget.region().y();

    int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

    unsigned int modifiers = createModifiersFromMouseMessage(wParam);

    MouseEvent mouseEvent(widget, x, y, (zDelta > 0) ? MouseEvent::WheelUp : MouseEvent::WheelDown, MouseEvent::Press, modifiers);
    event().send(mouseEvent);
}


void MainLoopImpl::processMouseLeaveMessage(Widget& widget)
{
    _trackingMouseEvent = false;

    MouseMoveEvent mouseMoveEvent(widget, 0, 0, MouseMoveEvent::Exited, MouseMoveEvent::NoButton);
    event().send(mouseMoveEvent);
}


void MainLoopImpl::processPaintMessage(HWND hwnd, Widget& widget)
{
    //std::cout << __FUNCTION__ << widget.impl().hwnd() <<std::endl;

    RECT gdiRectangle;

    GetUpdateRect(hwnd, &gdiRectangle, false);

     Pt::Gfx::Region region(
         Pt::Gfx::Point(gdiRectangle.left, gdiRectangle.top),
         Pt::Gfx::Size(gdiRectangle.right - gdiRectangle.left + 1, gdiRectangle.bottom - gdiRectangle.top + 1)
    );

    PaintEvent paintEvent(widget, region);

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    event().send(paintEvent); // commitEvent

    EndPaint(hwnd, &ps);
}


void MainLoopImpl::processMoveMessage(Widget& widget, int wParam, int lParam)
{
    //std::cout << __FUNCTION__ <<std::endl;

    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    MoveEvent moveEvent(widget, x, y);
    event().send(moveEvent);
}


void MainLoopImpl::processSizeMessage(Widget& widget, int wParam, int lParam)
{
    int width  = LOWORD(lParam);
    int height = HIWORD(lParam);

    //std::cout << __FUNCTION__ << "  width: " << width << "   height: " << height << std::endl;

    ResizeEvent::Type resizeType;

    switch (wParam) {
        case SIZE_MAXIMIZED:
            resizeType = ResizeEvent::Maximized;
            break;

        case SIZE_MINIMIZED:
            resizeType = ResizeEvent::Minimized;
            break;

        case SIZE_RESTORED:
            resizeType = ResizeEvent::Restored;
            break;

        default:
            resizeType = ResizeEvent::Resize;
            break;
    }

    ResizeEvent resizeEvent(widget, width, height, resizeType);
    event().send(resizeEvent);
}

unsigned int MainLoopImpl::createModifiersFromMouseMessage(int wParam)
{
    //std::cout << __FUNCTION__ << std::endl;


    unsigned int modifiers = 0;

    // TODO Alt-Key is missing.
    if (wParam & MK_CONTROL) { modifiers |= MouseEvent::CtrlDown;         }
    if (wParam & MK_SHIFT)   { modifiers |= MouseEvent::ShiftDown;        }
    if (wParam & MK_LBUTTON) { modifiers |= MouseEvent::LeftButtonDown;   }
    if (wParam & MK_MBUTTON) { modifiers |= MouseEvent::MiddleButtonDown; }
    if (wParam & MK_RBUTTON) { modifiers |= MouseEvent::RightButtonDown;  }

    return modifiers;
}

} // namespace Gui

} // namespace Pt

