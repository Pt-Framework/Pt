/*
 * Copyright (C) 2006-2010 Marc Boris Duerner
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

#ifndef PT_SYSTEM_MAINLOOP_H
#define PT_SYSTEM_MAINLOOP_H

#include <Pt/System/Api.h>
#include <Pt/System/EventLoop.h>

namespace Pt {

namespace System {

/** @brief Thread-safe event loop supporting I/O multiplexing and Timers.

    An %MainLoop can be used to monitor a set of Selectables and Timers
    and react to activity on them. The wait call can be performed with
    a timeout and the respective timeout signal is sent if it occurs.
    Clients can be notified about Timer and Selectable activity by
    connecting to the appropriate signals of the Timer and Selectable
    classes.

    The following example uses a %MainLoop to wait on acitvity on
    a Timer, which is set to time-out after 1000 msecs.

    @code
    // slot to handle timer activity
    void onTimer();

    int main()
    {
        using Pt::System;

        Timer timer;
        timer.start(1000);
        connect(timer.timeout, ontimer);

        MainLoop loop;
        loop.addTimer(timer);
        loop.run();

        return 0;
    }
    @endcode

    The MainLoop can be used as the central entity of a thread or
    process to dispatch application events and wait on multiple IODevices or
    Timers for activity.

    Events can be added to the internal event queue, even from other threads
    using the method MainLoop::commitEvent or MainLoop::queueEvent. The
    first method will add the event to the internal queue and wake the
    event loop, the latter allows queing multiple events and it is up to
    the caller to wake the event loop by calling MainLoop::wake when all
    events are added. When the event loop processes its event, the signal
    "event" is send for each processed event. Events are processes in the
    order they were added.

    To start the %MainLoop the method MainLoop::run must be executed. It blocks
    until the event loop is stopped. To stop the %MainLoop, MainLoop::exit
    must be called. The delivery of the events occurs inside the thread that
    started the execution of the event loop.

    %IODevices and %Timers can be added to an %MainLoop just as to Selector.
    In fact a %Selector is used internally to implement the %MainLoop.

    Since the %MainLoop is a Runnable, it can be easily assigned to a Thread
    to give it its own event loop.
*/   
class PT_SYSTEM_API MainLoop : public EventLoop
{
    public:
        /** @brief Constructs the MainLoop
        */
        MainLoop();

        MainLoop(Allocator& a);

        /** @brief Destructs the MainLoop
          */
        virtual ~MainLoop();

        //! @internal
        Selector& selector();

        //! @internal
        bool waitNext();

    protected:
        virtual void onAttachSelectable(Selectable&);

        virtual void onDetachSelectable(Selectable&);

        virtual void onCancel(Selectable& s);

        virtual void onReady(Selectable& s);

        virtual void onRun();

        virtual void onExit();
    
        virtual void onCommitEvent(const Event& ev);
    
        virtual void onQueueEvent(const Event& ev);
    
        virtual void onWake();
    
        virtual void onAttachTimer(Timer& timer);
    
        virtual void onDetachTimer(Timer& timer);

    private:
        class MainLoopImpl* _impl;
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_MAINLOOP_H
