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
#include "GfxOutputImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Hmi/WindowModel.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "KeyHandler.h"
#include <ctype.h>

namespace Pt{
namespace Hmi{

GfxOutputImpl::GfxOutputImpl()
: _model(0)
, _drawable(0)
{
	_mouseEvent.buttons().resize(3);
 	Display* display = Application::instance().impl()->display();

    AtomAppWake      = XInternAtom(display, "PT_APP_WAKE",      false);
    AtomWindowResize = XInternAtom(display, "PT_WINDOW_RESIZE", false);
    AtomWindowMove   = XInternAtom(display, "PT_WINDOW_MOVE",   false);
    AtomWindowClosed = XInternAtom(display, "WM_DELETE_WINDOW", false);
    AtomWMProtocols  = XInternAtom(display, "WM_PROTOCOLS",     false);

	Application::instance().impl()->WindowEvent += Pt::slot(*this, &GfxOutputImpl::onWindowEvent);
	create();
}

void GfxOutputImpl::drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height)
{
    Display* display = Application::instance().impl()->display();

    unsigned int screen = DefaultScreen(display);

    Visual* visual = XDefaultVisual(display, screen);
    
	int depth = XDefaultDepth(display, screen);

    XImage* ximage = XCreateImage(display, visual, depth, ZPixmap, 0, NULL, width, height, 8, 0);
    ximage->data = (char*)data;
    XPutImage(display, _drawable, _brushGc, ximage, 0, 0, x, y, width, height);

    XSync(display, false);
    ximage->data = NULL;
    XDestroyImage(ximage);	
}


void GfxOutputImpl::onClientMessage(XEvent& xev)
{
	if( _model == 0)
		return;

    if(xev.xclient.message_type == AtomWindowResize) 
	{
        int width = xev.xclient.data.l[0];
        int height = xev.xclient.data.l[1];

		Pt::Gfx::SizeF size = _model->toUnit(Pt::Gfx::Size(width, height));
		_model->Size = size;
        return;
    }

    if( xev.xclient.message_type == AtomWindowMove ) 
	{
        int x = xev.xclient.data.l[0];
        int y = xev.xclient.data.l[1];
		Pt::Gfx::PointF p = _model->toUnit(Pt::Gfx::Point(x,y));
		_model->Position = p;
        return;
    }

    if(xev.xclient.message_type == AtomWMProtocols) 
	{
		//TODO
        return;
    }
}

void GfxOutputImpl::onMotionNotify(XEvent& xev)
{
    int x = xev.xmotion.x;
    int y = xev.xmotion.y;
/*

    if( xev.xmotion.state & Button1Mask)
        modifiers |= MouseMoveEvent::LeftButtonDown;

    if( xev.xmotion.state & Button3Mask)
        modifiers |= MouseMoveEvent::RightButtonDown;
    if( xev.xmotion.state & Button2Mask)
        modifiers |= MouseMoveEvent::MiddleButtonDown;
    if( xev.xmotion.state & ControlMask)
        modifiers |= MouseMoveEvent::CtrlDown;
    if( xev.xmotion.state & Mod1Mask)
        modifiers |= MouseMoveEvent::AltDown;
*/

	Pt::Gfx::PointF pos = _model->toUnit(Pt::Gfx::Point(x,y));
	_mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());

	Application::instance().pointerEvent().send(_model->controller(), _mouseEvent);
}


void GfxOutputImpl::onMouseButtonPress(XEvent& xev)
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
	Application::instance().pointerEvent().send(_model->controller(), _mouseEvent);
}


void GfxOutputImpl::onMouseButtonRelease(XEvent& xev)
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

        //case Button4: button = MouseEvent::WheelUp; break;
        //case Button5: button = MouseEvent::WheelDown; break;
    }

	Pt::Gfx::PointF pos = _model->toUnit(Pt::Gfx::Point(x,y));

	_mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
}

void GfxOutputImpl::onConfigureNotify( XEvent& xev)
{
    const size_t width = xev.xconfigure.width;
    const size_t height = xev.xconfigure.height;
    const int x = xev.xconfigure.x;
    const int y = xev.xconfigure.y;	

	Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());
	Pt::Gfx::Point pos = _model->fromUnit(_model->Position.get());

    if( size.width() != width || size.height() != height ) 
	{
		Pt::Gfx::SizeF s = _model->toUnit(Pt::Gfx::Size(width, height));
		_model->Size = s;			
    }

    if(pos.x() != x || pos.y() != y) 
	{
		Pt::Gfx::PointF p = _model->toUnit(Pt::Gfx::Point(x, y));
		_model->Position = p;			
    }
}

void GfxOutputImpl::onKeyEvent(XEvent& xev)
{
    if(KeyRelease == xev.xkey.type)
        _keyEvent.setState(KeyEvent::KeyUp);
	else
		_keyEvent.setState(KeyEvent::KeyDown);
    
    Display* display = Application::instance().impl()->display();


    KeySym sym = 0;
	int vcode = 0;
	
	sym = *(XGetKeyboardMapping(display, xev.xkey.keycode,1,&vcode));

	vcode = (char) ::toupper((int)sym);

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
			
	_keyEvent.setVirtualCode(vcode);

	_keyEvent.setRepeatCount(0);

		
	Application::instance().keyDeviceEvent().send(_model->controller(), _keyEvent);
}

