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
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/PointingDevice.h>
#include <Pt/Hmi/GfxOutput.h>
#include <Pt/Hmi/WindowController.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "KeyHandler.h"
#include <ctype.h>

namespace Pt{
namespace Hmi{

GfxOutputImpl::GfxOutputImpl()
: _ignoreSizeEvent(false)
, _model(0)
, _window(0)
{
	_mouseEvent.buttons().resize(3);
	_display = Application::instance().impl()->display();

    AtomAppWake      = XInternAtom(_display, "PT_APP_WAKE",      false);
    AtomWindowResize = XInternAtom(_display, "PT_WINDOW_RESIZE", false);
    AtomWindowMove   = XInternAtom(_display, "PT_WINDOW_MOVE",   false);
    AtomWindowClosed = XInternAtom(_display, "WM_DELETE_WINDOW", false);
    AtomWMProtocols  = XInternAtom(_display, "WM_PROTOCOLS",     false);

	Application::instance().impl()->WindowEvent += Pt::slot(*this, &GfxOutputImpl::onWindowEvent);
	create();
}

void GfxOutputImpl::drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height)
{    
    unsigned int screen = DefaultScreen(_display);

    Visual* visual = XDefaultVisual(_display, screen);
    
	int depth = XDefaultDepth(_display, screen);

    XImage* ximage = XCreateImage(_display, visual, depth, ZPixmap, 0, NULL, width, height, 8, 0);
    ximage->data = (char*)data;
    XPutImage(_display,_window, _brushGc, ximage, 0, 0, x, y, width, height);

    XSync(_display, false);
    ximage->data = NULL;
    XDestroyImage(ximage);	
}


void GfxOutputImpl::onClientMessage(XEvent& xev)
{
	if( _model == 0)
		return;

	if(_ignoreSizeEvent)
	{
		_ignoreSizeEvent = false;
		return;
	}

    if(xev.xclient.message_type == AtomWindowResize) 
	{
        int width = xev.xclient.data.l[0];
        int height = xev.xclient.data.l[1];

		_model->Size = _model->toUnit(Pt::Gfx::Size(width, height));
		_model->WinSize =_model->Size;
		std::cout<<"Client message"<<std::endl;
        return;
    }

    if( xev.xclient.message_type == AtomWindowMove ) 
	{
        int x = xev.xclient.data.l[0];
        int y = xev.xclient.data.l[1];

		Window child_return;
		int x_return;
		int y_return;
   

		XTranslateCoordinates(_display, _window, RootWindow(_display,0),0,0,&x_return, &y_return, & child_return);
		
		_model->Position = _model->toUnit(Pt::Gfx::Point(x,y));
		_model->WinPos =_model->toUnit(Pt::Gfx::Point(x_return,y_return));
        return;
    }

    if(xev.xclient.message_type == AtomWMProtocols) 
 	{
		WindowController* controller =  (WindowController*) _model->controller();
	
		bool canClose = false;

		controller->Closing.send(canClose);

		if(canClose)
		{
			controller->close();
			_model->Closed = true;	
		}
	}	
}

void GfxOutputImpl::onMotionNotify(XEvent& xev)
{
    int x = xev.xmotion.x;
    int y = xev.xmotion.y;
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

		//TODO: wheel
        //case Button4: button = MouseEvent::WheelUp; break;
        //case Button5: button = MouseEvent::WheelDown; break;
    }

	Pt::Gfx::PointF pos = _model->toUnit(Pt::Gfx::Point(x,y));

	_mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
}

void GfxOutputImpl::onConfigureNotify( XEvent& xev)
{
	if(_ignoreSizeEvent)
	{
		_ignoreSizeEvent = false;
		return;
	}

	XWindowAttributes attr;
   
	XGetWindowAttributes(_display,  _window, &attr);
	

	_model->WinSize = _model->toUnit(Pt::Gfx::Size(xev.xconfigure.width, xev.xconfigure.height));
	_model->Size =  _model->toUnit(Pt::Gfx::Size(attr.width, attr.height));

	if((xev.xconfigure.x - attr.x != 0) && (xev.xconfigure.y - attr.y != 0))
	{
		_model->WinPos = _model->toUnit(Pt::Gfx::Point( xev.xconfigure.x - attr.x, xev.xconfigure.y - attr.y));
		_model->Position = 	_model->toUnit(Pt::Gfx::Point(xev.xconfigure.x, xev.xconfigure.y));
	}

}

