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
#include "ScreenImpl.h"
#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/WindowStateEvent.h>

#include <cassert>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(ScreenImpl& wm, Window& w)
: WindowFrame(wm, w)
, _wm(wm)
, _client(w)
, _window(None)
, _display(0)
, _hasFirstShow(false)
, _width(240)
, _height(160)
{
  _display = Application::instance().impl()->display();

  create( w.type() );
}


MainWindowImpl::~MainWindowImpl()
{
    destroy();
}


void MainWindowImpl::create(Window::Type type)
{
    // Display and Screen are inited in Application
    unsigned int screen = XDefaultScreen(_display);
    ::Window root       = XRootWindow(_display, screen);
    ::Visual* visual = Application::instance().impl()->visual();
    int depth = Application::instance().impl()->depth();

    XSetWindowAttributes wattr;
    memset(&wattr, 0, sizeof(wattr));
    wattr.colormap = XDefaultColormap(_display, screen);

    // The events we want to receive
    wattr.event_mask = StructureNotifyMask|ExposureMask|
                       PropertyChangeMask|EnterWindowMask|
                       LeaveWindowMask|KeyPressMask|
                       KeyReleaseMask|KeymapStateMask|
                       ButtonPressMask|ButtonReleaseMask|
                       PointerMotionMask|FocusChangeMask|
                       SubstructureNotifyMask|VisibilityChangeMask|
                       FocusChangeMask|EnterWindowMask|
                       LeaveWindowMask;

    wattr.do_not_propagate_mask = NoEventMask;/*KeyPressMask|KeyReleaseMask|
                                  ButtonPressMask| ButtonReleaseMask|
                                  PointerMotionMask|ButtonMotionMask;*/

    // Border
    wattr.border_pixel = 0; // Needed for OpenGL
    wattr.border_pixmap = CopyFromParent;

    // Background
    wattr.background_pixmap = None;
    //wattr.background_pixel = XWhitePixel(display, screen);

    // Backing store
    wattr.backing_store = NotUseful; // Always;
    wattr.save_under = False;   // True;

    // Region to be retained on resize
    wattr.bit_gravity = ForgetGravity;

    // How to to reposition when parent resizes
    wattr.win_gravity = NorthWestGravity;

    // None means parents cursor
    wattr.cursor = None;

    // no WM interaction if True
    wattr.override_redirect = type == WindowType::Popup ? True : False;

    // Determines which fields from XSetWindowAttributes are used
    unsigned long winMask = CWWinGravity|CWBitGravity|
                            CWBorderPixmap|CWBorderPixel|
                            CWEventMask|CWDontPropagate|
                            CWCursor|CWOverrideRedirect|
                            CWColormap|CWBackingStore|
                            CWSaveUnder|CWBackPixmap;

    unsigned int borderWidth = 0;

    // Create the X11 window
    _window = XCreateWindow(_display,
                            root,
                            0, 0,
                            _width, _height,
                            borderWidth,
                            depth,
                            InputOutput,
                            visual,
                            winMask,
                            &wattr);

    //std::clog << std::hex << "XCreateWindow: " << _window << std::dec << std::endl;

    Atom atomDeleteWindow = Application::instance().impl()->wmDeleteWindow();
    XSetWMProtocols(_display, _window, &atomDeleteWindow, 1);

    Base::onSetParent(&wm);
}


void MainWindowImpl::destroy()
{
    if( _window == 0)
        return;

    //std::clog << std::hex << "XDestroyWindow: " << _window << std::dec << std::endl;

    XDestroyWindow(_display, _window);
    _window = 0;
}


void MainWindowImpl::onInit(Window& w)
{
    Gfx::PaintSurface& surface = this->surface();
    Gfx::PointF surfacePos(0, 0);
    w.setSurface(&surface, surfacePos);

    w.setNextResponder(this);

    double scaling = scaleFactor();
    
    RescaleEvent ev(w, scaling);
    w.processEvent(ev);
}


void MainWindowImpl::onRelease(Window& w)
{
    w.setNextResponder(0);
    w.setSurface( 0, Gfx::PointF() );
}


void MainWindowImpl::setType(Window::Type type)
{
    //std::clog << "XChangeWindowAttributes: " << type << std::endl;

    XSetWindowAttributes swattr;
    swattr.override_redirect = (type == WindowType::Popup) ? True : False;

    XChangeWindowAttributes(_display, _window, CWOverrideRedirect, &swattr);

    XWindowAttributes wattr;
    XGetWindowAttributes(_display, _window, &wattr);

    if(wattr.map_state != IsUnmapped)
    {
        XUnmapWindow(_display, _window);
        XMapWindow(_display,_window);
    }
}


