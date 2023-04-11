/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015 Marc Boris Duerner
  
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

#ifndef Pt_Hmi_WindowFrame_h
#define Pt_Hmi_WindowFrame_h

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/PaintRegion.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Signal.h>
#include <vector>

namespace Pt {

namespace Hmi {

class Window;
class ShellWM;
class WindowFrame;
class Application;
class MouseEvent;
class KeyEvent;
class EnterEvent;
class LeaveEvent;
class PaintEvent;
class MoveEvent;
class ResizeEvent;

class WindowButton
{
    public:
        WindowButton();

        virtual ~WindowButton();

        WindowFrame* parent()
        { return _frame; }

        void setParent(WindowFrame& frame)
        { _frame  = &frame; }

        const Gfx::RectF&  geometry() const
        { return _geometry; }

        void setColor(const Gfx::Color& c)
        { _color = c; }

        const Gfx::Color& color() const
        { return _color; }

        Signal<>& clicked()
        { return _clicked; }

        void update();

        virtual void moveEvent(const MoveEvent& mev);

        virtual void resizeEvent(const ResizeEvent& rev);

        virtual void enterEvent(const EnterEvent& eev);

        virtual void leaveEvent(const LeaveEvent& lev);

        virtual void mouseEvent(const MouseEvent& mev);

        virtual void touchEvent(const TouchEvent& tev);

        virtual void paint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);

    private:
        Signal<>       _clicked;
        WindowFrame*   _frame;
        Gfx::RectF     _geometry;
        Gfx::Color     _color;
        bool           _isPressed;
};

class MinimizeButton : public WindowButton
{
    public:
        MinimizeButton();

        ~MinimizeButton();

        void paint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);
};


class MaximizeButton : public WindowButton
{
    public:
        MaximizeButton();

        ~MaximizeButton();

        void paint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);
};


class CloseButton : public WindowButton
{
    public:
        CloseButton();

        ~CloseButton();

        void paint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);
};


class MenuButton : public WindowButton
{
    public:
        MenuButton();

        ~MenuButton();

        void paint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);
};


class WindowFrame : public WindowImpl
{
    typedef WindowImpl Base;

    enum FrameItem
    {
        OnNone,
        OnTitle,
        OnButton,
        OnFrameLeft,
        OnFrameTopLeft,
        OnFrameTop,
        OnFrameTopRight,
        OnFrameRight,
        OnFrameBottomRight,
        OnFrameBottom,
        OnFrameBottomLeft
    };

    public:
        WindowFrame(ShellWM& wm, Window& window);

        virtual ~WindowFrame();

        Window* window();

        const Window* window() const;

        virtual Gfx::PointF clientPos() const;

        const Gfx::PointF& restorePosition() const;

        const Gfx::SizeF& restoreSize() const;
        
        void setRestore(const Gfx::PointF& pos, const Gfx::SizeF& size);

        const Gfx::RectF& clientRect() const;

        const Gfx::RectF& frameRect() const;

        void setFrame(double bw, double th);

        Gfx::PointF toWindow(const Gfx::PointF& pos) const;

        Gfx::PointF fromWindow(const Gfx::PointF& pos) const;

        void onProcessMouseEvent(const MouseEvent& mev);

        void onProcessTouchEvent(const TouchEvent& tev);

    public:
        virtual void onSetTitle(Window& w, const std::string& text);

        virtual void onSetIcon(Window& w, const Gfx::Image& icon);

        virtual void onSetState(Window& w, const WindowState& state);

        virtual void onRepaint(Window& w, const Gfx::RectF& rect);

        virtual void onMove(Window& w, const Gfx::PointF& to);

        virtual void onResize(Window& w, const Gfx::SizeF& s);

        virtual void onClose(Window& w);

    protected:
        virtual Visual* onHitTest(const Gfx::PointF& pos);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;
        
        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual void onRequestRepaint(const Gfx::RectF& rect);

        virtual void onRequestMove(const Gfx::PointF& pos);

        virtual void onRequestResize(const Gfx::SizeF& size);

        
        virtual void onProcessEvent(const Pt::Event& ev);

        
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);


        virtual void onProcessRescaleEvent(const RescaleEvent& ev);
        
        virtual void onRescaleEvent(const RescaleEvent& ev);


        virtual void onProcessShowEvent(const ShowEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev);

        
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);


        virtual void onProcessActivateEvent(const ActivateEvent& ev);

        virtual void onActivateEvent(const ActivateEvent& ev);


        virtual void onProcessMoveEvent(const MoveEvent& ev);

        virtual void onMoveEvent(const MoveEvent& ev);

        
        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

        
        virtual void onProcessWindowStateEvent(const WindowStateEvent& ev);

        virtual void onWindowStateEvent(const WindowStateEvent& ev);

        
        virtual void onProcessCloseEvent(const CloseEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

    protected:
        virtual bool onMouseEvent(const MouseEvent& mev);

        virtual bool onTouchEvent(const TouchEvent& tev);

        virtual bool onEnterEvent( const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

    protected:
        void onLayout();

        void onMenu();

        void onMinimize();

        void onMaximize();

        void onCloseButton();

    private:
        bool isTitle(const Gfx::PointF& p) const;

        bool isLeftBorder(const Pt::Gfx::PointF& p) const;
        
        bool isRightBorder(const Pt::Gfx::PointF& p) const;
        
        bool isTopBorder(const Pt::Gfx::PointF& p) const;
        
        bool isBottomBorder(const Pt::Gfx::PointF& p) const;

        Window* checkWindow(const Gfx::PointF& pos);

        WindowButton* checkButton(const Gfx::PointF& pos);

        //! @brief Returns true if window was grabbed for moving or resizing.
        bool checkMove(const Gfx::PointF& pos, bool isDrag, bool isPress);

        //! @brief Returns true if window was grabbed for moving or resizing.
        bool checkResize(const Gfx::PointF& pos, bool isDrag, bool isPress);

        void setCurrentFrameItem(FrameItem item);

    private:
        ShellWM*       _wm;
        Window*        _window;
        double         _borderWidth;
        double         _titleHeight;
        Gfx::RectF     _frameRect;
        Gfx::RectF     _frameBounds;
        Gfx::RectF     _clientRect;
        Gfx::RectF     _clientBounds;
        Gfx::PointF    _restorePos;
        Gfx::SizeF     _restoreSize;
        Window::State  _state;

        Gfx::PointF    _lastPointer;
        bool           _isClient;
        bool           _isMoving;
        bool           _isLeftResizing;
        bool           _isRightResizing;
        bool           _isTopResizing;
        bool           _isBottomResizing;

        FrameItem      _currentFrameItem;

        std::vector<WindowButton*> _buttons;

        MaximizeButton _maximizeButton;
        MinimizeButton _minimizeButton;  
        CloseButton    _closeButton;
        MenuButton     _menuButton;
};

} // namespace

} // namespace

#endif