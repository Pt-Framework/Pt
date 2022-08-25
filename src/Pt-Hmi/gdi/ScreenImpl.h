/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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

#ifndef Pt_Hmi_ScreenImpl_H
#define Pt_Hmi_ScreenImpl_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Screen.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/LayoutEvent.h> // RescaleEvent
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>

#include <vector>

#include <Windows.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl;
class Window;
class MouseEvent;
class TouchEvent;
class ScrollEvent;
class KeyEvent;

class ScreenImpl : public Visual
                 , public Responder
                 , public WindowManager
                 , public Connectable
{
    public:
        ScreenImpl(ApplicationImpl& app);
        
        virtual ~ScreenImpl();


        void setParent(Screen* screen);

        void setNextResponder(Responder* r);

        
        void addWindow(Window& w);

        void removeWindow(Window& w);

        const std::vector<Window*>& windows() const;

        
        const Gfx::SizeF& size() const;

        double scaleFactor() const;

        
        void repaint(const Gfx::RectF& rect);

        
        bool isEnabled() const;

    //
    // Responder
    //
    protected:
        virtual Responder* onNextResponder();

        virtual Gfx::PointF onToScreen(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromScreen(const Gfx::PointF& pos) const;

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    //
    // Visual
    //
    protected:
        virtual void onEvent(const Event& ev);

        virtual void onSetCapture(bool capture);

    //
    // WindowManager
    //
    protected:
        virtual WindowImpl* onCreateWindow(const WindowType& type);

        virtual void onAttach(Window& w);
    
        virtual void onDetach(Window& w);

        virtual void onInit(Window& w);

        virtual void onRelease(Window& w);

        virtual Gfx::PointF onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const;

        virtual Gfx::PointF onToScreen(const Window& w, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromScreen(const Window& w, 
                                         const Gfx::PointF& pos) const;

        virtual void onRepaint(Window& w, const Gfx::RectF& rect);

        virtual void onShow(Window& w, bool visible); 

        virtual void onActivate(Window& w, bool active);

        virtual void onEnable(Window& w, bool enable);

        virtual void onMove(Window& w, const Gfx::PointF& to);

        virtual void onResize(Window& w, const Gfx::SizeF& to);

        virtual void onFrameChanged(Window& w);

        virtual void onStateChanged(Window& w);

        virtual void onClosing(Window& w);

        virtual void onEnter(Window& w, Visual& v);

        virtual void onSetCapture(Window& w, bool capture);

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
        void onProcessEnableEvent(const EnableEvent& ev);

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
        Pt::Signal<const Pt::Event&> _eventReceived;

        Screen*                      _parent;
        std::vector<Window*>         _windows;

        Responder*                   _nextResponder;

        Gfx::SizeF                   _size;
        double                       _screenScaling;
        double                       _scaling;

        bool                         _enabled;
        bool                         _enabledState;
};

} // namespace

} // namespace

#endif; // include guard
