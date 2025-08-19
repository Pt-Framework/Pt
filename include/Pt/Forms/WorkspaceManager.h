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

#ifndef PT_FORMS_WORKSPACE_MANAGER_H
#define PT_FORMS_WORKSPACE_MANAGER_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

#include <vector>
#include <map>

namespace Pt {

namespace Forms {

class WorkspaceFrame;

class Workspace;

class WorkspaceManager : public WindowManager
{
    typedef WindowManager Base;

    friend class Workspace;
    friend class WorkspaceFrame;

    public:
        WorkspaceManager();

        virtual ~WorkspaceManager();

        void setParent(Workspace* workspace);

        //Gfx::PaintSurface& surface();

        //const Gfx::PaintSurface& surface() const;

        //void setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos);

        bool processMouseEvent(const MouseEvent& ev);

        bool processTouchEvent(const TouchEvent& ev);
    
    public:
        Window* activeWindow();

        const std::vector<Window*>& windows() const;

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
    // layouting
    //
    protected:      
        virtual void onProcessLayoutEvent(const LayoutEvent& ev);

        virtual void onLayoutEvent(const LayoutEvent& ev);

        virtual void onLayout(const Gfx::RectF& rect);

    //
    // WindowManager
    //
    protected:
        virtual void onRequestRelayout();

    protected:
        virtual WindowFrame* onAttach(Window& w);

        virtual void onDetach(WindowFrame& w);

        virtual void onInit(WindowFrame& w);

        virtual void onRelease(WindowFrame& w);

    protected:
        Gfx::PointF toFrame(const WorkspaceFrame& w, 
                            const Gfx::PointF& pos) const;

        Gfx::PointF fromFrame(const WorkspaceFrame& w, 
                              const Gfx::PointF& pos) const;

        virtual void onSetAbove(WorkspaceFrame& w, bool above);

        virtual void onSetSizeLimits(WorkspaceFrame& w, 
                                     const Gfx::SizeF& minSize, 
                                     const Gfx::SizeF& maxSize);

        virtual void onAutoCenter(WindowFrame& w, const Gfx::SizeF* size);

        virtual void onShow(WorkspaceFrame& w, bool visible);

        virtual void onActivate(WorkspaceFrame& w, bool active); 

        virtual void onEnable(WorkspaceFrame& w, bool enable);

        virtual void onMove(WorkspaceFrame& w, const Gfx::PointF& to);

        virtual void onResize(WorkspaceFrame& w, const Gfx::SizeF& to);

        virtual void onClose(WorkspaceFrame& w);

    //
    // Widget
    //
    protected:
        virtual void onConnect(Screen& screen);

        virtual void onDisconnect();

        
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual Widget* onHitTest(const Gfx::PointF& p);

        
        virtual void onRequestRepaint(const Gfx::RectF& rect);

        virtual void onRequestActivate(bool active);
        
        virtual void onRequestCapture(bool capture);

    protected:
        virtual void onProcessEvent(const Pt::Event& ev);

        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onProcessPaintEvent(const PaintEvent& ev);
        
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);
        
        virtual void onProcessMouseEvent(const MouseEvent& ev);    

        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        virtual void onProcessEnterEvent(const EnterEvent& ev);

        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        virtual void onProcessKeyEvent(const KeyEvent& ev);

    private:
        Workspace*                     _parent;

        std::vector<Window*>           _windows;
        std::vector<Window*>           _windowStack;

        Window*                        _activeWindow;
        Window*                        _topMostWindow;
        std::map<Window*, Gfx::RectF>  _autoCenter;

        double                       _borderWidth;
        double                       _titleHeight;
        Gfx::Color                   _activeColor;
        Gfx::Color                   _inactiveColor;
        Gfx::Color                   _textColor;
        Gfx::Color                   _inactiveTextColor;  
};

} // namespace

} // namespace

#endif // include guard
