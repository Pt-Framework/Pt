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
#include "IOMonitorImpl.h"
#include "IODeviceImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/IOChannel.h"
#include "Pt/System/Selector.h"

#include <cerrno>
#include <iostream>


namespace Pt {

namespace System {

IOMonitorImpl::IOMonitorImpl()
{
    //Open a pipe to send wake up message.
    if( ::pipe( _wakePipe ) )
        throw std::runtime_error("Could not open pipe." + PT_SOURCEINFO);
}


IOMonitorImpl::~IOMonitorImpl()
{
    if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
    {
        ::close(_wakePipe[0]);
        ::close(_wakePipe[1]);
    }
}


void IOMonitorImpl::addChannel( IOChannel& channel )
{
    const int fd = channel.device().impl()->fd();
    _channels.insert( std::make_pair( fd, &channel ) );
}


void IOMonitorImpl::removeChannel( IOChannel& channel )
{
    _channels.erase( channel.device().impl()->fd() );
}


bool IOMonitorImpl::wait(unsigned int msecs)
{
    int maxfd   = 0;
    bool avail  = false;
    fd_set rfds;
    FD_ZERO(&rfds);
    fd_set wfds;
    FD_ZERO(&wfds);

    // The pipe to wake select is always passed to select
    FD_SET( _wakePipe[0], &rfds );
    maxfd = _wakePipe[0];

    // Add all waitable devices to the read and write descriptor
    // sets. Not waitable devices are handled differently.
    std::map<int, IOChannel*>::iterator it;
    for( it = _channels.begin(); it != _channels.end(); ++it )
    {
        IOChannel& channel = *it->second;
        IODevice& device = it->second->device();

        if( device.waitable() == false )
            continue;

        int fd = it->first;

        if( channel.waitMode() & IOChannel::WaitInput)
        {
            FD_SET( fd, &rfds );
            maxfd = std::max( maxfd , fd );
        }

        if( channel.waitMode() & IOChannel::WaitOutput )
        {
            FD_SET( fd, &wfds );
            maxfd = std::max( maxfd , fd );
        }
    }

    // The first select checks if any data is immediately available
    // on the waitable devices, therefore no timeout for select. This
    // way waitable devices get a chance to be serviced too when a
    // non-waitable device is registered as well
    avail = this->select(maxfd, rfds, wfds, 0);

    // Now we service all devices that are not waitable and thus
    // have always data available
    for( it = _channels.begin(); it != _channels.end(); ++it )
    {
        IOChannel& channel = *it->second;
        IODevice& device = it->second->device();

        if( device.waitable() )
            continue;

        avail = true;
        if( channel.waitMode() & IOChannel::WaitInput)
        {
            channel.inputReady();
        }
        if( channel.waitMode() & IOChannel::WaitOutput)
        {
            channel.outputReady();
        }
    }

    // if any not-waitable devices were present we can bail
    // out here and report activity
    if(avail)
        return true;

    // The second select waits until the timeout expires
    // or a waitable device becomes available
    return this->select(maxfd, rfds, wfds, msecs);
}


void IOMonitorImpl::wake()
{
    ::write( _wakePipe[1], "X", 1);
    ::fsync( _wakePipe[1] );
}


bool IOMonitorImpl::select(int maxfd, fd_set rfds, fd_set wfds, unsigned int msecs)
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

    std::map<int, IOChannel*>::iterator it;
    for( it = _channels.begin(); it != _channels.end(); ++it )
    {
        IOChannel& channel = *it->second;
        IODevice& device = it->second->device();

        if( FD_ISSET( device.impl()->fd(), &rfds ) )
        {
            channel.inputReady();
            avail = true;
        }

        if( FD_ISSET( device.impl()->fd(), &wfds  ))
        {
            channel.outputReady();
            avail = true;
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
