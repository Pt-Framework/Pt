/*
 * Copyright (C) 2006 Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_Gui_Application_h
#define Pt_Gui_Application_h

#include <Pt/Gui/Api.h>
#include <Pt/System/Application.h>
#include <Pt/Arg.h>
#include <Pt/Connectable.h>
#include <Pt/Event.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Gui {

class MainLoopImpl;

class PT_GUI_API MainLoop : public Pt::System::EventLoop
{
    public:
        MainLoop();

        MainLoop(Allocator& a);

        virtual ~MainLoop();

    protected:
        virtual void onAttach(System::Selectable&);

        virtual void onDetach(System::Selectable&);

        virtual void onEnable( System::Selectable& s );

        virtual void onDisable( System::Selectable& s );

        virtual void onReinit(System::Selectable& s);

        virtual void onChanged(System::Selectable& s);

    private:
        MainLoopImpl* _impl;
};

/**
 * \brief %Application with a GUI event-loop.
 *
 * This interface provides methods for running and stopping the application, for
 * adding and processing of events and a signal (event) to which slots can be connected to
 * listen for events that are sent to the event queue.
 *
 * A class that implements this interface may contain a main event loop, where event
 * sources can be registered and events from those sources are dispatched to listeners,
 * that were registered to the event loop. Events may for example be operating system
 * events (timer, file system changes) or gui-specific events (like repaint, mouse events).
 *
 * The application and therefore the event loop is started with a call to run() and
 * can be exited with a call to exit(). After calling exit() the application should
 * terminate.
 *
 * Events can be committed by calling commitEvent(). Long running operations
 * can call processEvents() to keep the application responsive.
 */ 
class PT_GUI_API Application : public Pt::System::Application
{
    public:
        Application(int argc = 0, char** argv = 0);

        ~Application();

        static Application& instance();

        MainLoop& loop();

        Signal<const Pt::Event&>& event();

    protected:
        /**
         * @brief Receives GUI events for widgets and delivers them to the widget.
         *
         * General events which are sent to the 'event' signal of ptv::Application are
         * passed to this method. If the event is a GUI event, it is sent to the method
         * Widget::event(). From there it may be dispatched to more specific event handling
         * methods.
         * If the event is not a GUI event, it is ignored.
         *
         * @param event An event that will be dispatched to the corresponding widget.
         */
        void dispatchGuiEvent(const Pt::Event& ev);

    private:     
        class AppImpl* _appImpl; 
        //MainLoop _loop;
};

} // namespace gui

} // namespace ptv

#endif
