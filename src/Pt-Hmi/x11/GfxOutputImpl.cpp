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
, _visible(false)
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


void GfxOutputImpl::pixelToScreen(char* data, const Pt::Gfx::ARgbColor& pixel)
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

//Direct draw
#if 0
void GfxOutputImpl::drawIndependentImage(const Pt::Gfx::ARgbImage& image)
{    

	for(size_t y = 0; y < image.height(); ++y)
	{
		for( size_t x = 0; x < image.width(); ++x)
		{
			const Pt::Gfx::ARgbColor& pixel = _model->PaintBuffer.pixel(x,y);

			XGCValues gcv;

			pixelToScreen((char*)&gcv.foreground, pixel);		
			
			XChangeGC (_display, _brushGc, GCForeground, &gcv);
			XDrawPoint(_display, _window, _brushGc, x,y);
		}
	}

  XSync(_display, false);
}
#endif

//Buffered draw
void GfxOutputImpl::drawIndependentImage(const Pt::Gfx::ARgbImage& image)
{    

	updateDrawBuffer();
	unsigned int sreen  = DefaultScreen(_display);
	int depth = XDefaultDepth(_display, sreen);

	int pixelSize = depth == 16 ? 2 : 4;	

	for(size_t y = 0; y < image.height(); ++y)
	{
		const int lineOffset = y *(image.width()*pixelSize);

		for( size_t x = 0; x < image.width(); ++x)
		{
			const Pt::Gfx::ARgbColor& pixel = _model->PaintBuffer.pixel(x,y);
			const int pixelOffset = lineOffset +(x* pixelSize);				
			pixelToScreen((char*)&_pixelBuffer[pixelOffset], pixel);					
		}
	}


	Visual* visual = XDefaultVisual(_display, sreen);

	XImage* ximage = XCreateImage(_display, visual, depth, ZPixmap, 0, NULL, image.width(), image.height(), 8, 0);
	ximage->data = (char*)&(_pixelBuffer[0]);
	
	XPutImage(_display,_window, _brushGc, ximage, 0, 0, 0,0, image.width(), image.height());
	
	XSync(_display, false);
	ximage->data = NULL;
	XDestroyImage(ximage);	
}

