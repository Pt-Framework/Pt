/*
 * Copyright (C) 2006-20012 Marc Boris Duerner
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
#ifndef PT_SYSTEM_SELECTOR_EPOLL_H
#define PT_SYSTEM_SELECTOR_EPOLL_H

#include "../SelectableList.h"
#include "Pt/System/Api.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Selectable.h"

#include <set>
#include <limits>
#include <cassert>
#include <cstddef>

#include <sys/types.h>
#include <sys/epoll.h>

namespace Pt {

namespace System {

class SelectorImpl  : public Selector
{
    public:
        SelectorImpl()
        : _epfd(-1)
        , _avail(0)
        {
            _epfd = epoll_create(16);

            epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.ptr = &_wakePipe;

            epoll_ctl(_epfd, EPOLL_CTL_ADD, _wakePipe.readFd(), &ev);
        }

        ~SelectorImpl()
        {         
            while( ! _selectables.empty() )
            {
                _selectables.first()->detach();
            }

            ::close(_epfd);
        }

        void attach(Selectable& s)
        {
            _selectables.insert(s);
        }
        
        void detach(Selectable& s)
        {
            SelectableList::unlink(s);
        }

        void cancel(IOHandle& h)
        {
            if(h.fd < 0)
                return;

            // remove from change list
            std::vector<IOHandle*>::iterator it = std::remove(_changelist.begin(), _changelist.end(), &h);
            _changelist.erase(it, _changelist.end());

            // disable in avail list
            for(int n = 0; n < _avail; ++n)
            {
                epoll_event& ev = _events[n];
                if(ev.data.ptr == &h)
                    ev.data.ptr  = 0;
            }

            // remove from epoll
            epoll_ctl(_epfd, EPOLL_CTL_DEL, h.fd, NULL);

            h.id = IOHandle::InvalidId;
            h.events = 0;
            h.changed = 0;
            h.ready = 0;
        }

        void beginRead(IOHandle* h)
        {
            bool isAdded = h->changed != h->events;
            if(! isAdded)
                _changelist.push_back(h);

            h->id = 1;
            h->changed |= IOHandle::Read;
        }

        void endRead(IOHandle* h)
        {
            bool isAdded = h->changed != h->events;
            if(! isAdded)
                _changelist.push_back(h);

            h->ready = 0;
            h->changed &= ~IOHandle::Read;
        }

        void beginWrite(IOHandle* h)
        {
            bool isAdded = h->changed != h->events;
            if(! isAdded)
                _changelist.push_back(h);

            h->id = 1;
            h->changed |= IOHandle::Write;
        }

        void endWrite(IOHandle* h)
        {
            bool isAdded = h->changed != h->events;
            if(! isAdded)
                _changelist.push_back(h);

            h->ready = 0;
            h->changed &= ~IOHandle::Write;
        }

        bool isReadable(IOHandle* h)
        {
            /*bool isReady = h->ev && (h->ev->events & (EPOLLIN|EPOLLHUP));
                       
            if(isReady)
                h->ev->events &= (EPOLLIN|EPOLLHUP);*/

            return h->ready & IOHandle::Read;
        }

        bool isWritable(IOHandle* h)
        {
            /*bool isReady = h->ev && (h->ev->events & (EPOLLOUT|EPOLLHUP));

            if(isReady)
                h->ev->events &= (EPOLLIN|EPOLLHUP);*/

            return h->ready & IOHandle::Write;
        }

        bool isError(IOHandle* h)
        {
            /*bool isReady = h->ev && (h->ev->events & EPOLLERR);

            if(isReady)
                h->ev->events &= EPOLLERR;*/

            return h->ready & IOHandle::Error;
        }

        void wake()
        {
            _wakePipe.wake();
        }

    public:
        bool waitForWake(size_t umsecs)
        {
            // process events which are left over from the last iteration
            // because of an exception
            if(_avail > 0)
                return processAvail();

            for( std::vector<IOHandle*>::iterator it = _changelist.begin(); it != _changelist.end(); ++it)
            {
                IOHandle* h = *it;
        
                if(h->changed == h->events)
                    continue;

                epoll_event ev;

                if(h->changed & IOHandle::Read)
                    ev.events |= EPOLLIN;
                if(h->changed & IOHandle::Write)
                    ev.events |= EPOLLOUT;
 
                ev.data.ptr = h;

                if(h->events)
                    epoll_ctl(_epfd, EPOLL_CTL_MOD, h->fd, &ev);
                else
                    epoll_ctl(_epfd, EPOLL_CTL_ADD, h->fd, &ev);

                h->events = h->changed;
            }
        
            _changelist.clear();

            int msecs = -1;
            if(umsecs != EventLoop::WaitInfinite)
            {
                const size_t maxMSecs = std::numeric_limits<int>::max();
                msecs = umsecs > maxMSecs ? maxMSecs : static_cast<int>(umsecs);
            }

            bool isWake = false;
        
            while( true )
            {     
                _clock.start();
                _avail = epoll_wait(_epfd, _events, EVENTS_SIZE, msecs);
                Pt::int64_t elapsed = _clock.stop().toMSecs();
        
                if( _avail < 0 && errno != EINTR )
                    throw IOError( PT_ERROR_MSG("select failed") );
        
                if(_avail > 0 || msecs == 0)
                {
                    isWake = processAvail();
                    break;
                }
        
                if(umsecs != EventLoop::WaitInfinite)
                { 
                    if(elapsed >= msecs)
                        break; // timeout
            
                    msecs -= int(elapsed);
                }
            }
        
            return isWake;
        }

        bool processAvail()
        {
            bool isWake = false;

            while(_avail > 0)
            {
                epoll_event& ev = _events[--_avail];

                void* p = ev.data.ptr;
                if(p == 0)
                {
                    continue;
                }
                else if( p == &_wakePipe )
                {
                    isWake = _wakePipe.isReady();
                }
                else
                {
                    IOHandle* h = reinterpret_cast<IOHandle*>(p);

                    if(ev.events & (EPOLLIN|EPOLLHUP))
                    {
                        h->ready |= IOHandle::Read;
                    }
                    if(ev.events & (EPOLLOUT|EPOLLHUP))
                    {
                        h->ready |= IOHandle::Write;
                    }
                    if(ev.events & EPOLLERR)
                    {
                        h->ready |= IOHandle::Error;
                    }

                    h->sel->run();
                }
            }

            return isWake;
        }

    private:
        SelectableList _selectables;
        Clock _clock;
        WakePipe _wakePipe;
        int _epfd;
        std::vector<IOHandle*> _changelist;
        static const unsigned EVENTS_SIZE = 32;
        struct epoll_event _events[EVENTS_SIZE];
        int _avail;
};

} //namespace System

} //namespace Pt

#endif

