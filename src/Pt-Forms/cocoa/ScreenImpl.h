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
  MA  02110-1301  USA
*/

#ifndef Pt_Forms_ScreenImpl_H
#define Pt_Forms_ScreenImpl_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Screen.h>
#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/LayoutEvent.h> // RescaleEvent
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Signal.h>

#ifdef __OBJC__
    #import <AppKit/NSWindow.h>
    #import <AppKit/NSApplication.h>
#else
    struct NSPoint;
    struct NSWindow;
#endif

namespace Pt {

namespace Forms {

class ApplicationImpl;
class WindowImpl;
class Window;
class WindowBase;
class MouseEvent;
class TouchEvent;
class ScrollEvent;

class ScreenImpl : public WindowManager
{
    typedef WindowManager Base;

    friend class WindowImpl;

    public:
        ScreenImpl(ApplicationImpl& app);
        
        virtual ~ScreenImpl();


        void setParent(Screen* screen);


        void addWindow(Window& w);

        void removeWindow(Window& w);

        const std::vector<Window*>& windows() const;

        WindowManager& windowManager();


        void setCapture(Visual* capture);

    protected:
        Gfx::PointF toFrame(const WindowImpl& w, 
                            const Gfx::PointF& pos) const;

        Gfx::PointF fromFrame(const WindowImpl& w, 
                              const Gfx::PointF& pos) const;

    //
    // Responder
    //
    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    //
    // Visual
    //
    protected:
        virtual Visual* onHitTest(const Gfx::PointF& pos);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual void onProcessEvent(const Event& ev);

        virtual void onRequestRepaint(const Gfx::RectF& rect);

    //
    // WindowManager
    //
    protected:
        virtual WindowFrame* onAttach(Window& w);
    
        virtual void onDetach(WindowFrame& w);

        virtual void onInit(WindowFrame& w);

        virtual void onRelease(WindowFrame& w);

        //virtual void onRepaint(Window& w, const Gfx::RectF& rect);

        //virtual void onShow(Window& w, bool visible); 

        //virtual void onActivate(Window& w, bool active);

        //virtual void onEnableRequest(Window& w, bool enable);

        //virtual void onMove(Window& w, const Gfx::PointF& to);

        //virtual void onResize(Window& w, const Gfx::SizeF& to);

        //virtual void onSetAbove(Window& w, bool above);

        //virtual void onSetTitle(Window& w, const std::string& text);

        //virtual void onSetIcon(Window& w, const Gfx::Image& icon);

        //virtual void onSetState(Window& w, const WindowState& state);

        //virtual void onSetSizeLimits(Window& w, const Gfx::SizeF& minSize, 
        //                                        const Gfx::SizeF& maxSize);

        //virtual void onClosing(Window& w);

    //
    // scaling
    //
    protected:
        void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onRescaleEvent(const RescaleEvent& ev);
        
    //
    // painting
    //
    protected:
        void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);

        virtual void onPaint(const Gfx::RectF& rect);

    //
    // enable
    //
    protected:
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);

        virtual void onEnable(bool e);

    //
    // input
    //
    protected:
        void onProcessMouseEvent(const MouseEvent& ev);

        void onProcessTouchEvent(const TouchEvent& ev);

        void onProcessScrollEvent(const ScrollEvent& ev);

        void onProcessKeyEvent(const KeyEvent& ev);

    private:
        Window* findWindow(NSWindow* wnd);

    private:
        Screen*                      _parent;
        std::vector<Window*>         _windows;

        void*                        _captureMonitor;

        double                       _screenScaling;
};

} // namespace

} // namespace

#endif // include guard
