/* Copyright (C) 2013 Marc Boris Duerner

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

#include "MainWindowImpl.h"
#include "ApplicationImpl.h"
#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>

#include <cassert>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(Window::Type type)
: _window(None)
, _display(0)
, _hasFirstShow(false)
, _scalingFactor(1.0)
, _width(240)
, _height(160)
{
  _display = Application::instance().impl()->display();

  create(type);
}


MainWindowImpl::~MainWindowImpl()
{
    destroy();
}


void MainWindowImpl::create(Window::Type type)
{
    // Display and Screen are inited in Application
    unsigned int screen = XDefaultScreen(_display);
    ::Window root       = XRootWindow(_display, screen);
    ::Visual* visual = Application::instance().impl()->visual();
    int depth = Application::instance().impl()->depth();

    XSetWindowAttributes wattr;
    memset(&wattr, 0, sizeof(wattr));
    wattr.colormap = XDefaultColormap(_display, screen);

    // The events we want to receive
    wattr.event_mask = StructureNotifyMask|ExposureMask|
                       PropertyChangeMask|EnterWindowMask|
                       LeaveWindowMask|KeyPressMask|
                       KeyReleaseMask|KeymapStateMask|
                       ButtonPressMask|ButtonReleaseMask|
                       PointerMotionMask|FocusChangeMask|
                       SubstructureNotifyMask|VisibilityChangeMask|
                       FocusChangeMask|EnterWindowMask|
                       LeaveWindowMask;

    wattr.do_not_propagate_mask = NoEventMask;/*KeyPressMask|KeyReleaseMask|
                                  ButtonPressMask| ButtonReleaseMask|
                                  PointerMotionMask|ButtonMotionMask;*/

    // Border
    wattr.border_pixel = 0; // Needed for OpenGL
    wattr.border_pixmap = CopyFromParent;

    // Background
    wattr.background_pixmap = None;
    //wattr.background_pixel = XWhitePixel(display, screen);

    // Backing store
    wattr.backing_store = NotUseful; // Always;
    wattr.save_under = False;   // True;

    // Region to be retained on resize
    wattr.bit_gravity = ForgetGravity;

    // How to to reposition when parent resizes
    wattr.win_gravity = NorthWestGravity;

    // None means parents cursor
    wattr.cursor = None;

    // no WM interaction if True
    wattr.override_redirect = type == WindowType::Popup ? True : False;

    // Determines which fields from XSetWindowAttributes are used
    unsigned long winMask = CWWinGravity|CWBitGravity|
                            CWBorderPixmap|CWBorderPixel|
                            CWEventMask|CWDontPropagate|
                            CWCursor|CWOverrideRedirect|
                            CWColormap|CWBackingStore|
                            CWSaveUnder|CWBackPixmap;

    unsigned int borderWidth = 0;

    // Create the X11 window
    _window = XCreateWindow(_display,
                            root,
                            0, 0,
                            _width, _height,
                            borderWidth,
                            depth,
                            InputOutput,
                            visual,
                            winMask,
                            &wattr);

    //std::clog << std::hex << "XCreateWindow: " << _window << std::dec << std::endl;

    Atom atomDeleteWindow = Application::instance().impl()->wmDeleteWindow();
    XSetWMProtocols(_display, _window, &atomDeleteWindow, 1);
}


void MainWindowImpl::destroy()
{
    if( _window == 0)
        return;

    //std::clog << std::hex << "XDestroyWindow: " << _window << std::dec << std::endl;

    XDestroyWindow(_display, _window);
    _window = 0;
}


double MainWindowImpl::scaleFactor() const
{
    return _scalingFactor;
}


Gfx::PointF MainWindowImpl::toScreen(const Gfx::PointF& windowPos) const
{
    ::Window root = DefaultRootWindow(_display);
    int windowX = lround( windowPos.x() );
    int windowY = lround( windowPos.y() );
    int screenX = 0;
    int screenY = 0;
    ::Window child = 0;

    XTranslateCoordinates(_display, _window, root,
                          windowX, windowY, &screenX, &screenY, &child);

    return Gfx::PointF(screenX, screenY);
}


Gfx::PointF MainWindowImpl::fromScreen(const Gfx::PointF& screenPos) const
{
    ::Window root = DefaultRootWindow(_display);
    int screenX = lround( screenPos.x() );
    int screenY = lround( screenPos.y() );
    int windowX = 0;
    int windowY = 0;
    ::Window child = 0;

    XTranslateCoordinates(_display, root, _window,
                          screenX, screenY, &windowX, &windowY, &child);

    return Gfx::PointF(windowX, windowY);
}


