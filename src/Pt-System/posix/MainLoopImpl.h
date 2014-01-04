/*
 * Copyright (C) 2006-2012 Marc Boris Duerner
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
#ifndef PT_SYSTEM_POSIX_MAINLOOPIMPL_H
#define PT_SYSTEM_POSIX_MAINLOOPIMPL_H

#include "Selector.h"
#include "Pt/System/Api.h"
#include <Pt/System/Mutex.h>
#include <Pt/System/EventLoop.h>
#include "Pt/Signal.h"
#include <vector>

#ifdef __APPLE__
    #define PT_WITH_BSD_KQUEUE
    //#define PT_WITH_POSIX_POLL
#endif

#ifdef __linux__
    //#define PT_WITH_LINUX_EPOLL
    #define PT_WITH_POSIX_POLL
#endif

#if defined (PT_WITH_LINUX_EPOLL)
    #include "Selector_epoll.h"
#elif defined(PT_WITH_BSD_KQUEUE)
    #include "Selector_kqueue.h"
#elif defined(PT_WITH_POSIX_POLL)
    #include "Selector_poll.h"
#else
    #include "Selector_select.h"
#endif

namespace Pt {

namespace System {

class MainLoopImpl
{
    public:
        MainLoopImpl(Signal<const Event&>& eventSignal);

        MainLoopImpl(Signal<const Event&>& eventSignal, Allocator& a);

        ~MainLoopImpl();

        Selector& selector()
        { return _selector; }

        void attach(Selectable& s)
        { _selector.attach(s); }
        
        void detach(Selectable& s)
        { _selector.detach(s); }

        void idle(Selectable& s);

        void avail(Selectable& s);

        void run();

        void exit();

        void wake()
        { _selector.wake(); }

        void commitEvent(const Event& event);

        void queueEvent(const Event& event);

        bool processEvents();

        void attach(Timer& timer)
        { _timerQueue.addTimer(timer); }

        void detach( Timer& timer )
        { _timerQueue.removeTimer(timer); }

        bool waitNext();

    private:
        Mutex _mutex;
        Signal<const Event&>* _event;
        TimerQueue _timerQueue;
        EventQueue _eventQueue;
        std::vector<Selectable*> _avail;
        SelectorImpl _selector;
};

} //namespace System

} //namespace Pt

#endif

