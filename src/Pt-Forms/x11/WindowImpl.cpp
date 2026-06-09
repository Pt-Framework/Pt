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

#include "WindowImpl.h"
#include "ApplicationImpl.h"
#include "ScreenImpl.h"
#include "../generic/GenericGraphicsBackend.h"

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/WindowStateEvent.h>

#include <cassert>

namespace Pt {

namespace Forms {

WindowImpl::WindowImpl(ScreenImpl& wm, Window& w, GraphicsBackend& graphicsBackend)
: WindowFrame(wm, w)
, _genericBackend(0)
, _commitFrame(&WindowImpl::commitFrameNone)
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

  bindBackend(graphicsBackend);
}


WindowImpl::~WindowImpl()
{
    destroy();
}


void WindowImpl::create(WindowType type)
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

    Base::onSetParent(&_wm);
}


void WindowImpl::destroy()
{
    if( _window == 0)
        return;

    //std::clog << std::hex << "XDestroyWindow: " << _window << std::dec << std::endl;

    XDestroyWindow(_display, _window);
    _window = 0;
}


void WindowImpl::bindBackend(GraphicsBackend& graphicsBackend)
{
    _genericBackend = dynamic_cast<GenericGraphicsBackend*>(&graphicsBackend);
    if( _genericBackend )
    {
        _commitFrame = &WindowImpl::commitFrameGeneric;
        return;
    }
}


void WindowImpl::commitFrame(int x, int y, int w, int h)
{
    (this->*_commitFrame)(x, y, w, h);
}


void WindowImpl::commitFrameNone(int /*x*/, int /*y*/, int /*w*/, int /*h*/)
{
}


void WindowImpl::commitFrameGeneric(int x, int y, int w, int h)
{
    const Gfx::Image& image = _genericBackend->image( pixmap() );
    if( ! image.data() )
        return;

    char* data = reinterpret_cast<char*>( const_cast<Pt::uint8_t*>(image.data()) );

    ApplicationImpl* app = Application::instance().impl();
    XImage* ximage = XCreateImage(app->display(), app->visual(), app->depth(),
                                  ZPixmap, 0,
                                  data, image.width(), image.height(),
                                  app->depth() == 24 ? 32 : app->depth(), 0);

    unsigned int screen = DefaultScreen(app->display());
    GC gc = DefaultGC(app->display(), screen);
    XPutImage(app->display(), _window, gc, ximage, x, y, x, y, w, h);

    ximage->data = NULL;
    XDestroyImage(ximage);
}


void WindowImpl::onInit(Window& w)
{
    Pixmap& surface = pixmap();
    Gfx::PointF surfacePos(0, 0);
    w.setSurface(&surface, surfacePos);

    w.setNextResponder(this);

    if( screen() )
    {
        double scaling = scaleFactor();
    
        RescaleEvent ev(w, scaling);
        w.processEvent(ev);
    }
}


void WindowImpl::onRelease(Window& w)
{
    w.setNextResponder(0);
    w.setSurface( 0, Gfx::PointF() );
}


void WindowImpl::onConnect(Screen& screen)
{
    Base::onConnect(screen);
}


void WindowImpl::onDisconnect()
{
    Base::onDisconnect();
}


Gfx::PointF WindowImpl::toScreen(const Gfx::PointF& pos) const
{
    Gfx::PointF physicalPos = scaling().toPhysical(pos);

    ::Window root = DefaultRootWindow(_display);
    int windowX = lround( physicalPos.x() );
    int windowY = lround( physicalPos.y() );
    int screenX = 0;
    int screenY = 0;
    ::Window child = 0;

    XTranslateCoordinates(_display, _window, root,
                          windowX, windowY, &screenX, &screenY, &child);

    Gfx::PointF screenPos(screenX, screenY);
    Gfx::PointF logicalPos = scaling().toLogical(screenPos);
    return logicalPos;
}


