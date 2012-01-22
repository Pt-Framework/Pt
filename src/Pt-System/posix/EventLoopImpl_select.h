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
#ifndef PT_SYSTEM_EVENTLOOPIMPL_SELECT_H
#define PT_SYSTEM_EVENTLOOPIMPL_SELECT_H

#include "Pt/System/Api.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Selectable.h"

#include <set>
#include <cstddef>
#include <cassert>

#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>

namespace Pt {

namespace System {

class Selector
{
    public:
        Selector()
        {
            _current = _devices.end();

            FD_ZERO(&_rfds);
            FD_ZERO(&_wfds);
            FD_ZERO(&_efds);
        
            FD_ZERO(&_rfdsOut);
            FD_ZERO(&_wfdsOut);
            FD_ZERO(&_efdsOut);
        
            FD_SET(_wakePipe.readFd(), &_rfds);
        }

        ~Selector()
        {         
            std::set<Selectable*>::iterator it;

            while( _selectables.size() )
            {
                it = _selectables.begin();
                (*it)->detach();
            }
        }

        void cancel(IOHandle& h)
        {
            std::set<Selectable*>::iterator it = _devices.find( h.sel );
            if( it == _devices.end() )
                return;

            assert(h.fd > 0);

            FD_CLR(h.fd, &_rfds);
            FD_CLR(h.fd, &_wfds);
            FD_CLR(h.fd, &_efds);
            h.id = IOHandle::InvalidId;
            
            if( (_current != _devices.end()) && (*_current == *it) )
            {
                _devices.erase(_current++);
            }
            else
            {
                _devices.erase(it);
            }
        }

        void enableSelect(IOHandle* h)
        {
            if( ! h->id != IOHandle::InvalidId )
            {
                _devices.insert( h->sel );
                FD_SET(h->fd, &_efds);
                h->id = 1;
            }
        }

        void beginRead(IOHandle* h)
        {
            enableSelect(h);
            FD_SET(h->fd, &_rfds);
        }

        void endRead(IOHandle* h)
        {
            FD_CLR( h->fd, &_rfds );
        }

        void beginWrite(IOHandle* h)
        {
            enableSelect(h);
            FD_SET(h->fd, &_wfds);
        }

        void endWrite(IOHandle* h)
        {
            FD_CLR( h->fd, &_wfds );
        }

        bool isReadable(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_rfdsOut);
        }

        bool isWritable(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_wfdsOut);
        }

        bool isError(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_efdsOut);
        }

        void wake()
        {
            _wakePipe.wake();
        }

    public:
        bool waitForWake(size_t msecs)
        {
            bool isWake = false;
        
            FD_ZERO(&_rfdsOut);
            FD_ZERO(&_wfdsOut);
            FD_ZERO(&_efdsOut);
        
            _rfdsOut = _rfds;
            _wfdsOut = _wfds;
            _efdsOut = _efds;
        
            int avail = -1;
        
            while( true )
            {
                struct timeval* timeout = 0;
                struct timeval tv;
                if(msecs != EventLoop::WaitInfinite)
                {
                    tv.tv_sec = msecs / 1000;
                    tv.tv_usec = (msecs % 1000) * 1000;
                    timeout = &tv;
                }
        
                _clock.start();
                avail = ::select(FD_SETSIZE, &_rfdsOut, &_wfdsOut, &_efdsOut, timeout);
                Pt::int64_t elapsed = _clock.stop().totalMSecs();
        
                if( avail < 0 && errno != EINTR )
                {
                    throw IOError( PT_ERROR_MSG("select failed") );
                }
        
                if( avail > 0 || msecs == 0 )
                    break;
        
                if(msecs != EventLoop::WaitInfinite)
                {
                    if(static_cast<Pt::uint64_t>(elapsed) >= msecs)
                        return isWake; // timeout
            
                    msecs -= int(elapsed);
                }
            }
        
            if( FD_ISSET(_wakePipe.readFd(), &_rfdsOut) )
            {
                --avail;
                isWake = _wakePipe.isReady();
            }
        
            try
            {
                for( _current = _devices.begin(); _current != _devices.end(); )
                {
                    Selectable* selectable = *_current;
        
                    bool isAvail = selectable->run();
        
                    if( isAvail )
                        --avail;
        
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
        
            return isWake;
        }

        void attach(Selectable& s)
        {
            _selectables.insert(&s);
        }
        
        void detach(Selectable& s)
        {
            _selectables.erase(&s);
        }

    private:
        WakePipe _wakePipe;
        std::set<Selectable*> _selectables; // inactive
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*> _devices; // active
        fd_set _rfds;
        fd_set _wfds;
        fd_set _efds;
        fd_set _rfdsOut;
        fd_set _wfdsOut;
        fd_set _efdsOut;
        Clock _clock;
};

} //namespace System

} //namespace Pt

#endif

