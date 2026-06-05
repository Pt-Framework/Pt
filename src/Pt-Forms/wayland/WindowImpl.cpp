/* Copyright (C) 2015-2026 Marc Boris Duerner 
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include "WindowImpl.h"
#include "ApplicationImpl.h"
#include "ScreenImpl.h"
#include "PixmapImpl.h"

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/WindowStateEvent.h>
#include <Pt/Forms/ResizeEvent.h>
#include <Pt/Forms/LayoutEvent.h>
#include <Pt/Forms/ShowEvent.h>

#include <cstring>

namespace Pt {

namespace Forms {

namespace {

void xdgSurfaceConfigure(void* data, struct xdg_surface* xdgSurface, uint32_t serial)
{
    static_cast<WindowImpl*>(data)->onXdgSurfaceConfigure(xdgSurface, serial);
}

void toplevelConfigure(void* data, struct xdg_toplevel* /*toplevel*/,
                       int32_t width, int32_t height, struct wl_array* states)
{
    static_cast<WindowImpl*>(data)->onXdgToplevelConfigure(width, height, states);
}

void toplevelClose(void* data, struct xdg_toplevel* /*toplevel*/)
{
    static_cast<WindowImpl*>(data)->onXdgToplevelClose();
}

void popupConfigure(void* data, struct xdg_popup* /*popup*/,
                    int32_t x, int32_t y, int32_t width, int32_t height)
{
    static_cast<WindowImpl*>(data)->onXdgPopupConfigure(x, y, width, height);
}

void popupDone(void* data, struct xdg_popup* /*popup*/)
{
    static_cast<WindowImpl*>(data)->onXdgPopupDone();
}

void decorationConfigure(void* /*data*/, struct zxdg_toplevel_decoration_v1* /*deco*/,
                         uint32_t /*mode*/)
{
    // Accept whatever decoration mode the compositor chooses.
    // On WSLg this confirms server-side decorations (Windows title bar).
}


static const struct xdg_surface_listener xdgSurfaceListener = {
    xdgSurfaceConfigure
};

static const struct xdg_toplevel_listener xdgToplevelListener = {
    toplevelConfigure,
    toplevelClose
};

static const struct xdg_popup_listener xdgPopupListener = {
    popupConfigure,
    popupDone
};

static const struct zxdg_toplevel_decoration_v1_listener decorationListener = {
    decorationConfigure
};

void surfaceFrameCallback(void* data, struct wl_callback* cb, uint32_t time)
{
    static_cast<WindowImpl*>(data)->onFrameCallback(cb, time);
}

static const struct wl_callback_listener frameListener = {
    surfaceFrameCallback
};

} // namespace

WindowImpl::WindowImpl(ScreenImpl& wm, Window& w)
: WindowFrame(wm, w)
, _wm(wm)
, _window(w)
, _surface(0)
, _xdgSurface(0)
, _xdgToplevel(0)
, _xdgPopup(0)
, _positioner(0)
, _decoration(0)
, _frameCallback(0)
, _configured(false)
, _framePending(false)
, _needsRepaint(false)
, _commitPending(false)
{
    ApplicationImpl* app = Application::instance().impl();

    _surface = wl_compositor_create_surface(app->compositor());
    wl_surface_set_user_data(_surface, this);

    _shmPool.init(app->shm());
    _shmPool.released() += Pt::slot(*this, &WindowImpl::onBufferReleased);

    if( w.type() == WindowType::Default )
        createWindow();

    // Popup creation deferred to onShow() when position and size are known

    Base::onSetParent(&wm);
}


WindowImpl::~WindowImpl()
{
    destroySurface();
}


