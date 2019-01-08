/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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

#ifndef Pt_Hmi_ViewImpl_H
#define Pt_Hmi_ViewImpl_H

#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Window.h>
#include <Pt/System/Timer.h>
#include <map>

#ifdef __OBJC__
    #import <AppKit/NSWindow.h>
    #import <AppKit/NSGraphicsContext.h>
    #import <AppKit/NSView.h>
    #import <AppKit/NSScreen.h> 
#else
	struct NSRect;
	struct NSView;
  struct NSWindow;
	struct NSResponder;
	struct NSGraphicsContext;
#endif

namespace Pt {

namespace Hmi {

class MainWindowImpl : public Pt::Connectable
{
    public:
        MainWindowImpl(Window::Type type);

        virtual ~MainWindowImpl();

        void setId(Window& w);

        void setType(Window::Type type)
        {}

        Gfx::PointF toScreen(const Gfx::PointF& pos) const;

        Gfx::PointF fromScreen(const Gfx::PointF& pos) const;

        void show(bool v);

        void close();

        void paint(const Gfx::RectF& rect);

        void activate();

        void enable(bool e);

        void setTopMost(bool e);

        void move(const Gfx::PointF& p);

        void resize(const Gfx::SizeF& size);

        void setIcon(const Gfx::Image& icon);

        void setTitle(const std::string& text);

        void setMinimumSize(const Gfx::SizeF& s);

        void setMaximumSize(const Gfx::SizeF& s);

        void setState(Window::State p);

        void grabPointer();

        NSView* view()
        { 
            return _view;
        }

        NSWindow* window()
        {
            return _window;
        }

    public:
        void onSize();
        void onPosition();
        void onClosing();
        void onMouseMove(double x,double y);
        void onLMouseDown(double x, double y);
        void onLMouseUp(double x, double y);
        void onKeyDown(int key);
        void onKeyUp(int key);
        void onSpezialKeyEvent(unsigned int mask);
        void onLostFocus();
    
    private:
        Pt::Gfx::PointF convertMousePosition(double x, double y);

    private:
        NSWindow*                _window;
        NSView*                  _view;
        int                      _windowStyle;
        Pt::uint64_t             _id;

        KeyEvent                 _keyEvent;
        MouseEvent               _mouseEvent;
        Pt::System::Timer        _timer;

        int                      _level;
        std::string              _title;
        bool                     _topMost;
};

} // namespace

} // namespace

#endif // include guard
