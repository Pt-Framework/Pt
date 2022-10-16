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
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

class ShellWM;

class PT_HMI_API Shell : public Widget
{
    friend class ShellWM;

    public:
        Shell();

        virtual ~Shell();

    public:
        void addWindow(Window& w);

        void removeWindow(Window& w);

        Widget* content();

        const Widget* content()  const;

        void setContent(Widget* widget);

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

    //
    // ShellWM
    //
    protected:
        Gfx::PointF onFromWM(const ShellWM& wm, const Gfx::PointF& pos) const;

        Gfx::PointF onToWM(const ShellWM& wm,  const Gfx::PointF& pos) const;

        Gfx::PointF onToScreen(const ShellWM& wm, const Gfx::PointF& pos) const;

        Gfx::PointF onFromScreen(const ShellWM& wm, const Gfx::PointF& pos) const;

        void onRepaint(ShellWM& wm, const Gfx::RectF& rect);

        void onActivate(ShellWM& wm, bool active);

        void onEnter(ShellWM& wm, Visual& v);

        bool onIsDescendantOf(const ShellWM& w, Visual& top) const;

        void onSetCapture(ShellWM& w, Visual& target, bool capture);

    //
    // Implementation
    //
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
        Widget*     _content;
        ShellWM*    _wm;  
};

} // namespace

} // namespace

#endif // include guard
