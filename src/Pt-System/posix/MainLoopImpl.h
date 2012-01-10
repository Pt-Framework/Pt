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
#include "Pt/System/Mutex.h"
#include "Pt/System/EventLoop.h"
#include "IODeviceImpl.h"
#include <sys/select.h>
#include <set>
#include <list>
#include <iostream>

namespace Pt {

namespace System {

class EventLoopImpl
{
    enum IOFlags
    {
        Input = 1,
        Output = 2,
        Error = 4,
        Enabled = 8
    };

    public:
        EventLoopImpl(Signal<const Event&>& eventSignal);

        ~EventLoopImpl();

        void cancel(IOHandle& h)
        {
           std::set<Selectable*>::iterator it = _devices.find( h.sel );
           if( it == _devices.end() )
                return;

           assert(h.fd > 0);

            if(h.wflags & Input)
                FD_CLR(h.fd, &_rfds);

            if(h.wflags & Output)
                FD_CLR(h.fd, &_wfds);

            FD_CLR(h.fd, &_efds);
            h.wflags = 0;
            h.flags = 0;

            remove(&h);

            if( _current == _devices.end() )
            {
                _devices.erase(it);
            }
            else if(*_current == *it)
            {
                _devices.erase(_current++);
            }
            else
            {
                _devices.erase(it);
            }
        }

        void beginRead(IOHandle* h)
        {
            if( (h->flags & Enabled) != Enabled )
            {
                h->flags |= Enabled;
                h->wflags |= Enabled;
                FD_SET(h->fd, &_efds);
                _devices.insert( h->sel );
            }

            h->flags |= Input;
            setChanged(h);
        }

        void endRead(IOHandle* h)
        {
            if(h->flags & Input)
            {
                h->flags &= ~Input;
                setChanged(h);
            }
        }

        void beginWrite(IOHandle* h)
        {
            if( (h->flags & Enabled) != Enabled )
            {
                h->flags |= Enabled;
                h->wflags |= Enabled;
                FD_SET(h->fd, &_efds);
                _devices.insert( h->sel );
            }

            h->flags |= Output;
            setChanged(h);
        }

        void endWrite(IOHandle* h)
        {
            if(h->flags & Output)
            {
                h->flags &= ~Output;
                setChanged(h);
            }
        }

        bool isReadable(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_rfdsR);
        }

        bool isWritable(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_wfdsR);
        }

        bool isError(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_efdsR);
        }

        void remove(IOHandle* h)
        {
            for( std::vector<IOHandle*>::iterator it = _dirty.begin(); it != _dirty.end();)
            {
                if(*it == h)
                    it = _dirty.erase(it);
                else
                    ++it;
            }
        }

        void setChanged(IOHandle* h)
        { _dirty.push_back(h); }

        void idle(Selectable& s);

        void avail(Selectable& s);

        void attach(Selectable& s);

        void detach(Selectable& s);

        void run();

        void exit();

        void wake();

        void commitEvent(const Event& event);

        void queueEvent(const Event& event);

        bool processEvents();

        void attach(Timer& timer)
        { _timerQueue.addTimer(timer); }

        void detach( Timer& timer )
        { _timerQueue.removeTimer(timer); }

    protected:
        bool waitNext();

    private:
        Mutex _mutex;
        bool _exited;
        TimerQueue _timerQueue;
        EventQueue _eventQueue;
        Signal<const Event&>* _event;
        int _wakePipe[2];
        std::vector<IOHandle*> _dirty;
        std::set<Selectable*> _selectables;
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*> _devices; // active
        std::vector<Selectable*> _avail;
        fd_set _rfds;
        fd_set _wfds;
        fd_set _efds;
        fd_set _rfdsR;
        fd_set _wfdsR;
        fd_set _efdsR;
        Clock _clock;
};

class MainLoopImpl : public EventLoopImpl
{
    public:
        MainLoopImpl(Signal<const Event&>& eventSignal);

        MainLoopImpl(Signal<const Event&>& eventSignal, Allocator& a);

        ~MainLoopImpl();
};

} //namespace System

} //namespace Pt

#endif
