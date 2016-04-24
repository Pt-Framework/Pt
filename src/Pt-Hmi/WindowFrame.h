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

#ifndef Pt_Hmi_WindowFrame_h
#define Pt_Hmi_WindowFrame_h

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

class WindowFrame
{
    public:
        enum ResizeDirection
        {		    
            None  = 0,
		    North = 1,		 
		    East  = 1<<1,
		    South = 1<<2,
		    West  = 1<<3,		 
        };	

    public:
        WindowFrame();

        WindowFrame(WindowManager& wm, Window& window);

        virtual ~WindowFrame();

        Window* window();

        const Window* window() const;

        bool isTitle(const Gfx::PointF& p) const;

        bool isBorder(const Gfx::PointF& p) const;

        Pt::uint8_t isResize(const Gfx::PointF& p) const;

        Gfx::RectF clientRect() const;

        Gfx::RectF frameRect() const;

        void update();

        void moveEvent(const MoveEvent& mev);

        void resizeEvent(const ResizeEvent& rev);

        bool mouseEvent(const MouseEvent& mev);

        void paintEvent(const PaintEvent& pev);
    
    protected:
        void onLayout();

    private:
        WindowManager* _wm;
        Window*        _window;
        Gfx::RectF     _frameRect;
        Gfx::RectF     _clientRect;
        Gfx::RectF     _closeButton;
        Gfx::RectF     _maximizeButton;
        Gfx::RectF     _minimizeButton;  
};

}}

#endif