Gfx::PointF MainWindowImpl::toScreen(const Gfx::PointF& pos) const
{
    Gfx::PointF physicalPos = surface().toPhysical(pos);

    ::Window root = DefaultRootWindow(_display);
    int windowX = lround( physicalPos.x() );
    int windowY = lround( physicalPos.y() );
    int screenX = 0;
    int screenY = 0;
    ::Window child = 0;

    XTranslateCoordinates(_display, _window, root,
                          windowX, windowY, &screenX, &screenY, &child);

    Gfx::PointF screenPos(screenX, screenY);
    Gfx::PointF logicalPos = surface().toLogical(screenPos);
    return logicalPos;
}


Gfx::PointF MainWindowImpl::fromScreen(const Gfx::PointF& pos) const
{
    Gfx::PointF physicalPos = surface().toPhysical(pos);

    ::Window root = DefaultRootWindow(_display);
    int screenX = lround( physicalPos.x() );
    int screenY = lround( physicalPos.y() );
    int windowX = 0;
    int windowY = 0;
    ::Window child = 0;

    XTranslateCoordinates(_display, root, _window,
                          screenX, screenY, &windowX, &windowY, &child);

    Gfx::PointF clientPos(windowX, windowY);
    Gfx::PointF logicalPos = surface().toLogical(clientPos);
    return logicalPos;
}


Gfx::PointF MainWindowImpl::onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF MainWindowImpl::onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF MainWindowImpl::onToParent(const Gfx::PointF& pos) const
{
    return _wm.fromFrame(*this, pos); 
}
     
        
Gfx::PointF MainWindowImpl::onFromParent(const Gfx::PointF& pos) const
{ 
    return _wm.toFrame(*this, pos); 
}


void MainWindowImpl::paint(const Gfx::RectF& rectF)
{
    Gfx::Rect rect = Gfx::round(rectF);
    if( rect.isNull() )
      return;

    //std::clog << "XClearArea " << title() << " "
    //                           << rectF.x() << ", " << rectF.y() << " "
    //                           << rectF.width() << "x" << rectF.height() << std::endl;

    //XExposeEvent ev = { Expose, 0, True, _display, _window,
    //                   static_cast<int>( rect.x()),
    //                   static_cast<int>( rect.y()),
    //                   static_cast<int>( rect.width()),
    //                   static_cast<int>( rect.height()),
    //                   0 };

    //Application::instance().impl()->processEvent( (XEvent&)ev);

    XClearArea(_display, _window,
               static_cast<int>( rect.x() ),
               static_cast<int>( rect.y() ),
               static_cast<int>( rect.width() ),
               static_cast<int>( rect.height() ),
               True);

    //XFlush(_display);
}


void MainWindowImpl::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF screenPos = toScreen( rect.topLeft() );
    Gfx::RectF screenRect( screenPos, rect.size() );

    _wm.repaint(screenRect);
}


void MainWindowImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    Base::onProcessPaintEvent(ev);

    PaintEvent rev( _window, ev.rect() );
    _window.processEvent(rev);
}


void MainWindowImpl::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


//void MainWindowImpl::show(bool visible)
//{
//    if(visible)
//    {
//        //std::clog  << "XMapWindow" << std::endl;
//        XMapWindow(_display, _window);
//        //XFlush(_display);
//
//        //while( XPending(_display) > 0 )
//        //{
//        //    XEvent xev;
//        //    XNextEvent(_display, &xev);
//
//        //    Application::instance().impl()->processEvent(xev);
//        //}
//    }
//    else
//    {
//        //std::clog  << "XUnmapWindow" << std::endl;
//        XUnmapWindow(_display, _window);
//        //XFlush(_display);
//    }
//}


void MainWindowImpl::onShow(Window& w, bool visible)
{
    if(visible)
    {
        //std::clog  << "XMapWindow" << std::endl;
        XMapWindow(_display, _window);
        //XFlush(_display);

        //while( XPending(_display) > 0 )
        //{
        //    XEvent xev;
        //    XNextEvent(_display, &xev);

        //    Application::instance().impl()->processEvent(xev);
        //}
    }
    else
    {
        //std::clog  << "XUnmapWindow" << std::endl;
        XUnmapWindow(_display, _window);
        //XFlush(_display);
    }
}


