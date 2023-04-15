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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_HMI_WINDOW_IMPL_H
#define PT_HMI_WINDOW_IMPL_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/WindowFrame.h>
#include <Pt/Connectable.h>

#ifdef __OBJC__
    #import <AppKit/NSWindow.h>
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

namespace Hmi {

class ScreenImpl;

class WindowImpl : public WindowFrame
                 , public Pt::Connectable
{
    typedef WindowFrame Base;

    friend class ScreenImpl;

    public:
        WindowImpl(ScreenImpl& wm,  Window& w);

        virtual ~WindowImpl();

        
        void setType(WindowType type);

        Gfx::PointF toScreen(const Gfx::PointF& pos) const;

        Gfx::PointF fromScreen(const Gfx::PointF& pos) const;

        void paint(const Gfx::RectF& rect);

        //void show(bool v);
    
        //void activate();

        //void enable(bool e);  
       
        //void move(const Gfx::PointF& pos);

        //void resize(const Gfx::SizeF& size);

        //void setAbove(bool isTop);

        //void setIcon(const Gfx::Image& p);

        //void setTitle(const std::string& text);

        //void setState(const WindowState& s);

        //void setMinimumSize(const Gfx::SizeF& s);
    
        //void setMaximumSize(const Gfx::SizeF& s);

    public:
        NSView* view()
        { 
            return _view;
        }

        NSWindow* window()
        {
            return _window;

    public:
        void onPaint(const NSRect& rect);

        void onActivate(bool isActive);

        void onShow(bool v);

        void onMove();

        void onResize(const NSSize& frameSize);

        void onClosing();

        void onKeyDown(unsigned key, Pt::Char ch);

        void onKeyUp(unsigned key, Pt::Char ch);

        void onKeyModifier(unsigned int mask);

        void onLMouseDown(double x, double y);

        void onLMouseUp(double x, double y);

        void onMouseMove(double x, double y);

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

        virtual void onRepaint(Window& w, const Gfx::RectF& rect);

        virtual void onShow(Window& w, bool visible);

        virtual void onActivate(Window& w, bool active);

        virtual void onEnable(Window& w, bool enable);

        virtual void onMove(Window& w, const Gfx::PointF& to);

        virtual void onResize(Window& w, const Gfx::SizeF& s);

        virtual void onClose(Window& w);

    protected:
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


        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);
        
        virtual void onRescaleEvent(const RescaleEvent& ev);

        
        virtual void onProcessWindowStateEvent(const WindowStateEvent& ev);

        virtual void onWindowStateEvent(const WindowStateEvent& ev);


        virtual void onProcessCloseEvent(const CloseEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

    private:
        ScreenImpl&              _wm;
        Window&                  _client;
        NSWindow*                _window;
        NSView*                  _view;
        int                      _windowStyle;
        int                      _level;

        unsigned                 _keyFlags;
        Key::Modifiers           _keyModifiers;
        KeyEvent                 _keyEvent;
        MouseEvent               _mouseEvent;
};

} // namespace

} // namespace

#endif // include guard
