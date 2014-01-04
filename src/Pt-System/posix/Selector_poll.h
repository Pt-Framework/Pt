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
#ifndef PT_SYSTEM_SELECTOR_POLL_H
#define PT_SYSTEM_SELECTOR_POLL_H

#include "../SelectableList.h"
#include "Pt/System/Api.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Selectable.h"

#include <vector>
#include <limits>
#include <cassert>
#include <cstddef>

#include <sys/types.h>
#include <sys/poll.h>

namespace Pt {

namespace System {

class SelectorImpl : public Selector
{
    public:
        SelectorImpl()
        {
            _current = 0;
        
            pollfd pfd;
            pfd.fd = _wakePipe.readFd();
            pfd.events = POLLIN;
            pfd.revents = 0;
            _pollfds.push_back(pfd);

            _iohandles.push_back(0);
        }

        ~SelectorImpl()
        {         
            while( ! _devices.empty() )
            {
                _devices.first()->detach();
            }

            while( ! _selectables.empty() )
            {
                _selectables.first()->detach();
            }

            assert( _devices.empty() );
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
            Selectable* s = h.sel;

            if( h.id == IOHandle::InvalidId)
                return;

            assert(h.fd > 0);
            assert(h.id != 0);

            if(h.id != _iohandles.back()->id)
            {
                size_t offset = h.id;
                _pollfds.at(offset) = _pollfds.back();
                _iohandles.at(offset) = _iohandles.back();
                _iohandles[offset]->id = offset;
            }

            _pollfds.resize(_pollfds.size() - 1);
            _iohandles.resize(_iohandles.size() - 1);
            h.id = IOHandle::InvalidId;

            if( _current == h.sel )
            {
                _current = _current->next();
            }
        
            _selectables.insert(*s);
        }

        pollfd& enablePoll(IOHandle* h)
        {
            Selectable* s = h->sel;

            if( h->id == IOHandle::InvalidId)
            {
                assert(_pollfds.size() == _iohandles.size());

                _iohandles.reserve(_iohandles.size() + 1);
                _pollfds.reserve(_pollfds.size() + 1);

                _devices.insert(*s);

                h->id = _pollfds.size();
                _iohandles.push_back(h);

                pollfd pfd;
                pfd.fd = h->fd;
                pfd.events = 0;
                pfd.revents = 0;
                _pollfds.push_back(pfd);

                return _pollfds.back();
            }

            return _pollfds[h->id];
        }

        void beginRead(IOHandle* h)
        {
            enablePoll(h).events |= POLLIN;
        }

        void endRead(IOHandle* h)
        {
            assert(h->id != IOHandle::InvalidId);
            _pollfds[h->id].events &= ~POLLIN;
        }

        void beginWrite(IOHandle* h)
        {
            enablePoll(h).events |= POLLOUT;
        }

        void endWrite(IOHandle* h)
        {
            assert(h->id != IOHandle::InvalidId);
            _pollfds[h->id].events &= ~POLLOUT;
        }

        bool isReadable(IOHandle* h)
        {
            if(h->id == IOHandle::InvalidId)
                return false;

            return _pollfds[h->id].revents & (POLLIN|POLLHUP);
        }

        bool isWritable(IOHandle* h)
        {
            if(h->id == IOHandle::InvalidId)
                return false;

            return _pollfds[h->id].revents & (POLLOUT|POLLHUP);
        }

        bool isError(IOHandle* h)
        {
            if(h->id == IOHandle::InvalidId)
                return false;

            return _pollfds[h->id].revents & (POLLERR|POLLNVAL);
        }

        void wake()
        {
            _wakePipe.wake();
        }

    public:
        bool waitForWake(size_t umsecs)
        {
            const size_t maxMSecs = std::numeric_limits<int>::max();

            int msecs = static_cast<int>(umsecs);
            if(umsecs == EventLoop::WaitInfinite)
                msecs = -1;
            else if(umsecs > maxMSecs)
                msecs = std::numeric_limits<int>::max();

            bool isWake = false;               
            int avail = -1;

            while( true )
            {            
                _clock.start();
                avail = ::poll(&_pollfds[0], _pollfds.size(), msecs);
                Pt::int64_t elapsed = _clock.stop().toMSecs();

                if( avail < 0 && errno != EINTR )
                    throw IOError( PT_ERROR_MSG("select failed") );
        
                if( avail > 0 || msecs == 0 )
                    break;
        
                if(umsecs != EventLoop::WaitInfinite) // negative poll time means infinite
                {
                    if(elapsed >= msecs)
                        return isWake; // timeout
            
                    msecs -= int(elapsed);
                }
            }

            if( _pollfds[0].revents & POLLIN )
            {
                --avail;
                isWake = _wakePipe.isReady();
            }

            try
            {
                for( _current = _devices.first(); _current != 0; )
                {
                    Selectable* selectable = _current;
        
                    bool isAvail = selectable->run();
        
                    if( isAvail )
                        --avail;
        
                    if(avail <= 0)
                        break;
        
                    if(_current == selectable)
                    {
                        _current = _current->next();
                    }
                }

                _current = 0;
            }
            catch (...)
            {
                _current = 0;
                throw;
            }
        
            return isWake;
        }

    private:
        WakePipe _wakePipe;
        std::vector<pollfd> _pollfds;
        std::vector<IOHandle*> _iohandles;
        SelectableList _selectables;
        SelectableList _devices;
        Selectable* _current;
        Clock _clock;
};

} //namespace System

} //namespace Pt

#endif

