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
#include <Pt/Hmi/ResizeDirection.h>
#include <Pt/Hmi/DeviceButton.h>

namespace Pt {

namespace Hmi {

class Window;
class MouseEvent;
class KeyEvent;
class Application;
class Screen;

class WindowManager : public Pt::Connectable
{
    public:
        WindowManager(Window* parent = 0);

        virtual ~WindowManager();

        void init(Window& parent);

        void add(Window& w);

        void remove(Window& window);

        // TODO: WindowIterator
        const std::vector<Window*>& windows() const;

        std::vector<Window*>& windows();

        // TODO: Window::activate() should be public
        void activate(Window& w);               

        bool pointerInput(const Pt::Hmi::MouseEvent& pointerEvent);

        bool keyInput(const Pt::Hmi::KeyEvent& keyEvent);

        void render(PaintSurface& surface, const Gfx::RectF& updateRect);

        double borderWidth() const;

        double titleHeight() const;

      void onResize(Window& w, const Gfx::SizeF& to);

    private:
        bool onBackground(const Pt::Hmi::MouseEvent& pev);

        bool onWindowFrame(const Pt::Hmi::MouseEvent& pev);

        bool onWindowContent(const Pt::Hmi::MouseEvent& pev);

        bool onWindowMove(const Pt::Hmi::MouseEvent& pev);

        bool onWindowResize(const MouseEvent& pev);

    private:
        // TODO: checkBorder, checkTitle
        ResizeDirection::Type isSizing(const Window& w, const Pt::Hmi::MouseEvent& ev);

        bool isMoving(const Window& w, const Pt::Hmi::MouseEvent& ev);

    private:
        bool updateActive(const Pt::Hmi::MouseEvent& mouseEvent);

        bool contains(const Window& w, double x, double y);

        Window* findWindow(double x, double y);

        void setSizingCursor( ResizeDirection::Type type );
 
        Window* activeWindow(WindowManager& manager); 

        Gfx::PointF renderFrame(const Window& w, PaintSurface& surface);

        MouseEvent toWindow(Window* w, const MouseEvent& pev);        

    private:
        Application&              _app;
        std::vector<Window*>      _children;

        typedef bool (WindowManager::*State)(const Pt::Hmi::MouseEvent&);
        State                     _state;
        Window*                   _managedWindow;
        Gfx::PointF               _managedWindowPosition;
        Gfx::SizeF                _managedWindowSize;
        Window*                   _container;          
        ResizeDirection::Type     _sizingDirection;
        
        Gfx::Color                _inactiveColor;
        Gfx::Color                _activeColor;
        Gfx::Color                _textColor;        
        size_t                    _actionButton;  
        Gfx::PointF               _lastPointerPosition;        
};

} // namespace

} // namespace

#endif

