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
, _xfd(_display)
, _mouseEvent(0)
, _keyEvent(0)
{
    // Open a X11 display connection
    //_display = XOpenDisplay(NULL);
    if( ! _display )
        throw AccessFailed("X11 display");

    //XSync(_display, false);

    ::Window root = XDefaultRootWindow(_display);

    // Set X11 to sync mode. Slow, for debugging only.
    XSynchronize(_display, True);

    _xfd.setActive(*this);
    _xfd.begin();
    //_xfd.flush();
    _xfd.eventReady() += Pt::slot(*this, &ApplicationImpl::onEvent);

    _paintGc = XCreateGC(_display, root, 0, NULL);

    _atomProtocols    = XInternAtom(_display, "WM_PROTOCOLS", false);
	_atomDeleteWindow = XInternAtom(_display, "WM_DELETE_WINDOW", false);
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


void ApplicationImpl::grabPointer(Window& grabber)
{
    grabber.mainWindow().impl()->grabPointer();
}


void ApplicationImpl::releasePointer(Window& grabber)
{
    XUngrabPointer(_display, CurrentTime);
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
    XUngrabPointer(_display, CurrentTime);
}


void ApplicationImpl::sendKeyEvent(const KeyEvent& ev)
{
    // XGetInputFocus(display, focus_return, revert_to_return)
    //     Display *display;
    //     Window *focus_return;
    //     int *revert_to_return;

    // Arguments

    // display	Specifies the connection to the X server.
    // focus_return	Returns the focus window, PointerRoot, or None.
    // revert_to_return	Returns the current focus state (RevertToParent, RevertToPointerRoot, or RevertToNone).

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
    Gfx::PointF pos = w->fromScreen( ev.position() );

    MouseEvent mev = ev;
    mev.setPosition(pos);
    mev.setId( w->vid() );

    // TODO dispatch synthetic mouse event
}


void ApplicationImpl::nextEvent()
{
	MainLoop::waitNext();
}


void ApplicationImpl::onRun()
{
    //_xfd.flush();    
    MainLoop::onRun();
}


Window* ApplicationImpl::findWindow(::Window window)
{
    const std::vector<Window*>& windows = Application::instance().screen().windows();

    for(size_t i = 0; i < windows.size(); ++i)
    {
        Window* w = windows[i];

        if( ! w->impl() )
            continue;
        
        if( w->impl()->window() == window )
            return w;
    }
    
    return 0;
}


void ApplicationImpl::onEvent(XEvent& ev)
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
        // 	if( _forceTopMost )
        // 		bringWindowToTop();
        // break;

        case EnterNotify:    
            break;

        case LeaveNotify:			
            break;

        default:
            break;
    }
}


void ApplicationImpl::onExpose(Window& window, XEvent& xev)
{
    //  std::clog << "Expose: " << window.size().width()
    //            << "x" << window.size().height() << std::endl;

    // const size_t width = xev.xexpose.width;
    // const size_t height = xev.xexpose.height;
    // const size_t x = xev.xexpose.x;
    // const size_t y = xev.xexpose.y;

    // Gfx::RectF rect( Gfx::PointF(x, y), 
    //                  Gfx::SizeF(width, height) );

    //PaintEvent pev( window.vid(), rect);
    //Application::instance().loop().commitEvent(pev);



    // ::Drawable from = window.surface().pixmapImpl()->drawable();
    // ::Window to = window.impl()->window();
    // const Pt::Gfx::SizeF& size = window.surface().size();

    // std::clog << "expose backbuffer to window: "
    //           << size.width() << "x" << size.height() << std::endl;

    // XCopyArea( _display, from, to, 
    //            _paintGc, 0, 0, size.width(), size.height(), 0, 0);

    // XSync(_display, False);
}


void ApplicationImpl::onClientMessage(Window& window, XEvent& xev)
{
    Pt::uint64_t id =  window.vid();

	if( xev.xclient.message_type == _atomProtocols ) 
	{
        if( (Atom) xev.xclient.data.l[0] == _atomDeleteWindow)
        {
            CloseEvent closeEvent(id);
            window.processEvent(closeEvent);
        }
	}	
}


void ApplicationImpl::onShow(Window& w, bool v)
{
    std::clog << "MapNotify: " << v << std::endl;

    ShowEvent sev(w.vid(), v);
    commitEvent( sev );

    w.invalidate();
    w.update();
}


void ApplicationImpl::onMotionNotify(Window& window, XEvent& xev)
{
    std::size_t x = xev.xmotion.x;
    std::size_t y = xev.xmotion.y;

	Pt::Gfx::PointF pos(x, y);
    _mouseEvent.setPosition(pos);
    _mouseEvent.setId( window.vid() );
    _mouseEvent.setMove();

    // TODO: call Application::processMouseEvent which returns true if the
    //       event was consumed. If it returns false and the event was not
    //       consumed call ApplicationImpl::dispatchMouseEvent

    Application::instance().processMouseEvent(_mouseEvent);
}