void GfxOutputImpl::onClientMessage(XEvent& xev)
{
	if( _model == 0)
		return;

    if(xev.xclient.message_type == AtomWMProtocols) 
 	{
		WindowController* controller =  (WindowController*) _model->controller();
	
		bool canClose = false;

		controller->Closing.send(canClose);

		if(canClose)
			destroy();
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

void GfxOutputImpl::redraw()
{
    XEvent exppp;
    memset(&exppp, 0, sizeof(exppp));
    exppp.type = Expose;
    exppp.xexpose.window = _window;

    XSendEvent(_display,_window,False,ExposureMask,&exppp);
    XFlush(_display);
}

void GfxOutputImpl::readClientSizeAndPos(Pt::Gfx::SizeF& size, Pt::Gfx::PointF& pos)
{
	if(_window == 0)
		return;

	XWindowAttributes xwa;
	XGetWindowAttributes(_display, _window, &xwa);

	size = _model->toUnit(Pt::Gfx::Size(xwa.width, xwa.height));
	pos = _model->toUnit(Pt::Gfx::Point(xwa.x, xwa.y));

}

void GfxOutputImpl::writeWindowSizeAndPos()
{	
	Pt::Gfx::Point posGlobal = _model->fromUnit(_model->WinPos.get());
	Pt::Gfx::Point posLocal = _model->fromUnit(_model->Position.get());

	Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());	
	int x = posGlobal.x() - posLocal.x()*2;
	int y = posGlobal.y() - posLocal.y()*2;

	XMoveResizeWindow(_display, _window,  x, y, size.width(), size.height());			
}


void GfxOutputImpl::onConfigureNotify( XEvent& xev)
{
	if(!_model->Enable.get())
	{
		writeWindowSizeAndPos();
		return;
	}
	
	if(_ignoreSizeEvent)
	{
		_ignoreSizeEvent = false;
		return;
	}
	
	XWindowAttributes xwa;
	Window child;
	int    x = 0;
    int    y = 0;

	XGetWindowAttributes(_display, _window, &xwa);

	Window root = DefaultRootWindow(_display);

  	XTranslateCoordinates(_display,_window, root, xwa.x, xwa.y, &x, &y,&child);

	_model->WinSize =  _model->toUnit(Pt::Gfx::Size(xev.xconfigure.width, xev.xconfigure.height));
	_model->WinPos = _model->toUnit(Pt::Gfx::Point(x, y));
	
	Pt::Gfx::SizeF clientSize;
	Pt::Gfx::PointF clientPos;

	readClientSizeAndPos(clientSize, clientPos);

	_model->Position = clientPos;
	_model->Size = clientSize;
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
		
	unsigned int ucode = KeyHandler::keySymToUtf(sym);

	if( ucode != 0)
		_keyEvent.setUnicode(ucode);
	else
		_keyEvent.setUnicode(sym & 0xFF);
		
	Application::instance().keyDeviceEvent().send(_model->controller(), _keyEvent);
}

void GfxOutputImpl::onWindowEvent(XEvent& ev)
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

void GfxOutputImpl::destroy()
{
	if(_window != 0)
	{
	    XFreeGC(_display, _brushGc);
		_brushGc = 0;
		XDestroyWindow(_display, _window);
		XSync(_display, false);
		_window = 0;
		WindowController* controller = (WindowController*)_model->controller();
		controller->Closed.send();
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
    _visible = 	true;
}

void GfxOutputImpl::hide()
{
	Display* display = Application::instance().impl()->display();
    XUnmapWindow(display, _window);
    XSync(display, false);
    _visible = 	false;
}


void GfxOutputImpl::bringWindowToTop()
{
	XRaiseWindow(_display, _window);
	XSetInputFocus(_display, _window, RevertToNone, CurrentTime);
}

void GfxOutputImpl::onPaint(XEvent& xev)
{
	if( _model != 0)
	{
		drawIndependentImage(_model->PaintBuffer);
	}
}

void GfxOutputImpl::writeWindowProperties()
{

	XSizeHints sizeHints;
	long suppliedReturn = 0;

    XGetWMNormalHints(_display,_window, &sizeHints, &suppliedReturn);


	if(_model->Visible.get() && !_visible)
	{
		show();
	}

	if(!_model->Visible.get() && _visible)
	{
		hide();
	}
		
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

void GfxOutputImpl::updateDrawBuffer()
{
	unsigned int sreen  = DefaultScreen(_display);

	int depth = XDefaultDepth(_display, sreen);
    
	size_t pixelSize = (depth == 16) ? 2 : 4;	

	size_t currentSize = _model->PaintBuffer.width() * _model->PaintBuffer.height() * pixelSize;

	if(_pixelBuffer.size() < currentSize)
		_pixelBuffer.resize(currentSize);

}

void GfxOutputImpl::output(Pt::Hmi::Model* model)
{
	_model = dynamic_cast<WindowModel*>(model);

	if( _model == 0)
		throw std::logic_error("ERROR: WindowModel model expected!");

        
	if(!_model->Visible.get() && !_visible)
	{
		return;
	}


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

	//Initial size and position

	Pt::Gfx::SizeF clientSize;
	Pt::Gfx::PointF pos;

	readClientSizeAndPos(clientSize, pos);

	if( clientSize.width() != _model->Size.get().width() ||  clientSize.height() != _model->Size.get().height())
	{
		_model->Position = pos;
		_model->Size = clientSize;

		_ignoreSizeEvent = true;
		writeWindowSizeAndPos();	
		return;
	}

	_ignoreSizeEvent = true;
	writeWindowSizeAndPos();
	
	writeWindowProperties();

	drawIndependentImage(_model->PaintBuffer);
	redraw();
	std::cout<<"INFO: GFX output done!"<<std::endl;
}

}}

