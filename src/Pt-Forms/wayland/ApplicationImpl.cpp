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

#include "posix/MainLoopImpl.h"
#include "ApplicationImpl.h"
#include "ScreenImpl.h"
#include "WindowImpl.h"

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Screen.h>
#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/ScrollEvent.h>
#include <Pt/IOError.h>
#include <Pt/SourceInfo.h>

#include <cstring>
#include <cmath>
#include <linux/input-event-codes.h>

namespace Pt {
    
namespace Forms {

namespace {

// Registry callbacks

void registryGlobal(void* data, struct wl_registry* registry,
                    uint32_t name, const char* interface, uint32_t version)
{
    static_cast<ApplicationImpl*>(data)->handleRegistryGlobal(
        registry, name, interface, version);
}

void registryGlobalRemove(void* data, struct wl_registry* /*registry*/,
                          uint32_t name)
{
    static_cast<ApplicationImpl*>(data)->handleRegistryGlobalRemove(name);
}

// xdg_wm_base

void xdgWmBasePing(void* /*data*/, struct xdg_wm_base* wmBase, uint32_t serial)
{
    xdg_wm_base_pong(wmBase, serial);
}

// wl_seat

void seatCapabilities(void* data, struct wl_seat* /*seat*/, uint32_t capabilities)
{
    static_cast<ApplicationImpl*>(data)->handleSeatCapabilities(capabilities);
}

void seatName(void* /*data*/, struct wl_seat* /*seat*/, const char* /*name*/)
{
}

// wl_pointer

void pointerEnter(void* data, struct wl_pointer* /*pointer*/,
                  uint32_t serial, struct wl_surface* surface,
                  wl_fixed_t sx, wl_fixed_t sy)
{
    static_cast<ApplicationImpl*>(data)->handlePointerEnter(serial, surface, sx, sy);
}

void pointerLeave(void* data, struct wl_pointer* /*pointer*/,
                  uint32_t serial, struct wl_surface* surface)
{
    static_cast<ApplicationImpl*>(data)->handlePointerLeave(serial, surface);
}

void pointerMotion(void* data, struct wl_pointer* /*pointer*/,
                   uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    static_cast<ApplicationImpl*>(data)->handlePointerMotion(time, sx, sy);
}

void pointerButton(void* data, struct wl_pointer* /*pointer*/,
                   uint32_t serial, uint32_t time,
                   uint32_t button, uint32_t state)
{
    static_cast<ApplicationImpl*>(data)->handlePointerButton(serial, time, button, state);
}

void pointerAxis(void* data, struct wl_pointer* /*pointer*/,
                 uint32_t time, uint32_t axis, wl_fixed_t value)
{
    static_cast<ApplicationImpl*>(data)->handlePointerAxis(time, axis, value);
}

void pointerFrame(void* /*data*/, struct wl_pointer* /*pointer*/)
{
    // Logical event group boundary — no action needed.
}

void pointerAxisSource(void* /*data*/, struct wl_pointer* /*pointer*/,
                       uint32_t /*axis_source*/)
{
}

void pointerAxisStop(void* /*data*/, struct wl_pointer* /*pointer*/,
                     uint32_t /*time*/, uint32_t /*axis*/)
{
}

void pointerAxisDiscrete(void* /*data*/, struct wl_pointer* /*pointer*/,
                         uint32_t /*axis*/, int32_t /*discrete*/)
{
}

// wl_keyboard

void keyboardKeymap(void* data, struct wl_keyboard* /*keyboard*/,
                    uint32_t format, int fd, uint32_t size)
{
    static_cast<ApplicationImpl*>(data)->handleKeyboardKeymap(format, fd, size);
}

void keyboardEnter(void* data, struct wl_keyboard* /*keyboard*/,
                   uint32_t serial, struct wl_surface* surface,
                   struct wl_array* /*keys*/)
{
    static_cast<ApplicationImpl*>(data)->handleKeyboardEnter(serial, surface);
}

void keyboardLeave(void* data, struct wl_keyboard* /*keyboard*/,
                   uint32_t serial, struct wl_surface* surface)
{
    static_cast<ApplicationImpl*>(data)->handleKeyboardLeave(serial, surface);
}

void keyboardKey(void* data, struct wl_keyboard* /*keyboard*/,
                 uint32_t serial, uint32_t time,
                 uint32_t key, uint32_t state)
{
    static_cast<ApplicationImpl*>(data)->handleKeyboardKey(serial, time, key, state);
}

void keyboardModifiers(void* data, struct wl_keyboard* /*keyboard*/,
                       uint32_t serial, uint32_t modsDepressed,
                       uint32_t modsLatched, uint32_t modsLocked,
                       uint32_t group)
{
    static_cast<ApplicationImpl*>(data)->handleKeyboardModifiers(
        serial, modsDepressed, modsLatched, modsLocked, group);
}

void keyboardRepeatInfo(void* /*data*/, struct wl_keyboard* /*keyboard*/,
                        int32_t /*rate*/, int32_t /*delay*/)
{
}

// wl_touch

void touchDown(void* data, struct wl_touch* /*touch*/,
               uint32_t serial, uint32_t time,
               struct wl_surface* surface, int32_t id,
               wl_fixed_t x, wl_fixed_t y)
{
    static_cast<ApplicationImpl*>(data)->handleTouchDown(serial, time, surface, id, x, y);
}

void touchUp(void* data, struct wl_touch* /*touch*/,
             uint32_t serial, uint32_t time, int32_t id)
{
    static_cast<ApplicationImpl*>(data)->handleTouchUp(serial, time, id);
}

void touchMotion(void* data, struct wl_touch* /*touch*/,
                 uint32_t time, int32_t id,
                 wl_fixed_t x, wl_fixed_t y)
{
    static_cast<ApplicationImpl*>(data)->handleTouchMotion(time, id, x, y);
}

void touchFrame(void* data, struct wl_touch* /*touch*/)
{
    static_cast<ApplicationImpl*>(data)->handleTouchFrame();
}

void touchCancel(void* data, struct wl_touch* /*touch*/)
{
    static_cast<ApplicationImpl*>(data)->handleTouchCancel();
}

// wl_output

void outputGeometry(void* data, struct wl_output* /*output*/,
                    int32_t /*x*/, int32_t /*y*/,
                    int32_t physicalWidth, int32_t physicalHeight,
                    int32_t /*subpixel*/, const char* /*make*/,
                    const char* /*model*/, int32_t /*transform*/)
{
    static_cast<ApplicationImpl*>(data)->handleOutputGeometry(
        physicalWidth, physicalHeight);
}

void outputMode(void* data, struct wl_output* /*output*/,
                uint32_t flags, int32_t width, int32_t height,
                int32_t /*refresh*/)
{
    static_cast<ApplicationImpl*>(data)->handleOutputMode(flags, width, height);
}

void outputDone(void* data, struct wl_output* /*output*/)
{
    static_cast<ApplicationImpl*>(data)->handleOutputDone();
}

void outputScale(void* data, struct wl_output* /*output*/, int32_t factor)
{
    static_cast<ApplicationImpl*>(data)->handleOutputScale(factor);
}

} // anonymous namespace


static const struct wl_registry_listener registryListener = {
    registryGlobal,
    registryGlobalRemove
};

static const struct xdg_wm_base_listener xdgWmBaseListener = {
    xdgWmBasePing
};

static const struct wl_seat_listener seatListener = {
    seatCapabilities,
    seatName
};

static const struct wl_pointer_listener pointerListener = {
    pointerEnter,
    pointerLeave,
    pointerMotion,
    pointerButton,
    pointerAxis,
    pointerFrame,
    pointerAxisSource,
    pointerAxisStop,
    pointerAxisDiscrete
};

static const struct wl_keyboard_listener keyboardListener = {
    keyboardKeymap,
    keyboardEnter,
    keyboardLeave,
    keyboardKey,
    keyboardModifiers,
    keyboardRepeatInfo
};

static const struct wl_touch_listener touchListener = {
    touchDown,
    touchUp,
    touchMotion,
    touchFrame,
    touchCancel
};

static const struct wl_output_listener outputListener = {
    outputGeometry,
    outputMode,
    outputDone,
    outputScale
};


ApplicationImpl::ApplicationImpl()
: _display( wl_display_connect(NULL) )
, _registry(0)
, _compositor(0)
, _shm(0)
, _xdgWmBase(0)
, _seat(0)
, _pointer(0)
, _keyboard(0)
, _touch(0)
, _output(0)
, _decorationManager(0)
, _wlFd(_display)
, _outputPhysicalWidth(0)
, _outputPhysicalHeight(0)
, _outputPixelWidth(0)
, _outputPixelHeight(0)
, _screenScaling(1.0)
, _outputScale(1)
, _pointerFocusSurface(0)
, _keyboardFocusSurface(0)
, _pointerSerial(0)
{
    if( ! _display )
        throw AccessFailed("Wayland display");

    _registry = wl_display_get_registry(_display);
    wl_registry_add_listener(_registry, &registryListener, this);

    // First roundtrip to get globals
    wl_display_roundtrip(_display);

    // Second roundtrip to receive initial output geometry/mode events
    wl_display_roundtrip(_display);

    _wlFd.setActive(*this);
    _wlFd.begin();
    _wlFd.eventReady() += Pt::slot(*this, &ApplicationImpl::onWaylandEvents);

    _cursorImpl.init(_shm, _pointer, _compositor);
}


ApplicationImpl::~ApplicationImpl()
{
    if( _touch )
        wl_touch_destroy(_touch);

    if( _keyboard )
        wl_keyboard_destroy(_keyboard);

    if( _pointer )
        wl_pointer_destroy(_pointer);

    if( _decorationManager )
        zxdg_decoration_manager_v1_destroy(_decorationManager);

    if( _seat )
        wl_seat_destroy(_seat);

    if( _output )
        wl_output_destroy(_output);

    if( _xdgWmBase )
        xdg_wm_base_destroy(_xdgWmBase);

    if( _shm )
        wl_shm_destroy(_shm);

    if( _compositor )
        wl_compositor_destroy(_compositor);

    if( _registry )
        wl_registry_destroy(_registry);

    _cursorImpl.destroy();

    if( _display )
    {
        wl_display_disconnect(_display);
        _display = 0;
    }
}


void ApplicationImpl::setCursor(const Cursor* cursor)
{
    _cursorImpl.setCursor(cursor, _pointerSerial);
}


Pt::Timespan ApplicationImpl::inactivityTime() const
{
    return Pt::Timespan(0);
}


void ApplicationImpl::sendKeyEvent(const KeyEvent& /*ev*/)
{
}


void ApplicationImpl::sendMouseEvent(const MouseEvent& /*ev*/)
{
}


void ApplicationImpl::nextEvent()
{
    MainLoop::waitNext();
}


void ApplicationImpl::onRun()
{
    _wlFd.processEvents();

    while( MainLoop::impl()->waitNext() )
    {
        _wlFd.processEvents();
    }
}


void ApplicationImpl::onWaylandEvents(int)
{
    _wlFd.processEvents();
}


Window* ApplicationImpl::findWindow(struct wl_surface* surface)
{
    if( ! surface )
        return 0;

    const std::vector<Window*>& windows = Application::instance().screen().windows();
    for(std::size_t i = 0; i < windows.size(); ++i)
    {
        Window* w = windows[i];
        if( ! w->frame() )
            continue;

        WindowImpl* wi = static_cast<WindowImpl*>( w->frame() );
        if( wi->surface() == surface )
            return w;
    }

    return 0;
}

// Registry callbacks

void ApplicationImpl::handleRegistryGlobal(struct wl_registry* registry,
                                           uint32_t name, const char* interface,
                                           uint32_t version)
{
    if( std::strcmp(interface, wl_compositor_interface.name) == 0 )
    {
        _compositor = static_cast<struct wl_compositor*>(
            wl_registry_bind(registry, name, &wl_compositor_interface, 4)
        );
    }
    else if( std::strcmp(interface, wl_shm_interface.name) == 0 )
    {
        _shm = static_cast<struct wl_shm*>(
            wl_registry_bind(registry, name, &wl_shm_interface, 1)
        );
    }
    else if( std::strcmp(interface, xdg_wm_base_interface.name) == 0 )
    {
        _xdgWmBase = static_cast<struct xdg_wm_base*>(
            wl_registry_bind(registry, name, &xdg_wm_base_interface, 1)
        );
        xdg_wm_base_add_listener(_xdgWmBase, &xdgWmBaseListener, this);
    }
    else if( std::strcmp(interface, wl_seat_interface.name) == 0 )
    {
        _seat = static_cast<struct wl_seat*>(
            wl_registry_bind(registry, name, &wl_seat_interface, 5)
        );
        wl_seat_add_listener(_seat, &seatListener, this);
    }
    else if( std::strcmp(interface, wl_output_interface.name) == 0 )
    {
        _output = static_cast<struct wl_output*>(
            wl_registry_bind(registry, name, &wl_output_interface, 2)
        );
        wl_output_add_listener(_output, &outputListener, this);
    }
    else if( std::strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0 )
    {
        _decorationManager = static_cast<struct zxdg_decoration_manager_v1*>(
            wl_registry_bind(registry, name, &zxdg_decoration_manager_v1_interface, 1)
        );
    }
}


void ApplicationImpl::handleRegistryGlobalRemove(uint32_t /*name*/)
{
}


// wl_seat

void ApplicationImpl::handleSeatCapabilities(uint32_t capabilities)
{
    bool hasPointer = (capabilities & WL_SEAT_CAPABILITY_POINTER) != 0;
    bool hasKeyboard = (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) != 0;
    bool hasTouch = (capabilities & WL_SEAT_CAPABILITY_TOUCH) != 0;

    if( hasPointer && ! _pointer )
    {
        _pointer = wl_seat_get_pointer(_seat);
        wl_pointer_add_listener(_pointer, &pointerListener, this);
        _cursorImpl.setPointer(_pointer);
    }
    else if( ! hasPointer && _pointer )
    {
        wl_pointer_destroy(_pointer);
        _pointer = 0;
        _cursorImpl.setPointer(0);
    }

    if( hasKeyboard && ! _keyboard )
    {
        _keyboard = wl_seat_get_keyboard(_seat);
        wl_keyboard_add_listener(_keyboard, &keyboardListener, this);
    }
    else if( ! hasKeyboard && _keyboard )
    {
        wl_keyboard_destroy(_keyboard);
        _keyboard = 0;
    }

    if( hasTouch && ! _touch )
    {
        _touch = wl_seat_get_touch(_seat);
        wl_touch_add_listener(_touch, &touchListener, this);
    }
    else if( ! hasTouch && _touch )
    {
        wl_touch_destroy(_touch);
        _touch = 0;
    }
}

// wl_pointer

void ApplicationImpl::handlePointerEnter(uint32_t serial, struct wl_surface* surface,
                                         wl_fixed_t sx, wl_fixed_t sy)
{
    _pointerFocusSurface = surface;
    _pointerSerial = serial;
    _pointerPosition.set( wl_fixed_to_double(sx), wl_fixed_to_double(sy) );

    _cursorImpl.show(serial);

    Window* w = findWindow(surface);
    if(w)
        Application::instance().screen().setPointer(w);
}


void ApplicationImpl::handlePointerLeave(uint32_t /*serial*/, struct wl_surface* /*surface*/)
{
    _pointerFocusSurface = 0;
    Application::instance().screen().setPointer(0);
}


void ApplicationImpl::handlePointerMotion(uint32_t /*time*/, wl_fixed_t sx, wl_fixed_t sy)
{
    _pointerPosition.set( wl_fixed_to_double(sx), wl_fixed_to_double(sy) );

    Window* w = findWindow(_pointerFocusSurface);
    if( ! w || ! w->frame() )
        return;

    // Convert surface-local to screen coordinates for Application dispatch
    WindowImpl* frame = static_cast<WindowImpl*>( w->frame() );
    Gfx::PointF screenPos = frame->toScreen(_pointerPosition);
    _mouseEvent.setMove();
    _mouseEvent.setWidget(w);
    _mouseEvent.setPosition(screenPos);
    Application::instance().processEvent(_mouseEvent);
}


void ApplicationImpl::handlePointerButton(uint32_t serial, uint32_t /*time*/,
                                          uint32_t button, uint32_t state)
{
    _pointerSerial = serial;

    Window* w = findWindow(_pointerFocusSurface);
    if( ! w || ! w->frame() )
        return;

    // Map Linux button codes to Pt button enumeration
    uint32_t btn = MouseEvent::Left;
    if( button == BTN_RIGHT )
        btn = MouseEvent::Right;
    else if( button == BTN_MIDDLE )
        btn = MouseEvent::Middle;

    if( state == WL_POINTER_BUTTON_STATE_PRESSED )
        _mouseEvent.setPress(btn);
    else
        _mouseEvent.setRelease(btn);

    // Convert surface-local to screen coordinates for Application dispatch
    WindowImpl* frame = static_cast<WindowImpl*>( w->frame() );
    Gfx::PointF screenPos = frame->toScreen(_pointerPosition);

    _mouseEvent.setWidget(w);
    _mouseEvent.setPosition(screenPos);
    Application::instance().processEvent(_mouseEvent);
}


void ApplicationImpl::handlePointerAxis(uint32_t /*time*/, uint32_t axis, wl_fixed_t value)
{
    Window* w = findWindow(_pointerFocusSurface);
    if( ! w )
        return;

    double delta = -wl_fixed_to_double(value) / 10.0;

    ScrollEvent ev(*w);

    if( axis == WL_POINTER_AXIS_VERTICAL_SCROLL )
        ev.set(ScrollEvent::Vertical, delta);
    else
        ev.set(ScrollEvent::Horizontal, delta);

    Application::instance().processEvent(ev);
}


// wl_keyboard

void ApplicationImpl::handleKeyboardKeymap(uint32_t format, int fd, uint32_t size)
{
    _keyHandler.updateKeymap(format, fd, size);
}


void ApplicationImpl::handleKeyboardEnter(uint32_t /*serial*/, struct wl_surface* surface)
{
    _keyboardFocusSurface = surface;
}


void ApplicationImpl::handleKeyboardLeave(uint32_t /*serial*/, struct wl_surface* /*surface*/)
{
    _keyboardFocusSurface = 0;
}


void ApplicationImpl::handleKeyboardKey(uint32_t /*serial*/, uint32_t /*time*/,
                                        uint32_t key, uint32_t state)
{
    Window* w = findWindow(_keyboardFocusSurface);
    if( ! w )
        return;

    KeyEvent ev = _keyHandler.translateKey(key, state);
    ev.setWidget(w);
    Application::instance().processEvent(ev);
}


void ApplicationImpl::handleKeyboardModifiers(uint32_t /*serial*/, uint32_t modsDepressed,
                                              uint32_t modsLatched, uint32_t modsLocked,
                                              uint32_t group)
{
    _keyHandler.updateModifiers(modsDepressed, modsLatched, modsLocked, group);
}


// wl_touch

void ApplicationImpl::handleTouchDown(uint32_t /*serial*/, uint32_t /*time*/,
                                      struct wl_surface* surface, int32_t id,
                                      wl_fixed_t x, wl_fixed_t y)
{
    Window* w = findWindow(surface);
    if( ! w || ! w->frame() )
        return;

    // Convert surface-local to screen coordinates for Application dispatch
    WindowImpl* frame = static_cast<WindowImpl*>( w->frame() );
    Gfx::PointF screenPos = frame->toScreen(
        Gfx::PointF(wl_fixed_to_double(x), wl_fixed_to_double(y)) );

    _touchEvent.clear();
    _touchEvent.setWidget(w);
    _touchEvent.setTrackingId(static_cast<Pt::uint32_t>(id));
    _touchEvent.setPosition(screenPos);
    _touchEvent.setPress();
    Application::instance().processEvent(_touchEvent);
}


void ApplicationImpl::handleTouchUp(uint32_t /*serial*/, uint32_t /*time*/, int32_t id)
{
    Window* w = findWindow(_keyboardFocusSurface);
    if( ! w )
        return;

    _touchEvent.clear();
    _touchEvent.setWidget(w);
    _touchEvent.setTrackingId(static_cast<Pt::uint32_t>(id));
    _touchEvent.setRelease();
    Application::instance().processEvent(_touchEvent);
}


void ApplicationImpl::handleTouchMotion(uint32_t /*time*/, int32_t id,
                                        wl_fixed_t x, wl_fixed_t y)
{
    Window* w = findWindow(_keyboardFocusSurface);
    if( ! w || ! w->frame() )
        return;

    // Convert surface-local to screen coordinates for Application dispatch
    WindowImpl* frame = static_cast<WindowImpl*>( w->frame() );
    Gfx::PointF screenPos = frame->toScreen(
        Gfx::PointF(wl_fixed_to_double(x), wl_fixed_to_double(y)) );

    _touchEvent.clear();
    _touchEvent.setWidget(w);
    _touchEvent.setTrackingId(static_cast<Pt::uint32_t>(id));
    _touchEvent.setPosition(screenPos);
    _touchEvent.setMove();
    Application::instance().processEvent(_touchEvent);
}


void ApplicationImpl::handleTouchFrame()
{
}


void ApplicationImpl::handleTouchCancel()
{
}


// wl_output

void ApplicationImpl::handleOutputGeometry(int32_t physicalWidth, int32_t physicalHeight)
{
    _outputPhysicalWidth = physicalWidth;
    _outputPhysicalHeight = physicalHeight;
}


void ApplicationImpl::handleOutputMode(uint32_t flags, int32_t width, int32_t height)
{
    if( ! (flags & WL_OUTPUT_MODE_CURRENT) )
        return;

    _outputPixelWidth = width;
    _outputPixelHeight = height;
}


void ApplicationImpl::handleOutputScale(int32_t factor)
{
    if( factor > 0 )
        _outputScale = factor;
}

void ApplicationImpl::handleOutputDone()
{
    // Calculate DPI-based scaling
    if( _outputPhysicalWidth > 0 && _outputPixelWidth > 0 )
    {
        double dpiX = static_cast<double>(_outputPixelWidth) /
                      (static_cast<double>(_outputPhysicalWidth) / 25.4);
        _screenScaling = dpiX / 96.0;

        // Clamp to reasonable range
        if( _screenScaling < 1.0 )
            _screenScaling = 1.0;
    }
}

} // namespace Forms

} // namespace Pt
