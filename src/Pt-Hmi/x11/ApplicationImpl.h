 /* Copyright (C) 2015 Marc Boris Duerner

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
  MA  02110-1301  USA
*/

#ifndef Pt_Hmi_x11_ApplicationImpl_h
#define Pt_Hmi_x11_ApplicationImpl_h

#include "X11Fd.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/MouseEvent.h>

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Timespan.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl : public Pt::System::MainLoop
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();

        void setCursor(const Cursor* cursor);
        
        void setFontDir(const Pt::System::Path& dir);

        void setDefaultFont(const std::string& fname);

        Pt::Timespan inactivityTime() const;

        void sendKeyEvent(const KeyEvent& ev);

        void sendMouseEvent(const MouseEvent& ev);

        void nextEvent();

    public:
        void processEvent(XEvent& xev)
        {
            onProcessXEvent(xev);
        }

        ::Display* display() const
        {
            return _display;
        }

        ::Visual* visual() const
        {
            return _visual;
        }

        int depth() const
        {
            return _depth;
        }

        Atom wmProtocols() const
        { return _wmProtocols; }

        Atom wmDeleteWindow() const
        { return _wmDeleteWindow; }

        Atom wmChangeState() const
        { return _wmChangeState; }

        Atom netWmState() const
        { return _netWmState; }

        Atom netWmStateMaximizedVert() const
        { return _netWmStateMaximizedVert; }

        Atom netWmStateMaximizedHorz() const
        { return _netWmStateMaximizedHorz; }

        Atom netWmStateHidden() const
        { return _netWmStateHidden; }

        Atom netWmStateAbove() const
        { return _netWmStateAbove; }

        Atom netWmActiveWindow() const
        { return _netWmActiveWindow; }

    protected:
        virtual void onRun();

    private:
        Window* findWindow(::Window w);

        void onProcessXEvent(XEvent& ev);

        void onEnterNotify(Window& window, XEvent& xev);

        void onLeaveNotify(Window& window, XEvent& xev);

        void onExpose(Window& window, XEvent& xev);

        void onClientMessage(Window& window, XEvent& xev);

        void onShow(Window& w, bool v);

        void onMotionNotify(Window& window, XEvent& xev);

        void onButtonPress(Window& window, XEvent& xev);

        void onButtonRelease(Window& window, XEvent& xev);

        void onKeyEvent(Window& window, XEvent& xev);

        void onConfigureNotify(Window& window, XEvent& xev);

    private:
        ::Display* _display;
        ::Visual*  _visual;
        int        _depth;
        X11Fd      _xfd;
        Atom       _wmProtocols;
        Atom       _wmDeleteWindow;
        Atom       _wmChangeState;
        Atom       _netWmState;
        Atom       _netWmStateMaximizedVert;
        Atom       _netWmStateMaximizedHorz;
        Atom       _netWmStateHidden;
        Atom       _netWmStateAbove;
        Atom       _netWmActiveWindow;

        GC         _paintGc;
        MouseEvent _mouseEvent;
        KeyEvent   _keyEvent;
};

} // namespace

} // namespace

#endif // include guard
