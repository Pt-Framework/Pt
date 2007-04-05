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
#include "Pt/System/MutexLock.h"
#include "Pt/System/IOError.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/IOMonitor.h"
#include "Pt/System/Thread.h"

#include <cerrno>
#include <iostream>

namespace Pt{

namespace System{

IOMonitorImpl::IOMonitorImpl()
{
    //Open a pipe to send wake up message.
    if( ::pipe( _wakePipe ) )
        throw std::runtime_error("Could not open pipe." + PT_SOURCEINFO);
}


IOMonitorImpl::~IOMonitorImpl()
{
    std::map<int, DeviceItem>::iterator it = _deviceMap.begin();

    for( ; it != _deviceMap.end(); ++it )
    {
        const DeviceItem& item = it->second;
        delete item.signal;
    }

    if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
    {
        ::close(_wakePipe[0]);
        ::close(_wakePipe[1]);
    }
}


Signal<const IOEvent&>& IOMonitorImpl::addDevice( IODevice& device, size_t waitMode )
{
    DeviceItem item;
    item.signal     = new Signal<const IOEvent&>();
    item.device     = &device;
    item.waitMode   = waitMode;

    const int fd = device.impl()->fd();
    _deviceMap.insert( std::make_pair( fd, item ) );

    return *item.signal;
}


void IOMonitorImpl::removeDevice( IODevice& device )
{
    DeviceItem& item = _deviceMap[ device.impl()->fd() ];
    delete item.signal;
    _deviceMap.erase( device.impl()->fd() );
}


bool IOMonitorImpl::wait(unsigned int msecs)
{
    int maxfd   = 0;
    int ret     = -1;
    bool avail  = false;
    fd_set rfds;
    FD_ZERO(&rfds);
    fd_set wfds;
    FD_ZERO(&wfds);

    // The pipe to wake a wait call is always passed to select
    FD_SET( _wakePipe[0], &rfds );
    maxfd = _wakePipe[0];

    // Add all waitable devices to the read and write descriptor
    // sets. Not waitable devices are handled differently.
    std::map<int, DeviceItem>::iterator it;
    for( it = _deviceMap.begin(); it != _deviceMap.end(); ++it )
    {
        if( it->second.device->waitable() == false )
            continue;

        int fd = it->first;

        if( (it->second.waitMode & IODevice::WaitInput) == IODevice::WaitInput )
        {
            FD_SET( fd, &rfds );
            maxfd = std::max( maxfd , fd );
        }

        if( (it->second.waitMode & IODevice::WaitOutput ) == IODevice::WaitOutput )
        {
            FD_SET( fd, &wfds );
            maxfd = std::max( maxfd , fd );
        }
    }

    // The first select checks if any data is immediately available
    // on waitable devices, therefore no timeout for select. This
    // way waitable devices get a chance to be serviced too when a
    // non-waitable device is registered as well
    struct  timeval no_timeout;
    no_timeout.tv_sec = 0;
    no_timeout.tv_usec = 0;
    while( true )
    {
        ret = ::select( maxfd+1, &rfds, &wfds, 0, &no_timeout );

        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( "Could not select on file descriptors", PT_SOURCEINFO );
    }

    // Now we service all devices that are not waitable and thus
    // have always data available
    for( it = _deviceMap.begin(); it != _deviceMap.end(); ++it )
    {
        const DeviceItem& item = it->second;

        if( item.device->waitable() )
            continue;

        avail = true;
        if( it->second.waitMode == IODevice::WaitInput)
        {
            ReadEvent ev( *item.device );
            item.signal->send( ev ) ;
        }
        if( it->second.waitMode == IODevice::WaitOutput)
        {
            WriteEvent ev( *item.device );
            item.signal->send( ev );
        }
    }

    // if any not waitable devices were present we can bail
    // out here and report activity
    if(avail)
        return true;

    timeval* timeout = 0;
    struct   timeval tv;
    if(msecs != IOMonitor::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    // The second select waits until the timeout expires
    // or a waitable device becomes available
    while( true )
    {
        ret = ::select( maxfd+1, &rfds, &wfds, 0, timeout );

        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( "Could not select on file descriptors", PT_SOURCEINFO );
    }

    for( it = _deviceMap.begin(); it != _deviceMap.end(); ++it )
    {
        const DeviceItem& item = it->second;

        if( FD_ISSET( item.device->impl()->fd(), &rfds ) )
        {
            ReadEvent ev( *item.device );
            item.signal->send( ev ) ;
            avail = true;
        }

        if( FD_ISSET( item.device->impl()->fd(), &wfds  ))
        {
            WriteEvent ev( *item.device );
            item.signal->send( ev );
            avail = true;
        }
    }

    if( FD_ISSET( _wakePipe[0], &rfds ) )
    {
        std::vector<char> msgbuf(100);
        read( _wakePipe[0], &msgbuf[0], msgbuf.size() );
        avail = true;
    }

    return avail;
}


void IOMonitorImpl::wake()
{
    ::write( _wakePipe[1], "XXXXXXXXXXX", 11);
}

}//namespace System
}//namespace Pt
