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

namespace Pt { 

namespace Hmi { 

enum
{
  _NET_WM_STATE_REMOVE = 0,
  _NET_WM_STATE_ADD    = 1,
  _NET_WM_STATE_TOGGLE = 2
};


MainWindowImpl::MainWindowImpl(Window::Type type)
: _window(None)
, _display(0)
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

    XSetWindowAttributes wattr;
    memset(&wattr, 0, sizeof(wattr));
    wattr.colormap = DefaultColormap(_display, screen);

    // The events we want to receive
    wattr.event_mask = StructureNotifyMask|ExposureMask|
                       PropertyChangeMask|EnterWindowMask|
                       LeaveWindowMask|KeyPressMask|
                       KeyReleaseMask|KeymapStateMask|
                       ButtonPressMask|ButtonReleaseMask|
                       PointerMotionMask|FocusChangeMask|
                       SubstructureNotifyMask;

    wattr.do_not_propagate_mask = 0;/*KeyPressMask|KeyReleaseMask|
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
    wattr.override_redirect = False; 

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
                            XDefaultDepth(_display, screen), 
                            InputOutput, 
                            XDefaultVisual(_display, screen), 
                            winMask, 
                            &wattr);

    Atom atomDeleteWindow = XInternAtom(_display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(_display, _window, &atomDeleteWindow, 1);
    //XSync(_display, False);

    XUnmapWindow(_display, _window);
}


void MainWindowImpl::destroy()
{
    if( _window == 0)
        return;
    
    XDestroyWindow(_display, _window);
    _window = 0;

    //XSync(_display, False);
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


void MainWindowImpl::close()
{
    XEvent ev;
    memset(&ev, 0, sizeof (ev));
 
    ev.xclient.type         = ClientMessage;
    ev.xclient.window       = _window;
    ev.xclient.message_type = XInternAtom(_display, "WM_PROTOCOLS", true);
    ev.xclient.format       = 32;
    ev.xclient.data.l[0]    = XInternAtom(_display, "WM_DELETE_WINDOW", false);
    ev.xclient.data.l[1]    = CurrentTime;
    XSendEvent(_display, _window, False, NoEventMask, &ev);
}


void MainWindowImpl::paint(const Gfx::RectF& rect)
{    
    std::clog << "MainWindowImpl::paint" << std::endl;

    unsigned int screen = XDefaultScreen(_display);

    const std::vector<Window*>& windows = Application::instance().screen().windows();

    std::vector<Window*>::const_iterator it;
    for(it = windows.begin(); it != windows.end(); ++it)
    {
        if( (*it)->impl() == this )
        {
            const Pt::Gfx::SizeF& size = (*it)->surface().size();

            std::clog << "paint backbuffer to window: "
                      << rect.width() << "x" << rect.height() << std::endl;
    
            ::Drawable from = (*it)->surface().pixmapImpl()->drawable();
            ::Window to = _window;
            

             XCopyArea( _display, from, to, XDefaultGC(_display, screen), 
                        0, 0, size.width(), size.height(), 0, 0);

            XSync(_display, False);
            break;
        }
    }
}


void MainWindowImpl::show(bool visible)
{    
    if(visible)
    {      
        // TODO: does TOPMOST state survive a hide/show?
        //setTopMost(_isTopMost);    
        std::clog << "show: map" << std::endl;
        XMapWindow(_display, _window);
    }
    else
    {
        std::clog << "show: unmap" << std::endl;
        XUnmapWindow(_display, _window);
    }
    
    XSync(_display, False);
    XFlush(_display);
    std::clog << "pending: " << XPending(_display) << std::endl;
}


void MainWindowImpl::activate()
{
    //XSetInputFocus(_display, _window, RevertToNone, CurrentTime);
}


void MainWindowImpl::enable(bool enabled)
{
    // XSetWindowAttributes wattr;
    // memset(&wattr, 0, sizeof(wattr));

    // //XGetWindowAttributes(_display, _window, &wattr)

    // if(enabled)
    // {
    //     wattr.event_mask = StructureNotifyMask|ExposureMask|
    //                        PropertyChangeMask|EnterWindowMask|
    //                        LeaveWindowMask|KeyPressMask|
    //                        KeyReleaseMask|KeymapStateMask|
    //                        ButtonPressMask|ButtonReleaseMask|
    //                        PointerMotionMask|FocusChangeMask|
    //                        SubstructureNotifyMask;

    //     wattr.override_redirect = False; 
    // }
    // else
    // {
    //     wattr.event_mask = 0;
    //     wattr.override_redirect = False; 
    // }

    // unsigned long winMask = CWEventMask|CWOverrideRedirect;

    // XChangeWindowAttributes(_display, _window, winMask, &wattr);
}


void MainWindowImpl::setTopMost(bool topMost)
{
    std::clog << "setTopMost: " << topMost << std::endl;

    Atom wm_state = XInternAtom(_display, "_NET_WM_STATE", False);
    Atom wm_state_above = XInternAtom(_display, "_NET_WM_STATE_ABOVE", False);

    // ClientMessage event
    XEvent event;
    event.xclient.type = ClientMessage;

    // value unimportant in this case
    event.xclient.serial = 0;

    // coming from a SendEvent request, so True
    event.xclient.send_event = True;

    // the event originates from disp
    event.xclient.display = _display;

    // the window whose state will be modified
    event.xclient.window = _window;

    // the component Atom being modified in the window
    event.xclient.message_type = wm_state;

    // specifies that data.l will be used
    event.xclient.format = 32;

    // _NET_WM_STATE_ADD or _NET_WM_STATE_REMOVE
    event.xclient.data.l[0] = topMost ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;

    // the atom being added
    event.xclient.data.l[1] = wm_state_above;

    // unused
    event.xclient.data.l[2] = 0;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;
    
    XSendEvent(_display, XDefaultRootWindow(_display), False,
               SubstructureRedirectMask, &event);
}


void MainWindowImpl::move(const Gfx::PointF& pos)
{
    XMoveWindow(_display, _window, pos.x(), pos.y());

    std::clog << "move: " << pos.x() << "x" << pos.y() << std::endl;

    XSync(_display, False);
    std::clog << "pending: " << XPending(_display) << std::endl;
}


void MainWindowImpl::resize(const Gfx::SizeF& size)
{
    XResizeWindow( _display, _window, size.width(), size.height() );

    std::clog << "resize: " << size.width() << "x" << size.height() << std::endl;

    XSync(_display, False);
    std::clog << "pending: " << XPending(_display) << std::endl;

    // TODO: try to poll() here on ConnectionNumber fd
}


void MainWindowImpl::setType(Window::Type type)
{
    // TODO
    std::clog << "Window::setType not implemented" << std::endl;
}


void MainWindowImpl::setIcon(const Gfx::Image& icon)
{
    // TODO
    std::clog << "Window::setIcon not implemented" << std::endl;
}


void MainWindowImpl::setTitle(const std::string& text)
{
    XStoreName(_display, _window, text.c_str());    
}


void MainWindowImpl::setMinimumSize(const Gfx::SizeF& s)
{
//    XSizeHints sh;
//    memset(&sh, 0, sizeof(sh));
//    sh.flags = PMinSize | PMaxSize;
//    sh.min_width  = sh.max_width  = width;
//    sh.min_height = sh.max_height = height;
//    XSetStandardProperties(display, window, window_name, icon_name, icon_pixmap, argv, argc, hints)

}


void MainWindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
}


void MainWindowImpl::setState(Window::State s)
{
    XClientMessageEvent ev;
        
    ev.type   = ClientMessage;
    ev.window = _window;
    ev.format = 32;

    switch(s)
    {
        default:
        case Window::Normal:
            ev.message_type = XInternAtom(_display, "WM_CHANGE_STATE", False);
            ev.data.l[0] = NormalState;
            break;

        case Window::Minimized:
            ev.message_type = XInternAtom(_display, "WM_CHANGE_STATE", False);
            ev.data.l[0] = IconicState;
            break;

        case Window::Maximized:
            ev.serial     = 0;
            ev.send_event = True;
            ev.message_type = XInternAtom(_display, "_NET_WM_STATE",False);
            ev.data.l[0]  = (unsigned long)1;
            ev.data.l[1]  = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
            ev.data.l[2]  = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
            break;
    }

    XSendEvent(_display, DefaultRootWindow(_display), 
               False, SubstructureRedirectMask|SubstructureNotifyMask,
               (XEvent *)&ev);    

    //if(s == Window::Maximized)
    //{
    //    XRaiseWindow(_display,_window);
    //}
}

void MainWindowImpl::grabPointer()
{
    XGrabPointer(_display, _window, True, 
                 ButtonPressMask|ButtonReleaseMask|
                 PointerMotionMask,
                 GrabModeAsync,
                 GrabModeAsync, 
                 None, None, CurrentTime);
}


bool MainWindowImpl::isMinimized()
{
    Atom actual_type;
    int actual_format;
    unsigned long num_items, bytes_after;
    Atom* atoms = 0;        
    Atom requestAtom = XInternAtom(_display,"_NET_WM_STATE", False);
    Atom compareAtom = XInternAtom(_display,"_NET_WM_STATE_HIDDEN", False);
        
    XGetWindowProperty(_display, _window, 
                       requestAtom, 0, 1024, False, XA_ATOM, 
                       &actual_type, &actual_format, 
                       &num_items, &bytes_after, 
                       (unsigned char**)&atoms);
        
    for(unsigned long i = 0; i < num_items; ++i)
    {
        if(atoms[i]==compareAtom)
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
    Atom requestAtom = XInternAtom(_display,"_NET_WM_STATE", False);
    Atom compareAtom = XInternAtom(_display,"_NET_WM_STATE_MAXIMIZED_HORZ", False);

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
