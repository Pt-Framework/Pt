/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
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
#include "Pt/System/MutexLock.h"
#include "Pt/System/ReadEvent.h"
#include "Pt/System/WriteEvent.h"
#include "Pt/System/IOTimeout.h"
#include "Pt/System/IOMonitor.h"
#include "IODeviceImpl.h"

namespace Pt{
namespace System{

IOMonitorImpl::IOMonitorImpl()
{ 
    _wakeHandle = CreateEvent( NULL, FALSE, FALSE, NULL );    
}

IOMonitorImpl::~IOMonitorImpl()
{ 
    std::vector<IOChannel>::iterator it;

    for (it = _channels.begin(); it != _channels.end(); ++it)
    {
        delete it->signal;
    }

    CloseHandle( _wakeHandle );
}
 
Signal<const IOEvent&>& IOMonitorImpl::addDevice( IODevice& device, size_t waitMode )
{
    IOChannel   channel;

    channel.signal    = new Signal<const IOEvent&>();
    channel.device    = &device; 
    channel.waitMode  = waitMode;

    _channels.push_back(channel);
   
    return *(channel.signal);
}

void IOMonitorImpl::removeDevice( IODevice& device )
{
    std::vector<IOChannel>::iterator it;

    for (it = _channels.begin(); it != _channels.end(); ++it)
    {
        if ( &device == it->device )
        {
            delete it->signal;
            _channels.erase(it);
            break;
        }
    }    
}

void IOMonitorImpl::collectWaitHandles(std::vector<HANDLE>& waitHandles)
{
    std::vector<HANDLE>                 currentHandles;
    std::vector<IOChannel>::iterator    it;
    std::vector<HANDLE>::iterator       currentHandlesIt;
    
    _channelMap.clear();

    waitHandles.push_back(_wakeHandle);
    
    for (it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (!it->device->waitable())
            continue;       
        
        currentHandles.clear();

        it->device->impl()->beginWait(it->waitMode);

        it->device->impl()->eventHandles(currentHandles, it->waitMode);

        for (currentHandlesIt = currentHandles.begin(); currentHandlesIt != currentHandles.end(); ++currentHandlesIt)
        {
            _channelMap[*currentHandlesIt] = &(*it);
            waitHandles.push_back(*currentHandlesIt);
        }
    }
}

bool IOMonitorImpl::areNonWaitableDevicesAvailable()
{
    std::vector<IOChannel>::iterator it;    
    
    bool available = false;
    try
    { 
        for (it = _channels.begin(); it != _channels.end(); ++it)
        {
            if (it->device->waitable())
                continue;
            
            available = true;

            if (it->waitMode & IODevice::WaitInput)
            {
                ReadEvent ev( *it->device );
                it->signal->send( ev );    
            }
            if (it->waitMode & IODevice::WaitOutput)
            {
                WriteEvent ev( *it->device );
                it->signal->send( ev );
            }            
        }
    }
    catch(const std::exception& e )
    {
        std::cerr<< e.what()<<std::endl;
    }

    return available;
}

void IOMonitorImpl::sendEvents(const HANDLE activeHandle)
{
    try
    {
        IOChannel* activeChannel = _channelMap[activeHandle];

        switch( activeChannel->device->impl()->waitResult( activeHandle ) )
        {
            case IODeviceImpl::ReadyRead:
            {
                ReadEvent ev( *activeChannel->device );                    
                activeChannel->signal->send( ev );    
            }
            break;
            case IODeviceImpl::ReadyWrite:
            {
                WriteEvent ev( *activeChannel->device );                    
                activeChannel->signal->send( ev );                    
            }
            break;
        }            
                
        activeChannel->device->impl()->endWait( activeHandle );
        
     }
     catch(const std::exception& e )
     {
        std::cerr<< e.what()<<std::endl;
     }
}

bool IOMonitorImpl::wait( unsigned int msecs )
{   
    DWORD               result = 0;
    std::vector<HANDLE> waitHandles;

    if( msecs == IOMonitor::WaitInfinite ) {
        msecs = INFINITE;    
    }
    
    collectWaitHandles(waitHandles);   

    result = WaitForMultipleObjects( waitHandles.size(), &waitHandles[0], false, 0 );
    
    if( result == WAIT_TIMEOUT &&
        areNonWaitableDevicesAvailable() ) 
    {    
        return true;
    }   

    result = WaitForMultipleObjects( waitHandles.size(), &waitHandles[0], false, msecs );    
    
    if( result == WAIT_TIMEOUT ) {    
        return false;
    }       
    
    const Pt::ssize_t handleIndex  = (result - WAIT_OBJECT_0);
        
    if (waitHandles[handleIndex] == _wakeHandle) {
        return true;  
    }

    sendEvents(waitHandles[ handleIndex ]);
     
    return true;
}

void IOMonitorImpl::wake()
{
    SetEvent( _wakeHandle ); 
}

}//namespace System
}//namespace Pt
