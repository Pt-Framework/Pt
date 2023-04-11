/* Copyright (C) 2013 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */

#include "posix/MainLoopImpl.h"
#include "ApplicationImpl.h"
#include "MainWindowImpl.h"
#include "PixmapSurfaceImpl.h"
#include "KeyHandler.h"

#ifndef _AIX
#include <X11/Xft/Xft.h>
#endif

#include <Pt/Hmi/Application.h>
#include <Pt/IOError.h>
#include <Pt/SourceInfo.h>

namespace Pt {

namespace Hmi {

ApplicationImpl::ApplicationImpl()
: _display( XOpenDisplay(NULL) )
, _visual(0)
, _depth(0)
, _xfd(_display)
{
    // Open a X11 display connection
    //_display = XOpenDisplay(NULL);
    if( ! _display )
        throw AccessFailed("X11 display");

    //XSync(_display, false);

    unsigned int screen = XDefaultScreen(_display);
    ::Window root = XDefaultRootWindow(_display);

    XVisualInfo visualInfo = {};
    if( ! XMatchVisualInfo(_display, screen, 24, TrueColor, &visualInfo) )
    {
        assert(false);
        throw std::runtime_error("invalid X11 visual");
    }

    _visual = visualInfo.visual;
    _depth = visualInfo.depth;

    // Set X11 to sync mode. Slow, for debugging only.
    //XSynchronize(_display, True);

    _xfd.setActive(*this);
    _xfd.begin();
    //_xfd.flush();
    _xfd.eventReady() += Pt::slot(*this, &ApplicationImpl::onProcessXEvent);

    _paintGc = XCreateGC(_display, root, 0, NULL);

    _wmProtocols     = XInternAtom(_display, "WM_PROTOCOLS", False);
    _wmDeleteWindow  = XInternAtom(_display, "WM_DELETE_WINDOW", False);
    _wmChangeState   = XInternAtom(_display, "WM_CHANGE_STATE", False);

    _netWmState = XInternAtom(_display, "_NET_WM_STATE", False);
    _netWmStateMaximizedVert = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
    _netWmStateMaximizedHorz = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    _netWmStateHidden = XInternAtom(_display, "_NET_WM_STATE_HIDDEN", False);
    _netWmStateAbove = XInternAtom(_display, "_NET_WM_STATE_ABOVE", False);
    _netWmActiveWindow = XInternAtom(_display, "_NET_ACTIVE_WINDOW", False);
}


ApplicationImpl::~ApplicationImpl()
{
    XFreeGC(_display, _paintGc);
    _paintGc = 0;

    //XSync(_display, true);
    XCloseDisplay(_display);
    _display = 0;
}


void ApplicationImpl::setCursor(const Cursor* cursor)
{
}


void ApplicationImpl::setFontDir(const Pt::System::Path& dir)
{
    PixmapSurfaceImpl::setFontDir(dir);
}

void ApplicationImpl::setDefaultFont(const std::string& fname)
{
    PixmapSurfaceImpl::setDefaultFont(fname);
}


Pt::Timespan ApplicationImpl::inactivityTime() const
{
  return Pt::Timespan(0);
}	


void ApplicationImpl::sendKeyEvent(const KeyEvent& ev)
{
    // XGetInputFocus(display, focus_return, revert_to_return)
    //     Display *display;
    //     Window *focus_return;
    //     int *revert_to_return;

    // Arguments

    // display    Specifies the connection to the X server.
    // focus_return    Returns the focus window, PointerRoot, or None.
    // revert_to_return    Returns the current focus state (RevertToParent, RevertToPointerRoot, or RevertToNone).

    // KeyEvent kev = ev;
    // kev.setId( window->vid() );
    // commitEvent(kev);
}


void ApplicationImpl::sendMouseEvent(const MouseEvent& ev)
{
    Window* w = 0; // TODO
    if( ! w )
        return;

    // screen to window coordinates
    Gfx::PointF pos = w->fromGlobal( ev.position() );

    MouseEvent mev = ev;
    mev.setPosition(pos);
    mev.setVisual(w);

    // TODO dispatch synthetic mouse event
}


void ApplicationImpl::nextEvent()
{
    MainLoop::waitNext();
}


void ApplicationImpl::onRun()
{
    _xfd.processEvents();

    while( MainLoop::impl()->waitNext() )
    {
        _xfd.processEvents();
    }
}


Window* ApplicationImpl::findWindow(::Window window)
{
    const std::vector<Window*>& windows = Application::instance().screen().windows();

    for(size_t i = 0; i < windows.size(); ++i)
    {
        Window* w = windows[i];

        if( ! w->impl() )
            continue;

        MainWindowImpl* windowImpl = static_cast<MainWindowImpl*>( w->impl() );

        if( windowImpl->window() == window )
            return w;
    }

    return 0;
}


void ApplicationImpl::onProcessXEvent(XEvent& ev)
{
    Window* w = findWindow(ev.xany.window);
    if( ! w )
        return;

    switch( ev.xany.type )
    {
        case ClientMessage:
            onClientMessage(*w, ev);
            break;

        case MapNotify:
            onShow(*w, true);
            break;

        case UnmapNotify:
            onShow(*w, false);
            break;

        case MotionNotify:
            onMotionNotify(*w, ev);
            break;

        case ButtonPress:
            onButtonPress(*w, ev);
            break;

        case ButtonRelease:
            onButtonRelease(*w, ev);
            break;

        case Expose:
            onExpose(*w, ev);
            break;

        case NoExpose:
            break;

        case ConfigureNotify:
            onConfigureNotify(*w, ev);
            break;

        case KeyPress:
        case KeyRelease:
            onKeyEvent(*w, ev);
            break;

        // case FocusOut:
        //     if( _forceTopMost )
        //         bringWindowToTop();
        // break;

        case EnterNotify:
            onEnterNotify(*w, ev);
            break;

        case LeaveNotify:
            onLeaveNotify(*w, ev);
            break;

        default:
            break;
    }
}


void ApplicationImpl::onEnterNotify(Window& window, XEvent& xev)
{
    //std::clog << "EnterNotify" << std::endl;
    //Application::instance().setPointerWindow(&window);
}


void ApplicationImpl::onLeaveNotify(Window& window, XEvent& xev)
{
    //std::clog << "LeaveNotify" << std::endl;

    Application::instance().screen().setPointer(0);
}


void ApplicationImpl::onExpose(Window& window, XEvent& xev)
{
    int width = xev.xexpose.width;
    int height = xev.xexpose.height;
    int x = xev.xexpose.x;
    int y = xev.xexpose.y;

    Gfx::PointF pos(x, y);
    Gfx::SizeF size(width, height);
    Gfx::RectF rect(pos, size);

    rect = Gfx::RectF( rect.topLeft() / window.scaleFactor(), 
                       rect.size() / window.scaleFactor() );

    //static int eee = 0;
    //std::clog << "EXPOSE NOTIFY " << eee++ << " "
    //                              << window.title() << " "
    //                              << rect.x() << ", " << rect.y() << " " 
    //                              << rect.width() << "x" << rect.height() << std::endl;

    WindowImpl* frame = window.impl();
    PaintEvent pev(*frame, rect);
    frame->processEvent(pev);

#ifdef PT_HMI_X11_CORE
    MainWindowImpl* windowImpl = static_cast<MainWindowImpl*>( window.impl() );
    ::Drawable from = windowImpl->surface().pixmapImpl()->drawable();
    ::Window to = windowImpl->window();

    XCopyArea( _display, from, to,
               _paintGc, x, y, width, height, x, y);
#endif

#ifdef PT_HMI_X11_RASTER
    MainWindowImpl* windowImpl = static_cast<MainWindowImpl*>( window.impl() );
    const Gfx::Image& image = windowImpl->surface().pixmapImpl()->image();
    char* data = reinterpret_cast<char*>( const_cast<Pt::uint8_t*>(image.data()) );

    //std::clog << "  EXPOSE " << window.title() << " "
    //                              << x << ", " << y << " " 
    //                              << image.width() << "x" << image.height() << std::endl;

    XImage* ximage = XCreateImage(_display, _visual, _depth, ZPixmap, 0, 
                                  data, image.width(), image.height(), 
                                  _depth == 24 ? 32 : _depth, 0);
    
    unsigned int screen = DefaultScreen(_display);
    ::Window drawable = windowImpl->window();
    GC gc = DefaultGC(_display, screen);
    
    XPutImage( _display, drawable, gc, ximage, x, y, x, y, width, height);
    
    ximage->data = NULL;
    XDestroyImage(ximage); 
#endif

    //XFlush(_display);
}


void ApplicationImpl::onClientMessage(Window& window, XEvent& xev)
{
    if( xev.xclient.message_type == _wmProtocols )
    {
        if( (Atom) xev.xclient.data.l[0] == _wmDeleteWindow )
        {
            WindowImpl* frame = window.impl();

            CloseEvent ev(*frame);
            commitEvent(ev);
        }
    }
}


void ApplicationImpl::onShow(Window& w, bool v)
{
    //std::clog << "   ### MapNotify: " << std::boolalpha << v << std::endl;

    ShowEvent sev(w, v);
    commitEvent( sev );
}


void ApplicationImpl::onMotionNotify(Window& window, XEvent& xev)
{
    int x = xev.xmotion.x;
    int y = xev.xmotion.y;

    const double scaling = window.scaleFactor();

    Pt::Gfx::PointF pos(x/scaling, y/scaling);

    _mouseEvent.setPosition( window.toGlobal(pos) );
    _mouseEvent.setVisual(&window);
    _mouseEvent.setMove();

    //std::clog << "MOVE: " << x << ", " << y << std::endl;

    Application::instance().processEvent(_mouseEvent);
}


void ApplicationImpl::onButtonPress(Window& window, XEvent& xev)
{
    //bool isPress = ButtonPress == xev.xbutton.type;
    int x = xev.xbutton.x;
    int y = xev.xbutton.y;
    MouseEvent::Button button = MouseEvent::Left;

    switch(xev.xbutton.button)
    {
        case Button1:
            button = MouseEvent::Left;
            break;

        case Button2:
            button = MouseEvent::Middle;
            break;

        case Button3:
            button = MouseEvent::Right;
            break;

        default:
            return;
    }
   
    const double scaling = window.scaleFactor();

    Pt::Gfx::PointF pos(x/scaling, y/scaling);
    _mouseEvent.setPosition( window.toGlobal(pos) );
    _mouseEvent.setPress(button);
    _mouseEvent.setVisual(&window);

    //std::clog << "PRESS: " << x << ", " << y << std::endl;

    Application::instance().processEvent(_mouseEvent);
}


void ApplicationImpl::onButtonRelease(Window& window, XEvent& xev)
{
    //bool isPress = ButtonPress == xev.xbutton.type;
    int x = xev.xbutton.x;
    int y = xev.xbutton.y;
    MouseEvent::Button button = MouseEvent::Left;

    // ev.xbutton.state contains pressed buttons

    switch(xev.xbutton.button)
    {
        case Button1:
            button = MouseEvent::Left;
            break;

        case Button2:
            button = MouseEvent::Middle;
            break;

        case Button3:
            button = MouseEvent::Right;
            break;

        case Button4: // wheel up
        {
            ScrollEvent sev(window);
            sev.set(ScrollEvent::Vertical, 20);
            Application::instance().processEvent(sev);
            return;
        }

        case Button5: // wheel down;
        {
            ScrollEvent sev(window);
            sev.set(ScrollEvent::Vertical, -20);
            Application::instance().processEvent(sev);
            return;
        }

        default:
            return;
    }

    const double scaling = window.scaleFactor();

    Pt::Gfx::PointF pos(x/scaling, y/scaling);
    _mouseEvent.setPosition( window.toGlobal(pos) );
    _mouseEvent.setRelease(button);
    _mouseEvent.setVisual(&window);

    //std::clog << "RELEASE: " << x << ", " << y << std::endl;

    Application::instance().processEvent(_mouseEvent);
}


void ApplicationImpl::onKeyEvent(Window& window, XEvent& xev)
{
    bool isPress = KeyPress == xev.xkey.type;

    KeySym keySym = 0;
    char buffer[20];
    XLookupString(&xev.xkey, buffer, sizeof(buffer), &keySym, NULL);

    Key::Modifiers modifiers;

    if(xev.xkey.state & ShiftMask)
    {
        modifiers.add(Key::Shift);
    }
    if(xev.xkey.state & ControlMask)
    {
        modifiers.add(Key::Control);
    }
    if(xev.xkey.state & Mod1Mask)
    {
        modifiers.add(Key::Alt);
    }
    if(xev.xkey.state & Mod5Mask)
    {
        modifiers.add(Key::Alt);
    }
    if(xev.xkey.state & Mod4Mask)
    {
        modifiers.add(Key::Meta);
    }

    Pt::Char ch = KeyHandler::keySymToUtf(keySym);
    Pt::uint32_t keyCode = KeyHandler::keySymToCode(keySym);

    Key key(modifiers, keyCode);

    if(isPress)
        _keyEvent.setPress(key, ch);
    else
        _keyEvent.setRelease(key, ch);

    _keyEvent.setVisual(&window);
    
    Application::instance().processEvent(_keyEvent);
}


void ApplicationImpl::onConfigureNotify(Window& window, XEvent& xev)
{
    //static int ccc = 0;
    //std::clog << "CONFIGURE NOTIFY: " << ccc++ << " "
    //                                  << window.title() << std::endl;

    // Use only last configure event for the window in queue
    //XPending(_display);

    //while( XCheckTypedWindowEvent(_display, xev.xany.window, ConfigureNotify, &xev) )
    //    ;

    // PropertyNotify:
    //
    // if( isWindowMinimized() )
    //     _resizeEvent.setState( WindowState::Minimized );
    // else if( isWindowMaximized() )
    //     _resizeEvent.setState( WindowState::Maximazed );
    // else
    //     _resizeEvent.setState( WindowState::Normal );

    MainWindowImpl* windowImpl = static_cast<MainWindowImpl*>( window.impl() );

    const int width  = xev.xconfigure.width;
    const int height = xev.xconfigure.height;
    const int x      = xev.xconfigure.x;
    const int y      = xev.xconfigure.y;

    if( windowImpl->width() != width || windowImpl->height() != height )
    {
        windowImpl->setSize(width, height);

        //static int rrr = 0;
        //std::clog << "  RESIZE: " << rrr++ << " "
        //                          << window.title() << " " 
        //                          << width << "x" << height << std::endl;
        Gfx::SizeF to(width, height);
        to = window.surface().toLogical(to);
    
        ResizeEvent rev(*window.impl(), to);
        Application::instance().processEvent(rev);

        Gfx::RectF updateRect(Gfx::PointF(0,0), to);
        window.repaint(updateRect);
    }

    Gfx::PointF currentPos = window.surface().toPhysical( window.position() );

    if( x != currentPos.x() || y != currentPos.y() )
    {
        //static int mmm = 0;
        //std::clog << "  MOVE: " << mmm++ << " " 
        //                        << window.title() << " " 
        //                        << x << "," << y << std::endl;
        Gfx::PointF to(x, y);
        to = window.surface().toLogical(to);

        MoveEvent ev(*window.impl(), to);
        Application::instance().processEvent(ev);
    }
}

} // namespace

} // namespace
