/*
 * Copyright (C) 2006 Marc Boris Duerner
 * Copyright (C) 2006 Aloysius Indrayanto
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "WidgetImpl.h"
#include "WidgetPainterImpl.h"
#include "ApplicationImpl.h"

#include <Pt/Gui/Application.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Text/TextStream.h>
#include <Pt/Text/Utf8Codec.h>

#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;


namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Gfx::Point& at, const Gfx::Size& size)
: _parent(parent)
, _painter(0)
{
    // Display and Screen are inited in Application
    Display* display = X11EventLoop::instance().display();
    unsigned int screen = DefaultScreen(display);

    XSetWindowAttributes wattr;
    memset(&wattr, 0, sizeof(wattr));
    wattr.colormap = DefaultColormap(display, screen);

    // The events we want to receive
    wattr.event_mask = StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|
                       LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask|
                       ButtonPressMask|ButtonReleaseMask|PointerMotionMask;

    // Propagate these events?
    // NOTE (blue_wind_25): * It will depends on how we will send event to our controls,
    //                        normally, we just let our our controls draw/do whatever
    //                        they need based on the event (so just give the controls
    //                        the event the main window just got)
    //                      * However, for performance, seems that emulating the event
    //                        propagation will be better (thats is propagate the event
    //                        via Pt's internal event managament instead of asking the
    //                        X server to do this automatically). Isn't it what the
    //                        current implementation does?
    wattr.do_not_propagate_mask = KeyPressMask|KeyReleaseMask|ButtonPressMask|
                                  ButtonReleaseMask|PointerMotionMask|ButtonMotionMask;

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
    /*if(_parent) {
        wattr.override_redirect = True;
    }*/

    // Determines which fields from XSetWindowAttributes are used
    unsigned long winMask = CWWinGravity|CWBitGravity|CWBorderPixmap|CWBorderPixel|CWEventMask|CWDontPropagate|
                            CWCursor|CWOverrideRedirect|CWColormap|CWBackingStore|CWSaveUnder|CWBackPixmap;

    Window parentId;
    // Top level window
    if(!_parent)
        parentId = RootWindow(display, screen);
    // Subwindow
    else
        parentId = _parent->impl().x11Drawable();

    unsigned int borderWidth = 0;

    // Create the X11 window
    _drawable = XCreateWindow(display,
                              parentId,
                              at.x(),
                              at.y(),
                              std::max(size_t(1), size.width() ), // at least 1
                              std::max(size_t(1), size.height() ), // at least 1
                              borderWidth,
                              DefaultDepth(display, screen),
                              InputOutput,
                              DefaultVisual(display, screen),
                              winMask,
                              &wattr);

    XSync(display, false);

    // Closing a window generates a ClientMessage, which we convert to a close event.
    Atom atomWMDeleteWindow = X11EventLoop::instance().AtomWindowClosed;
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

    // Remove this window from widget map
    X11EventLoop::instance().unregisterWidget(_drawable);

    Display* display = X11EventLoop::instance().display();
    XDestroyWindow(display, _drawable);
    XSync(display, false);
}


void WidgetImpl::setTitle(const Pt::String& text)
{
    _title = text;
    Display* display = X11EventLoop::instance().display();
    XTextProperty tp;

    std::stringstream ss;
    Pt::Text::TextStream textStream(ss, new Pt::Text::Utf8Codec());
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


Painter WidgetImpl::painter()
{
    if (!_painter) _painter = new WidgetPainterImpl(*this);

    return Painter(_painter);
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


void WidgetImpl::setParent(Widget* parent)
{
    Window parentId;
    Display* display = X11EventLoop::instance().display();
    unsigned int screen = XDefaultScreen(display);

    if(!parent)
        parentId = XRootWindow(display, screen);
    else
        parentId = parent->impl().x11Drawable();

    XReparentWindow(display, _drawable, parentId, 0, 0);
    XSync(display, false);
}


void WidgetImpl::move(size_t x, size_t y)
{
    Display* display = X11EventLoop::instance().display();
    XMoveWindow(display, _drawable, x, y);

    // X11 does not create move events, when we resize ourselves
    // so we report it directly to the X11 event loop.
    XClientMessageEvent event;
    event.send_event = False;
    event.type = ClientMessage;
    event.display = display;
    event.window = _drawable;
    event.message_type = X11EventLoop::instance().AtomWindowMove;
    event.format = 32;
    event.data.l[0] = x;
    event.data.l[1] = y;

    XPutBackEvent(display, (XEvent*)&event);
    XSync(display, false);
}


void WidgetImpl::resize(size_t width, size_t height)
{
    width = std::max(size_t(1), width);
    height = std::max(size_t(1), height);

    Display* display = X11EventLoop::instance().display();
    XResizeWindow( display, _drawable, width, height );

    // X11 does not create resize events, when we resize ourselves
    // so we report it directly to the X11 event loop.
    XClientMessageEvent event;
    event.send_event = False;
    event.type = ClientMessage;
    event.display = display;
    event.window = _drawable;
    event.message_type = X11EventLoop::instance().AtomWindowResize;
    event.format = 32;
    event.data.l[0] = width;
    event.data.l[1] = height;

    XPutBackEvent(display, (XEvent*)&event);
    XSync(display, false);
}


//setMinimumSize:
//    XSizeHints sh;
//    memset(&sh, 0, sizeof(sh));
//    sh.flags = PMinSize | PMaxSize;
//    sh.min_width  = sh.max_width  = width;
//    sh.min_height = sh.max_height = height;
//    XSetStandardProperties(display, window, window_name, icon_name, icon_pixmap, argv, argc, hints)


} // namespace Gui

} // namespace Pt
