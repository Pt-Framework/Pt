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

struct IOHandle
{
    IOHandle(Selectable& sel, int fd)
    : sel(&sel)
    , _fd(fd)
    , wflags(0)
    , flags(0)
    , next(0)
    , prev(0)
    {}

    Selectable* sel;
    int fd;
    int wflags;
    int flags;
    Handle* next;
    Handle* prev;
};

class SelectorImpl : public Selector
{
    enum IOFlags
    {
        Input = 1,
        Output = 2,
        Error = 4
    };

    public:
        SelectorImpl()
        : _first(0)
        , _last(0)
        {
            _current = _devices.end();
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

        IOHandle* enable(Selectable& s, int fd)
        {
            _devices.insert( &s );

            if( fd > FD_SETSIZE )
                throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );

            // no change required, move to back
            IOHandle* h = new IOHandle(s, fd);
            push_back(h);
            FD_SET(h->fd, &efds);

            
        }

        void disable(IOHandle* h)
        {
           std::set<Selectable*>::iterator it = _devices.find( h->sel );
           if( it == _devices.end() )
                return;

            if( h->fd > 0)
            {
                if(h->flags & Input)
                    FD_CLR(h->fd, _rfds);

                if(h->flags & Output)
                    FD_CLR(h->fd, _wfds);

                FD_CLR(h->fd, _efds);
            }

            pop(h);
            delete h;

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
            pop(h);
            h->flags |= Input;
 
            // TODO: if no change is required, the handle does not have to be 
            //       int the list at all... -> it would be a real changelist
            if(h->flags == h->wflags)
                push_back(h); // no change required, move to back
            else
                push_front(h); // update before next wait, move to front
        }

        void endRead(IOHandle* h)
        {
            h->flags &= ~Input;

            // update before next wait, move to front
            pop(h);
            push_front(h);
        }

        void beginWrite(IOHandle* h)
        {
            pop(h);
            h->flags |= Output;
 
            if(h->flags == h->wflags)
                push_back(h); // no change required, move to back
            else
                push_front(h); // update before next wait, move to front

        }

        void endWrite(IOHandle* h)
        {
            h->flags &= ~Output;

            // update before next wait, move to front
            pop(h);
            push_front(h);
        }

        void beginWait()
        {
            for(IOHandle* h = _first; h != 0; h = h->next)
            {
                if(h->flags == h->wflags)
                    break;

                if(h->flags & Input &&  0 == (h->wflags & Input))
                {
                    FD_SET(h->fd, &rfds);
                    h->wflags |= Input;
                }

                if(0 == h->flags & Input && h->wflags & Input)
                {
                    FD_CLR( h->fd, _rfds );
                    h->wflags &= ~Intput;
                }

                if(h->flags & Output &&  0 == (h->wflags & Output))
                {
                    FD_SET(h->fd, &wfds);
                    h->wflags |= Output;
                }

                if(0 == h->flags & Output &&  h->wflags & Output)
                {
                    FD_CLR( h->fd, _wfds );
                    h->wflags &= ~Output;
                }
            }
        }

        void checkAvail(int avail)
        {
            try
            {
                avail += _avail.size();
        
                for( _current = _devices.begin(); _current != _devices.end(); )
                {
                    Selectable* selectable = *_current;
                    avail -= selectable->onAvail(*this);
        
                    if(avail <= 0)
                        break;
        
                    if(_current != _devices.end())
                    {
                        if(*_current == selectable)
                        {
                            ++_current;
                        }
                    }
                }
            }
            catch (...)
            {
                _current = _devices.end();
                throw;
            }
        }

        bool isReadable(IOHandle* h)
        {
            FD_ISSET(h->fd, &rfds);
        }

        bool isWritable(IOHandle* h)
        {
            FD_ISSET(h->fd, &wfds);
        }

        bool isError(IOHandle* h)
        {
            FD_ISSET(h->fd, &efds);
        }

        void pop(IOHandle* h)
        {
            IOHandle* prev = h->prev;
            IOHandle* next = h->next;

            if(prev)
                prev->next = next;

            if(next)
                next->prev = prev;

            h->next = 0;
            h->prev = 0;
        }

        void push_back(IOHandle* h)
        {
            if( ! _first)
            {
                _first = h;
                _last = h;
            }
            else
            {
                _last->next = h;
                h->prev = _last;
                _last = h;
            }
        }

        void push_front(IOHandle* h)
        {
            if( ! _first)
            {
                _first = h;
                _last = h;
            }
            else
            {
                h->next = _first;
                _first->prev = h;
                _first = h;
            }
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
        IOHandle* _first;
        IOHandle* _last;
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*> _devices; // active
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
