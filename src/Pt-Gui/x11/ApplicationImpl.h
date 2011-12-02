/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                  *
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

// X11 defines these two globally, which conflicts with enum values in Pt/Char.h
#undef Above
#undef Below

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Application.h>
#include <Pt/Singleton.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Event.h>

#include "posix/SelectableImpl.h"

#include <map>

namespace Pt {

namespace Gui {

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

        public:
            Signal<const Pt::Event&> event;

        protected:
            X11EventLoop();

        public:
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

        protected:
            //! @brief Converts the X11 key symbol to a unicode character.
            wchar_t keysymToUtf(int sym);

        private:
            Display* _display;
            std::map<Window, Widget*> _widgets;
    };


class  X11Fd : public System::Selectable
             , private System::FdImpl
{
    public:
        X11Fd();

        ~X11Fd();

        void setFd(int fd)
        { System::FdImpl::setFd(fd); }
        
        // inherit doc
        virtual System::SelectableImpl& simpl()
        { return *this; }

        void flush()
        { this->onInput(); }
    
    protected:
        virtual void onInput();

        // inherit doc
        virtual void onClose()
        { System::FdImpl::closeFd(); }
        
        // inherit doc
        virtual bool onWait(std::size_t msecs)
        { return false; }
        
        // inherit doc
        virtual void onAttach(System::EventLoop& s)
        { System::FdImpl::attach(s); }

        // inherit doc
        virtual void onDetach(System::EventLoop& s)
        { System::FdImpl::detach(s); }

    private:
        XEvent _xev;
};


class AppImpl : public Pt::System::MainLoop
{
    public:
        AppImpl();

        ~AppImpl();

        System::EventLoop& loop()
        { return *this; }

    private:
        X11Fd _xfd;
};

} // namespace Gui

} // namespace Pt

#endif
