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
#ifndef PT_SYSTEM_SELECTOR_KQUEUE_H
#define PT_SYSTEM_SELECTOR_KQUEUE_H

#include "../SelectableList.h"
#include "Pt/System/Api.h"
#include "Pt/System/Clock.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/Selectable.h"

#include <vector>
#include <set>
#include <cassert>
#include <cstddef>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

namespace Pt {

namespace System {

class SelectorImpl : public Selector
{
    public:
        SelectorImpl()
        : _kd(-1)
        , _avail(0)
        {
            _kd = kqueue();

            struct kevent kev;
            EV_SET(&kev, _wakePipe.readFd(), EVFILT_READ, EV_ADD, 0, 0, &_wakePipe);

            timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 0;
            kevent(_kd, &kev, 1, NULL, 0, &ts);
        }

        ~SelectorImpl()
        {         
            while( ! _selectables.empty() )
            {
                _selectables.first()->detach();
            }

            ::close(_kd);
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

            // remove from avail list
            for(int n = 0; n < _avail; ++n)
            {
                struct kevent& kev = _events[n];
                if(kev.udata == &h)
                {
                    kev.udata = 0;
                }
            }

            struct kevent kev[2];
            EV_SET(&kev[0], h.fd, EVFILT_READ, EV_DELETE, 0, 0, &h);
            EV_SET(&kev[1], h.fd, EVFILT_WRITE, EV_DELETE, 0, 0, &h);

            timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 0;
            kevent(_kd, kev, 2, NULL, 0, &ts);

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
            return h->ready & IOHandle::Read;
        }

        bool isWritable(IOHandle* h)
        {
            return h->ready & IOHandle::Write;
        }

        bool isError(IOHandle* h)
        {
            return false;
        }

        void wake()
        {
            _wakePipe.wake();
        }

        bool waitForWake(std::size_t msecs)
        {
            // process kevents which are left over from the last iteration
            // because of an exception
            if(_avail > 0)
                return processAvail();
        
            std::vector<struct kevent> changedEvents;
            for( std::vector<IOHandle*>::iterator it = _changelist.begin(); it != _changelist.end(); ++it)
            {
                IOHandle* h = *it;
        
                if(h->changed == h->events)
                    continue;

                struct kevent kev;

                if(h->changed & IOHandle::Read)
                {
                    if(0 == (h->events & IOHandle::Read))
                    {
                        EV_SET(&kev, h->fd, EVFILT_READ, EV_ADD|EV_ENABLE|EV_CLEAR, 0, 0, h);
                        changedEvents.push_back(kev);
                    }
                }
                else
                {
                    if(h->events & IOHandle::Read)
                    {
                        EV_SET(&kev, h->fd, EVFILT_READ, EV_DISABLE, 0, 0, h);
                        changedEvents.push_back(kev);
                    }
                }

                if(h->changed & IOHandle::Write)
                {
                    if(0 == (h->events & IOHandle::Write))
                    {
                        EV_SET(&kev, h->fd, EVFILT_WRITE, EV_ADD|EV_ENABLE|EV_CLEAR, 0, 0, h);
                        changedEvents.push_back(kev);
                    }
                }
                else
                {
                    if(h->events & IOHandle::Write)
                    {
                        EV_SET(&kev, h->fd, EVFILT_WRITE, EV_DISABLE, 0, 0, h);
                        changedEvents.push_back(kev);
                    }
                }

                h->events = h->changed;
            }
        
            _changelist.clear();
               
            bool isWake = false;
        
            while( true )
            {
                struct timespec* timeout = 0;
                struct timespec ts;
                if(msecs != EventLoop::WaitInfinite)
                {
                    ts.tv_sec = msecs / 1000;
                    ts.tv_nsec = (msecs % 1000) * 1000000;
                    timeout = &ts;
                }
        
                _clock.start();
                _avail = ::kevent(_kd, &changedEvents[0], changedEvents.size(), _events, EVENTS_SIZE, timeout);
                Pt::int64_t elapsed = _clock.stop().toMSecs();
        
                if( _avail < 0 && errno != EINTR )
                    throw IOError( PT_ERROR_MSG("select failed") );
        
                if(_avail > 0 || msecs == 0)
                {
                    isWake = processAvail();
                    break;
                }
        
                if(msecs != EventLoop::WaitInfinite)
                { 
                    if( static_cast<Pt::uint64_t>(elapsed) >= msecs )
                        break; // timeout
            
                    msecs -= static_cast<std::size_t>(elapsed);
                }
            }
        
            return isWake;
        }

    private:
        bool processAvail()
        {
            bool isWake = false;

            while(_avail > 0)
            {
                struct kevent& kev = _events[--_avail];

                void* p = kev.udata;
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

                    if(kev.filter & EVFILT_READ)
                    {
                        h->ready |= IOHandle::Read;
                    }

                    if(kev.filter & EVFILT_WRITE)
                    {
                        h->ready |= IOHandle::Write;
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
        int _kd;
        std::vector<IOHandle*> _changelist;
        static const unsigned EVENTS_SIZE = 32;
        struct kevent _events[EVENTS_SIZE];
        int _avail;
};

} //namespace System

} //namespace Pt

#endif