void MainWindowImpl::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);

    ShowEvent rev( _window, ev.visible() );
    _window.processEvent(rev);
}


void MainWindowImpl::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}


//void MainWindowImpl::activate()
//{
//    //XSetInputFocus(_display, _window, RevertToNone, CurrentTime);
//
//    XEvent xev;
//    memset(&xev, 0, sizeof(xev));
//    xev.type = ClientMessage;
//    xev.xclient.display      = _display;
//    xev.xclient.window       = _window;
//    xev.xclient.message_type = Application::instance().impl()->netWmActiveWindow();
//    xev.xclient.format       = 32;
//    xev.xclient.data.l[0]    = 2L; /* 2 == Message from a window pager */
//    xev.xclient.data.l[1]    = CurrentTime;
//
//    XWindowAttributes wattr;
//    XGetWindowAttributes(_display, _window, &wattr);
//
//    XSendEvent(_display, XDefaultRootWindow(_display),
//               False, SubstructureNotifyMask|SubstructureRedirectMask, &xev);
//
//    //XFlush(_display);
//}


void MainWindowImpl::onActivate(Window& w, bool active)
{
    if( ! active )
        return;

    //XSetInputFocus(_display, _window, RevertToNone, CurrentTime);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.display      = _display;
    xev.xclient.window       = _window;
    xev.xclient.message_type = Application::instance().impl()->netWmActiveWindow();
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 2L; /* 2 == Message from a window pager */
    xev.xclient.data.l[1]    = CurrentTime;

    XWindowAttributes wattr;
    XGetWindowAttributes(_display, _window, &wattr);

    XSendEvent(_display, XDefaultRootWindow(_display),
               False, SubstructureNotifyMask|SubstructureRedirectMask, &xev);

    //XFlush(_display);
}


void MainWindowImpl::onProcessActivateEvent(const ActivateEvent& ev)
{
    Base::onProcessActivateEvent(ev);

    ActivateEvent aev( _window, ev.isActive() );
    _window.processEvent(aev);
}


void MainWindowImpl::onActivateEvent(const ActivateEvent& ev)
{
    Base::onActivateEvent(ev);
}


//void MainWindowImpl::enable(bool enabled)
//{
//    //std::clog  << "XChangeWindowAttributes: " << enabled << std::endl;
//
//    XSetWindowAttributes wattr;
//
//    if(enabled)
//    {
//        wattr.event_mask = StructureNotifyMask|ExposureMask|
//                           PropertyChangeMask|EnterWindowMask|
//                           LeaveWindowMask|KeyPressMask|
//                           KeyReleaseMask|KeymapStateMask|
//                           ButtonPressMask|ButtonReleaseMask|
//                           PointerMotionMask|FocusChangeMask|
//                           SubstructureNotifyMask;
//    }
//    else
//    {
//        wattr.event_mask = 0;
//    }
//
//    XChangeWindowAttributes(_display, _window, CWEventMask, &wattr);
//    //XFlush(_display);
//}


void MainWindowImpl::onEnable(Window& w, bool enable)
{
    //std::clog  << "XChangeWindowAttributes: " << enabled << std::endl;

    XSetWindowAttributes wattr;

    if(enabled)
    {
        wattr.event_mask = StructureNotifyMask|ExposureMask|
                           PropertyChangeMask|EnterWindowMask|
                           LeaveWindowMask|KeyPressMask|
                           KeyReleaseMask|KeymapStateMask|
                           ButtonPressMask|ButtonReleaseMask|
                           PointerMotionMask|FocusChangeMask|
                           SubstructureNotifyMask;
    }
    else
    {
        wattr.event_mask = 0;
    }

    XChangeWindowAttributes(_display, _window, CWEventMask, &wattr);
    //XFlush(_display);
}


void MainWindowImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    EnableEvent eev( _window, ev.enabled() );
    _window.processEvent(eev);
}


void MainWindowImpl::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);
}


void MainWindowImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    Gfx::PointF aligedPos = w.surface().align(pos);

    Gfx::PointF p = w.surface().toPhysical(aligedPos);

    //std::clog  << "XMoveWindow: " << pos.x() << ", " << pos.y() << std::endl;
    XMoveWindow(_display, _window, p.x(), p.y());
    //XFlush(_display);

    //while( XPending(_display) > 0 )
    //{
    //    XEvent xev;
    //    XNextEvent(_display, &xev);

    //    Application::instance().impl()->processEvent(xev);
    //}
}


void MainWindowImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();
    RescaleEvent rev(_client, scaling);
    _client.processEvent(rev);
}


void MainWindowImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void MainWindowImpl::onResize(Window& w, const Gfx::SizeF& s)
{
    //std::clog  << "XResizeWindow: " << s.width() << "x" << s.height() << std::endl;

    Gfx::SizeF alignedSize = surface().align(s);

    //
    // maximum width and height
    //
    if( alignedSize.width() > w.maximumSize().width() )
        alignedSize.setWidth( w.maximumSize().width() );

    if( alignedSize.height() > w.maximumSize().height() )
        alignedSize.setHeight( w.maximumSize().height() );

    if( alignedSize.width() < w.minimumSize().width() )
        alignedSize.setWidth( w.minimumSize().width() );

    if( alignedSize.height() < w.minimumSize().height() )
        alignedSize.setHeight( w.minimumSize().height() );

    Gfx::SizeF psize = w.surface().toPhysical(alignedSize);
    Gfx::Size size = round(psize);

    XResizeWindow( _display, _window, size.width(), size.height() );

    //XFlush(_display);

    //XClearArea(_display, _window, 0, 0, size.width(), size.height(), True);
    //XFlush(_display);

    //while( XPending(_display) > 0 )
    //{
    //    XEvent xev;
    //    XNextEvent(_display, &xev);

    //    Application::instance().impl()->processEvent(xev);
    //}
}


void MainWindowImpl::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);

    ResizeEvent rev( _client, ev.size() );
    _client.processEvent(rev);
}


void MainWindowImpl::onClose(Window& w)
{
    MainWindowImpl* impl = static_cast<MainWindowImpl*>( w.impl() );
    if(impl)
    {
        XEvent ev;
        memset(&ev, 0, sizeof (ev));

        ev.xclient.type         = ClientMessage;
        ev.xclient.window       = impl->window();
        ev.xclient.message_type = Application::instance().impl()->wmProtocols();
        ev.xclient.format       = 32;
        ev.xclient.data.l[0]    = Application::instance().impl()->wmDeleteWindow();
        ev.xclient.data.l[1]    = CurrentTime;
        XSendEvent(_display, _window, False, NoEventMask, &ev);
    }
}


void MainWindowImpl::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);

    CloseEvent cev(_client);
    _client.processEvent(cev);
}


void MainWindowImpl::onCloseEvent(const CloseEvent& ev)
{
}


void MainWindowImpl::onSetAbove(Window& w, bool above)
{
    //std::clog << "setTopMost: " << topMost << std::endl;

    enum
    {
    _NET_WM_STATE_REMOVE = 0,
    _NET_WM_STATE_ADD    = 1,
    _NET_WM_STATE_TOGGLE = 2
    };

    XEvent ev;
    ev.xclient.type = ClientMessage;
    ev.xclient.serial = 0; // not important
    ev.xclient.send_event = True;
    ev.xclient.display = _display;
    ev.xclient.window = _window; // modified window
    ev.xclient.message_type = Application::instance().impl()->netWmState();
    ev.xclient.format = 32; // use data.l
    ev.xclient.data.l[0] = above ? _NET_WM_STATE_ADD
                                 : _NET_WM_STATE_REMOVE;
    ev.xclient.data.l[1] = Application::instance().impl()->netWmStateAbove();
    ev.xclient.data.l[2] = 0;
    ev.xclient.data.l[3] = 0;
    ev.xclient.data.l[4] = 0;

    XSendEvent(_display, XDefaultRootWindow(_display), False,
               SubstructureRedirectMask, &ev);
}


