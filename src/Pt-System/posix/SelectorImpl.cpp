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
#include "SelectorImpl.h"
#include "IODeviceImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/Application.h"
#include "Pt/System/Selector.h"
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <cassert>
#include <iostream>

namespace Pt {

namespace System {

SelectorImpl::SelectorImpl()
: _app(0)
{
    _current = _devices.end();

    //Open a pipe to send wake up message.
    if( ::pipe( _wakePipe ) )
        throw std::runtime_error("Could not open pipe." + PT_SOURCEINFO);

    int flags = ::fcntl(_wakePipe[0], F_GETFL);
    if(-1 == flags)
        throw std::runtime_error("Could not get pipe flags." + PT_SOURCEINFO);

    int ret = ::fcntl(_wakePipe[0], F_SETFL, flags|O_NONBLOCK);
    if(-1 == ret)
        throw std::runtime_error("Could not set pipe to non-blocking." + PT_SOURCEINFO);

    flags = ::fcntl(_wakePipe[1], F_GETFL);
    if(-1 == flags)
        throw std::runtime_error("Could not get pipe flags." + PT_SOURCEINFO);

    ret = ::fcntl(_wakePipe[1], F_SETFL, flags|O_NONBLOCK);
    if(-1 == ret)
        throw std::runtime_error("Could not set pipe to non-blocking." + PT_SOURCEINFO);

    FD_ZERO(&_rfds);
    FD_ZERO(&_wfds);
    FD_ZERO(&_efds);

    FD_SET(_wakePipe[0], &_rfds);
}


SelectorImpl::~SelectorImpl()
{
    std::set<Selectable*>::iterator it;
    while( _devices.size() )
    {
        it = _devices.begin();
        (*it)->setSelector(0);
    }

    if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
    {
        ::close(_wakePipe[0]);
        ::close(_wakePipe[1]);
    }
}


void SelectorImpl::add(Selectable& s)
{
    s.simpl().initSelect(_rfds, _wfds, _efds);
    _devices.insert( &s );
}


void SelectorImpl::remove(Selectable& s)
{
   std::set<Selectable*>::iterator it = _devices.find( &s );
   if( it == _devices.end() )
        return;

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

    s.simpl().exitSelect(_rfds, _wfds, _efds);
}


void SelectorImpl::onEnabled(Selectable& s)
{
    s.simpl().initSelect(_rfds, _wfds, _efds);
}


void SelectorImpl::onDisabled(Selectable& s)
{
    s.simpl().exitSelect(_rfds, _wfds, _efds);
}


bool SelectorImpl::wait(std::size_t msecs)
{
    fd_set rfds = _rfds;
    fd_set wfds = _wfds;
    fd_set efds = _efds;

    int avail = 0;
    while( true )
    {
        struct timeval* timeout = 0;
        struct timeval tv;
        if(msecs != Selector::WaitInfinite)
        {
            tv.tv_sec = msecs / 1000;
            tv.tv_usec = (msecs % 1000) * 1000;
            timeout = &tv;
        }

        _clock.start();
        avail = ::select(FD_SETSIZE, &rfds, &wfds, &efds, timeout);
        Pt::int64_t elapsed = _clock.stop().totalMSecs();

        if( avail > 0 )
            break;

        if( avail < 0 && errno != EINTR )
        {
            throw IOError( "select failed", PT_SOURCEINFO );
        }

        if(msecs == SelectorBase::WaitInfinite)
            continue;

        if(elapsed >= msecs)
            return false;

        msecs -= int(elapsed);
    }


    if( FD_ISSET(_wakePipe[0], &_efds) )
    {
        throw IOError("select error on event pipe", PT_SOURCEINFO);
    }

    if( FD_ISSET(_wakePipe[0], &_rfds) )
    {
        static char buffer[1024];
        while(true)
        {
            int ret = ::read(_wakePipe[0], buffer, sizeof(buffer));
            if(ret > 0)
            {
                avail = true;
                continue;
            }

            if (ret == -1)
            {
                if(errno == EINTR)
                    continue;

                if(errno == EAGAIN)
                    break;
            }

            throw IOError("Cound not read from pipe", PT_SOURCEINFO);
        }

        --avail;
    }

    try
    {
        for( _current = _devices.begin(); _current != _devices.end(); )
        {
            Selectable* selectable = *_current;
            avail -= selectable->simpl().checkEvent(rfds, wfds, efds);

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

    return true;
}


void SelectorImpl::wake()
{
    ::write( _wakePipe[1], "W", 1);
    ::fsync( _wakePipe[1] );
}

} //namespace System

} //namespace Pt
