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
 
void SelectorImpl::addDevice( IODevice& device, int waitMode )
{
    _items.push_back( Item(device, waitMode) );
}

void SelectorImpl::removeDevice( IODevice& device )
{
	std::vector<Item>::iterator it = _items.begin();
	while(it != _items.end())
	{
		IODevice* dev = it->device;

		if(dev == &device)
			it = _items.erase(it);
		else
			++it;
	}
}

void SelectorImpl::collectWaitHandles(std::vector<HANDLE>& waitHandles)
{
    std::vector<HANDLE>           currentHandles;
    std::vector<Item>::iterator   it;
    std::vector<HANDLE>::iterator currentHandlesIt;
    
    _itemMap.clear();

    waitHandles.push_back(_wakeHandle);
    
    for (it = _items.begin(); it != _items.end(); ++it)
    {
		int waitMode = it->waitMode;
		IODevice& device = *it->device;
		
        if ( !device.waitable() )
            continue;       
        
        currentHandles.clear();

        device.impl()->beginWait( waitMode );

        device.impl()->eventHandles( currentHandles, waitMode );

        for (currentHandlesIt = currentHandles.begin(); currentHandlesIt != currentHandles.end(); ++currentHandlesIt)
        {
            _itemMap[*currentHandlesIt] = *it;
            waitHandles.push_back(*currentHandlesIt);
        }
    }
}

bool SelectorImpl::areNonWaitableDevicesAvailable()
{
    std::vector<Item>::iterator it;    
    
    bool available = false;
    try
    { 
        for (it = _items.begin(); it != _items.end(); ++it)
        {
			int waitMode = it->waitMode;
			IODevice* device = it->device;

            if( device->waitable() )
                continue;
            
            available = true;

            if (waitMode & IOChannel::WaitInput)
            {
                device->inputReady();    
            }
            if (waitMode & IOChannel::WaitOutput)
            {
                device->outputReady();
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
        Item& activeItem = _itemMap[activeHandle];
		IODevice* activeDevice = activeItem.device;

        switch( activeDevice->impl()->waitResult( activeHandle ) )
        {
            case IODeviceImpl::ReadyRead:
            {                   
                activeDevice->inputReady(); 
                break;   
            }

            case IODeviceImpl::ReadyWrite:
            {                   
                activeDevice->outputReady();
                break;           
            }
        }            
                
        activeDevice->impl()->endWait( activeHandle );
        
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