void GfxOutputImpl::onKeyEvent(XEvent& xev)
{
    if(KeyRelease == xev.xkey.type)
        _keyEvent.setState(KeyEvent::KeyUp);
	else
		_keyEvent.setState(KeyEvent::KeyDown);
    
    KeySym sym = 0;
	int vcode = 0;
	
	sym = *(XGetKeyboardMapping(_display, xev.xkey.keycode,1,&vcode));

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
	
	if(ev.xany.window != _window)
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
    unsigned int screen = DefaultScreen(_display);

    XSetWindowAttributes wattr;
    memset(&wattr, 0, sizeof(wattr));
    wattr.colormap = DefaultColormap(_display, screen);

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

    Window parentId = RootWindow(_display, screen);

    unsigned int borderWidth = 0;

    // Create the X11 window
    _window = XCreateWindow(_display, parentId, 20, 20, 800, 600, borderWidth, DefaultDepth(_display, screen), InputOutput, DefaultVisual(_display, screen), winMask, &wattr);

    _brushGc = XCreateGC( _display, _window,0, 0);

	XSetWMProtocols(_display, _window, &AtomWindowClosed, 1);
    XSync(_display, false);

	show();
}

void GfxOutputImpl::destroy()
{
	if(_window != 0)
	{
	    XFreeGC(_display, _brushGc);
		_brushGc = 0;
		XDestroyWindow(_display, _window);
		XSync(_display, false);
		_window = 0;
	}
}

GfxOutputImpl::~GfxOutputImpl()
{
	destroy();
}

void GfxOutputImpl::show()
{
    XMapWindow(_display, _window);
    XSync(_display, false);
}

void GfxOutputImpl::hide()
{
	Display* display = Application::instance().impl()->display();
    XUnmapWindow(display, _window);
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


void GfxOutputImpl::writeWindowProperties()
{

	XSizeHints sizeHints;
	long suppliedReturn = 0;

    XGetWMNormalHints(_display,_window, &sizeHints, &suppliedReturn);


	if(_model->Visible.get())
		show();
	else
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
		{//Fixed size
			sizeHints.flags |= PMinSize | PMaxSize;
			sizeHints.min_width  = _model->WinSize.get().width();
			sizeHints.max_width  = _model->WinSize.get().width();
			sizeHints.min_height = _model->WinSize.get().height();
			sizeHints.max_height = _model->WinSize.get().height();
		}
		break;
		default:
		break;
	}


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

	//TODO : Windows state min/max/normal
	switch(_model->WindowState.get())
	{
		case Pt::Hmi::WindowStateType::Normal:
		break;

		case Pt::Hmi::WindowStateType::Maximazed:

		break;

		case Pt::Hmi::WindowStateType::Minimized:

		break;
	}

	//TODO: window border 
	switch( _model->Border.get())
	{
		case Pt::Hmi::WindowBorderType::NoBorder:
			
		break;

		case Pt::Hmi::WindowBorderType::Sizeable:

		break;

		case Pt::Hmi::WindowBorderType::Dialog:
		break;

		case Pt::Hmi::WindowBorderType::DialogSizeable:
		break;

		case Pt::Hmi::WindowBorderType::Tool:
		break;

		case Pt::Hmi::WindowBorderType::ToolSizeable:
		break;

	}

	//TODO: show in taskbar
	if(_model->ShowInTaskbar.get())
	{
	}
	else
	{
	}


	XSetWMNormalHints(_display, _window,  &sizeHints);
}

void GfxOutputImpl::writeWindowSizeAndPos()
{
	_ignoreSizeEvent = true;

	Pt::Gfx::Point pos = _model->fromUnit(_model->WinPos.get());
	Pt::Gfx::Size size = _model->fromUnit(_model->WinSize.get());		

	XMoveResizeWindow(_display, _window,  pos.x(), pos.y(), size.width(), size.height());			
}

void GfxOutputImpl::output(Pt::Hmi::Model* model)
{
	WindowModel* wmodel = dynamic_cast<WindowModel*>(model);

	_model = wmodel;

	if( wmodel == 0)
		throw std::logic_error("ERROR: WindowModel model expected!");

	if(wmodel->Closed.get())
	{
		if(_window != 0)
		{
			destroy();
			return;
		}
	}
	else
	{
		if(_window == 0)
		{
			if(wmodel->Visible.get())
				create();
			else
				return;
		}
	}


	writeWindowSizeAndPos();
	writeWindowProperties();
	output();
	paint();
}

}}

