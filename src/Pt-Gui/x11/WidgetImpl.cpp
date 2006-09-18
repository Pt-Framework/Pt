/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "WidgetImpl.h"
#include "WidgetPainterImpl.h"
#include "ApplicationImpl.h"

#include <Pt/Gui/Application.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>

#include <iostream>
using namespace std;


namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Gfx::Point& at, const Gfx::Size& size)
: _apiWidget(apiWidget),
  _parent(parent),
  _rect(at, size)
, _painter(0)
{
	// Display and Screen are inited in Application
	Display* display = X11EventLoop::instance().display();
	unsigned int screen = DefaultScreen(display);

	XSetWindowAttributes wattr;
	memset(&wattr, 0, sizeof(wattr));
	wattr.colormap = DefaultColormap(display, screen);

	// the events we want to receive
	wattr.event_mask = StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|
	                   LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask|
	                   ButtonPressMask|ButtonReleaseMask|PointerMotionMask;

	// propagate these events?
	wattr.do_not_propagate_mask = KeyPressMask|KeyReleaseMask|ButtonPressMask|
	                              ButtonReleaseMask|PointerMotionMask|ButtonMotionMask;

	// border
	wattr.border_pixel = 0; // needed for OpenGL
	wattr.border_pixmap = CopyFromParent;

	// Background
	wattr.background_pixmap = None ;
	//wattr.background_pixel = XWhitePixel(display, screen);

	// backing
	wattr.backing_store = Always;
	wattr.save_under = True;

	// gravity
	wattr.bit_gravity = ForgetGravity; // region to be retained on resize
	wattr.win_gravity = NorthWestGravity; // how to to reposition when parent resizes

	wattr.cursor = None; // None means parents cursor

	wattr.override_redirect = False; // no WM interaction if True
	if(_parent) {
		wattr.override_redirect = True;
	}

	// determines which fields from XSetWindowAttributes are used
	unsigned long winMask = CWBackPixmap|CWWinGravity|CWBitGravity|CWBorderPixmap|CWBorderPixel|CWEventMask|CWDontPropagate|
	                        CWCursor|CWOverrideRedirect|CWSaveUnder|CWColormap|CWBackingStore;

	Window parentId;
	if(!_parent) { // top level window
		parentId = RootWindow(display, screen);
	}
	else { // subwindow
		parentId = _parent->impl().x11Drawable();
		parent->impl().addChild(apiWidget);
	}

	unsigned int borderWidth = 0;

	// create the X11 window
	_drawable = XCreateWindow(display,
	                          parentId,
	                          _rect.x1(),
	                          _rect.y1(),
	                          _rect.width(), // at least 1
	                          _rect.height(), // at least 1
	                          borderWidth,
	                          DefaultDepth(display, screen),
	                          InputOutput,
	                          DefaultVisual(display, screen),
	                          winMask,
	                          &wattr);

	XSync(display, false);

	// closing a window generates a ClientMessage, which we convert to a close event.
	Atom atomWMDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(display, _drawable, &atomWMDeleteWindow, 1);

	// Child windows are visible by default
	if(parent) {
		XMapWindow(display, _drawable);
	}

	XSync(display, false);

	// Store for X11 window Id to C++ object mapping
	X11EventLoop::instance().registerWidget(_drawable, apiWidget);
}


WidgetImpl::~WidgetImpl()
{
	delete _painter;

	// remove this window from widget map
	X11EventLoop::instance().unregisterWidget(_drawable);

	// unregister from parent
	if(_parent) {
		_parent->impl().removeChild(_apiWidget);
		_parent = 0;
	}

	// unparent any leftover children
	while(_childWidgets.empty() == false) {
		Widget* child = _childWidgets.front();
		child->impl().reparent(0);
	}

	Display* display = X11EventLoop::instance().display();
	XDestroyWindow(display, _drawable);
	XSync(display, false);

	// mark window invalid
	_drawable = 0;
}


void WidgetImpl::setTitle(const char* text)
{
	Display* display = X11EventLoop::instance().display();
	XTextProperty tp;
	XmbTextListToTextProperty(display, (char**)&text, 1, XStringStyle, &tp);
	XSetWMName(display, _drawable, &tp);
	XFree( tp.value );
	XSync(display, false);
}


WidgetPainter& WidgetImpl::getPainter()
{
	if (0 == _painter) {
		_painter = new WidgetPainter(*this);
	}

	return *_painter;
}


void WidgetImpl::show()
{
	Display* display = X11EventLoop::instance().display();
	XMapWindow(display, _drawable);

	XSync(display, false);
}


void WidgetImpl::hide()
{
	Display* display = X11EventLoop::instance().display();
	XUnmapWindow(display, _drawable);
	XSync(display, false);
}


void WidgetImpl::resizeEvent(const ResizeEvent& event)
{
	_rect.setWidth( event.width() );
	_rect.setHeight( event.height() );
}


void WidgetImpl::unparent()
{
	this->reparent(0);
}


void WidgetImpl::reparent(Widget* parent)
{
	if (_parent == parent) {
		return;
	}

	Window parentId;
	Display* display = X11EventLoop::instance().display();
	unsigned int screen = XDefaultScreen(display);

	if( 0 == parent) {
		parentId = XRootWindow(display, screen);
	}
	else {
		parent->impl().addChild(_apiWidget);
		parentId = parent->impl().x11Drawable();
	}

	if(_parent) {
		_parent->impl().removeChild(_apiWidget);
		_parent = parent;
	}

	XReparentWindow(display, _drawable, parentId, 0, 0);
	XSync(display, false);
}


void WidgetImpl::move(size_t x, size_t y)
{
	Display* display = X11EventLoop::instance().display();
	XMoveWindow(display, _drawable, x, y);
	XSync(display, false);

	_rect.setX1(x);
	_rect.setY1(y);
}


void WidgetImpl::resize(size_t width, size_t height)
{
	Display* display = X11EventLoop::instance().display();
	_rect.setWidth( std::max(width, size_t(1)) );
	_rect.setHeight( std::max(height, size_t(1)) );

	XResizeWindow(display, _drawable, _rect.width(), _rect.height());
	XSync(display, false);
}


//setMinimumSize:
//	XSizeHints sh;
//	memset(&sh, 0, sizeof(sh));
//	sh.flags = PMinSize | PMaxSize;
//	sh.min_width  = sh.max_width  = width;
//	sh.min_height = sh.max_height = height;
//	XSetStandardProperties(display, window, window_name, icon_name, icon_pixmap, argv, argc, hints)


} // namespace Gui

} // namespace Pt
