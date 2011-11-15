/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#ifndef PT_SYSTEM_POSIX_MAINLOOPIMPL_H
#define PT_SYSTEM_POSIX_MAINLOOPIMPL_H

#include "Pt/System/Api.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/Clock.h"
#include "Pt/System/EventLoop.h"
#include <sys/select.h>
#include <set>

namespace Pt {

namespace System {

class MainLoopImpl : public EventLoopImpl
{
    public:
        MainLoopImpl();

        MainLoopImpl(Allocator& a);

        ~MainLoopImpl();

        void attach( Selectable& s );

        void detach( Selectable& s );

        void enable( Selectable& dev );

        void disable( Selectable& dev );

        void changed(Selectable& s);

    protected:
        virtual void onRun();

        virtual void onWake();

        void waitNext(WaitResult& result, std::size_t msecs);

    private:
        int _wakePipe[2];
        fd_set _rfds;
        fd_set _wfds;
        fd_set _efds;
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*> _attached;
        std::set<Selectable*> _devices;
        std::set<Selectable*> _avail;
        Clock _clock;
};

} //namespace System

} //namespace Pt

#endif