void WindowImpl::createWindow()
{
    ApplicationImpl* app = Application::instance().impl();

    _xdgSurface = xdg_wm_base_get_xdg_surface(app->xdgWmBase(), _surface);
    xdg_surface_add_listener(_xdgSurface, &xdgSurfaceListener, this);

    _xdgToplevel = xdg_surface_get_toplevel(_xdgSurface);
    xdg_toplevel_add_listener(_xdgToplevel, &xdgToplevelListener, this);

    // Set a default app_id so compositors like WSLg's weston_rdprail_shell
    // can register the window as a proper desktop window with decorations.
    xdg_toplevel_set_app_id(_xdgToplevel, "Pt.Forms");

    // Request server-side decorations
    if( app->decorationManager() )
    {
        _decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(
            app->decorationManager(), _xdgToplevel);
        zxdg_toplevel_decoration_v1_add_listener(_decoration, &decorationListener, this);
        zxdg_toplevel_decoration_v1_set_mode(
            _decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
    }

    wl_surface_commit(_surface);
}


void WindowImpl::createPopup(Window& w)
{
    ApplicationImpl* app = Application::instance().impl();

    _surface = wl_compositor_create_surface(app->compositor());
    wl_surface_set_user_data(_surface, this);

    _xdgSurface = xdg_wm_base_get_xdg_surface(app->xdgWmBase(), _surface);
    xdg_surface_add_listener(_xdgSurface, &xdgSurfaceListener, this);

    // Find parent surface
    struct xdg_surface* parentXdgSurface = 0;
    const std::vector<Window*>& windows = _wm.windows();
    for(std::size_t i = 0; i < windows.size(); ++i)
    {
        if( windows[i] != &w && windows[i]->frame() )
        {
            WindowImpl* parentImpl = static_cast<WindowImpl*>( windows[i]->frame() );
            if( parentImpl->_xdgSurface )
            {
                parentXdgSurface = parentImpl->_xdgSurface;
                break;
            }
        }
    }

    // Create positioner
    _positioner = xdg_wm_base_create_positioner(app->xdgWmBase());
    xdg_positioner_set_size(_positioner,
                            static_cast<int32_t>( size().width() ),
                            static_cast<int32_t>( size().height() ));

    // Place the anchor rect at the desired position on the parent surface.
    // The popup's top-left corner is anchored to the top-left of this rect.
    int32_t anchorX = static_cast<int32_t>( position().x() );
    int32_t anchorY = static_cast<int32_t>( position().y() );
    xdg_positioner_set_anchor_rect(_positioner, anchorX, anchorY, 1, 1);
    xdg_positioner_set_anchor(_positioner, XDG_POSITIONER_ANCHOR_TOP_LEFT);
    xdg_positioner_set_gravity(_positioner, XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT);
    xdg_positioner_set_constraint_adjustment(_positioner,
                                             XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X |
                                             XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y |
                                             XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y);

    _xdgPopup = xdg_surface_get_popup(_xdgSurface, parentXdgSurface, _positioner);
    xdg_popup_add_listener(_xdgPopup, &xdgPopupListener, this);

    wl_surface_commit(_surface);
}


void WindowImpl::destroyPopup()
{
    if( _xdgPopup )
    {
        xdg_popup_destroy(_xdgPopup);
        _xdgPopup = 0;
    }

    if( _positioner )
    {
        xdg_positioner_destroy(_positioner);
        _positioner = 0;
    }

    if( _xdgSurface )
    {
        xdg_surface_destroy(_xdgSurface);
        _xdgSurface = 0;
    }

    // Destroy the wl_surface so the compositor removes it from display.
    // A new surface will be created when the popup is shown again.
    if( _surface )
    {
        wl_surface_destroy(_surface);
        _surface = 0;
    }

    _configured = false;
}


void WindowImpl::destroySurface()
{
    if( _decoration )
    {
        zxdg_toplevel_decoration_v1_destroy(_decoration);
        _decoration = 0;
    }

    if( _xdgPopup )
    {
        xdg_popup_destroy(_xdgPopup);
        _xdgPopup = 0;
    }

    if( _positioner )
    {
        xdg_positioner_destroy(_positioner);
        _positioner = 0;
    }

    if( _xdgToplevel )
    {
        xdg_toplevel_destroy(_xdgToplevel);
        _xdgToplevel = 0;
    }

    if( _xdgSurface )
    {
        xdg_surface_destroy(_xdgSurface);
        _xdgSurface = 0;
    }

    if( _surface )
    {
        if( _frameCallback )
        {
            wl_callback_destroy(_frameCallback);
            _frameCallback = 0;
            _framePending = false;
        }

        wl_surface_destroy(_surface);
        _surface = 0;
        _configured = false;
    }
}

///////////////////////////////////////////////////////////////////////
// xdg callbacks
///////////////////////////////////////////////////////////////////////

void WindowImpl::onXdgSurfaceConfigure(struct xdg_surface* xdgSurface,
                                           uint32_t serial)
{
    xdg_surface_ack_configure(xdgSurface, serial);

    _configured = true;

    // Apply compositor-suggested size if it changed
    if( ! _pendingSize.isEmpty() && 
        ! _pendingSize.isEqual( size() ) )
    {
        // Cancel pending frame callback — the old frame is for the old size.
        // This allows the repaint below to commit immediately.
        if( _frameCallback )
        {
            wl_callback_destroy(_frameCallback);
            _frameCallback = 0;
        }
        _framePending = false;
        _commitPending = false;
        _commitDamage = Gfx::RectF();

        ResizeEvent rev(*this, _pendingSize);
        Application::instance().processEvent(rev);
    }

    if( _window.isVisible() )
    {
        _window.repaint( Gfx::RectF(Gfx::PointF(0, 0), size()) );
    }
}


void WindowImpl::onXdgToplevelConfigure(int32_t width, int32_t height,
                                         struct wl_array* /*states*/)
{
    // width/height of 0 means the compositor doesn't care — use our own size
    if( width > 0 && height > 0 )
        _pendingSize.set(width, height);
    else
        _pendingSize = size();
}


void WindowImpl::onXdgToplevelClose()
{
    // The CloseEvent must target the client Window, not the frame.
    // Window::onProcessCloseEvent is only registered on Window's dispatcher;
    // WindowFrame::onCloseEvent is a no-op and does not forward to _window.
    // Use commitEvent (async) so the event is processed in the normal queue-
    // drain phase of waitNext(), which correctly terminates the loop when
    // Application::exit() is called from the close handler.
    CloseEvent ev(_window);
    Application::instance().commitEvent(ev);
}


void WindowImpl::onXdgPopupConfigure(int32_t /*x*/, int32_t /*y*/,
                                      int32_t width, int32_t height)
{
    if( width > 0 && height > 0 )
        _pendingSize.set(width, height);
}


void WindowImpl::onXdgPopupDone()
{
    // The compositor dismissed the popup (click outside the grabbed surface).
    // Close the client Window (Popup) so it goes through the normal
    // Popup::onCloseEvent path which updates the popup list and visibility.
    CloseEvent ev(_window);
    Application::instance().processEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// window frame
///////////////////////////////////////////////////////////////////////

void WindowImpl::onInit(Window& w)
{
    Pixmap& surface = pixmap();
    Gfx::PointF surfacePos(0, 0);
    w.setSurface(&surface, surfacePos);

    w.setNextResponder(this);

    if( screen() )
    {
        // Propagate total scale to the client window so widget tree is
        // initialized with the correct logical→physical mapping.
        double scaling = scaleFactor();
        RescaleEvent ev(w, scaling);
        w.processEvent(ev);
    }

}


void WindowImpl::onRelease(Window& w)
{
    w.setNextResponder(0);
    w.setSurface(0, Gfx::PointF());
    destroySurface();
}


void WindowImpl::onConnect(Screen& screen)
{
    Base::onConnect(screen);
}


void WindowImpl::onDisconnect()
{
}


Gfx::PointF WindowImpl::toScreen(const Gfx::PointF& pos) const
{
    // Translate surface-local coordinates to screen coordinates.
    // position() returns the realized window offset: (0,0) for toplevels
    // and the popup anchor offset for popup windows.
    return pos + position();
}


Gfx::PointF WindowImpl::fromScreen(const Gfx::PointF& pos) const
{
    return pos - position();
}


Gfx::PointF WindowImpl::onToWindow(const Window& /*w*/,
                                    const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF WindowImpl::onFromWindow(const Window& /*w*/,
                                      const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF WindowImpl::onToParent(const Gfx::PointF& pos) const
{
    return pos + position();
}


Gfx::PointF WindowImpl::onFromParent(const Gfx::PointF& pos) const
{
    return pos - position();
}


void WindowImpl::onSetTitle(Window& /*w*/, const std::string& text)
{
    if( _xdgToplevel )
    {
        xdg_toplevel_set_title(_xdgToplevel, text.c_str());
        // Keep app_id in sync with the title so the Windows taskbar
        // (via WSLg weston_rdprail_shell) uses the correct application name.
        if( ! text.empty() )
            xdg_toplevel_set_app_id(_xdgToplevel, text.c_str());
    }
}


void WindowImpl::onSetIcon(Window& /*w*/, const Gfx::Image& /*icon*/)
{
    // Wayland has no standard protocol for window icons yet.
}


void WindowImpl::onSetAbove(Window& /*w*/, bool /*above*/)
{
    // Wayland does not support setting z-order from the client side.
}


void WindowImpl::onSetSizeLimits(Window& /*w*/, const Gfx::SizeF& minSize,
                                 const Gfx::SizeF& maxSize)
{
    if( ! _xdgToplevel )
        return;

    xdg_toplevel_set_min_size(_xdgToplevel,
                              static_cast<int32_t>(minSize.width()),
                              static_cast<int32_t>(minSize.height()));
    xdg_toplevel_set_max_size(_xdgToplevel,
                              static_cast<int32_t>(maxSize.width()),
                              static_cast<int32_t>(maxSize.height()));
}


void WindowImpl::onAutoCenter(Window& /*w*/, const Gfx::SizeF* /*size*/)
{
    // On Wayland the compositor decides window placement.
}

///////////////////////////////////////////////////////////////////////
// window state
///////////////////////////////////////////////////////////////////////

void WindowImpl::onSetState(Window& /*w*/, const WindowState& state)
{
    if( ! _xdgToplevel )
        return;

    if( state == WindowState::Maximized )
        xdg_toplevel_set_maximized(_xdgToplevel);
    else
        xdg_toplevel_unset_maximized(_xdgToplevel);

    if( state == WindowState::Minimized )
        xdg_toplevel_set_minimized(_xdgToplevel);
}


void WindowImpl::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    Base::onProcessWindowStateEvent(ev);
}


void WindowImpl::onWindowStateEvent(const WindowStateEvent& ev)
{
    Base::onWindowStateEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// closing
///////////////////////////////////////////////////////////////////////

void WindowImpl::onClose(Window& /*w*/)
{
    destroySurface();
    CloseEvent ev(_window);
    Application::instance().commitEvent(ev);
}


void WindowImpl::onProcessCloseEvent(const CloseEvent& ev)
{
    Base::onProcessCloseEvent(ev);
}


void WindowImpl::onCloseEvent(const CloseEvent& ev)
{
    Base::onCloseEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// activation
///////////////////////////////////////////////////////////////////////

void WindowImpl::onActivate(Window& /*w*/, bool /*active*/)
{
    // Activation is compositor-managed on Wayland.
}


void WindowImpl::onProcessActivateEvent(const ActivateEvent& ev)
{
    Base::onProcessActivateEvent(ev);
}


void WindowImpl::onActivateEvent(const ActivateEvent& ev)
{
    Base::onActivateEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// enabling
///////////////////////////////////////////////////////////////////////

void WindowImpl::onEnable(Window& /*w*/, bool /*enable*/)
{
}


void WindowImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);
}


void WindowImpl::onEnableEvent(const EnableEvent& ev)
{
    Base::onEnableEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// scaling
///////////////////////////////////////////////////////////////////////

void WindowImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);

    // Propagate the total scale (system × user) to the client Window.
    // All widgets work in logical units; the total scale is what drives
    // canvas rendering and physical buffer sizing throughout the widget tree.
    RescaleEvent rev(_window, ev.scaleFactor());
    _window.processEvent(rev);
}


void WindowImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// visibility
///////////////////////////////////////////////////////////////////////

void WindowImpl::onShow(Window& w, bool visible)
{
    ShowEvent sev(w, visible);
    w.processEvent(sev);

    bool isWindow = w.type() == WindowType::Default;

    if( isWindow )
        onShowWindow(w, visible);
    else
        onShowPopup(w, visible);
}


void WindowImpl::onShowWindow(Window& w, bool visible)
{
    if( visible )
    {
        if( _configured )
        {
            w.repaint( Gfx::RectF(Gfx::PointF(0, 0), size()) );
        }
        else
        {
            wl_surface_commit(_surface);
        }
    }
    else
    {
        if( _frameCallback )
        {
            wl_callback_destroy(_frameCallback);
            _frameCallback = 0;
            _framePending = false;
        }

        wl_surface_attach(_surface, 0, 0, 0);
        wl_surface_commit(_surface);
    }
}


void WindowImpl::onShowPopup(Window& w, bool visible)
{
    if( visible )
    {
        destroyPopup();
        createPopup(w);
    }
    else
    {
        destroyPopup();
    }
}


void WindowImpl::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);
}


void WindowImpl::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// painting
///////////////////////////////////////////////////////////////////////

void WindowImpl::paint(const Gfx::RectF& rect)
{
    if( _framePending )
    {
        _needsRepaint = true;
        return;
    }

    PaintEvent ev(*this, rect);
    processEvent(ev);
}


void WindowImpl::onRepaint(Window& /*w*/, const Gfx::RectF& rect)
{
    if( ! _configured || ! _surface )
        return;

    Gfx::PointF screenPos = toScreen( rect.topLeft() );
    Gfx::RectF screenRect( screenPos, rect.size() );
    _wm.repaint(screenRect);
}


void WindowImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    // Cannot paint until the compositor has configured the xdg_surface.
    // The configure callback will trigger a full repaint.
    if( ! _configured )
        return;

    PaintEvent rev( _window, ev.rect() );
    _window.processEvent(rev);

    Base::onProcessPaintEvent(ev);
}


