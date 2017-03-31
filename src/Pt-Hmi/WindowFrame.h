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

#include <Pt/Hmi/PaintSurface.h>
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
class WindowManager;
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

        const Gfx::Rect&  geometry() const
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

        virtual void paint(PaintSurface& surface, const Gfx::Rect& rect);

    private:
        Signal<>       _clicked;
        WindowFrame*   _frame;
        Gfx::Rect     _geometry;
        Gfx::Color     _color;
        bool           _isPressed;
};

class MinimizeButton : public WindowButton
{
    public:
        MinimizeButton();

        ~MinimizeButton();

        void paint(PaintSurface& surface, const Gfx::Rect& rect);
};


class MaximizeButton : public WindowButton
{
    public:
        MaximizeButton();

        ~MaximizeButton();

        void paint(PaintSurface& surface, const Gfx::Rect& rect);
};


class CloseButton : public WindowButton
{
    public:
        CloseButton();

        ~CloseButton();

        void paint(PaintSurface& surface, const Gfx::Rect& rect);
};


class MenuButton : public WindowButton
{
    public:
        MenuButton();

        ~MenuButton();

        void paint(PaintSurface& surface, const Gfx::Rect& rect);
};


class WindowFrame : public Pt::Connectable
{
    public:
        WindowFrame(WindowManager& wm, Window& window);

        virtual ~WindowFrame();

        Window* window();

        const Window* window() const;

        Window::State state() const;

        void setState(Window::State state);

        const Gfx::Point& restorePosition() const;

        const Gfx::Size& restoreSize() const;
        
        void setRestore(const Gfx::Point& pos, const Gfx::Size& size);

        const Gfx::Rect& clientRect() const;

        const Gfx::Rect& frameRect() const;

        void setFrame(Pt::ssize_t bw, Pt::ssize_t th);

        Gfx::Point toFrame(const Gfx::Point& pos) const;

        Gfx::Point fromFrame(const Gfx::Point& pos) const;

        Gfx::Size fromFrame(const Gfx::Size& size) const;

        void update();

        void update(const Gfx::Rect& rect);

        void moveEvent(const MoveEvent& mev);

        void resizeEvent(const ResizeEvent& rev);

        void enterEvent(const EnterEvent& eev);

        void leaveEvent(const LeaveEvent& lev);

        //! @brief Returns true if window was grabbed for moving or resizing.
        bool mouseEvent(const MouseEvent& mev);

        //! @brief Returns true if window was grabbed for moving or resizing.
        bool touchEvent(const TouchEvent& tev);

        void paint(PaintSurface& surface, const Gfx::Rect& rect);

    protected:
        bool onMouseEvent(const MouseEvent& mev);

        bool onTouchEvent(const TouchEvent& tev);

        void onLayout();

        void onMenu();

        void onMinimize();

        void onMaximize();

        void onClose();

    private:
        bool isTitle(const Gfx::Point& p) const;

        bool isLeftBorder(const Pt::Gfx::Point& p) const;
        
        bool isRightBorder(const Pt::Gfx::Point& p) const;
        
        bool isTopBorder(const Pt::Gfx::Point& p) const;
        
        bool isBottomBorder(const Pt::Gfx::Point& p) const;

        Window* checkWindow(const Gfx::Point& pos);

        WindowButton* checkButton(const Gfx::Point& pos);

        //! @brief Returns true if window was grabbed for moving or resizing.
        bool checkMove(const Gfx::Point& pos, bool isDrag, bool isPress);

        //! @brief Returns true if window was grabbed for moving or resizing.
        bool checkResize(const Gfx::Point& pos, bool isDrag, bool isPress);

    private:
        WindowManager* _wm;
        Window*        _window;
        Pt::ssize_t   _borderWidth;
        Pt::ssize_t   _titleHeight;
        Gfx::Rect     _frameRect;
        Gfx::Rect     _clientRect;
        Gfx::Point    _restorePos;
        Gfx::Size     _restoreSize;
        Window::State  _state;

        Gfx::Point    _lastPointer;
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