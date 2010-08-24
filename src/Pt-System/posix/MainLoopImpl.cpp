/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
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
#include "MainLoopImpl.h"
#include "IODeviceImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/MainLoop.h"
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>


namespace Pt {

namespace System {

MainLoopImpl::MainLoopImpl()
{
    _current = _devices.end();

    //Open a pipe to send wake up message.
    if( ::pipe( _wakePipe ) )
        throw SystemError( PT_ERROR_MSG("pipe failed") );

    int flags = ::fcntl(_wakePipe[0], F_GETFL);
    if(-1 == flags)
        throw SystemError(PT_ERROR_MSG("fcntl failed"));

    int ret = ::fcntl(_wakePipe[0], F_SETFL, flags|O_NONBLOCK);
    if(-1 == ret)
        throw SystemError( PT_ERROR_MSG("fcntl failed") );

    flags = ::fcntl(_wakePipe[1], F_GETFL);
    if(-1 == flags)
        throw SystemError( PT_ERROR_MSG("fcntl failed") );

    ret = ::fcntl(_wakePipe[1], F_SETFL, flags|O_NONBLOCK);
    if(-1 == ret)
        throw SystemError( PT_ERROR_MSG("fcntl failed") );

    FD_ZERO(&_rfds);
    FD_ZERO(&_wfds);
    FD_ZERO(&_efds);

    FD_SET(_wakePipe[0], &_rfds);
}


MainLoopImpl::~MainLoopImpl()
{
    std::set<Selectable*>::iterator it;
    // while( _devices.size() )
    // {
    //     it = _devices.begin();
    //     (*it)->setSelector(0);
    // }

    while( _attached.size() )
    {
        it = _attached.begin();
        (*it)->setSelector(0);
    }

    if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
    {
        ::close(_wakePipe[0]);
        ::close(_wakePipe[1]);
    }
}


void MainLoopImpl::attach(Selectable& s)
{
    _attached.insert(&s);
}


void MainLoopImpl::detach(Selectable& s)
{
    _attached.erase(&s);
}


void MainLoopImpl::add(Selectable& s)
{
    s.simpl().initSelect(_rfds, _wfds, _efds);
    _devices.insert( &s );
}


void MainLoopImpl::remove(Selectable& s)
{
   std::set<Selectable*>::iterator it = _devices.find( &s );
   if( it == _devices.end() )
        return;

    s.simpl().exitSelect();

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


void MainLoopImpl::changed(Selectable& s)
{
    if( s.avail() )
    {
        _avail.insert(&s);
    }
    else
    {
        _avail.erase(&s);
    }
}


void MainLoopImpl::run(MainLoop& loop)
{
    WaitResult result;
    result.setInit();

    while(true)
    {
        size_t timeout = loop.runNext(result);

        if( result.isExit() )
            return;

        this->waitNext(result, timeout);
    }
}


void MainLoopImpl::exit()
{

}


void MainLoopImpl::waitNext( WaitResult& result, std::size_t msecs )
{
    fd_set rfds = _rfds;
    fd_set wfds = _wfds;
    fd_set efds = _efds;

    msecs = _avail.size() ? 0 : msecs;
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
        avail = ::select(FD_SETSIZE, &rfds, &wfds, &efds, timeout);
        Pt::int64_t elapsed = _clock.stop().totalMSecs();

        if( avail < 0 && errno != EINTR )
        {
            throw IOError( PT_ERROR_MSG("select failed") );
        }

        if( avail > 0 || _avail.size() )
            break;

        if(msecs == EventLoop::WaitInfinite)
            continue;

        if(static_cast<Pt::uint64_t>(elapsed) >= msecs)
            return; // timeout

        msecs -= int(elapsed);
    }

    if( FD_ISSET(_wakePipe[0], &efds) )
    {
        throw IOError( PT_ERROR_MSG("pipe failed") );
    }

    if( FD_ISSET(_wakePipe[0], &rfds) )
    {
        --avail;

        static char buffer[1024];
        while(true)
        {
            int ret = ::read(_wakePipe[0], buffer, sizeof(buffer));
            if(ret > 0)
            {
                //avail = true;
                result.setEvent();
                continue;
            }

            if (ret == -1)
            {
                if(errno == EINTR)
                    continue;

                if(errno == EAGAIN)
                    break;
            }

            throw IOError( PT_ERROR_MSG("pipe read failed") );
        }
    }

    try
    {
        avail += _avail.size();
        if(avail > 0)
            result.setDevice();

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

    return;
}


void MainLoopImpl::wake()
{
    ::write( _wakePipe[1], "W", 1);
    ::fsync( _wakePipe[1] );
}

} //namespace System

} //namespace Pt
