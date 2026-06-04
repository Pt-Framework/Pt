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

#ifndef Pt_Forms_wayland_ScreenImpl_h
#define Pt_Forms_wayland_ScreenImpl_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Screen.h>
#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/LayoutEvent.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Signal.h>

#include <vector>

namespace Pt {
namespace Forms {

class ApplicationImpl;
class WindowImpl;
class Window;
class MouseEvent;
class TouchEvent;
class ScrollEvent;

class ScreenImpl : public WindowManager
{
    typedef WindowManager Base;
    friend class Screen;
    friend class WindowImpl;

    public:
        ScreenImpl(ApplicationImpl& app);
        virtual ~ScreenImpl();

        void setParent(Screen* screen);
        void addWindow(Window& w);
        void removeWindow(Window& w);
        const std::vector<Window*>& windows() const;
        WindowManager& windowManager();
        void setCapture(Widget* capture);

        ApplicationImpl& app() { return _app; }

    protected:
        Gfx::PointF toFrame(const WindowImpl& w, const Gfx::PointF& pos) const;
        Gfx::PointF fromFrame(const WindowImpl& w, const Gfx::PointF& pos) const;
        void onAutoCenter(WindowFrame& w, const Gfx::SizeF* size);

    // Responder
    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);
        virtual bool onTouchEvent(const TouchEvent& ev);
        virtual bool onScrollEvent(const ScrollEvent& ev);
        virtual bool onKeyEvent(const KeyEvent& ev);

    // Widget
    protected:
        virtual Widget* onHitTest(const Gfx::PointF& pos);
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;
        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;
        virtual void onProcessEvent(const Event& ev);
        virtual void onRequestRepaint(const Gfx::RectF& rect);

    // WindowManager
    protected:
        virtual WindowFrame* onAttach(Window& w);
        virtual void onDetach(WindowFrame& w);
        virtual void onInit(WindowFrame& w);
        virtual void onRelease(WindowFrame& w);

    // scaling
    protected:
        void onProcessRescaleEvent(const RescaleEvent& ev);
        virtual void onRescaleEvent(const RescaleEvent& ev);

    // painting
    protected:
        virtual void onProcessPaintEvent(const PaintEvent& ev);
        virtual void onPaintEvent(const PaintEvent& ev);
        virtual void onPaint(const Gfx::RectF& rect);

    // enable
    protected:
        virtual void onProcessEnableEvent(const EnableEvent& ev);
        virtual void onEnableEvent(const EnableEvent& ev);
        virtual void onEnable(bool e);

    // input
    protected:
        void onProcessMouseEvent(const MouseEvent& ev);
        void onProcessTouchEvent(const TouchEvent& ev);
        void onProcessScrollEvent(const ScrollEvent& ev);
        void onProcessKeyEvent(const KeyEvent& ev);

    private:
        ApplicationImpl&        _app;
        Screen*                 _parent;
        std::vector<Window*>    _windows;
        double                  _screenScaling;
};

} // namespace Forms
} // namespace Pt

#endif
