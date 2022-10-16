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
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>
#include <vector>

namespace Pt {

namespace Hmi {

class Window;
class Shell;
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


class WindowFrame : public Visual
                  , public Pt::Connectable
{
    public:
        WindowFrame(Shell& wm, Window& window);

        virtual ~WindowFrame();

        Window* window();

        const Window* window() const;

        Window::State state() const;

        void setState(Window::State state);

        const Gfx::PointF& restorePosition() const;

        const Gfx::SizeF& restoreSize() const;
        
        void setRestore(const Gfx::PointF& pos, const Gfx::SizeF& size);

        const Gfx::PointF& position() const;

        const Gfx::SizeF& size() const;

        const Gfx::RectF& clientRect() const;

        const Gfx::RectF& frameRect() const;

        void setFrame(double bw, double th);

        Gfx::PointF toFrame(const Gfx::PointF& pos) const;

        Gfx::PointF fromFrame(const Gfx::PointF& pos) const;

        Gfx::SizeF fromFrame(const Gfx::SizeF& size) const;

        void repaint();

        void repaint(const Gfx::RectF& rect);

        void moveEvent(const Gfx::PointF& pos);

        void resizeEvent(const Gfx::SizeF& size);

        void enterEvent(const EnterEvent& eev);

        void leaveEvent(const LeaveEvent& lev);

        //! @brief Returns true if window was grabbed for moving or resizing.
        bool mouseEvent(const MouseEvent& mev);

        //! @brief Returns true if window was grabbed for moving or resizing.
        bool touchEvent(const TouchEvent& tev);

        void paint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);

    protected:
        virtual void onEvent(const Pt::Event& ev);

        virtual void onSetCapture(bool capture);

    protected:
        bool onMouseEvent(const MouseEvent& mev);

        bool onTouchEvent(const TouchEvent& tev);

        void onLayout();

        void onMenu();

        void onMinimize();

        void onMaximize();

        void onClose();

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

    private:
        Shell*       _wm;
        Window*        _window;
        double         _borderWidth;
        double         _titleHeight;
        Gfx::RectF     _frameRect;
        Gfx::RectF     _clientRect;
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

        std::vector<WindowButton*> _buttons;

        MaximizeButton _maximizeButton;
        MinimizeButton _minimizeButton;  
        CloseButton    _closeButton;
        MenuButton     _menuButton;
};

} // namespace

} // namespace

#endif