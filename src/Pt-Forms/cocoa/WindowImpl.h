/* Copyright (C) 2015 Marc Boris Duerner 
  
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

#ifndef PT_FORMS_WINDOW_IMPL_H
#define PT_FORMS_WINDOW_IMPL_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/KeyEvent.h>
#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/MoveEvent.h>
#include <Pt/Forms/ResizeEvent.h>
#include <Pt/Forms/CloseEvent.h>
#include <Pt/Forms/ActivateEvent.h>
#include <Pt/Forms/WindowFrame.h>

#include <CoreGraphics/CGGeometry.h>
#include <CoreGraphics/CGContext.h>

#ifdef __OBJC__
    #import <AppKit/NSWindow.h>
    #import <AppKit/NSColorSpace.h>
    #import <AppKit/NSGraphicsContext.h>
    #import <AppKit/NSView.h>
    #import <AppKit/NSScreen.h>
    #import <AppKit/NSWindowController.h>
#else
    struct NSRect;
    struct NSPoint;
    struct NSSize;
    struct NSView;
    struct NSWindow;
    struct NSResponder;
    struct NSGraphicsContext;
    struct NSWindowController;
#endif

namespace Pt {

namespace Forms {

class ScreenImpl;
class GraphicsBackend;
class GenericGraphicsBackend;
class CocoaGraphicsBackend;

class WindowImpl : public WindowFrame
{
    typedef WindowFrame Base;

    friend class ScreenImpl;

    public:
        WindowImpl(ScreenImpl& wm, Window& w, GraphicsBackend& graphicsBackend);

        virtual ~WindowImpl();

        WindowType type() const;

        Gfx::PointF toScreen(const Gfx::PointF& pos) const;

        Gfx::PointF fromScreen(const Gfx::PointF& pos) const;

        void paint(const Gfx::RectF& rect);

    public:
        NSView* view()
        { 
            return _view;
        }

        NSWindow* window()
        {
            return _window;
        }

        void onViewPaint(const NSRect& rect);

        void onViewActivate(bool isActive);

        void onViewShow(bool v);

        void onViewMove(const NSPoint& viewPos);

        void onViewResize(const NSSize& frameSize);

        void onViewDidRescale();

        void onViewClosing();

        void onViewKeyDown(unsigned key, Pt::Char ch);

        void onViewKeyUp(unsigned key, Pt::Char ch);

        void onViewKeyModifier(unsigned int mask);

        void onViewLMouseDown(double x, double y);

        void onViewLMouseUp(double x, double y);

        void onViewMouseMove(double x, double y);

        void onViewRMouseDown(double x, double y);

        void onViewRMouseUp(double x, double y);

    protected:
        virtual void onInit(Window& w);

        virtual void onRelease(Window& w);

        virtual Gfx::PointF onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const;

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

    private:
        typedef void (WindowImpl::*PaintWindow)(CGContextRef ctx, const CGRect& nativeRect);

    private:
        void bindBackend(GraphicsBackend& graphicsBackend);

        void paintWindowNone(CGContextRef ctx, const CGRect& nativeRect);

        void paintWindowGeneric(CGContextRef ctx, const CGRect& nativeRect);

        void paintWindowCocoa(CGContextRef ctx, const CGRect& nativeRect);

    private:
        double applicationScaleFactor() const;

        double backingScaleFactor() const;

        double totalScaleFactor() const;

        /** @brief Converts a point from logical to native window-system coordinates.
        */
        Gfx::PointF toNative(const Gfx::PointF& pos) const;

        /** @brief Converts a size from logical to native window-system coordinates.
        */
        Gfx::SizeF toNative(const Gfx::SizeF& size) const;

        /** @brief Converts a rectangle from logical to native window-system coordinates.
        */
        Gfx::RectF toNative(const Gfx::RectF& rect) const;

        /** @brief Converts a point from native window-system to logical coordinates.
        */
        Gfx::PointF fromNative(const Gfx::PointF& pos) const;

        /** @brief Converts a size from native window-system to logical coordinates.
        */
        Gfx::SizeF fromNative(const Gfx::SizeF& size) const;

        /** @brief Converts a rectangle from native window-system to logical coordinates.
        */
        Gfx::RectF fromNative(const Gfx::RectF& rect) const;

        ScreenImpl&              _wm;
        Window&                  _client;
        NSWindow*                _window;
        NSView*                  _view;
        int                      _windowStyle;

        GenericGraphicsBackend*  _genericBackend;
        CocoaGraphicsBackend*    _cocoaBackend;
        PaintWindow              _paintWindow;

        unsigned                 _keyFlags;
        Key::Modifiers           _keyModifiers;
        KeyEvent                 _keyEvent;
        MouseEvent               _mouseEvent;
};

} // namespace

} // namespace

#endif // include guard
