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
    _waitHandles.push_back( _wakeHandle );
}

IOMonitorImpl::~IOMonitorImpl()
{ 
    std::map<HANDLE,DeviceItem*>::iterator it = _devHandleMap.begin();

    for( ; it != _devHandleMap.end(); ++it)
    {
        delete it->second->signal;
        delete it->second;
    }

    _devHandleMap.clear(); 
    _waitHandleMap.clear();
    _waitHandles.clear();
    CloseHandle( _wakeHandle );
}
 
Signal<const IOEvent&>& IOMonitorImpl::addDevice( IODevice& device, size_t waitMode )
{
    MutexLock lock( _mutex );
    
    wake();
    
    //Create a device description item.
    DeviceItem* item = new DeviceItem();    
    
    item->signal = new Signal<const IOEvent&>();
    item->device = &device;       
    
    device.impl()->eventHandles( item->waitHandles, waitMode );
        
    //Initialize the device handle / device item map.
    _devHandleMap.insert( std::make_pair( device.impl()->deviceHandle(), item ) );
    
    //Initialize the wait handle / device item map.    
    for( size_t i = 0; i < item->waitHandles.size(); ++i )
    {
       _waitHandleMap.insert( std::make_pair( item->waitHandles[i] , item ) );
       _waitHandles.push_back( item->waitHandles[i] );
    }    
   
    return *(item->signal);
}

void IOMonitorImpl::removeDevice( IODevice& device )
{
    MutexLock lock( _mutex );

    wake();
    
    //Obtain the device item.
    DeviceItem* item = _devHandleMap[ device.impl()->deviceHandle() ];
    
    std::vector<HANDLE>::iterator it;
    std::vector<HANDLE>::iterator itSearch;
    
    for( it = item->waitHandles.begin(); it != item->waitHandles.end(); ++it )
    {       
        //Remove the wait Handles from the handle array.
        for( itSearch = _waitHandles.begin(); itSearch != _waitHandles.end(); ++itSearch )
        {
            if( *it != *itSearch)
                continue;
                
            itSearch = _waitHandles.erase( itSearch );
            
            if( itSearch == _waitHandles.end() )
                break;
        }
        
         //Remove the wait handles from waitHandleMap.
        _waitHandleMap.erase( *it );
    }    

    delete item->signal;

    //Remove the device from the device handle map.
    _devHandleMap.erase(  device.impl()->deviceHandle() );              
    
    delete item;
}

bool IOMonitorImpl::wait( unsigned int msecs )
{      
    if( msecs == IOMonitor::WaitInfinite )
        msecs = INFINITE;
        
    DWORD result = WaitForMultipleObjects( _waitHandles.size(), &_waitHandles[0], false, msecs );
    
    MutexLock lock( _mutex );        
    
    //Check for timeout. 
    if( result == WAIT_TIMEOUT )
    {               
        //IOTimeout ev; 
        //std::map<HANDLE,DeviceItem*>::iterator it = _devHandleMap.begin();
        
        //for( ; it != _devHandleMap.end(); it++ )
        //    it->second->signal->send( ev );
        
        return false;
    }
        
    //Check for wake up. 
    const Pt::ssize_t handleIndex  = ( result - WAIT_OBJECT_0 );        
        
    if( handleIndex == InternalWake )
        return true;        
             
    //Check for devices.             
    try
    {  
        const HANDLE waitHandle = _waitHandles[ handleIndex ];
        
        DeviceItem* item = _waitHandleMap[ waitHandle ];
        
        switch( item->device->impl()->waitResult( waitHandle ) )
        {
            case IODeviceImpl::ReadyRead:
            {
                ReadEvent ev( *item->device );                    
                item->signal->send( ev );    
            }
            break;
            case IODeviceImpl::ReadyWrite:
            {
                WriteEvent ev( *item->device );                    
                item->signal->send( ev );                    
            }
            break;
        }            
                
        item->device->impl()->resetEvent( waitHandle );
        
     }
     catch(const std::exception& e )
     {
        std::cerr<< e.what()<<std::endl;
     }
     
    return true;
}

void IOMonitorImpl::wake()
{
    SetEvent( _wakeHandle ); 
}

}//namespace System
}//namespace Pt
