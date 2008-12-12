/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
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

    std::list<IOResultImpl*>::iterator it;
    for( it = _results.begin(); it != _results.end(); ++it )
    {
        (*it)->setSelector(0);
    }
}


void SelectorImpl::complete( IOResult& result )
{
    // insert at the front so that the added result wont be checked
    // if it is added from a IODevice::inputReady slot
    _results.push_front( result.impl() );
}


void SelectorImpl::cancel(IOResult& result)
{
    std::list<IOResultImpl*>::iterator it;
    for( it = _results.begin(); it != _results.end(); ++it )
    {
        if(&result == *it)
        {
            (*it)->setSelector(0);
            _results.erase(it);
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
    std::list<IOResultImpl*>::iterator iter;
    for( iter = _results.begin(); iter != _results.end(); ++iter )
    {
        IOResultImpl* result = *iter;
        int fd = result->impl()->fd();

        result->add(rfds, wfds);
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

    // we use a list so that removal of IOResults in a slot connected to
    // IODevice::inputReady doesn't invalidate the iterator. If a slot
    // adds IOResults they are pushed to front and we dont process them here
    std::list<IOResultImpl*>::iterator iter;
    for( iter = _results.begin(); iter != _results.end();  )
    {
        IOResult* result = *iter;
        int fd = result->impl()->fd();

        if( FD_ISSET(fd, &rfds) )
        {
            result->setSelector(0);
            iter = _results.erase(iter);
            result->device()->inputReady(*result);
            avail = true;
            continue;
        }
        else if( FD_ISSET(fd, &wfds) )
        {
            result->setSelector(0);
            iter = _results.erase(iter);
            result->device()->outputReady(*result);
            avail = true;
            continue;
        }

        ++iter;
    }

    if( FD_ISSET( _wakePipe[0], &rfds ) )
    {
        char msgbuf[10];
        ::read( _wakePipe[0], msgbuf, 10 );
        avail = true;
    }

    return avail;
}

} //namespace System

} //namespace Pt