//void MainWindowImpl::setState(const WindowState& s)
//{
//    //std::clog  << "setState: " << s << std::endl;
//
//    XClientMessageEvent ev;
//    ev.type       = ClientMessage;
//    ev.serial     = 0;
//    ev.window     = _window;
//    ev.format     = 32;
//    ev.send_event = True;
//
//    switch(s)
//    {
//        default:
//        case WindowState::Normal:
//            ev.message_type = Application::instance().impl()->wmChangeState();
//            ev.data.l[0] = NormalState;
//            break;
//
//        case WindowState::Minimized:
//            ev.message_type = Application::instance().impl()->wmChangeState();
//            ev.data.l[0] = IconicState;
//            break;
//
//        case WindowState::Maximized:
//            ev.message_type = Application::instance().impl()->netWmState();
//            ev.data.l[0]  = 1ul;
//            ev.data.l[1]  = Application::instance().impl()->netWmStateMaximizedVert();
//            ev.data.l[2]  = Application::instance().impl()->netWmStateMaximizedHorz();
//            break;
//    }
//
//    XSendEvent(_display, XDefaultRootWindow(_display),False,
//               SubstructureRedirectMask,
//               (XEvent*)&ev);
//
//    //XFlush(_display);
//}


void MainWindowImpl::onSetState(Window& w, const WindowState& s)
{
    //std::clog  << "setState: " << s << std::endl;

    XClientMessageEvent ev;
    ev.type       = ClientMessage;
    ev.serial     = 0;
    ev.window     = _window;
    ev.format     = 32;
    ev.send_event = True;

    switch(s)
    {
        default:
        case WindowState::Normal:
            ev.message_type = Application::instance().impl()->wmChangeState();
            ev.data.l[0] = NormalState;
            break;

        case WindowState::Minimized:
            ev.message_type = Application::instance().impl()->wmChangeState();
            ev.data.l[0] = IconicState;
            break;

        case WindowState::Maximized:
            ev.message_type = Application::instance().impl()->netWmState();
            ev.data.l[0]  = 1ul;
            ev.data.l[1]  = Application::instance().impl()->netWmStateMaximizedVert();
            ev.data.l[2]  = Application::instance().impl()->netWmStateMaximizedHorz();
            break;
    }

    XSendEvent(_display, XDefaultRootWindow(_display), False,
               SubstructureRedirectMask, (XEvent*)&ev);

    //XFlush(_display);
}


void MainWindowImpl::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    Base::onProcessWindowStateEvent(ev);

    WindowStateEvent wse( _client, ev.state() );
    Application::instance().processEvent(wse);
}


void MainWindowImpl::onWindowStateEvent(const WindowStateEvent& ev)
{
}


void MainWindowImpl::onSetTitle(Window& w, const std::string& text)
{
    XStoreName(_display, _window, text.c_str());
}


void MainWindowImpl::onSetIcon(Window& w, const Gfx::Image& icon)
{
    //std::clog << "XAllocWMHints" << std::endl;

    XWMHints* hints = XAllocWMHints();
    if ( ! hints )
        return;

    hints->flags = IconPixmapHint | IconPositionHint;
    hints->icon_pixmap = None;
    hints->icon_x = 0;
    hints->icon_y = 0;

    XSetWMHints(_display, _window, hints);
    XFree(hints);
}


void MainWindowImpl::setSizeLimits(Window& w, const Gfx::SizeF& minSize,
                                              const Gfx::SizeF& maxSize)
{
    XSizeHints hints;
    memset(&hints, 0, sizeof(hints));

    hints.flags = PMinSize | PMaxSize;
    hints.min_width  = lround( minSize.width() );
    hints.min_height = lround( minSize.height() );
    hints.max_width  = lround( maxSize.width() );
    hints.max_height = lround( maxSize.height() );

    XSetWMNormalHints(_display, _window, &hints);
}


bool MainWindowImpl::isMinimized()
{
    Atom actual_type;
    int actual_format;
    unsigned long num_items, bytes_after;
    Atom* atoms = 0;
    Atom requestAtom = Application::instance().impl()->netWmState();
    Atom compareAtom = Application::instance().impl()->netWmStateHidden();

    XGetWindowProperty(_display, _window,
                       requestAtom, 0, 1024, False, XA_ATOM,
                       &actual_type, &actual_format,
                       &num_items, &bytes_after,
                       (unsigned char**)&atoms);

    for(unsigned long i = 0; i < num_items; ++i)
    {
        if(atoms[i] == compareAtom)
        {
            XFree(atoms);
            return true;
        }
    }

    XFree(atoms);
    return false;
}


bool MainWindowImpl::isMaximized()
{
    Atom actual_type;
    int actual_format;
    unsigned long num_items, bytes_after;
    Atom* atoms = 0;
    Atom requestAtom = Application::instance().impl()->netWmState();
    Atom compareAtom = Application::instance().impl()->netWmStateMaximizedHorz();

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

} // namespace

} // namespace