void GfxOutputImpl::onWindowEvent(XEvent& ev)
{
	
	if(ev.xany.window != _drawable)
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
		    Display* display = Application::instance().impl()->display();
            XPending(display);
            
			while( XCheckTypedWindowEvent(display, ev.xany.window, ConfigureNotify, &ev) );
			
			onConfigureNotify(ev);            
        }
		break;

        case KeyPress:        		
        case KeyRelease:  
			onKeyEvent(ev);        		
		break;

		/*
        case EnterNotify:     MainLoop::instance().enterNotify(*widget, _xev);     break;
        case LeaveNotify:     MainLoop::instance().leaveNotify(*widget, _xev);     break;
		*/
        default:
            break;
    }
}



void GfxOutputImpl::create()
{
   // Display and Screen are inited in Application
    Display* display = Application::instance().impl()->display();
    unsigned int screen = DefaultScreen(display);

    XSetWindowAttributes wattr;
    memset(&wattr, 0, sizeof(wattr));
    wattr.colormap = DefaultColormap(display, screen);

    // The events we want to receive
    wattr.event_mask = StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|
                       LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask|
                       ButtonPressMask|ButtonReleaseMask|PointerMotionMask;

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

    Window parentId = RootWindow(display, screen);

    unsigned int borderWidth = 0;

    // Create the X11 window
    _drawable = XCreateWindow(display, parentId, 20, 20, 800, 600, borderWidth, DefaultDepth(display, screen), InputOutput, DefaultVisual(display, screen), winMask, &wattr);

    _brushGc = XCreateGC( display, _drawable,0, 0);

    XSync(display, false);

	show();
}

void GfxOutputImpl::destroy()
{
	if(_drawable != 0)
	{
		Display* display = Application::instance().impl()->display();
	    XFreeGC(display, _brushGc);
		XDestroyWindow(display, _drawable);
		XSync(display, false);
	}
}

GfxOutputImpl::~GfxOutputImpl()
{
	destroy();
}

void GfxOutputImpl::setTitle(const Pt::String& text)
{
    Display* display = Application::instance().impl()->display();
    XTextProperty tp;

    std::stringstream ss;
    Pt::TextStream textStream(ss, new Pt::Utf8Codec());
    textStream << text << Char(0); // Append extra \0 for proper line termination.
    textStream.flush();

    std::string textString = ss.str();
    const char* addressOfTextString = textString.c_str();
    if (XmbTextListToTextProperty(display, (char**)&addressOfTextString, 1, XStringStyle, &tp) >= 0)
    {
        //no error occured, but possibly not all characters could be converted
        XSetWMName(display, _drawable, &tp);
        XFree( tp.value );
    }
    XSync(display, false);
}


void GfxOutputImpl::show()
{
    Display* display = Application::instance().impl()->display();
    XMapWindow(display, _drawable);

    XSync(display, false);
}

void GfxOutputImpl::hide()
{
	Display* display = Application::instance().impl()->display();
    XUnmapWindow(display, _drawable);
    XSync(display, false);
}

void GfxOutputImpl::move(size_t x, size_t y)
{
	Display* display = Application::instance().impl()->display();
    XMoveWindow(display, _drawable, x, y);

    // X11 does not create move events, when we resize ourselves
    // so we report it directly to the X11 event loop.
    XClientMessageEvent event;
    event.send_event = False;
    event.type = ClientMessage;
    event.display = display;
    event.window = _drawable;
    event.message_type = AtomWindowMove;
    event.format = 32;
    event.data.l[0] = x;
    event.data.l[1] = y;

    XPutBackEvent(display, (XEvent*)&event);
    XSync(display, false);
}

void GfxOutputImpl::resize(size_t width, size_t height)
{
    width = std::max(size_t(1), width);
    height = std::max(size_t(1), height);

	Display* display = Application::instance().impl()->display();
    XResizeWindow( display, _drawable, width, height );

    // X11 does not create resize events, when we resize ourselves
    // so we report it directly to the X11 event loop.
    XClientMessageEvent event;
    event.send_event = False;
    event.type = ClientMessage;
    event.display = display;
    event.window = _drawable;
    event.message_type = AtomWindowResize;
    event.format = 32;
    event.data.l[0] = width;
    event.data.l[1] = height;

    XPutBackEvent(display, (XEvent*)&event);
    XSync(display, false);
}


void GfxOutputImpl::onPaint(XEvent& xev)
{
	paint();
}

void GfxOutputImpl::paint()
{
	if( _rgb88Image.width() > 0 && _rgb88Image.height() > 0)
		drawIndependentImage(0, 0, (char*)_rgb88Image.data(), _rgb88Image.width(), _rgb88Image.height());  
}

void GfxOutputImpl::output()
{
	_rgb88Image.resize(_model->PaintBuffer.width(), _model->PaintBuffer.height());

	for( size_t x = 0; x < _model->PaintBuffer.width(); ++x)
	{
		for(size_t y = 0; y < _model->PaintBuffer.height(); ++y)
		{
			const Pt::Gfx::ARgbColor& pixel = _model->PaintBuffer.pixel(x,y);

			Pt::Gfx::Rgb888Color color((Pt::uint8_t) pixel.red(), (Pt::uint8_t) pixel.green(), (Pt::uint8_t) pixel.blue());
			_rgb88Image.setColor(x,y,color);
		}
	}
}

void GfxOutputImpl::output(Pt::Hmi::Model* model)
{
	WindowModel* wmodel = dynamic_cast<WindowModel*>(model);

	_model = wmodel;

	if( wmodel == 0)
		throw std::logic_error("ERROR: WindowModel model expected!");

	if(wmodel->Closed.get())
	{
		if(_drawable != 0)
		{
			destroy();
			return;
		}
	}
	else
	{
		if(_drawable == 0)
		{
			if(wmodel->Visible.get())
				create();
			else
				return;
		}
	}

	output();
	paint();
}

}}

