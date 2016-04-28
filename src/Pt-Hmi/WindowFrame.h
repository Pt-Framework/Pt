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
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>

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

        virtual void paintEvent(const PaintEvent& pev);

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

        void paintEvent(const PaintEvent& pev);
};


class MaximizeButton : public WindowButton
{
    public:
        MaximizeButton();

        ~MaximizeButton();

        void paintEvent(const PaintEvent& pev);
};


class CloseButton : public WindowButton
{
    public:
        CloseButton();

        ~CloseButton();

        void paintEvent(const PaintEvent& pev);
};


class MenuButton : public WindowButton
{
    public:
        MenuButton();

        ~MenuButton();

        void paintEvent(const PaintEvent& pev);
};


class WindowFrame : public Pt::Connectable
{
    public:
        WindowFrame();

        WindowFrame(WindowManager& wm, Window& window);

        virtual ~WindowFrame();

        Window* window();

        const Window* window() const;

        Gfx::RectF clientRect() const;

        Gfx::RectF frameRect() const;

        void update();

        void moveEvent(const MoveEvent& mev);

        void resizeEvent(const ResizeEvent& rev);

        void enterEvent(const EnterEvent& eev);

        void leaveEvent(const LeaveEvent& lev);

        bool mouseEvent(const MouseEvent& mev);

        void paintEvent(const PaintEvent& pev);

    protected:
        bool onMouseEvent(const MouseEvent& mev);

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

    private:
        WindowManager* _wm;
        Window*        _window;
        Gfx::RectF     _frameRect;
        Gfx::RectF     _clientRect;

        Gfx::PointF    _lastPointer;
        bool           _isClient;
        bool           _isMoving;
        bool           _isLeftResizing;
        bool           _isRightResizing;
        bool           _isTopResizing;
        bool           _isBottomResizing;

        MaximizeButton _maximizeButton;
        MinimizeButton _minimizeButton;  
        CloseButton    _closeButton;
        MenuButton     _menuButton;
};

} // namespace

} // namespace

#endif