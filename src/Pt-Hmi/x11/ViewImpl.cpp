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
#include "ViewImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/Widget.h>

#include <Pt/Hmi/View.h>
#include <Pt/Hmi/Window.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "KeyHandler.h"
#include <ctype.h>
#include <assert.h>

#include <Pt/Hmi/NativePaintSurface.h>
#include "PaintSurfaceImpl.h"

namespace Pt{
namespace Hmi{

enum
{
	_NET_WM_STATE_REMOVE = 0,
	_NET_WM_STATE_ADD = 1,
	_NET_WM_STATE_TOGGLE =2
};

ViewImpl::ViewImpl()
: _ignoreSizeEvent(false)
, _model(0)
, _window(0)
, _visible(false)
, _x(0)
, _y(0)
, _width(0)
, _height(0)
{
	_mouseEvent.buttons().resize(3);
	_display = Application::instance().impl()->display();

    AtomAppWake      = XInternAtom(_display, "PT_APP_WAKE",      false);
    AtomWindowResize = XInternAtom(_display, "PT_WINDOW_RESIZE", false);
    AtomWindowMove   = XInternAtom(_display, "PT_WINDOW_MOVE",   false);
    AtomWindowClosed = XInternAtom(_display, "WM_DELETE_WINDOW", false);
    AtomWMProtocols  = XInternAtom(_display, "WM_PROTOCOLS",     false);

	Application::instance().impl()->WindowEvent += Pt::slot(*this, &ViewImpl::onWindowEvent);
	create();
}


void ViewImpl::pixelToScreen(char* data, const Pt::Gfx::ARgbColor& pixel)
{
    unsigned int screen = DefaultScreen(_display);
	const int depth = XDefaultDepth(_display, screen);

	switch(depth)
	{
		case 24:
			data[2] = (char) pixel.red();
			data[1] = (char) pixel.green();
			data[0] = (char) pixel.blue();
		break;

		case 16:
		{
			unsigned short* pix = (unsigned short*) data;
			*pix = ((pixel.red()&0xF8)<<8)| ((pixel.green()&0xFC)<<3)|((pixel.blue()&0xF8)>>3);

		}
		break;
	}
}

void ViewImpl::drawSurface(Pt::Hmi::PaintSurface& surface)
{    
    NativePaintSurface* nativeSurface = (NativePaintSurface*) &surface;

    ::Drawable from = nativeSurface->impl()->drawable();
    Pt::Gfx::Size size = _model->fromUnit(surface.size());

    XCopyArea( _display, from, _window, _brushGc, 0, 0, size.width(), size.height(), 0, 0);

    XSync(_display, false);
}

void ViewImpl::onClientMessage(XEvent& xev)
{
	if( _model == 0)
		return;

	if(xev.xclient.message_type == AtomWMProtocols) 
 	{			
		bool canClose = false;

		_controller->ClosingAction.send(_controller, canClose);

		if(canClose)
			destroy();
	}	
}

void ViewImpl::onMotionNotify(XEvent& xev)
{
    int x = xev.xmotion.x;
    int y = xev.xmotion.y;
	Pt::Gfx::PointF pos = _model->toUnit(Pt::Gfx::Point(x,y));
	_mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
    _mouseEvent.setController(_controller);

	Application::instance().systemEvent().send(_mouseEvent);
}


void ViewImpl::onMouseButtonPress(XEvent& xev)
{
    int x = xev.xbutton.x;
    int y = xev.xbutton.y;
    
    switch(xev.xbutton.button) 
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

	Pt::Gfx::PointF pos = _model->toUnit(Pt::Gfx::Point(x,y));

	_mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
    _mouseEvent.setController(_controller);
	Application::instance().systemEvent().send(_mouseEvent);
}


void ViewImpl::onMouseButtonRelease(XEvent& xev)
{
    int  x = xev.xbutton.x;
    int  y = xev.xbutton.y;
    
    switch(xev.xbutton.button) 
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

	Pt::Gfx::PointF pos = _model->toUnit(Pt::Gfx::Point(x,y));

	_mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
    _mouseEvent.setController(_controller);
	Application::instance().systemEvent().send(_mouseEvent);
}

void ViewImpl::redraw()
{
    XEvent exppp;
    memset(&exppp, 0, sizeof(exppp));
    exppp.type = Expose;
    exppp.xexpose.window = _window;

    XSendEvent(_display,_window,False,ExposureMask,&exppp);
    XFlush(_display);
}

void ViewImpl::writeWindowSizeAndPos()
{
	Pt::Gfx::Point pos = _model->fromUnit(_model->Position.get());
	Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());

