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
#include <list>

namespace Pt {

namespace System {

class SelectorImpl : public Selector
{
    enum IOFlags
    {
        Input = 1,
        Output = 2,
        Error = 4
    };

    public:
        struct Node
        {
            Node()
            : sel(0)
            , wflags(0)
            , flags(0)
            {}

            Selectable* sel;
            int wflags;
            int flags;
        };

        typedef std::list<Node> HandleQueue;
        typedef std::list<Node>::iterator Handle;

    public:
        SelectorImpl()
        {
            FD_ZERO(&_rfds);
            FD_ZERO(&_wfds);
            FD_ZERO(&_efds);
        }
        
        ~SelectorImpl()
        { }

        void beginRead(Selectable&, int fd)
        {}

        void endRead(Selectable&, int fd)
        {}

        Handle enable(Selectable& s, int fd)
        {
            Node n;
            n.sel = &s;
            return _hqueue.insert(_hqueue.end(), n);
        }

        void beginRead(Handle& h, int fd)
        {
            h->flags |= Input;

            if(h->flags == h->wflags)
            {
                // no change required, move to back
                if( h != _hqueue.end() )
                    _hqueue.splice(_hqueue.end(), _hqueue, h);
            }
            else
            {
                // update before next wait, move to front
                h = _hqueue.insert(_hqueue.begin(), *h);
            }
        }

        void endRead(Handle& h, int fd)
        {
            h->flags &= ~Input;

            // update before next wait, move to front
            if( h != _hqueue.begin() )
                _hqueue.splice(_hqueue.begin(), _hqueue, h);
        }

        fd_set& rfds()
        { return _rfds; }

        fd_set& wfds()
        { return _wfds; }

        fd_set& efds()
        { return _efds; }

        SelectorImpl& impl()
        { return *this; }

    private:
        HandleQueue _hqueue;
        fd_set _rfds;
        fd_set _wfds;
        fd_set _efds;
};

class MainLoopImpl : public EventLoopImpl
{
    public:
        MainLoopImpl();

        MainLoopImpl(Allocator& a);

        ~MainLoopImpl();

        Selector& selector()
        { return _selector; }

        void attach(Selectable& s);

        void detach(Selectable& s);

        void enable(Selectable& s);

        void disable(Selectable& s);

        void idle(Selectable& s);

        void active(Selectable& s);

        void avail(Selectable& s);

    protected:
        virtual void onRun();

        virtual void onWake();

        void waitNext(std::size_t timeout, bool& isActive);

    private:
        int _wakePipe[2];
        SelectorImpl _selector;
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
