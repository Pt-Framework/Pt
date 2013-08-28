/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
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
#ifndef PT_SYSTEM_MainLoopImpl_H
#define PT_SYSTEM_MainLoopImpl_H

#include "Selector.h"
#include "Pt/WinVer.h"
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

        void wake()
        { _selector.wake(); }

        void commitEvent(const Event& event);

        void queueEvent(const Event& event);

        bool processEvents();

        void attach(Timer& timer)
        { _timerQueue.addTimer(timer); }

        void detach(Timer& timer)
        { _timerQueue.removeTimer(timer); }

        void attach(Selectable& s)
        { _selector.attach(s); }

        void detach(Selectable& s)
        { _selector.detach(s); }

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