void ApplicationImpl::onButtonPress(Window& window, XEvent& xev)
{
    //bool isPress = ButtonPress == xev.xbutton.type;
    std::size_t x = xev.xbutton.x;
    std::size_t y = xev.xbutton.y;
    MouseEvent::Button button = MouseEvent::Left;

    switch(xev.xbutton.button) 
    {
        default:
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
            break;
        
        case Button5: // wheel down; 
            break;
    }

    Pt::Gfx::PointF pos(x, y);
    _mouseEvent.setPosition(pos);
    _mouseEvent.setPress(button);
    _mouseEvent.setId( window.vid() );

    // TODO: call Application::processMouseEvent which returns true if the
    //       event was consumed. If it returns false and the event was not
    //       consumed call ApplicationImpl::dispatchMouseEvent

    Application::instance().processMouseEvent(_mouseEvent);
}


void ApplicationImpl::onButtonRelease(Window& window, XEvent& xev)
{
    //bool isPress = ButtonPress == xev.xbutton.type;
    std::size_t x = xev.xbutton.x;
    std::size_t y = xev.xbutton.y;
    MouseEvent::Button button = MouseEvent::Left;

    switch(xev.xbutton.button) 
    {
        default:
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
            break;
        
        case Button5: // wheel down; 
            break;
    }

    Pt::Gfx::PointF pos(x, y);
    _mouseEvent.setPosition(pos);
    _mouseEvent.setRelease(button);
    _mouseEvent.setId( window.vid() );

    // TODO: call Application::processMouseEvent which returns true if the
    //       event was consumed. If it returns false and the event was not
    //       consumed call ApplicationImpl::dispatchMouseEvent

    Application::instance().processMouseEvent(_mouseEvent);
}


void ApplicationImpl::onKeyEvent(Window& window, XEvent& xev)
{
    bool isPress = KeyPress == xev.xkey.type;

    KeySym keySym = 0;
    char buffer[20]; // For dummy since the X function wants it
    XLookupString(&xev.xkey, buffer, sizeof(buffer), &keySym, NULL);

    Key::Modifiers modifiers;

    // int modifiers = 0;
    // if( xev.xkey.state & ShiftMask)
    //     modifiers |= MouseMoveEvent::ShiftDown;
    // if( xev.xkey.state & Button1Mask)
    //     modifiers |= MouseMoveEvent::LeftButtonDown;
    // if( xev.xkey.state & Button3Mask)
    //     modifiers |= MouseMoveEvent::RightButtonDown;
    // if( xev.xkey.state & Button2Mask)
    //     modifiers |= MouseMoveEvent::MiddleButtonDown;
    // if( xev.xkey.state & ControlMask)
    //     modifiers |= MouseMoveEvent::CtrlDown;
    // if( xev.xkey.state & Mod1Mask)
    //     modifiers |= MouseMoveEvent::AltDown;

	switch(keySym) 
	{
		case XK_Control_L: 
		case XK_Control_R: 
			modifiers.add(Key::Control);
		    break;

		case XK_Alt_L: 
		case XK_Alt_R:
			modifiers.add(Key::Alt);
            break;

		case XK_Shift_L :
		case XK_Shift_R :
			modifiers.add(Key::Shift);
		    break;

        // TODO modifiers.add(Key::Meta);

		default:
		    break;
	}		
		
	Pt::Char ch = KeyHandler::keySymToUtf(keySym);

    //TODO: translate keySym to Key::Code
    Pt::uint32_t keyCode = Key::NoKey;

    if(keySym == XK_A)
        keyCode = Key::A;

    Key key(modifiers, keyCode);

    if(isPress)
        _keyEvent.setPress(key, ch);
    else
        _keyEvent.setRelease(key, ch); 

    _keyEvent.setId( window.vid() );
    commitEvent(_keyEvent);
}


void ApplicationImpl::onConfigureNotify(Window& window, XEvent& xev)
{
    std::clog << "ConfigureNotify" << std::endl;

    // Use only last configure event for the window in queue
    XPending(_display);
        
    while( XCheckTypedWindowEvent(_display, xev.xany.window, ConfigureNotify, &xev) )
        ;

    // PropertyNotify:
    //
    // if( isWindowMinimized() )    
    //     _resizeEvent.setState( WindowState::Minimized );
    // else if( isWindowMaximized() )
    //     _resizeEvent.setState( WindowState::Maximazed );
    // else
    //     _resizeEvent.setState( WindowState::Normal );

    const int width  = xev.xconfigure.width;
    const int height = xev.xconfigure.height;
    const int x      = xev.xconfigure.x;
    const int y      = xev.xconfigure.y;

    if( window.impl()->width() != width || window.impl()->height() != height ) 
    {
        window.impl()->setSize(width, height);

        std::clog << "resize event: " << width << "x" << height << std::endl;
        Gfx::SizeF to(width, height);
        ResizeEvent rev( window.vid(), to );
        commitEvent(rev);

        window.update();
    }

    if( window.position().x() != x || window.position().y() != y) 
    {
        std::clog << "move event: " << x << "x" << y << std::endl;
        Gfx::PointF to(x, y);
        MoveEvent ev(window.vid(), to);
        commitEvent( ev );  
    }
}

} // namespace

} // namespace