void MainWindowImpl::paint(const Gfx::RectF& rectF)
{
    Gfx::Rect rect = Gfx::round(rectF);
    if( rect.isNull() )
      return;

    //std::clog << "XClearArea " << title() << " "
    //                           << rectF.x() << ", " << rectF.y() << " "
    //                           << rectF.width() << "x" << rectF.height() << std::endl;

    //XExposeEvent ev = { Expose, 0, True, _display, _window,
    //                   static_cast<int>( rect.x()),
    //                   static_cast<int>( rect.y()),
    //                   static_cast<int>( rect.width()),
    //                   static_cast<int>( rect.height()),
    //                   0 };

    //Application::instance().impl()->processEvent( (XEvent&)ev);

    XClearArea(_display, _window,
               static_cast<int>( rect.x() ),
               static_cast<int>( rect.y() ),
               static_cast<int>( rect.width() ),
               static_cast<int>( rect.height() ),
               True);

    //XFlush(_display);
}


void MainWindowImpl::show(bool visible)
{
    if(visible)
    {
        //std::clog  << "XMapWindow" << std::endl;
        XMapWindow(_display, _window);
        //XFlush(_display);

        //while( XPending(_display) > 0 )
        //{
        //    XEvent xev;
        //    XNextEvent(_display, &xev);

        //    Application::instance().impl()->processEvent(xev);
        //}
    }
    else
    {
        //std::clog  << "XUnmapWindow" << std::endl;
        XUnmapWindow(_display, _window);
        //XFlush(_display);
    }
}


void MainWindowImpl::activate()
{
    //XSetInputFocus(_display, _window, RevertToNone, CurrentTime);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.display      = _display;
    xev.xclient.window       = _window;
    xev.xclient.message_type = Application::instance().impl()->netWmActiveWindow();
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 2L; /* 2 == Message from a window pager */
    xev.xclient.data.l[1]    = CurrentTime;

    XWindowAttributes wattr;
    XGetWindowAttributes(_display, _window, &wattr);

    XSendEvent(_display, XDefaultRootWindow(_display),
               False, SubstructureNotifyMask|SubstructureRedirectMask, &xev);

    //XFlush(_display);
}


void MainWindowImpl::enable(bool enabled)
{
    //std::clog  << "XChangeWindowAttributes: " << enabled << std::endl;

    XSetWindowAttributes wattr;

    if(enabled)
    {
        wattr.event_mask = StructureNotifyMask|ExposureMask|
                           PropertyChangeMask|EnterWindowMask|
                           LeaveWindowMask|KeyPressMask|
                           KeyReleaseMask|KeymapStateMask|
                           ButtonPressMask|ButtonReleaseMask|
                           PointerMotionMask|FocusChangeMask|
                           SubstructureNotifyMask;
    }
    else
    {
        wattr.event_mask = 0;
    }

    XChangeWindowAttributes(_display, _window, CWEventMask, &wattr);
    //XFlush(_display);
}


void MainWindowImpl::move(const Gfx::PointF& pos)
{
    //std::clog  << "XMoveWindow: " << pos.x() << ", " << pos.y() << std::endl;
    XMoveWindow(_display, _window, pos.x(), pos.y());
    //XFlush(_display);

    //while( XPending(_display) > 0 )
    //{
    //    XEvent xev;
    //    XNextEvent(_display, &xev);

    //    Application::instance().impl()->processEvent(xev);
    //}
}


void MainWindowImpl::resize(const Gfx::SizeF& size)
{
    //std::clog  << "XResizeWindow: " << size.width()
    //           << "x" << size.height() << std::endl;

    XResizeWindow( _display, _window, size.width(), size.height() );
    //XFlush(_display);

    //XClearArea(_display, _window, 0, 0, size.width(), size.height(), True);
    //XFlush(_display);

    //while( XPending(_display) > 0 )
    //{
    //    XEvent xev;
    //    XNextEvent(_display, &xev);

    //    Application::instance().impl()->processEvent(xev);
    //}
}


void MainWindowImpl::onSetType(Window::Type type)
{
    //std::clog << "XChangeWindowAttributes: " << type << std::endl;

    XSetWindowAttributes swattr;
    swattr.override_redirect = (type == WindowType::Popup) ? True : False;

    XChangeWindowAttributes(_display, _window, CWOverrideRedirect, &swattr);

    XWindowAttributes wattr;
    XGetWindowAttributes(_display, _window, &wattr);

    if(wattr.map_state != IsUnmapped)
    {
        XUnmapWindow(_display, _window);
        XMapWindow(_display,_window);
    }
}


void MainWindowImpl::setAbove(bool above)
{
    //std::clog << "setTopMost: " << topMost << std::endl;

    enum
    {
    _NET_WM_STATE_REMOVE = 0,
    _NET_WM_STATE_ADD    = 1,
    _NET_WM_STATE_TOGGLE = 2
    };

    XEvent ev;
    ev.xclient.type = ClientMessage;
    ev.xclient.serial = 0; // not important
    ev.xclient.send_event = True;
    ev.xclient.display = _display;
    ev.xclient.window = _window; // modified window
    ev.xclient.message_type = Application::instance().impl()->netWmState();
    ev.xclient.format = 32; // use data.l
    ev.xclient.data.l[0] = above ? _NET_WM_STATE_ADD
                                 : _NET_WM_STATE_REMOVE;
    ev.xclient.data.l[1] = Application::instance().impl()->netWmStateAbove();
    ev.xclient.data.l[2] = 0;
    ev.xclient.data.l[3] = 0;
    ev.xclient.data.l[4] = 0;

    XSendEvent(_display, XDefaultRootWindow(_display), False,
               SubstructureRedirectMask, &ev);
}