Gfx::PointF WindowImpl::fromScreen(const Gfx::PointF& pos) const
{
    Gfx::PointF physicalPos = scaling().toPhysical(pos);

    ::Window root = DefaultRootWindow(_display);
    int screenX = lround( physicalPos.x() );
    int screenY = lround( physicalPos.y() );
    int windowX = 0;
    int windowY = 0;
    ::Window child = 0;

    XTranslateCoordinates(_display, root, _window,
                          screenX, screenY, &windowX, &windowY, &child);

    Gfx::PointF clientPos(windowX, windowY);
    Gfx::PointF logicalPos = scaling().toLogical(clientPos);
    return logicalPos;
}


Gfx::PointF WindowImpl::onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF WindowImpl::onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF WindowImpl::onToParent(const Gfx::PointF& pos) const
{
    return _wm.fromFrame(*this, pos); 
}
     
        
Gfx::PointF WindowImpl::onFromParent(const Gfx::PointF& pos) const
{ 
    return _wm.toFrame(*this, pos); 
}


void WindowImpl::paint(const Gfx::RectF& rectF)
{
    int x = Pt::lround(rectF.left());
    int y = Pt::lround(rectF.top());

    unsigned int width = Pt::lround(rectF.right()) - x;
    unsigned int height = Pt::lround(rectF.bottom()) - y;
    
    if( width == 0 || height == 0 )
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
               x, y, width, height,
               True);

    //XFlush(_display);
}


void WindowImpl::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF screenPos = toScreen( rect.topLeft() );
    Gfx::RectF screenRect( screenPos, rect.size() );

    _wm.repaint(screenRect);
}


void WindowImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    Base::onProcessPaintEvent(ev);

    PaintEvent rev( _client, ev.rect() );
    _client.processEvent(rev);
}


void WindowImpl::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


//void WindowImpl::show(bool visible)
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


void WindowImpl::onShow(Window& w, bool visible)
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


void WindowImpl::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);

    ShowEvent rev( _client, ev.visible() );
    _client.processEvent(rev);
}


void WindowImpl::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}


//void WindowImpl::activate()
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


void WindowImpl::onActivate(Window& w, bool active)
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


void WindowImpl::onProcessActivateEvent(const ActivateEvent& ev)
{
    Base::onProcessActivateEvent(ev);

    ActivateEvent aev( _client, ev.isActive() );
    _client.processEvent(aev);
}


void WindowImpl::onActivateEvent(const ActivateEvent& ev)
{
    Base::onActivateEvent(ev);
}


//void WindowImpl::enable(bool enabled)
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


void WindowImpl::onEnable(Window& w, bool enable)
{
    //std::clog  << "XChangeWindowAttributes: " << enabled << std::endl;

    XSetWindowAttributes wattr;

    if(enable)
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


void WindowImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    EnableEvent eev( _client, ev.enabled() );
    _client.processEvent(eev);
}


void WindowImpl::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);
}


void WindowImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    Gfx::PointF aligedPos = w.scaling().align(pos);

    Gfx::PointF p = w.scaling().toPhysical(aligedPos);

    //std::clog  << "XMoveWindow: " << pos.x() << ", " << pos.y() << std::endl;
    XMoveWindow(_display, _window, p.x(), p.y());
    XFlush(_display);

    //while( XPending(_display) > 0 )
    //{
    //    XEvent xev;
    //    XNextEvent(_display, &xev);

    //    Application::instance().impl()->processEvent(xev);
    //}
}


void WindowImpl::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);

    MoveEvent mev( _client, ev.position() );
    _client.processEvent(mev);
}


void WindowImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();
    RescaleEvent rev(_client, scaling);
    _client.processEvent(rev);
}


void WindowImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