    if( _x != pos.x() || _y != pos.y() || _width != size.width() || _height != size.height())
    {
        XMoveResizeWindow(_display, _window,  pos.x(), pos.y(), size.width(), size.height());
        _x = pos.x();
        _y = pos.y();
        _width = size.width();
        _height = size.height();
    }
}

void ViewImpl::onConfigureNotify( XEvent& xev)
{
	if(!_model->Enabled.get())
	{
		writeWindowSizeAndPos();
		return;
	}
    
    if(isWindowMinimized())
    {
        if(_model->WindowState.get() !=  WindowStateType::Minimized)
            _model->WindowState = WindowStateType::Minimized;
    }
    else if(isWindowMaximazed())
    {
        if(_model->WindowState.get() !=  WindowStateType::Maximazed)
            _model->WindowState = WindowStateType::Maximazed;
    }
    else
    {
        if(_model->WindowState.get() !=  WindowStateType::Normal)
            _model->WindowState = WindowStateType::Normal;
    }


    if( _x != xev.xconfigure.x || _y != xev.xconfigure.y)
    {
        _x = xev.xconfigure.x;
        _y = xev.xconfigure.y;

        _model->Position =  _model->toUnit(Pt::Gfx::Point(_x, _y));
    }

    if(_width != xev.xconfigure.width || _height != xev.xconfigure.height)
    {
        _width = xev.xconfigure.width;
        _height = xev.xconfigure.height;
        
        _model->Size = _model->toUnit(Pt::Gfx::Size(_width, _height));    
    }
}

void ViewImpl::onKeyEvent(XEvent& xev)
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
		
    _keyEvent.setController(_controller);
	Application::instance().systemEvent().send(_keyEvent);
}

void ViewImpl::onWindowEvent(XEvent& ev)
{	
	if(ev.xany.window != _window)
		return;
        
   if(_window == 0)
		return;

	if( _model == 0)
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
			if(_model->TopMost.get())
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


void ViewImpl::create()
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
    _window = XCreateWindow(_display, parentId, 20, 20, 800, 600, borderWidth, DefaultDepth(_display, screen), InputOutput, DefaultVisual(_display, screen), winMask, &wattr);

    _brushGc = XCreateGC( _display, _window,0, 0);

    XSetWMProtocols(_display, _window, &AtomWindowClosed, 1);
    XSync(_display, false);
}

void ViewImpl::destroy()
{
	if(_window != 0)
	{
	    XFreeGC(_display, _brushGc);
		_brushGc = 0;
		XDestroyWindow(_display, _window);
		XSync(_display, false);
		_window = 0;

		_controller->ClosedAction.send(_controller);
	}
}

ViewImpl::~ViewImpl()
{
	destroy();
}

void ViewImpl::show()
{	
    XMapWindow(_display, _window);
    XSync(_display, false);
    _visible = 	true;
}

void ViewImpl::hide()
{
	Display* display = Application::instance().impl()->display();
    XUnmapWindow(display, _window);
    XSync(display, false);
    _visible = 	false;
}

void ViewImpl::bringWindowToTop()
{
	XRaiseWindow(_display, _window);
	XSetInputFocus(_display, _window, RevertToNone, CurrentTime);
}

void ViewImpl::onPaint(XEvent& xev)
{
    if( _model != 0  && _window != 0)
        drawSurface(*_model->paintSurface());
}