void WindowImpl::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
    onPaintContent( ev.rect() );
}


void WindowImpl::commitFrame()
{
    if( ! _commitPending )
        return;

    _commitPending = false;

    const Gfx::Image& image = pixmap().impl()->bitmap().image();
    if( ! image.data() )
        return;

    int imgWidth = static_cast<int>( image.width() );
    int imgHeight = static_cast<int>( image.height() );

    if( imgWidth <= 0 || imgHeight <= 0 )
        return;

    _shmPool.resize(imgWidth, imgHeight);

    ShmBuffer* buf = _shmPool.acquireBuffer();
    if( ! buf )
    {
        _shmPool.setRepaintOnRelease(true);
        return;
    }

    if( buf->width() != imgWidth || buf->height() != imgHeight )
    {
        buf->create(_shmPool.shm(), imgWidth, imgHeight, imgWidth * 4);
    }

    // Copy pixel data from the raster bitmap to the shm buffer
    const Pt::uint8_t* src = image.data();
    Pt::uint8_t* dst = buf->data();
    std::size_t rowBytes = static_cast<std::size_t>(imgWidth) * 4;

    for(int y = 0; y < imgHeight; ++y)
    {
        std::memcpy(dst + y * buf->stride(),
                    src + y * static_cast<int>(image.stride()),
                    rowBytes);
    }

    buf->setBusy(true);

    int bufferScale = _wm.app().outputScale();
    if( bufferScale < 1 ) bufferScale = 1;
    wl_surface_set_buffer_scale(_surface, bufferScale);

    wl_surface_attach(_surface, buf->buffer(), 0, 0);

    const Gfx::RectF& damage = _commitDamage;
    int dx = static_cast<int>(damage.x() * bufferScale);
    int dy = static_cast<int>(damage.y() * bufferScale);
    int dw = static_cast<int>(damage.width() * bufferScale) + 1;
    int dh = static_cast<int>(damage.height() * bufferScale) + 1;

    if( dx < 0 ) dx = 0;
    if( dy < 0 ) dy = 0;
    if( dw > imgWidth ) dw = imgWidth;
    if( dh > imgHeight ) dh = imgHeight;

    wl_surface_damage_buffer(_surface, dx, dy, dw, dh);

    _frameCallback = wl_surface_frame(_surface);
    wl_callback_add_listener(_frameCallback, &frameListener, this);

    wl_surface_commit(_surface);
    _framePending = true;
    _needsRepaint = false;

    _commitDamage = Gfx::RectF();

    wl_display_flush( _wm.app().display() );
}


