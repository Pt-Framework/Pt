/*
 * Copyright (C) 2013 by Marc Boris Duerner
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

#ifndef PT_SYSTEM_MainLoopImpl_H
#define PT_SYSTEM_MainLoopImpl_H

#include "Selector.h"
#include "Pt/System/Api.h"

namespace Pt {

namespace System {

class PT_SYSTEM_API MainLoopImpl
{
    public:
        MainLoopImpl(Signal<const Pt::Event&>& eventSignal);

        MainLoopImpl(Signal<const Pt::Event&>& eventSignal, Allocator& a);

        ~MainLoopImpl();

        Selector& selector()
        { return _selector; }

        void run();

        void exit();

        void wake();

        void commitEvent(const Event& event);

        void queueEvent(const Event& event);

        bool processEvents();

        void attach(Timer& timer);

        void detach(Timer& timer);

        void attach(Selectable& s);

        void detach(Selectable& s);

        void idle(Selectable& s);

        void avail(Selectable& s);

        bool waitNext();

    private:
        Mutex _mutex;
        TimerQueue _timerQueue;
        EventQueue _eventQueue;
        Signal<const Event&>* _event;
        std::vector<Selectable*> _avail;
        Selector _selector;
};

}//namespace System

}//namespace Pt

#endif
