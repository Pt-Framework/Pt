/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#include "Pt/System/IODevice.h"
#include "Pt/System/Selector.h"

#include <cerrno>
#include <iostream>

#ifdef __QNX__
#include <unistd.h>
#endif

namespace Pt {

namespace System {

SelectorImpl::SelectorImpl()
{
    //Open a pipe to send wake up message.
    if( ::pipe( _wakePipe ) )
        throw std::runtime_error("Could not open pipe." + PT_SOURCEINFO);
}


SelectorImpl::~SelectorImpl()
{
    if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
    {
        ::close(_wakePipe[0]);
        ::close(_wakePipe[1]);
    }
}


void SelectorImpl::complete( IOResult& result )
{
    _readers.push_back( result.impl() );
}


void SelectorImpl::cancel( IOResult& result )
{
    std::vector<IOResultImpl*>::iterator it;
    for( it = _readers.begin(); it != _readers.end(); ++it )
    {
        if(&result == *it)
        {
            _readers.erase(it);
            return;
        }
    }
}


bool SelectorImpl::wait(unsigned int msecs)
{
    int maxfd   = 0;
    fd_set rfds;
    FD_ZERO(&rfds);
    fd_set wfds;
    FD_ZERO(&wfds);

    // The pipe to wake select is always passed to select
    FD_SET( _wakePipe[0], &rfds );
    maxfd = _wakePipe[0];

    // Add all waitable handles to the read descriptor sets.
    // Not waitable handles are handled differently later
    std::vector<IOResultImpl*>::iterator iter;
    for( iter = _readers.begin(); iter != _readers.end(); ++iter )
    {
        IOResultImpl* result = *iter;
        int fd = result->impl()->fd();

        result->add(rfds, wfds);
        //FD_SET( fd, &rfds );
        maxfd = std::max( maxfd , fd );
    }

    // The second select waits until the timeout expires
    // or a waitable device becomes available
    return this->select(maxfd, rfds, wfds, msecs);
}


void SelectorImpl::wake()
{
    ::write( _wakePipe[1], "X", 1);
    ::fsync( _wakePipe[1] );
}


bool SelectorImpl::select(int maxfd, fd_set rfds, fd_set wfds, unsigned int msecs)
{
    bool avail  = false;
    int ret     = -1;

    timeval* timeout = 0;
    struct   timeval tv;
    if(msecs != Selector::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    while( true )
    {
        ret = ::select( maxfd + 1, &rfds, &wfds, 0, timeout );
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( "Could not select on file descriptors", PT_SOURCEINFO );
    }

    //Do not use iterators here since clients may insert new IOResults during the inputReady signal!
    size_t size = _readers.size();
    for( size_t n = 0; n < size; )
    {
        IOResult* result = _readers[n];
        int fd = result->impl()->fd();

        if( FD_ISSET(fd, &rfds) )
        {
            result->device()->inputReady(*result);
            _readers.erase(_readers.begin() + n);
            size--;
            avail = true;
        }
        else if( FD_ISSET(fd, &wfds) )
        {
            result->device()->outputReady(*result);
            _readers.erase(_readers.begin() + n);
            size--;
            avail = true;
        }
        else
        {
            ++n;
        }
    }

    if( FD_ISSET( _wakePipe[0], &rfds ) )
    {
        std::vector<char> msgbuf(10);
        read( _wakePipe[0], &msgbuf[0], msgbuf.size() );
        avail = true;
    }

    return avail;
}

} //namespace System

} //namespace Pt
