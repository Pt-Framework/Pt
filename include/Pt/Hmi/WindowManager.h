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

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Connectable.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/MoveEvent.h>

namespace Pt {

namespace Hmi {

class Window;
class WindowManager;
class Application;
class MouseEvent;
class KeyEvent;
class WindowFrame;

class WindowManager : public Pt::Connectable
{
    public:
        WindowManager();

        virtual ~WindowManager();

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

        void init(Window& parent);

        void add(Window& w);

        void remove(Window& window);

        WindowFrame* findWindow(const Gfx::PointF& p);

        WindowFrame* findWindow(Window& w);

        PaintSurface& surface();
                  
    public:
        bool keyInput(const Pt::Hmi::KeyEvent& keyEvent);
        
        bool pointerInput(const MouseEvent& mev);

        void paintEvent(const PaintEvent& ev);

    public:
        void onResize(Window& w, const Gfx::SizeF& to);

        void onMove(Window& w, const Gfx::PointF& to);

        void onUpdate(Window& child, const Gfx::RectF& rect);

        void onShow( Window& w, bool visible );

        void onActivate(Window& w);

        void onEnable(Window& w, bool enable);

        void onClosing(Window& w);
    
    private:        
        bool onBackground(const Pt::Hmi::MouseEvent& pev);

        bool onWindowFrame(const Pt::Hmi::MouseEvent& pev);

        bool onWindowContent(const Pt::Hmi::MouseEvent& pev);

        bool onWindowMove(const Pt::Hmi::MouseEvent& pev);

        bool onWindowResize(const MouseEvent& pev);    

    private:
        Application&             _app;
        std::vector<WindowFrame> _windows;

        double _borderWidth;
        double _titleHeight;

        typedef bool (WindowManager::*State)(const Pt::Hmi::MouseEvent&);
        State                     _state;
        WindowFrame*              _managedWindow;
        Gfx::PointF               _managedWindowPosition;
        Gfx::SizeF                _managedWindowSize;
        Window*                   _parent;          
        Pt::uint8_t               _resizeDirection;

        Gfx::Color                _inactiveColor;
        Gfx::Color                _activeColor;
        Gfx::Color                _textColor;           
        Gfx::PointF               _lastPointerPosition;                          
};

} // namespace

} // namespace

#endif