void ViewImpl::writeWindowProperties()
{
    XSizeHints sizeHints;
	long suppliedReturn = 0;

    XGetWMNormalHints(_display,_window, &sizeHints, &suppliedReturn);


	if(_model->Visible.get() && !_visible)
		show();

	if(!_model->Visible.get() && _visible)
		hide();
		
	if( _model->ShowTitle.get())
		XStoreName(_display, _window, _model->Caption.get().c_str());
	else
		XStoreName(_display, _window, "");


	Pt::Gfx::Size winMinSize =  _model->fromUnit(_model->MinimumSize.get());
	Pt::Gfx::Size winMaxSize =  _model->fromUnit(_model->MaximumSize.get());

	switch( _model->Border.get())
	{
		case Pt::Hmi::WindowBorderType::Sizeable:
		case Pt::Hmi::WindowBorderType::DialogSizeable:
		case Pt::Hmi::WindowBorderType::ToolSizeable:
		{//Sizeable
			sizeHints.flags |= PMinSize | PMaxSize;
			sizeHints.min_width  = winMinSize.width();
			sizeHints.max_width  = winMaxSize.width();
			sizeHints.min_height = winMinSize.height();
			sizeHints.max_height = winMaxSize.height();
		}
		break;

		case Pt::Hmi::WindowBorderType::Dialog:
		case Pt::Hmi::WindowBorderType::Tool:
		case Pt::Hmi::WindowBorderType::Fixed:
		{//Fixed size
			sizeHints.flags |= PMinSize | PMaxSize;
			sizeHints.min_width  = _model->Size.get().width();
			sizeHints.max_width  = _model->Size.get().width();
			sizeHints.min_height = _model->Size.get().height();
			sizeHints.max_height = _model->Size.get().height();
		}
		break;
		case Pt::Hmi::WindowBorderType::NoBorder:
	
		default:
		break;
	}

	XSetWMNormalHints(_display, _window,  &sizeHints);

	//TODO : show/hide minimize button.
	if( _model->ShowMinimizeButton.get())
	{
	}
	else
	{
	}

	//TODO : show/hide maximize button.
	if( _model->ShowMaximizeButton.get())
	{
	}	
	else
	{
	}

	//TODO : show/hide system menu
	if( _model->ShowSysMenu.get())
	{
	}
	else
	{
	}

	switch(_model->WindowState.get())
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

	//TODO: show in taskbar
	if(_model->ShowInTaskbar.get())
	{
	}
	else
	{
	}
}
    
bool ViewImpl::isWindowMinimized()
{
    Atom actual_type;
    int actual_format;
    unsigned long i, num_items, bytes_after;
    Atom* atoms;
        
	Atom requestAtom = XInternAtom(_display,"_NET_WM_STATE",False);
	Atom compareAtom = XInternAtom(_display,"_NET_WM_STATE_HIDDEN",False);
    atoms=NULL;
        
    XGetWindowProperty(_display, _window, requestAtom, 0, 1024, False, XA_ATOM, &actual_type, &actual_format, &num_items, &bytes_after, (unsigned char**)&atoms);
        
    for(i=0; i<num_items; ++i)
    {
        if(atoms[i]==compareAtom)
        {
            XFree(atoms);
            return true;
        }
    }

    XFree(atoms);
    return 0;
}
    
bool ViewImpl::isWindowMaximazed()
{
    Atom actual_type;
    int actual_format;
    unsigned long i, num_items, bytes_after;
    Atom* atoms;
    Atom requestAtom = XInternAtom(_display,"_NET_WM_STATE",False);
	Atom compareAtom = XInternAtom(_display,"_NET_WM_STATE_MAXIMIZED_HORZ",False);

    atoms=NULL;
        
    XGetWindowProperty(_display, _window,requestAtom, 0, 1024, False, XA_ATOM, &actual_type, &actual_format, &num_items, &bytes_after, (unsigned char**)&atoms);
        
    for(i=0; i<num_items; ++i)
    {
        if(atoms[i]==compareAtom)
        {
            XFree(atoms);
            return true;
        }
    }
    XFree(atoms);
    return 0;
}
    

void ViewImpl::minimizeWindow()
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

void ViewImpl::restoreWindow()
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

void ViewImpl::maximizeWindow()
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

void ViewImpl::updateDrawBuffer()
{
	unsigned int sreen  = DefaultScreen(_display);

	int depth = XDefaultDepth(_display, sreen);
    
	size_t pixelSize = (depth == 16) ? 2 : 4;	

	size_t currentSize = _model->paintSurface()->size().width() * _model->paintSurface()->size().height() * pixelSize;

	if(_pixelBuffer.size() < currentSize)
		_pixelBuffer.resize(currentSize);
}

void ViewImpl::output(Pt::Hmi::Controller* controller,Pt::Hmi::Model* model)
{
	_model = dynamic_cast<WindowModel*>(model);
  _controller = dynamic_cast<Window*>(controller);

	//Handle open/close
	if(_model->Closed.get())
	{
		if(_window != 0)
			destroy();

		return;
	}
	else
	{
		if(_window == 0)
		{
			if(_model->Visible.get())
				create();
			else
				return;
		}
	}
    
	if(!_model->Visible.get() && !_visible)
	{
		return;
	}
    //TODO: Center parent handling
    
	//Initial size and position

	writeWindowSizeAndPos();
	
	writeWindowProperties();

    drawSurface(*_model->paintSurface());
	redraw();
}

}}

