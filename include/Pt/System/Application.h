/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
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
#ifndef PT_SYSTEM_APPLICATION_H
#define PT_SYSTEM_APPLICATION_H

#include <Pt/System/Api.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Application.h>
#include <Pt/Connectable.h>
#include <Pt/Event.h>
#include <Pt/Signal.h>

namespace Pt {

namespace System {

    class ApplicationImpl;

    /**
     * \brief The Application class provides an event loop for console applications
     * without a GUI.
     *
     * This class is used by non-GUI applications to provide the applications's event
     * loop. There should be only exactly one instance of Application (or one of its
     * subclasses) per application. This is not ensured, though.
     *
     * Application contains the main event loop, where event sources can be registered
     * and events from those sources are dispatched to listeners, that were registered
     * to the event loop. Events may for example be operating system events (timer, file
     * system changes).
     *
     * The application and therefore the event loop is started with a call to run() and
     * can be exited with a call to exit(). After calling exit() the application should
     * terminate.
     *
     * The event loop can be access by calling eventLoop(). Events can be committed by
     * calling EventLoop::commitEvent(). Long running operations can call
     * EventLoop::processEvents() to keep the application responsive.
     *
     * There are convenience methods available for easier access to functionality of
     * the underlying event loop. commitEvent() delegates to EventLoop::commitEvent(),
     * queueEvent() delegates to EventLoop::delegateEvent() and processEvents() delegates
     * to EventLoop::processEvents() without making it necessary to first obtain the
     * event loop manually.
     */
    class PT_SYSTEM_API Application : public Pt::Application
    {
        public:
            explicit Application(int argc = 0, char** argv = 0);

            Application(EventLoopBase* loop, int argc = 0, char** argv = 0);

            ~Application();

            static Application& instance();

            EventLoopBase& loop();

            void run();

            void exit();

            bool catchSystemSignal(int sig);

            bool raiseSystemSignal(int sig);

            Signal<int> systemSignal;

            Signal<> aboutToStart;

            Signal<> aboutToExit;

            ApplicationImpl& impl();

        protected:
            void init(EventLoopBase& loop);

        private:
            ApplicationImpl* _impl;
            int     _argc;
            char**  _argv;
            EventLoopBase* _loop;
            EventLoop* _owner;
    };

} // namespace system

} // namespace ptv

#endif
