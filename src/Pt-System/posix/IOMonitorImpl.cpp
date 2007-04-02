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

    //Clear the file descriptors.
    FD_ZERO(&_rfds);
    FD_ZERO(&_wfds);

    //Add the wake pipe to the rdfs.
    FD_SET( _wakePipe[0], &_rfds );
}


IOMonitorImpl::~IOMonitorImpl()
{
    //Clear the map.
    std::map<int, DeviceItem>::iterator it = _deviceMap.begin();

    for( ; it != _deviceMap.end(); ++it )
    {
        const DeviceItem& item = it->second;
        delete item.signal;
    }

    _deviceMap.clear();

    //Close the pipe.
    if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
    {
        ::close(_wakePipe[0]);
        ::close(_wakePipe[1]);
    }
}


Signal<const IOEvent&>& IOMonitorImpl::addDevice( IODevice& device, size_t waitMode )
{
    //Exclusive access to the _deviceMap.
    MutexLock lock( _mutex );

    //Wake up the monitor => it will wait on _mutex.
    this->wake();

    //Create a new device description item.
    DeviceItem item;
    item.signal     = new Signal<const IOEvent&>();
    item.device     = &device;
    item.waitMode   = waitMode;

    //Insert the new item to the device description map.
    const int fd = device.impl()->fd();
    _deviceMap.insert( std::make_pair( fd, item ) );

    //Return the new signal.
    return *item.signal;
}

void IOMonitorImpl::removeDevice( IODevice& device )
{
    MutexLock lock( _mutex );

    //Wake up the monitor.
    this->wake();

    //Obtain the device item.
    DeviceItem& item = _deviceMap[ device.impl()->fd() ];

    //Delete the device signal. 
    delete item.signal;

    //Remove the device item from the map.
    _deviceMap.erase( device.impl()->fd() );
}

bool IOMonitorImpl::wait(unsigned int msecs)
{
    int maxfd   = 0;
    int ret     = -1;
    bool avail  = false;

    fd_set rfds;
    fd_set wfds;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    //Add the wake pipe to the rdfs.
    FD_SET( _wakePipe[0], &rfds );
    maxfd = _wakePipe[0];

    //Add the devices to the rfds and wfds
    std::map<int, DeviceItem>::iterator it = _deviceMap.begin();

    for( ; it != _deviceMap.end(); ++it )
    {
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

    //Setup the timeout. 
    timeval* timeout = 0;
    struct   timeval tv;

    if(msecs != IOMonitor::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    //Execute the select.
    while( true )
    {
        ret = ::select( maxfd, &rfds, &wfds, 0, timeout );

        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( "Could not select on file descriptors", PT_SOURCEINFO );
    }

    //Exclusive access to the _deviceMap and device descriptors.
    MutexLock lock( _mutex );

    //Check the the wake up reason.
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

    //Reset the wake pipe.
    if( FD_ISSET( _wakePipe[0], &rfds ) )
    {
        std::vector<char> msgbuf(100);
        read( _wakePipe[0], &msgbuf[0], msgbuf.size() );
        printf("wake\n");
        avail = true;
    }

    return avail;
}

void IOMonitorImpl::wake()
{
    ::write( _wakePipe[1], "XXXXXXXXXXX", 11);
    Thread::yield();
}

}//namespace System
}//namespace Pt