void WindowImpl::onBufferReleased(ShmPool& /*pool*/)
{
    if( ! _configured || ! _window.isVisible() )
        return;

    _window.repaint( Gfx::RectF(Gfx::PointF(0, 0), size()) );
}


void WindowImpl::onFrameCallback(struct wl_callback* cb, uint32_t /*time*/)
{
    wl_callback_destroy(cb);
    _frameCallback = 0;
    _framePending = false;

    if( _needsRepaint && _configured && _window.isVisible() )
    {
        _needsRepaint = false;
        _window.repaint( Gfx::RectF(Gfx::PointF(0, 0), size()) );
    }
}


void WindowImpl::onPaintContent(const Gfx::RectF& damage)
{
    _commitDamage.unify(damage);
    _commitPending = true;
}

///////////////////////////////////////////////////////////////////////
// move
///////////////////////////////////////////////////////////////////////

void WindowImpl::onMove(Window& w, const Gfx::PointF& to)
{
    // On Wayland, toplevel windows cannot be positioned by the client;
    // their compositor-assigned position is unknown, so we treat it as (0,0).
    // Popup windows are placed at the requested offset relative to the parent.
    // Fire a MoveEvent with the realized position so that position() returns
    // the correct offset for toScreen()/fromScreen() and createPopup().
    const Gfx::PointF realized = (w.type() == WindowType::Default)
                                  ? Gfx::PointF(0, 0)
                                  : scaling().align(to);
    MoveEvent ev(*this, realized);
    processEvent(ev);
}


