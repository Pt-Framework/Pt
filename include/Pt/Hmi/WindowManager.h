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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef Pt_Hmi_WindowManager_h
#define Pt_Hmi_WindowManager_h

#include <Pt/Hmi/WindowBase.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Connectable.h>
#include <vector>

namespace Pt {

namespace Hmi {

class Window;
class Screen;
class WindowFrame;
class MouseEvent;
class TouchEvent;
class ScrollEvent;
class KeyEvent;
class EnterEvent;
class LeaveEvent;

class WindowManager : public Pt::Connectable
{
    public:
        WindowManager();

        virtual ~WindowManager();

        void init(WindowBase& parent);

        void add(Window& w);

        void remove(Window& window);

        void setScreen(Screen* screen);

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
                  
    public:
        void enterEvent(const EnterEvent& ev);

        void leaveEvent(const LeaveEvent& ev);

        bool keyEvent(const KeyEvent& keyEvent);
        
        bool mouseEvent(const MouseEvent& mev);

        bool touchEvent(const TouchEvent& tev);

        bool scrollEvent(const ScrollEvent& ev);

        void paint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);

    public:
        void onUpdate(Window& child, const Gfx::RectF& rect);

        void onResize(Window& w, const Gfx::SizeF& to);

        void onMove(Window& w, const Gfx::PointF& to);

        void onShow( Window& w, bool visible );

        void onActivate(Window* w);

        void onEnable(Window& w, bool enable);

        void onFrameChanged(Window& w);

        void onStateChanged(Window& w);

        void onClosing(Window& w);

        void onClose(Window& w);

        Gfx::PointF toParent(const Window& w, const Gfx::PointF& pos) const;

        Gfx::PointF fromParent(const Window& w, const Gfx::PointF& pos) const;

    private:
        WindowFrame* findWindow(const Gfx::PointF& p);

        WindowFrame* findWindow(const Window& w) const;

    private:
        WindowBase*               _parent; 
        std::vector<WindowFrame*> _windows;
        
        WindowFrame*              _activeWindow;
        WindowFrame*              _currentWindow;
        WindowFrame*              _grabbedWindow;
        WindowFrame*              _topMostWindow;

        double                    _borderWidth;
        double                    _titleHeight;
        Gfx::Color                _inactiveColor;
        Gfx::Color                _activeColor;
        Gfx::Color                _textColor;
        Gfx::Color                _inactiveTextColor;                        
};

} // namespace

} // namespace

#endif

