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

#include "ApplicationImpl.h"

#include <X11/Xft/Xft.h>

#include "Pt/Exception.h"
#include "Pt/Gui/Application.h"
#include "Pt/Gui/Widget.h"
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Gui/CloseEvent.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/MouseEvent.h>
#include <Pt/Gui/MouseMoveEvent.h>
#include <Pt/Gui/MoveEvent.h>
#include <Pt/Gui/KeyEvent.h>
#include <Pt/Gui/Widget.h>

#include <iostream>
using namespace std;


namespace Pt {

namespace Gui {

/*
struct SymPoint {
	int keySmy;
	wchar_t utfChar;
};


static SymPoint sympoints[] = {
	{ 0x01a1, 0x0104 }, // Aogonek LATIN CAPITAL LETTER A WITH OGONEK
	{ 0x01a2, 0x02d8 }, // breve BREVE
	{ 0x01a3, 0x0141 }, // Lstroke LATIN CAPITAL LETTER L WITH STROKE
	{ 0x01a3, 0x0000 }, // unknown
	{ 0x01a5, 0x013d }, // Lcaron LATIN CAPITAL LETTER L WITH CARON
};
*/



X11EventLoop::X11EventLoop()
: _stop(false), _display(0)
{
	_display = XOpenDisplay(NULL);
	if(0 == _display) {
		throw RuntimeError("Could not open X11 display.", PT_SOURCEINFO);
	}

	XSync(_display, false);

	XSynchronize(_display, true);
	// do we really need this?
	XftInit(0);
}


X11EventLoop::~X11EventLoop()
{
	XSync(_display, true);
	XCloseDisplay(_display);
	_display = NULL;
}


void X11EventLoop::registerWidget(Window winId, Widget& widget)
{
	_widgets.insert( std::make_pair(winId, &widget) );
}


void X11EventLoop::unregisterWidget(Window winId)
{
	_widgets.erase(winId);
}


Widget* X11EventLoop::findWidget(Window winId)
{
	std::map<Window, Widget*>::iterator it = _widgets.find(winId);
	if( it == _widgets.end() ) {
		return 0;
	}

	return it->second;
}


int X11EventLoop::run()
{
	while( _stop == false)
	{
		struct timeval delta;
		delta.tv_usec = 1000;
		delta.tv_sec = 0;

		Display* display = X11EventLoop::instance().display();

		fd_set readfds;
		int xfd = XConnectionNumber(display);

		FD_ZERO(&readfds);
		FD_SET( xfd, &readfds );

		int nfds = ::select(xfd + 1, &readfds, 0, 0, &delta);
		if(nfds < 0 && errno != EAGAIN && errno != EINTR) {
			std::clog << "Error: select failed." << std::endl;
		}

		// check if we have pending X events
		if( XPending(display) <= 0 ) {
			continue;
		}

		// get next event
		XNextEvent(display, &_xev);

		// check which window receives the event
		Widget* widget = X11EventLoop::instance().findWidget( _xev.xany.window );
		if(widget == 0) {
			continue;
		}

		//clog << "X11 Event: #" << _xev.xany.type << endl;
		switch( _xev.xany.type ) {
			case ClientMessage:   clientMessage(*widget, _xev);   break;
			case MotionNotify:    motionNotify(*widget, _xev);    break;
			case ButtonPress:     buttonPress(*widget, _xev);     break;
			case ButtonRelease:   buttonRelease(*widget, _xev);   break;
			case Expose:          expose(*widget, _xev);          break;
			case NoExpose:        noExpose(*widget, _xev);        break;
			case ConfigureNotify: configureNotify(*widget, _xev); break;
			case KeyPress:        keyEvent(*widget, _xev);        break;
			case KeyRelease:      keyEvent(*widget, _xev);        break;
		}

		this->processEvents();
	}

	return 0;
}


void X11EventLoop::wake()
{
}


void X11EventLoop::commitEvent(const Pt::Event& event)
{
	this->queueEvent(event);
	this->wake();
}


void X11EventLoop::queueEvent(const Pt::Event& event)
{
	_queueMutex.lock();

	Pt::Event* ev = event.clone();
	_eventQueue.push_back(ev);

	_queueMutex.unlock();
}


void X11EventLoop::processEvents()
{
	while( true )
	{
		_queueMutex.lock();

		if( _eventQueue.empty() ) {
			_queueMutex.unlock();
			break;
		}

		Pt::Event* ev = _eventQueue.front();
		_eventQueue.remove(ev);
		_queueMutex.unlock();

		event.send<const Pt::Event&>(*ev);
		delete ev;
	}
}


void X11EventLoop::exit()
{
	_stop = true;
	this->wake();
}


void X11EventLoop::clientMessage(Widget& widget, XEvent& xev)
{
	CloseEvent ev(widget);
	event.send(ev);
}


void X11EventLoop::keyEvent(Widget& widget, XEvent& xev)
{
	KeyEvent::Type type = KeyEvent::Press;
	if(KeyRelease == xev.xkey.type) {
		type = KeyEvent::Release;
	}

	KeySym sym = 0;
	char buffer[20]; // unused really
	XLookupString(&xev.xkey, buffer, sizeof(buffer), &sym, NULL);

	wchar_t ch = this->keysymToUtf(sym);

	KeyEvent::KeyCode code = KeyEvent::Void;
	switch( sym ) {
		case XK_Control_L: code = KeyEvent::ControlL; break;
		case XK_Control_R: code = KeyEvent::ControlR; break;
		case XK_Alt_L:     code = KeyEvent::AltL; break;
		case XK_Alt_R:     code = KeyEvent::AltR; break;
		case XK_F1:        code = KeyEvent::F1; break;
		case XK_F2:        code = KeyEvent::F2; break;
		case XK_F3:        code = KeyEvent::F3; break;
		case XK_F4:        code = KeyEvent::F4; break;
		case XK_F5:        code = KeyEvent::F5; break;
		case XK_F6:        code = KeyEvent::F6; break;
		case XK_F7:        code = KeyEvent::F7; break;
		case XK_F8:        code = KeyEvent::F8; break;
		case XK_F9:        code = KeyEvent::F9; break;
		case XK_F10:       code = KeyEvent::F10; break;
		case XK_F11:       code = KeyEvent::F11; break;
		case XK_F12:       code = KeyEvent::F12; break;
	}

	KeyEvent ev(widget, type, code, ch);
	event.send( ev );
}


void X11EventLoop::motionNotify(Widget& widget, XEvent& xev)
{
	size_t x = xev.xmotion.x;
	size_t y = xev.xmotion.y;

	int modifiers = 0;
	if( xev.xmotion.state & ShiftMask)
		modifiers |= MouseMoveEvent::ShiftDown;
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

	MouseMoveEvent ev(widget, x, y, modifiers);
	event.send( ev );
}


void X11EventLoop::buttonPress(Widget& widget, XEvent& xev)
{
	size_t x = xev.xbutton.x;
	size_t y = xev.xbutton.y;
	MouseEvent::Button button = MouseEvent::LeftButton;

	switch(xev.xbutton.button) {
		case Button1: button = MouseEvent::LeftButton; break;
		case Button2: button = MouseEvent::MiddleButton; break;
		case Button3: button = MouseEvent::RightButton; break;
		case Button4: button = MouseEvent::WheelUp; break;
		case Button5: button = MouseEvent::WheelDown; break;
	}

	MouseEvent ev( widget, x, y, button, MouseEvent::Press, 0 );
	event.send( ev );
}


void X11EventLoop::buttonRelease(Widget& widget, XEvent& xev)
{
	size_t x = xev.xbutton.x;
	size_t y = xev.xbutton.y;
	MouseEvent::Button button = MouseEvent::LeftButton;

	switch(xev.xbutton.button) {
		case Button1: button = MouseEvent::LeftButton; break;
		case Button2: button = MouseEvent::MiddleButton; break;
		case Button3: button = MouseEvent::RightButton; break;
		case Button4: button = MouseEvent::WheelUp; break;
		case Button5: button = MouseEvent::WheelDown; break;
	}

	MouseEvent ev( widget, x, y, button, MouseEvent::Release, 0);
	event.send( ev );
}


void X11EventLoop::expose(Widget& widget, XEvent& xev)
{
	const size_t width = xev.xexpose.width;
	const size_t height = xev.xexpose.height;
	const size_t x = xev.xexpose.x;
	const size_t y = xev.xexpose.y;

	PaintEvent ev(widget, x, y, width, height);
	event.send( ev );
}


void X11EventLoop::noExpose(Widget& widget, XEvent& xev)
{
	//TODO: ... but what?
}


void X11EventLoop::configureNotify(Widget& widget, XEvent& xev)
{
	const size_t width = xev.xconfigure.width;
	const size_t height = xev.xconfigure.height;
	const size_t x = xev.xconfigure.x;
	const size_t y = xev.xconfigure.y;

	if(widget.rect().width() != width || widget.rect().height() != height) {
		ResizeEvent ev(widget, width, height);
		event.send( ev );
	}

	if(widget.rect().x1() != x || widget.rect().height() != y) {
		MoveEvent ev(widget, x, y);
		event.send( ev );
	}
}


wchar_t X11EventLoop::keysymToUtf(int sym)
{
  // directly map latin-1 characters
	if((0x0020 <= sym && sym <= 0x007e) || (0x00a0 <= sym && sym <= 0x00ff)) {
		return sym;
	}

  // key pad to Latin-1
	if(0xffaa <= sym && sym <= 0xffb9)
		return sym - 0xff80;

  // Also check for directly encoded 24-bit UCS characters
	if( (sym & 0xff000000) == 0x01000000 )
		return sym & 0x00ffffff;

	//TODO: use SymPoint map for remaining chars

	// No translation possible, yes returning 0 is correct!
 	return 0;
}





ApplicationImpl::ApplicationImpl(Application& app)
: _app(app)
{
	connect(X11EventLoop::instance().event, *this, &ApplicationImpl::dispatchEvent);
}


ApplicationImpl::~ApplicationImpl()
{
	X11EventLoop::instance().exit();
}


void ApplicationImpl::commitEvent(const Pt::Event& event)
{
	X11EventLoop::instance().commitEvent(event);
}


void ApplicationImpl::queueEvent(const Pt::Event& event)
{
	X11EventLoop::instance().queueEvent(event);
}


void ApplicationImpl::processEvents()
{
	X11EventLoop::instance().processEvents();
}


int ApplicationImpl::run()
{
	return X11EventLoop::instance().run();
}


void ApplicationImpl::exit()
{
	X11EventLoop::instance().exit();
}


} // namespace Gui

} // namespace Pt

