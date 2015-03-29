/* Copyright (C) 2013 Marc Boris Duerner
 * Copyright (C) 2013 Aloysius Indrayanto
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include "WindowImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "KeyHandler.h"
#include <ctype.h>
#include <assert.h>
#include "PaintSurfaceImpl.h"

namespace Pt{
namespace Hmi{

enum
{
	_NET_WM_STATE_REMOVE = 0,
	_NET_WM_STATE_ADD = 1,
	_NET_WM_STATE_TOGGLE =2
};

WindowImpl::WindowImpl(PaintSurface* surface)
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
	create();
}

void WindowImpl::create()
{
   // Display and Screen are inited in Application
    unsigned int screen = DefaultScreen(_display);

    XSetWindowAttributes wattr;
    memset(&wattr, 0, sizeof(wattr));
    wattr.colormap = DefaultColormap(_display, screen);

    // The events we want to receive
    wattr.event_mask = StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|
                       LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask|
                       ButtonPressMask|ButtonReleaseMask|PointerMotionMask|FocusChangeMask;

    wattr.do_not_propagate_mask = KeyPressMask|KeyReleaseMask|ButtonPressMask| ButtonReleaseMask|PointerMotionMask|ButtonMotionMask;

    // Border
    wattr.border_pixel = 0; // Needed for OpenGL
    wattr.border_pixmap = CopyFromParent;

    // Background
    wattr.background_pixmap = None ;
    //wattr.background_pixel = XWhitePixel(display, screen);

    // Backing store
    // NOTE (blue_wind_25): IMO, it is not too useful and just eating up
    //                      server's and/or clients' memory
    wattr.backing_store = None;//Always;
    wattr.save_under = False;//True;

    // Gravity
    wattr.bit_gravity = ForgetGravity; // Region to be retained on resize
    wattr.win_gravity = NorthWestGravity; // How to to reposition when parent resizes

    wattr.cursor = None; // None means parents cursor

    wattr.override_redirect = False; // no WM interaction if True

    // Determines which fields from XSetWindowAttributes are used
    unsigned long winMask = CWWinGravity|CWBitGravity|CWBorderPixmap|CWBorderPixel|CWEventMask|CWDontPropagate|
                            CWCursor|CWOverrideRedirect|CWColormap|CWBackingStore|CWSaveUnder|CWBackPixmap;

    Window parentId = RootWindow(_display, screen);

    unsigned int borderWidth = 0;

    // Create the X11 window
    _window = XCreateWindow(_display, parentId, _x, _y, _width, _height borderWidth, DefaultDepth(_display, screen), InputOutput, DefaultVisual(_display, screen), winMask, &wattr);

    _brushGc = XCreateGC( _display, _window,0, 0);

    XSetWMProtocols(_display, _window, &AtomWindowClosed, 1);
    XSync(_display, false);
}


void WindowImpl::destroy()
{
	if( _window == 0)
		return;

	XFreeGC(_display, _brushGc);
	_brushGc = 0;
	XDestroyWindow(_display, _window);
	XSync(_display, false);
	_window = 0;
}


void WindowImpl::show()
{	
  XMapWindow(_display, _window);
  XSync(_display, false);
}


void WindowImpl::hide()
{
	Display* display = Application::instance().impl()->display();
  XUnmapWindow(display, _window);
  XSync(display, false);
}


WindowImpl::~WindowImpl()
{
	destroy();
}


void WindowImpl::render()
{	
  XEvent exppp;
  memset(&exppp, 0, sizeof(exppp));
  exppp.type = Expose;
  exppp.xexpose.window = _window;

  XSendEvent(_display,_window,False,ExposureMask,&exppp);
  XFlush(_display);
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
            
			while( XCheckTypedWindowEvent(_display, ev.xany.window, ConfigureNotify, &ev) );
			
			onConfigureNotify(ev);            
		}
		break;

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

void WindowImpl::setSize(const Gfx::SizeF& s)
{
	Pt::Gfx::Size size = _app.fromUnit( s );

  if( _width != size.width() || _height != size.height())
  {
    XMoveResizeWindow( _display, _window,  _x, _y, size.width(), size.height() );
    _width = size.width();
    _height = size.height();
	}
}


void WindowImpl::setPosition(const Gfx::PointF& p)
{
	Pt::Gfx::Point pos = _app.fromUnit( p );

  if( _x != pos.x() || _y != pos.y())
  {
    XMoveResizeWindow( _display, _window,  pos.x(), pos.y(), _width, _height );
    _x = pos.x();
    _y = pos.y();
	}
}


void WindowImpl::bringWindowToTop()
{
	XRaiseWindow(_display, _window);
	XSetInputFocus(_display, _window, RevertToNone, CurrentTime);
}


bool WindowImpl::isWindowMinimized()
{
	Atom actual_type;
	int actual_format;
	unsigned long i, num_items, bytes_after;
	Atom* atoms = 0;        
	Atom requestAtom = XInternAtom(_display,"_NET_WM_STATE",False);
	Atom compareAtom = XInternAtom(_display,"_NET_WM_STATE_HIDDEN",False);
        
	XGetWindowProperty(_display, _window, requestAtom, 0, 1024, False, XA_ATOM, &actual_type, &actual_format, &num_items, &bytes_after, (unsigned char**)&atoms);
        
	for( i = 0; i < num_items; ++i )
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

    
bool WindowImpl::isWindowMaximazed()
{
	Atom actual_type;
	int actual_format;
	unsigned long i, num_items, bytes_after;
	Atom* atoms = 0;
	Atom requestAtom = XInternAtom(_display,"_NET_WM_STATE",False);
	Atom compareAtom = XInternAtom(_display,"_NET_WM_STATE_MAXIMIZED_HORZ",False);

  XGetWindowProperty(_display, _window,requestAtom, 0, 1024, False, XA_ATOM, &actual_type, &actual_format, &num_items, &bytes_after, (unsigned char**)&atoms);
        
	for( i = 0; i < num_items; ++i )
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



void WindowImpl::showTitle( bool p )
{
	_showTitle = p;
	setCaption( _title );
}


void WindowImpl::setCaption( const std::string& t )
{
	_title = t;

	if( _showTitle )
		XStoreName(_display, _window, t.c_str());
	else
		XStoreName(_display, _window, "");	
}


void WindowImpl::showMinimizedButton( bool s ) 
{
	//TODO : 
}


void WindowImpl::showMaximizeButton( bool s ) 
{
	//TODO : 
}


void WindowImpl::showInTaskbar( bool s)
{
	//TODO : 
}


void WindowImpl::showSysMenu( bool s)
{
	//TODO : 
}


void WindowImpl::setIcon(const Pt::Gfx::ARgbImage& p)
{
	//TODO;
}


void WindowImpl::setEnable(bool e)
{
	//TODO:
}


void WindowImpl::setForceTopMost( bool force )
{	
	_forceTopMost = force;

	if( _forceTopMost )
		bringWindowToTop();
}


void WindowImpl::setWindowState( WindowState::Type state )
{
	switch( state )
	{
		case Pt::Hmi::WindowStateType::Normal:
			restoreWindow();
		break;

		case Pt::Hmi::WindowStateType::Maximazed:
			maximizeWindow();
		break;

		case Pt::Hmi::WindowStateType::Minimized:
			minimizeWindow();
		break;
	}
}


void WindowImpl::restoreWindow()
{
  XClientMessageEvent ev;
  unsigned int screen = DefaultScreen(_display);
    
  ev.type = ClientMessage;
  ev.window = _window;
  ev.message_type = XInternAtom(_display, "WM_CHANGE_STATE", False);
  ev.format = 32;
  ev.data.l[0] = NormalState;
    
	XSendEvent(_display, RootWindow(_display, screen), False, SubstructureRedirectMask|SubstructureNotifyMask,(XEvent *)&ev);	
}


void WindowImpl::minimizeWindow()
{
  XClientMessageEvent ev;
  unsigned int screen = DefaultScreen(_display);
    
  ev.type = ClientMessage;
  ev.window = _window;
  ev.message_type = XInternAtom(_display, "WM_CHANGE_STATE", False);
  ev.format = 32;
  ev.data.l[0] = IconicState;
    
	XSendEvent(_display, RootWindow(_display, screen), False, SubstructureRedirectMask|SubstructureNotifyMask,(XEvent *)&ev);	
}


void WindowImpl::maximizeWindow()
{
	XEvent xev;

	memset(&xev,0,sizeof(xev));
	xev.xclient.type		 = ClientMessage;
	xev.xclient.serial		 = 0;
	xev.xclient.send_event	 = True;
	xev.xclient.window		 = _window;
	xev.xclient.format		 = 32;
	xev.xclient.message_type = XInternAtom(_display,"_NET_WM_STATE",False);
	xev.xclient.data.l[0]	 = (unsigned long)1;
	xev.xclient.data.l[1]	 = XInternAtom(_display,"_NET_WM_STATE_MAXIMIZED_VERT",False);
	xev.xclient.data.l[2]	 = XInternAtom(_display,"_NET_WM_STATE_MAXIMIZED_HORZ",False);

	XSendEvent(_display,DefaultRootWindow(_display),False,SubstructureRedirectMask|SubstructureNotifyMask,&xev);

	XRaiseWindow(_display,_window);
}


void Window::setBorder(WindowBorder::Type border )
{
  XSizeHints sizeHints;
	long suppliedReturn = 0;

	XGetWMNormalHints(_display,_window, &sizeHints, &suppliedReturn);

	Pt::Gfx::Size winMinSize =  _model->fromUnit(_model->MinimumSize.get());
	Pt::Gfx::Size winMaxSize =  _model->fromUnit(_model->MaximumSize.get());

	switch( _model->Border.get())
	{
		case Pt::Hmi::WindowBorderType::Sizeable:
		case Pt::Hmi::WindowBorderType::DialogSizeable:
		case Pt::Hmi::WindowBorderType::ToolSizeable:
		{//Sizeable
			sizeHints.flags |= PMinSize | PMaxSize;
			sizeHints.min_width  = 0;
			sizeHints.max_width  = std::numeric_limits<Pt::uint16_t>::max();
			sizeHints.min_height = 0;
			sizeHints.max_height = std::numeric_limits<Pt::uint16_t>::max();
		}
		break;

		case Pt::Hmi::WindowBorderType::Dialog:
		case Pt::Hmi::WindowBorderType::Tool:
		case Pt::Hmi::WindowBorderType::Fixed:
		{//Fixed size
			sizeHints.flags |= PMinSize | PMaxSize;
			sizeHints.min_width  = _width;
			sizeHints.max_width  = _width;
			sizeHints.min_height = _height;
			sizeHints.max_height = _height;
		}
		break;
		case Pt::Hmi::WindowBorderType::NoBorder:
	
		default:
		break;
	}

	XSetWMNormalHints(_display, _window,  &sizeHints);
} 

}}

