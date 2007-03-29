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
    std::map<int,DeviceItem>::iterator it = _deviceMap.begin();

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


Signal<const IOEvent&>& IOMonitorImpl::addDevice( IODeviceImpl& device )
{
    //Exclusive access to the _deviceMap.
    MutexLock lock( _mutex );

    //Wake up the monitor => it will wait on _mutex.
    this->wake();

    //Create a new device description item.
    DeviceItem item;

    //Create a new signal.
    item.signal = new Signal<const IOEvent&>();
    item.device = &device;

    //Insert the new item to the device description map.
    const int fd = device.fd();

    _deviceMap.insert( std::make_pair( fd, item ) );

    //Set the bit in the device descriptor set.
/*
    const std::ios_base::openmode mode = device.mode();

    if( ( mode & std::ios_base::in )  == std::ios_base::in )
       FD_SET( fd, &_rfds );

    if( ( mode & std::ios_base::out )  == std::ios_base::out )
       FD_SET( fd, &_wfds );
*/
    //Return the new signal.
    return *item.signal;
}


int IOMonitorImpl::maxFd()
{
    //Determinate the max device descriptor.
    std::map<int,DeviceItem>::iterator it = _deviceMap.begin();
    int maxfd = -1;

    for( ; it != _deviceMap.end(); ++it )
    {
        const DeviceItem& item = it->second;
        maxfd = std::max( maxfd , item.device->fd() );
    }

    return std::max( maxfd, _wakePipe[0] );
}


void IOMonitorImpl::removeDevice( IODeviceImpl& device )
{
    MutexLock lock( _mutex );

    //Wake up the monitor.
    this->wake();

    //Obtain the device item.
    DeviceItem& item = _deviceMap[ device.fd() ];

    //Delete the device signal. 
    delete item.signal;

    //Clear the bit for the device descriptor.
    const std::ios_base::openmode mode = device.mode();

    if( ( mode & std::ios_base::in )  == std::ios_base::in )
       FD_CLR( device.fd(), &_rfds );

    if( ( mode & std::ios_base::out )  == std::ios_base::out )
       FD_CLR( device.fd(), &_wfds  );

    //Remove the device item from the map.
    _deviceMap.erase( device.fd() );
}


bool IOMonitorImpl::wait(unsigned int msecs)
{
    int maxfd   = maxFd() + 1;
    int ret     = -1;

    fd_set rfds;
    fd_set wfds;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    //Add the wake pipe to the rdfs.
    FD_SET( _wakePipe[0], &rfds );

    std::map<int,DeviceItem>::iterator it = _deviceMap.begin();
    for( ; it != _deviceMap.end(); ++it )
    {
        int fd = it->first;
        FD_SET(fd, &rfds);
    }
    
    timeval* timeout = 0;
    struct timeval tv;
    if(msecs != IOMonitor::WaitTimeInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }
    
    //Execute the select.
    while( true )
    {
        ret = ::select( FD_SETSIZE, &rfds, &wfds, 0, timeout );

        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( "Could not select on file descriptors", PT_SOURCEINFO );
    }

    //Exclusive access to the _deviceMap and device descriptors.
    MutexLock lock( _mutex );

    //Select returned => test why and send an event if necessary.
    it = _deviceMap.begin();
    std::ios_base::openmode mode;

    bool avail = false;
    for( ; it != _deviceMap.end(); ++it )
    {
        const DeviceItem& item = it->second;
        mode = item.device->mode();

        if( ( mode & std::ios_base::in )  == std::ios_base::in )
        {
            if( FD_ISSET( item.device->fd(), &rfds ) )
            {
                item.signal->send( item.device->event( IODeviceImpl::ReadFds ) ) ;
                avail = true;
            }
        }

        if( ( mode & std::ios_base::out )  == std::ios_base::out )
        {
           if( FD_ISSET( item.device->fd(), &wfds  ))
           {
                item.signal->send( item.device->event( IODeviceImpl::WriteFds ) );
                avail = true;
           }
        }
    }

    //Reset the wake pipe.
    if( FD_ISSET( _wakePipe[0], &rfds ) )
    {
        std::vector<char> msgbuf(100);
        read( _wakePipe[0], &msgbuf[0], msgbuf.size() );
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