void WindowImpl::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// resize
///////////////////////////////////////////////////////////////////////

Gfx::SizeF WindowImpl::onResize(Window& w, const Gfx::SizeF& s)
{
    // Update the WindowFrame's widget size and (re)allocate the pixmap.
    // WindowFrame::onRescaleEvent runs before Window::onConnect (and thus
    // before onResize), so the pixmap was reset to empty at that time because
    // the frame's size was still (0,0).  Processing a ResizeEvent here
    // allocates the pixmap at the correct physical size using the already-set
    // canvas scale factor.
    Gfx::SizeF alignedSize = scaling().align(s);

    if( alignedSize.width() > w.maximumSize().width() )
        alignedSize.setWidth( w.maximumSize().width() );

    if( alignedSize.height() > w.maximumSize().height() )
        alignedSize.setHeight( w.maximumSize().height() );

    if( alignedSize.width() < w.minimumSize().width() )
        alignedSize.setWidth( w.minimumSize().width() );

    if( alignedSize.height() < w.minimumSize().height() )
        alignedSize.setHeight( w.minimumSize().height() );

    if( alignedSize.width() > 0 && alignedSize.height() > 0 )
    {
        ResizeEvent frev(*this, alignedSize);
        processEvent(frev);
    }

    return alignedSize;
}


void WindowImpl::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);

    // Forward resize to the client Window so Form::onResizeEvent triggers
    // relayout(), which sizes the main control (Workspace etc.).
    ResizeEvent rev(_window, ev.size());
    _window.processEvent(rev);
}

} // namespace Forms

} // namespace Pt
