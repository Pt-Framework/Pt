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

#ifndef Pt_Forms_wayland_WindowImpl_h
#define Pt_Forms_wayland_WindowImpl_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/WindowFrame.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Size.h>

#include "ShmPool.h"

#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#include "xdg-decoration-client-protocol.h"

#include <string>

namespace Pt {
namespace Forms {

class ScreenImpl;

class WindowImpl : public WindowFrame
{
    typedef WindowFrame Base;
    friend class ScreenImpl;

    public:
        WindowImpl(ScreenImpl& wm, Window& w);

        virtual ~WindowImpl();

        void setType(WindowType type);

        Gfx::PointF toScreen(const Gfx::PointF& pos) const;

        Gfx::PointF fromScreen(const Gfx::PointF& pos) const;

        void paint(const Gfx::RectF& rect);

    public:
        struct wl_surface* surface() const 
        { return _surface; }

        int width() const 
        { return _width; }

        int height() const 
        { return _height; }

    protected:
        virtual void onInit(Window& w);
        virtual void onRelease(Window& w);
        virtual Gfx::PointF onToWindow(const Window& w, const Gfx::PointF& pos) const;
        virtual Gfx::PointF onFromWindow(const Window& w, const Gfx::PointF& pos) const;
        virtual void onSetTitle(Window& w, const std::string& text);
        virtual void onSetIcon(Window& w, const Gfx::Image& icon);
        virtual void onSetState(Window& w, const WindowState& state);
        virtual void onSetAbove(Window& w, bool above);
        virtual void onSetSizeLimits(Window& w, const Gfx::SizeF& minSize,
                                     const Gfx::SizeF& maxSize);
        virtual void onAutoCenter(Window& w, const Gfx::SizeF* size);
        virtual void onRepaint(Window& w, const Gfx::RectF& rect);
        virtual void onShow(Window& w, bool visible);
        virtual void onActivate(Window& w, bool active);
        virtual void onEnable(Window& w, bool enable);
        virtual void onMove(Window& w, const Gfx::PointF& to);
        virtual Gfx::SizeF onResize(Window& w, const Gfx::SizeF& s);
        virtual void onClose(Window& w);

    protected:
        virtual void onConnect(Screen& screen);
        virtual void onDisconnect();
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;
        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual void onProcessPaintEvent(const PaintEvent& ev);
        virtual void onPaintEvent(const PaintEvent& ev);
        virtual void onProcessShowEvent(const ShowEvent& ev);
        virtual void onShowEvent(const ShowEvent& ev);
        virtual void onProcessEnableEvent(const EnableEvent& ev);
        virtual void onEnableEvent(const EnableEvent& ev);
        virtual void onProcessActivateEvent(const ActivateEvent& ev);
        virtual void onActivateEvent(const ActivateEvent& ev);
        virtual void onProcessMoveEvent(const MoveEvent& ev);
        virtual void onProcessResizeEvent(const ResizeEvent& ev);
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);
        virtual void onRescaleEvent(const RescaleEvent& ev);
        virtual void onProcessWindowStateEvent(const WindowStateEvent& ev);
        virtual void onWindowStateEvent(const WindowStateEvent& ev);
        virtual void onProcessCloseEvent(const CloseEvent& ev);
        virtual void onCloseEvent(const CloseEvent& ev);

    public:
        // Wayland event handlers called from C listener callbacks
        void handleXdgSurfaceConfigure(struct xdg_surface* xdgSurface,
                                       uint32_t serial);
        void handleToplevelConfigure(int32_t width, int32_t height,
                                     struct wl_array* states);
        void handleToplevelClose();
        void handlePopupConfigure(int32_t x, int32_t y,
                                  int32_t width, int32_t height);
        void handlePopupDone();

    private:
        void createToplevel();
        void createPopup(Window& w);
        void destroyPopup();
        void destroySurface();
        void onPaintContent(const Gfx::RectF& damage);
        void onBufferReleased(ShmPool& pool);

    private:
        ScreenImpl&                          _wm;
        Window&                              _client;
        struct wl_surface*                   _surface;
        struct xdg_surface*                  _xdgSurface;
        struct xdg_toplevel*                 _xdgToplevel;
        struct xdg_popup*                    _xdgPopup;
        struct xdg_positioner*               _positioner;
        struct zxdg_toplevel_decoration_v1*  _decoration;
        ShmPool                              _shmPool;
        Gfx::PointF                          _popupOffset;
        int                                  _width;
        int                                  _height;
        int                                  _pendingWidth;
        int                                  _pendingHeight;
        bool                                 _configured;
        bool                                 _visible;
};

} // namespace Forms
} // namespace Pt

#endif
