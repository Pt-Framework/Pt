/* Copyright (C) 2022 Marc Boris Duerner 
  
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

#ifndef PT_HMI_SHELL_WM_H
#define PT_HMI_SHELL_WM_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

class WindowFrame;

class Shell;

class ShellWM : public WindowManager
{
    typedef WindowManager Base;

    public:
        ShellWM();

        virtual ~ShellWM();

        void setParent(Shell* shell);

        Gfx::PaintSurface& surface();

        const Gfx::PaintSurface& surface() const;

        void setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos);

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
    // Visual
    //
    protected:
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        Visual* onHitTest(const Gfx::PointF& p);

        WindowFrame* onHitTestFrame(const Gfx::PointF& p);


        virtual void onRequestRepaint(const Gfx::RectF& rect);

        virtual void onRequestActivate(bool active);
        
        virtual void onRequestCapture(bool capture);

        
        virtual void onProcessEvent(const Pt::Event& ev);

    //
    // WindowManager
    //
    protected:
        virtual WindowImpl* onAttach(Window& w);

        virtual void onDetach(Window& w);

        virtual void onInit(Window& w);

        virtual void onRelease(Window& w);

        virtual Gfx::PointF onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const;

        virtual void onRepaint(Window& w, const Gfx::RectF& rect);

        virtual void onShow(Window& w, bool visible); 

        virtual void onActivate(Window& w, bool active); 

        virtual void onEnableRequest(Window& w, bool enable);

        virtual void onMove(Window& w, const Gfx::PointF& to);

        virtual void onResize(Window& w, const Gfx::SizeF& to);

        virtual void onSetAbove(Window& w, bool above);

        virtual void onSetTitle(Window& w, const std::string& text);

        virtual void onSetIcon(Window& w, const Gfx::Image& icon);

        virtual void onSetState(Window& w, const WindowState& state);
        
        virtual void onSetSizeLimits(Window& w, const Gfx::SizeF& minSize, 
                                                const Gfx::SizeF& maxSize); 

        virtual void onClosing(Window& w);

    //
    // Implementation
    //
    protected:
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onProcessPaintEvent(const PaintEvent& ev);
        
        virtual void onProcessEnableEvent(const EnableEvent& ev); 
        
        virtual void onProcessMouseEvent(const MouseEvent& ev);    

        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        virtual void onProcessEnterEvent(const EnterEvent& ev);

        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        virtual void onProcessKeyEvent(const KeyEvent& ev);

    private:
        Shell*                       _parent;

        Gfx::PaintRegion             _surface;

        std::vector<Window*>         _windowList;

        Window*                      _activeWindow;
        WindowFrame*                 _grabbedFrame;
        Window*                      _topMostWindow;

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
