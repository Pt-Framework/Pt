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
  02110-1301  USA
*/

#ifndef PT_HMI_SHELL_H
#define PT_HMI_SHELL_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

class WindowFrame;

class PT_HMI_API Shell : public Widget
                       , public WindowManager
{
    public:
        Shell();

        virtual ~Shell();

    public:
        void addWindow(Window& w);

        void removeWindow(Window& w);

    public:
        Widget* content();

        const Widget* content()  const;

        void setContent(Widget* widget);

    public:
        double borderWidth() const
        {
            return _borderWidth;
        }

        double titleHeight()  const
        {
            return _titleHeight;
        }

        const Gfx::Color& inactiveColor() const
        {
            return _inactiveColor;
        }

        const Gfx::Color& activeColor() const
        {
            return _activeColor;
        }

        const Gfx::Color& textColor() const
        {
            return _textColor;
        }

        const Gfx::Color& inactiveTextColor() const
        {
            return _inactiveTextColor;
        }

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
    // Widget
    //
    protected:
        virtual void onSetCapture(bool capture);

    protected:
        virtual void onRemoveWidget(Widget& w);

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onPaint(Gfx::PaintSurface&, const Gfx::RectF&);

    protected:
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onProcessPaintEvent(const PaintEvent& ev);
        
        virtual void onProcessEnableEvent(const EnableEvent& ev);

    protected:
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        virtual void onProcessEnterEvent(const EnterEvent& ev);

        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        virtual void onProcessKeyEvent(const KeyEvent& ev);

    private:
        WindowFrame* findWindowFrame(const Gfx::PointF& p) const;

        WindowFrame* getWindowFrame(const Window& w) const;

    private:
        Widget*                      _content;
        Visual*                      _pointer;
        Visual*                      _capture;

        std::vector<WindowFrame*>    _windows;

        WindowFrame*                 _activeWindow;
        WindowFrame*                 _grabbedFrame;
        WindowFrame*                 _topMostWindow;

        double                       _borderWidth;
        double                       _titleHeight;
        Gfx::Color                   _inactiveColor;
        Gfx::Color                   _activeColor;
        Gfx::Color                   _textColor;
        Gfx::Color                   _inactiveTextColor;   
};

} // namespace

} // namespace

#endif // include guard
