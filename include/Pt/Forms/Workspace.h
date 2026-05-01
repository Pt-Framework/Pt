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

#ifndef PT_FORMS_WORKSPACE_H
#define PT_FORMS_WORKSPACE_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/WorkspaceManager.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API Workspace : public Control
{
    friend class WorkspaceManager;

    typedef Control Base;

    public:
        Workspace();

        virtual ~Workspace();

    public:
        void addWindow(Window& w);

        void removeWindow(Window& w);

        const std::vector<Window*>& windows() const;

        WindowManager& windowManager();

    public:
        Control* content();

        const Control* content()  const;

        void setContent(Control* control);

    //
    // Widget
    //
    protected:
        virtual void onConnect(Screen& screen);

        virtual void onDisconnect();
      
        
        virtual Widget* onHitTest(const Gfx::PointF& p);

        virtual void onRequestResize(const Gfx::SizeF& s);

    //
    // Control
    //
    protected:
        virtual void onSetSurface(PaintSurface* surface, const Gfx::PointF& pos);

        virtual void onRemoveControl(Control& control);

        virtual Gfx::SizeF onMeasure(PaintContext& ctx, const SizePolicy& policy);

        virtual void onLayout(PaintContext& ctx, const Gfx::RectF& rect);

        virtual void onPaint(PaintContext&, const Gfx::RectF&);

    protected:
        virtual void onProcessLayoutEvent(const LayoutEvent& ev);

        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        virtual void onProcessPaintEvent(const PaintEvent& ev);
        
        virtual void onProcessEnableEvent(const EnableEvent& ev);

    protected:
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        virtual void onProcessEnterEvent(const EnterEvent& ev);

        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        virtual void onProcessKeyEvent(const KeyEvent& ev);

    //
    // WindowManager
    //
    protected:
        void onRelayoutRequest(WorkspaceManager& wm);

        virtual void onRepaint(WindowManager& wm, const Gfx::RectF& rect);

        virtual void onActivate(WindowManager& wm, bool active);

    private:
        WorkspaceManager  _wm;
        Control*          _content;
        Widget*           _pointer;
        Gfx::SizeF        _requestedSize;
};

} // namespace

} // namespace

#endif // include guard
