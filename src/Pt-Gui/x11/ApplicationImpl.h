/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Gui_x11_ApplicationImpl_h
#define Pt_Gui_x11_ApplicationImpl_h

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

// X11 defines these two globally, which conflicts with enum values in ptv/text/Char.h
#undef Above
#undef Below

#include <Pt/Api.h>
#include <Pt/Singleton.h>
#include <Pt/Signal.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Event.h>

#include <map>
#include <iostream>
#include <cerrno>


namespace Pt {

namespace Gui {

    class Application;
    class Widget;


    class X11EventLoop : public Pt::Singleton<X11EventLoop>
    {
        friend class Pt::Singleton<X11EventLoop>;

        public:
            Atom AtomAppWake;
            Atom AtomWindowResize;
            Atom AtomWindowMove;
            Atom AtomWindowClosed;
            Atom AtomWMProtocols;

        public:
            ~X11EventLoop();

            Display* display()
            { return _display; }

            void registerWidget(Window winId, Widget& widget);

            void unregisterWidget(Window winId);

            Widget* findWidget(Window winId);

            int run();

            void wake();

            void commitEvent(const Pt::Event& event);

            void queueEvent(const Pt::Event& event);

            void processX11Events();

            void processEvents();

            void exit();

        public:
            Signal<const Pt::Event&> event;

        protected:
            X11EventLoop();

            //! @brief Creates CloseEvents from X11 ClientMessage events
            void clientMessage(Widget& widget, XEvent& xev);

            //! @brief Creates a MouseEvent from an X11 ButtonPress
            void keyEvent(Widget& widget, XEvent& xev);

            //! @brief Creates a MouseEvent from an X11 MotionNotify
            void motionNotify(Widget& widget, XEvent& xev);

            //! @brief Creates a MouseEvent from an X11 ButtonPress
            void buttonPress(Widget& widget, XEvent& xev);

            //! @brief Creates a MouseEvent from an X11 ButtonRelease
            void buttonRelease(Widget& widget, XEvent& xev);

            //! @brief Creates PaintEvents from X11 Expose
            void expose(Widget& widget, XEvent& xev);

            //! @brief Creates PaintEvents from X11 Expose
            void noExpose(Widget& widget, XEvent& xev);

            //! @brief Creates a MouseEvent from an X11 ButtonRelease
            void configureNotify(Widget& widget, XEvent& xev);

            //! @brief Creates a MouseEvent from an X11 EnterNotify
            void enterNotify(Widget& widget, XEvent& xev);

            //! @brief Creates a MouseEvent from an X11 LeaveNotify
            void leaveNotify(Widget& widget, XEvent& xev);

            //! @brief Converts the X11 key symbol to a unicode character.
            wchar_t keysymToUtf(int sym);

        private:
            bool _stop;
            Display* _display;
            int _wakeFds[2];
            XEvent _xev;
            std::list<Pt::Event*> _eventQueue;
            System::Mutex _queueMutex;
            std::map<Window, Widget*> _widgets;
    };


    class PT_API ApplicationImpl : public Pt::Connectable
    {
        friend class Gui::Application;

        public:
            ApplicationImpl(Application& app);

            ~ApplicationImpl();

            void commitEvent(const Pt::Event& event);

            void queueEvent(const Pt::Event& event);

            void processEvents();

            int run();

            void wake();

            void exit();
    };

} // namespace Gui

} // namespace Pt

#endif
