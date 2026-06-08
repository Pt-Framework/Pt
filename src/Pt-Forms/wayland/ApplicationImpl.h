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

#ifndef Pt_Forms_wayland_ApplicationImpl_h
#define Pt_Forms_wayland_ApplicationImpl_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Cursor.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/TouchEvent.h>

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Timespan.h>

#include "WaylandFd.h"
#include "KeyHandler.h"
#include "CursorImpl.h"

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>

#include "xdg-shell-client-protocol.h"
#include "xdg-decoration-client-protocol.h"

#ifdef PT_FORMS_WAYLAND_VULKAN
#include "../vulkan/VulkanDevice.h"
#endif

#ifdef PT_FORMS_WAYLAND_NANOVG
#include "../nanovg/NanoVGGraphicsBackend.h"
#include "../nanovg/NanoVGDevice.h"
#endif

namespace Pt {

namespace Forms {

class WindowImpl;
class GraphicsBackend;

class ApplicationImpl : public Pt::System::MainLoop
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();

        void setCursor(const Cursor* cursor);

        Pt::Timespan inactivityTime() const;

        void sendKeyEvent(const KeyEvent& ev);

        void sendMouseEvent(const MouseEvent& ev);

        void nextEvent();

        GraphicsBackend* queryBackend()
        {
#ifdef PT_FORMS_WAYLAND_NANOVG
            return new NanoVGGraphicsBackend();
#else
            return 0;
#endif
        }

    public:
        struct wl_display* display() const 
        { return _display; }

        struct wl_compositor* compositor() const 
        { return _compositor; }

        struct wl_shm* shm() const 
        { return _shm; }

        struct xdg_wm_base* xdgWmBase() const 
        { return _xdgWmBase; }

        struct wl_seat* seat() const 
        { return _seat; }

        struct zxdg_decoration_manager_v1* decorationManager() const 
        { return _decorationManager; }

        double screenScaling() const 
        { return _screenScaling; }

        int outputScale()   const 
        { return _outputScale; }

#ifdef PT_FORMS_WAYLAND_VULKAN
        VulkanDevice& vulkanDevice() { return _vulkanDevice; }
#endif

#ifdef PT_FORMS_WAYLAND_NANOVG
        NanoVGDevice& nanovgDevice() { return _nanovgDevice; }
#endif

    protected:
        virtual void onRun();

    public:
        // Wayland event handlers called from C listener callbacks
        void handleRegistryGlobal(struct wl_registry* registry,
                                  uint32_t name, const char* interface, uint32_t version);
        void handleRegistryGlobalRemove(uint32_t name);
        void handleSeatCapabilities(uint32_t capabilities);
        void handlePointerEnter(uint32_t serial, struct wl_surface* surface,
                                wl_fixed_t sx, wl_fixed_t sy);
        void handlePointerLeave(uint32_t serial, struct wl_surface* surface);
        void handlePointerMotion(uint32_t time, wl_fixed_t sx, wl_fixed_t sy);
        void handlePointerButton(uint32_t serial, uint32_t time,
                                 uint32_t button, uint32_t state);
        void handlePointerAxis(uint32_t time, uint32_t axis, wl_fixed_t value);
        void handleKeyboardKeymap(uint32_t format, int fd, uint32_t size);
        void handleKeyboardEnter(uint32_t serial, struct wl_surface* surface);
        void handleKeyboardLeave(uint32_t serial, struct wl_surface* surface);
        void handleKeyboardKey(uint32_t serial, uint32_t time,
                               uint32_t key, uint32_t state);
        void handleKeyboardModifiers(uint32_t serial, uint32_t modsDepressed,
                                     uint32_t modsLatched, uint32_t modsLocked,
                                     uint32_t group);
        void handleTouchDown(uint32_t serial, uint32_t time,
                             struct wl_surface* surface, int32_t id,
                             wl_fixed_t x, wl_fixed_t y);
        void handleTouchUp(uint32_t serial, uint32_t time, int32_t id);
        void handleTouchMotion(uint32_t time, int32_t id,
                               wl_fixed_t x, wl_fixed_t y);
        void handleTouchFrame();
        void handleTouchCancel();
        void handleOutputGeometry(int32_t physicalWidth, int32_t physicalHeight);
        void handleOutputMode(uint32_t flags, int32_t width, int32_t height);
        void handleOutputScale(int32_t factor);
        void handleOutputDone();

    private:
        void onWaylandEvents(int);
        void flushPendingPaints();
        Window* findWindow(struct wl_surface* surface);

    private:
        struct wl_display*                    _display;
        struct wl_registry*                   _registry;
        struct wl_compositor*                 _compositor;
        struct wl_shm*                        _shm;
        struct xdg_wm_base*                   _xdgWmBase;
        struct wl_seat*                       _seat;
        struct wl_pointer*                    _pointer;
        struct wl_keyboard*                   _keyboard;
        struct wl_touch*                      _touch;
        struct wl_output*                     _output;
        struct zxdg_decoration_manager_v1*    _decorationManager;

        WaylandFd                             _wlFd;
        KeyHandler                            _keyHandler;
        CursorImpl                            _cursorImpl;

        // Output geometry for DPI calculation
        int32_t                               _outputPhysicalWidth;
        int32_t                               _outputPhysicalHeight;
        int32_t                               _outputPixelWidth;
        int32_t                               _outputPixelHeight;
        double                                _screenScaling;
        int                                   _outputScale;

        // Input state
        struct wl_surface*                    _pointerFocusSurface;
        struct wl_surface*                    _keyboardFocusSurface;
        Gfx::PointF                           _pointerPosition;
        uint32_t                              _pointerSerial;
        MouseEvent                            _mouseEvent;
        TouchEvent                            _touchEvent;

#ifdef PT_FORMS_WAYLAND_VULKAN
        VulkanDevice                          _vulkanDevice;
#endif

#ifdef PT_FORMS_WAYLAND_NANOVG
        NanoVGDevice                          _nanovgDevice;
#endif
};

} // namespace Forms

} // namespace Pt

#endif
