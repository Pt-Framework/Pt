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
#include "SelectorImpl.h"
#include "IODeviceImpl.h"
#include "Pt/System/IOChannel.h"
#include "Pt/System/Selector.h"
#include <algorithm>


namespace Pt {

namespace System {

SelectorImpl::SelectorImpl()
{ 
    _wakeHandle = CreateEvent( NULL, FALSE, FALSE, NULL );    
}

SelectorImpl::~SelectorImpl()
{ 
    CloseHandle( _wakeHandle );
}
 
void SelectorImpl::addChannel( IOChannel& channel )
{
    _channels.push_back(&channel);
}

void SelectorImpl::removeChannel( IOChannel& channel )
{
	_channels.erase( std::remove(_channels.begin(), _channels.end(), &channel),
	                 _channels.end() );
}

void SelectorImpl::collectWaitHandles(std::vector<HANDLE>& waitHandles)
{
    std::vector<HANDLE>                 currentHandles;
    std::vector<IOChannel*>::iterator    it;
    std::vector<HANDLE>::iterator       currentHandlesIt;
    
    _channelMap.clear();

    waitHandles.push_back(_wakeHandle);
    
    for (it = _channels.begin(); it != _channels.end(); ++it)
    {
		IOChannel& channel = **it;
		IODevice& device = channel.device();
		
        if ( !device.waitable() )
            continue;       
        
        currentHandles.clear();

        device.impl()->beginWait( channel.waitMode() );

        device.impl()->eventHandles( currentHandles, channel.waitMode() );

        for (currentHandlesIt = currentHandles.begin(); currentHandlesIt != currentHandles.end(); ++currentHandlesIt)
        {
            _channelMap[*currentHandlesIt] = &channel;
            waitHandles.push_back(*currentHandlesIt);
        }
    }
}

bool SelectorImpl::areNonWaitableDevicesAvailable()
{
    std::vector<IOChannel*>::iterator it;    
    
    bool available = false;
    try
    { 
        for (it = _channels.begin(); it != _channels.end(); ++it)
        {
			IOChannel& channel = **it;
			IODevice& device = channel.device();

            if( device.waitable() )
                continue;
            
            available = true;

            if (channel.waitMode() & IOChannel::WaitInput)
            {
                channel.inputReady();    
            }
            if (channel.waitMode() & IOChannel::WaitOutput)
            {
                channel.outputReady();
            }            
        }
    }
    catch(const std::exception& e )
    {
        std::cerr<< e.what()<<std::endl;
    }

    return available;
}

void SelectorImpl::sendEvents(const HANDLE activeHandle)
{
    try
    {
        IOChannel& activeChannel = *( _channelMap[activeHandle] );
		IODevice& activeDevice = activeChannel.device();

        switch( activeDevice.impl()->waitResult( activeHandle ) )
        {
            case IODeviceImpl::ReadyRead:
            {                   
                activeChannel.inputReady(); 
                break;   
            }

            case IODeviceImpl::ReadyWrite:
            {                   
                activeChannel.outputReady();
                break;           
            }
        }            
                
        activeDevice.impl()->endWait( activeHandle );
        
     }
     catch(const std::exception& e )
     {
        std::cerr<< e.what()<<std::endl;
     }
}

bool SelectorImpl::wait( unsigned int msecs )
{   
    DWORD               result = 0;
    std::vector<HANDLE> waitHandles;

    if( msecs == Selector::WaitInfinite ) {
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

void SelectorImpl::wake()
{
    SetEvent( _wakeHandle ); 
}

}//namespace System
}//namespace Pt
