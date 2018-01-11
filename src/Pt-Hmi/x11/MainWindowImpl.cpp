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
#include "KeyHandler.h"
#include "PaintSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>

#include <iostream>
#include <sstream>
#include <algorithm>

#include <ctype.h>
#include <assert>

namespace Pt { 

namespace Hmi { 

enum
{
	_NET_WM_STATE_REMOVE = 0,
	_NET_WM_STATE_ADD    = 1,
	_NET_WM_STATE_TOGGLE = 2
};


MainWindowImpl::MainWindowImpl(Window::Type type)
: _surface( surface )
, _forceTopMost(false)
, _x(20)
, _y(20)
, _width(800)
, _height(600)
, _showTitle(true)
{
	_mouseEvent.buttons().resize(3);
	_app = Application::instance();
	_display = _app.impl()->display();

	AtomAppWake      = XInternAtom(_display, "PT_APP_WAKE",      false);
	AtomWindowResize = XInternAtom(_display, "PT_WINDOW_RESIZE", false);
	AtomWindowMove   = XInternAtom(_display, "PT_WINDOW_MOVE",   false);
	AtomWindowClosed = XInternAtom(_display, "WM_DELETE_WINDOW", false);
	AtomWMProtocols  = XInternAtom(_display, "WM_PROTOCOLS",     false);

	_app.impl()->WindowEvent += Pt::slot(*this, &WindowImpl::onWindowEvent);
	create(type);
}


MainWindowImpl::~MainWindowImpl()
{
	destroy();
}


void MainWindowImpl::create(Window::Type type)
{
   // Display and Screen are inited in Application
    unsigned int screen = DefaultScreen(_display);

    XSetWindowAttributes wattr;
    memset(&wattr, 0, sizeof(wattr));
    wattr.colormap = DefaultColormap(_display, screen);

    // The events we want to receive
    wattr.event_mask = StructureNotifyMask|ExposureMask|
	                   PropertyChangeMask|EnterWindowMask|
                       LeaveWindowMask|KeyPressMask|
					   KeyReleaseMask|KeymapStateMask|
                       ButtonPressMask|ButtonReleaseMask|
					   PointerMotionMask|FocusChangeMask;

    wattr.do_not_propagate_mask = KeyPressMask|KeyReleaseMask|
	                              ButtonPressMask| ButtonReleaseMask|
								  PointerMotionMask|ButtonMotionMask;

    // Border
    wattr.border_pixel = 0; // Needed for OpenGL
    wattr.border_pixmap = CopyFromParent;

    // Background
    wattr.background_pixmap = None;
    //wattr.background_pixel = XWhitePixel(display, screen);

    // Backing store
    wattr.backing_store = None; // Always;
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

	if(type == Window::Pupup)
	{
		winMask |= CWOverrideRedirect;
		wattr.override_redirect = True; 
	}

    Window parentId = RootWindow(_display, screen);

    unsigned int borderWidth = 0;

    // Create the X11 window
    _window = XCreateWindow(_display, 
	                        parentId, 
							_x, _y, 
							_width, _height,
	                        borderWidth, 
							DefaultDepth(_display, screen), 
							InputOutput, 
							DefaultVisual(_display, screen), 
							winMask, 
							&wattr);

    _brushGc = XCreateGC( _display, _window,0, 0);

    XSetWMProtocols(_display, _window, &AtomWindowClosed, 1);
    XSync(_display, false);
}


void MainWindowImpl::destroy()
{
	if( _window == 0)
		return;

	XFreeGC(_display, _brushGc);
	_brushGc = 0;
	
	XDestroyWindow(_display, _window);
	_window = 0;

	XSync(_display, false);
}


Gfx::PointF MainWindowImpl::toScreen(const Gfx::PointF& windowPos) const
{
	Bool XTranslateCoordinates(display, src_w, dest_w, src_x, src_y, dest_x_return, 
                               dest_y_return, child_return)
      Display *display;
      Window src_w, dest_w;
      int src_x, src_y;
      int *dest_x_return, *dest_y_return;
      Window *child_return;

Arguments:

display	Specifies the connection to the X server.
src_w	Specifies the source window.
dest_w	Specifies the destination window.
src_x
src_y	Specify the x and y coordinates within the source window.
dest_x_return
dest_y_return	Return the x and y coordinates within the destination window.
child_return	Returns the child if the coordinates are contained in a mapped child of the destination window.



    POINT p = { lround(windowPos.x()), 
                lround(windowPos.y()) };

    //ClientToScreen(_hwnd, &p);

    return Gfx::PointF( p.x, p.y );
}


Gfx::PointF MainWindowImpl::fromScreen(const Gfx::PointF& screenPos) const
{
    POINT p = { lround(screenPos.x()), 
                lround(screenPos.y()) };

    //ScreenToClient(_hwnd, &p);

    return Gfx::PointF( p.x, p.y );
}


void MainWindowImpl::close()
{
	XEvent ev;
	memset(&ev, 0, sizeof (ev));
 
	ev.xclient.type         = ClientMessage;
	ev.xclient.window       = _window;
	ev.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", true);
	ev.xclient.format       = 32;
	ev.xclient.data.l[0]    = XInternAtom(display, "WM_DELETE_WINDOW", false);
	ev.xclient.data.l[1]    = CurrentTime;
	XSendEvent(_display, _window, False, NoEventMask, &ev);
}


void MainWindowImpl::paint(const Gfx::RectF& rect)
{	
  XEvent exppp;
  memset(&exppp, 0, sizeof(exppp));
  exppp.type = Expose;
  exppp.xexpose.window = _window;

  XSendEvent(_display, _window, False, ExposureMask, &exppp);
  XFlush(_display);
}


void MainWindowImpl::show(bool visible)
{	
	if(visible)
	{  	
		setTopMost(_isTopMost);	
		XMapWindow(_display, _window);
	}
	else
	{
		XUnmapWindow(_display, _window);
	}
  
  	XSync(_display, false);
}


void MainWindowImpl::activate()
{
	XSetInputFocus(_display, _window, RevertToNone, CurrentTime)
}


void MainWindowImpl::enable(bool e)
{
    XSetWindowAttributes wattr;
    memset(&wattr, 0, sizeof(wattr));

	//XGetWindowAttributes(_display, _window, &wattr)

	if(e)
	{
		wattr.event_mask = StructureNotifyMask|ExposureMask|
						   PropertyChangeMask|EnterWindowMask|
						   LeaveWindowMask|KeyPressMask|
						   KeyReleaseMask|KeymapStateMask|
						   ButtonPressMask|ButtonReleaseMask|
						   PointerMotionMask|FocusChangeMask;
	}
	else
	{
		wattr.event_mask = 0;
	}

    unsigned long winMask = CWEventMask;

	XChangeWindowAttributes(_display, _window, winMask, &wattr);
}


void MainWindowImpl::setTopMost(bool e)
{
	Atom wm_state = XInternAtom(disp, "_NET_WM_STATE", False);
	Atom wm_state_above = XInternAtom(disp, "_NET_WM_STATE_ABOVE", False);

	// ClientMessage event
	XEvent event;
	event.xclient.type = ClientMessage;

	// value unimportant in this case
	event.xclient.serial = 0;

	// coming from a SendEvent request, so True
	event.xclient.send_event = True;

	// the event originates from disp
	event.xclient.display = disp;

	// the window whose state will be modified
	event.xclient.window = _window;

	// the component Atom being modified in the window
	event.xclient.message_type = wm_state;

	// specifies that data.l will be used
	event.xclient.format = 32;

	// 1 is _NET_WM_STATE_ADD
	event.xclient.data.l[0] = 1;

	// the atom being added
	event.xclient.data.l[1] = wm_state_above;

	// unused
	event.xclient.data.l[2] = 0;
	event.xclient.data.l[3] = 0;
	event.xclient.data.l[4] = 0;
	
	XSendEvent(_display, DefaultRootWindow(_display), False,
		       SubstructureRedirectMask|SubstructureNotifyMask, 
			   &event);
}


void MainWindowImpl::move(const Gfx::PointF& pos)
{
	XMoveResizeWindow(_display, _window,  
	                  pos.x(), pos.y(), _width, _height);

    _x = pos.x();
    _y = pos.y();
}


void MainWindowImpl::resize(const Gfx::SizeF& size)
{
    XMoveResizeWindow( _display, _window,  
	                   _x, _y, size.width(), size.height() );
    
	_width = size.width();
    _height = size.height();
}


void MainWindowImpl::setType(Window::Type type)
{
	std::clog << "Window::setType not implemented" << std::endl;
}


void MainWindowImpl::setIcon(const Gfx::Image& icon)
{
	// TODO
}


void MainWindowImpl::setTitle(const std::string& text)
{
	_title = t;

	if( _showTitle )
		XStoreName(_display, _window, t.c_str());
	else
		XStoreName(_display, _window, "");	
}


void MainWindowImpl::setMinimumSize(const Gfx::SizeF& s)
{
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
	//	XRaiseWindow(_display,_window);
	//}
}


bool WindowImpl::isMinimized()
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

    
bool WindowImpl::isMaximized()
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


void WindowImpl::onPaint(XEvent& xev)
{
  ::Drawable from = _surface->impl()->drawable();
  Pt::Gfx::Size size = surface.size();
  XCopyArea( _display, from, _window, _brushGc, 0, 0, size.width(), size.height(), 0, 0);
  XSync(_display, false);
}


void WindowImpl::onWindowEvent(XEvent& ev)
{	
	if(ev.xany.window != _window)
		return;
        
	if(_window == 0)
		return;

  switch( ev.xany.type )
  {
		case ClientMessage:  
			onClientMessage(ev);
		break;

		case MotionNotify:   
			onMotionNotify(ev);
		break;

		case ButtonPress:
			onMouseButtonPress(ev);     
		break;

		case ButtonRelease:   
			onMouseButtonRelease(ev);     
		break;

    case Expose:         
			onPaint(ev);
		break;

		case NoExpose: 
		break;

    case ConfigureNotify:
    {
        // Use only last configure event for the window in queue
        XPending(_display);
            
		while( XCheckTypedWindowEvent(_display, ev.xany.window, ConfigureNotify, &ev) )
			;
			
		onConfigureNotify(ev); 
		break;           
	}
		
    case KeyPress:
    case KeyRelease:  
			onKeyEvent(ev);
		break;
	
		case FocusOut:
			if( _forceTopMost )
				bringWindowToTop();
		break;

		case EnterNotify:    
		break;

    case LeaveNotify:			
		break;

		default:
		break;
	}
}


void WindowImpl::onClientMessage(XEvent& xev)
{
	if( xev.xclient.message_type == AtomWMProtocols ) 
	{
		CloseEvent closeEvent;
		_windowEvent.send( closeEvent );
	}	
}


void WindowImpl::onMotionNotify(XEvent& xev)
{
	Pt::Gfx::PointF pos = _app.toUnit( Pt::Gfx::Point( xev.xmotion.x, xev.xmotion.y ) );
	_mouseEvent.setX( pos.x() );
	_mouseEvent.setY( pos.y() );
	_mouseEvent.setController(_controller);
	_windowEvent.send( _mouseEvent );
}


void WindowImpl::onMouseButtonPress(XEvent& xev)
{
  int x = xev.xbutton.x;
  int y = xev.xbutton.y;
    
	switch( xev.xbutton.button ) 
	{
    case Button1: 
			_mouseEvent.buttons()[0].setState(DeviceButton::Pressed);
		break;
        
		case Button2: 
			_mouseEvent.buttons()[1].setState(DeviceButton::Pressed);
		break;

		case Button3: 
			_mouseEvent.buttons()[2].setState(DeviceButton::Pressed);
		break;

    //case Button4: button = MouseEvent::WheelUp; break;
    //case Button5: button = MouseEvent::WheelDown; break;
	}

	Pt::Gfx::PointF pos = _app.toUnit( Pt::Gfx::Point(x, y) );

	_mouseEvent.setX( pos.x() );
	_mouseEvent.setY( pos.y() );
	_windowEvent.send( _mouseEvent );
}


void WindowImpl::onMouseButtonRelease(XEvent& xev)
{
  int  x = xev.xbutton.x;
  int  y = xev.xbutton.y;
    
  switch( xev.xbutton.button ) 
	{
    case Button1: 
			_mouseEvent.buttons()[0].setState(DeviceButton::Released);
		break;
        
		case Button2: 
			_mouseEvent.buttons()[1].setState(DeviceButton::Released);
		break;

		case Button3: 
			_mouseEvent.buttons()[2].setState(DeviceButton::Released);
		break;

		//TODO: wheel
    //case Button4: button = MouseEvent::WheelUp; break;
    //case Button5: button = MouseEvent::WheelDown; break;
	}

	Pt::Gfx::PointF pos = _app.toUnit(Pt::Gfx::Point(x,y));

	_mouseEvent.setX( pos.x() );
	_mouseEvent.setY( pos.y() );
	_windowEvent.send( _mouseEvent );
}


void WindowImpl::onKeyEvent(XEvent& xev)
{
	if(KeyRelease == xev.xkey.type)
		_keyEvent.setState(KeyEvent::KeyUp);
	else
		_keyEvent.setState(KeyEvent::KeyDown);
    
	KeySym sym = 0;
	int vcode = 0;
	
	sym = *(XGetKeyboardMapping(_display, xev.xkey.keycode,1,&vcode));

	switch(sym ) 
	{
		case XK_Control_L: 
		case XK_Control_R: 
			_keyEvent.setCtrl(_keyEvent.state() == KeyEvent::KeyDown);
		break;

		case XK_Alt_L: 
		case XK_Alt_R:
			_keyEvent.setAlt(_keyEvent.state() == KeyEvent::KeyDown);
        break;

		case XK_Shift_L :
		case XK_Shift_R :
			_keyEvent.setShift(_keyEvent.state() == KeyEvent::KeyDown);
		break;

		default:
		{
	  }
		break;
	}		
		
	const unsigned int ucode = KeyHandler::keySymToUtf(sym);

	if( ucode != 0)
		_keyEvent.setUnicode(ucode);
	else
		_keyEvent.setUnicode(sym & 0xFF);
		
	_windowEvent.send( _keyEvent );		    	
}


void WindowImpl::onConfigureNotify( XEvent& xev)
{
  if(isWindowMinimized())    
		_resizeEvent.setState( WindowState::Minimized );
  else if(isWindowMaximazed())
		_resizeEvent.setState( WindowState::Maximazed );
  else
		_resizeEvent.setState( WindowState::Normal );


  if( _x != xev.xconfigure.x || _y != xev.xconfigure.y)
  {
    _x = xev.xconfigure.x;
    _y = xev.xconfigure.y;

	    Pt::Gfx::PointF pos = _app.toUnit( Pt::Gfx::Point( _x, _y );
		_positionEvent.setPosition(pos);
		_windowEvent.send( _positionEvent );
  }

  if(_width != xev.xconfigure.width || _height != xev.xconfigure.height)
  {
    _width = xev.xconfigure.width;
		_height = xev.xconfigure.height;
        
		Pt::Gfx::SizeF size  = _app.toUnit(Pt::Gfx::Size(_width, _height));    
		_resizeEvent.setSize( size );        
  }

	_windowEvent.send( _resizeEvent );
}

} // namespace

} // namespace