void MainWindowImpl::setTitle(Window& w, const std::string& text)
{
    XStoreName(_display, _window, text.c_str());
}


void MainWindowImpl::setIcon(Window& w, const Gfx::Image& icon)
{
    //std::clog << "XAllocWMHints" << std::endl;

    XWMHints* hints = XAllocWMHints();
    if ( ! hints )
        return;

    hints->flags = IconPixmapHint | IconPositionHint;
    hints->icon_pixmap = None;
    hints->icon_x = 0;
    hints->icon_y = 0;

    XSetWMHints(_display, _window, hints);
    XFree(hints);
}


void MainWindowImpl::onSetState(Window::State s)
{
    //std::clog  << "setState: " << s << std::endl;

    XClientMessageEvent ev;
    ev.type       = ClientMessage;
    ev.serial     = 0;
    ev.window     = _window;
    ev.format     = 32;
    ev.send_event = True;

    switch(s)
    {
        default:
        case WindowState::Normal:
            ev.message_type = Application::instance().impl()->wmChangeState();
            ev.data.l[0] = NormalState;
            break;

        case WindowState::Minimized:
            ev.message_type = Application::instance().impl()->wmChangeState();
            ev.data.l[0] = IconicState;
            break;

        case WindowState::Maximized:
            ev.message_type = Application::instance().impl()->netWmState();
            ev.data.l[0]  = 1ul;
            ev.data.l[1]  = Application::instance().impl()->netWmStateMaximizedVert();
            ev.data.l[2]  = Application::instance().impl()->netWmStateMaximizedHorz();
            break;
    }

    XSendEvent(_display, XDefaultRootWindow(_display),False,
               SubstructureRedirectMask,
               (XEvent*)&ev);

    //XFlush(_display);
}



void MainWindowImpl::onSetMinimumSize(const Gfx::SizeF& s)
{
    XSizeHints hints;
    memset(&hints, 0, sizeof(hints));

    hints.flags = PMinSize;
    hints.min_width  = lround( s.width() );
    hints.min_height = lround( s.height() );

    XSetWMNormalHints(_display, _window, &hints);
}


void MainWindowImpl::onSetMaximumSize(const Gfx::SizeF& s)
{
    XSizeHints hints;
    memset(&hints, 0, sizeof(hints));

    hints.flags = PMaxSize;
    hints.max_width  = lround( s.width() );
    hints.max_height = lround( s.height() );

    XSetWMNormalHints(_display, _window, &hints);
}


//void MainWindowImpl::grabPointer()
//{
//    XGrabPointer(_display, _window, True,
//                 ButtonPressMask|ButtonReleaseMask|
//                 PointerMotionMask,
//                 GrabModeAsync,
//                 GrabModeAsync,
//                 None, None, CurrentTime);
//}


bool MainWindowImpl::isMinimized()
{
    Atom actual_type;
    int actual_format;
    unsigned long num_items, bytes_after;
    Atom* atoms = 0;
    Atom requestAtom = Application::instance().impl()->netWmState();
    Atom compareAtom = Application::instance().impl()->netWmStateHidden();

    XGetWindowProperty(_display, _window,
                       requestAtom, 0, 1024, False, XA_ATOM,
                       &actual_type, &actual_format,
                       &num_items, &bytes_after,
                       (unsigned char**)&atoms);

    for(unsigned long i = 0; i < num_items; ++i)
    {
        if(atoms[i] == compareAtom)
        {
            XFree(atoms);
            return true;
        }
    }

    XFree(atoms);
    return false;
}


bool MainWindowImpl::isMaximized()
{
    Atom actual_type;
    int actual_format;
    unsigned long num_items, bytes_after;
    Atom* atoms = 0;
    Atom requestAtom = Application::instance().impl()->netWmState();
    Atom compareAtom = Application::instance().impl()->netWmStateMaximizedHorz();

    XGetWindowProperty(_display, _window,
                       requestAtom, 0, 1024, False, XA_ATOM,
                       &actual_type, &actual_format,
                       &num_items, &bytes_after,
                       (unsigned char**)&atoms);

    for(unsigned long i = 0; i < num_items; ++i)
    {
        if( atoms[i] == compareAtom )
        {
            XFree(atoms);
            return true;
        }
    }

    XFree(atoms);
    return 0;
}

} // namespace

} // namespace