Gfx::SizeF WindowImpl::onResize(Window& w, const Gfx::SizeF& s)
{
    //std::clog  << "XResizeWindow: " << s.width() << "x" << s.height() << std::endl;

    Gfx::SizeF alignedSize = scaling().align(s);

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

    Gfx::SizeF psize = w.scaling().toPhysical(alignedSize);

    unsigned int width = Pt::lround(psize.width());
    unsigned int height = Pt::lround(psize.height());

    //XFlush(_display);
    //XSync(_display, False);

    //while( XPending(_display) > 0 )
    //{
    //    XEvent xev;
    //    XNextEvent(_display, &xev);

    //    std::clog  << "XResizeWindow: event pending: " << xev.xany.type << std::endl;
    //    Application::instance().impl()->processEvent(xev);
    //}

    XResizeWindow( _display, _window, width, height );
    XFlush(_display);

    //::Window root_return;
    //int x_return, y_return;
    //unsigned int width_return, height_return;
    //unsigned int border_width_return;
    //unsigned int depth_return;

    //XGetGeometry(_display, _window, &root_return, &x_return, &y_return, &width_return, 
    //                                &height_return, &border_width_return, &depth_return);

    //std::clog  << "XResizeWindow: DONE1: " << width_return << " " 
    //                                      << height_return << std::endl;

    //XFlush(_display);
    //XSync(_display, False);
    //



    //XGetGeometry(_display, _window, &root_return, &x_return, &y_return, &width_return, 
    //                                &height_return, &border_width_return, &depth_return);

    //XFlush(_display);

    //XClearArea(_display, _window, 0, 0, size.width(), size.height(), True);
    //XFlush(_display);

    //while( XPending(_display) > 0 )
    //{
    //    XEvent xev;
    //    XNextEvent(_display, &xev);

    //    std::clog  << "XResizeWindow: event pending: " << xev.xany.type << std::endl;
    //    Application::instance().impl()->processEvent(xev);
    //}

    //std::clog  << "XResizeWindow: DONE2: " << width_return << " " 
    //                                      << height_return << std::endl;

    //XSync(_display, False);
    //XWindowAttributes attrs;
    //XGetWindowAttributes(_display, _window, &attrs);
    //
    //std::clog  << "XGetWindowAttributes: " << attrs.width << " " 
    //                                      << attrs.height << std::endl;

    /* NOTE: to check if the size was actually changed and not rejected:
        
        Sync(_display, False);
        XWindowAttributes attrs;
        XGetWindowAttributes(_display, _window, &attrs);
        bool hasChanged = attrs.width == width && attrs.height == height;
    */

    return alignedSize;
}


void WindowImpl::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);

    ResizeEvent rev( _client, ev.size() );
    _client.processEvent(rev);
}


void WindowImpl::onClose(Window& w)
{
    WindowImpl* frame = static_cast<WindowImpl*>( w.frame() );
    if(frame)
    {
        XEvent ev;
        memset(&ev, 0, sizeof (ev));

        ev.xclient.type         = ClientMessage;
        ev.xclient.window       = frame->window();
        ev.xclient.message_type = Application::instance().impl()->wmProtocols();
        ev.xclient.format       = 32;
        ev.xclient.data.l[0]    = Application::instance().impl()->wmDeleteWindow();
        ev.xclient.data.l[1]    = CurrentTime;
        XSendEvent(_display, _window, False, NoEventMask, &ev);
    }
}


void WindowImpl::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);

    CloseEvent cev(_client);
    _client.processEvent(cev);
}


void WindowImpl::onCloseEvent(const CloseEvent& ev)
{
}


void WindowImpl::onSetAbove(Window& w, bool above)
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


//void WindowImpl::setState(const WindowState& s)
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


void WindowImpl::onSetState(Window& w, const WindowState& s)
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


void WindowImpl::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    Base::onProcessWindowStateEvent(ev);

    WindowStateEvent wse( _client, ev.state() );
    Application::instance().processEvent(wse);
}


void WindowImpl::onWindowStateEvent(const WindowStateEvent& ev)
{
}


void WindowImpl::onSetTitle(Window& w, const std::string& text)
{
    XStoreName(_display, _window, text.c_str());
}


void WindowImpl::onSetIcon(Window& w, const Gfx::Image& icon)
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


void WindowImpl::onSetSizeLimits(Window& w, const Gfx::SizeF& minSize,
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


void WindowImpl::onAutoCenter(Window& w, const Gfx::SizeF* size) 
{
    _wm.onAutoCenter(*this, size);
}


bool WindowImpl::isMinimized()
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


bool WindowImpl::isMaximized()
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
