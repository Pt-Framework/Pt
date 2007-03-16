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
 
Signal<const IOEvent&>& IOMonitorImpl::addDevice( IODeviceImpl& device )
{
    MutexLock lock( _mutex );
    
    wake();
    
    //Create a device description item.
    DeviceItem* item = new DeviceItem();    
    
    item->signal     = new Signal<const IOEvent&>();
    item->device     = &device;       

    device.eventHandles( item->waitHandles );
        
    //Initialize the device handle / device item map.
    _devHandleMap.insert( std::make_pair( device.deviceHandle(), item ) );
    
    //Initialize the wait handle / device item map.    
    for( size_t i = 0; i < item->waitHandles.size(); ++i )
    {
       _waitHandleMap.insert( std::make_pair( item->waitHandles[i] , item ) );
       _waitHandles.push_back( item->waitHandles[i] );
    }    
   
    return *(item->signal);
}

void IOMonitorImpl::removeDevice( IODeviceImpl& device )
{
    MutexLock lock( _mutex );

    wake();
    
    //Obtain the device item.
    DeviceItem* item = _devHandleMap[ device.deviceHandle() ];
    
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
    _devHandleMap.erase(  device.deviceHandle() );              
    
    delete item;
}

void IOMonitorImpl::wait()
{      
    DWORD result = WaitForMultipleObjects( _waitHandles.size(), &_waitHandles[0], false, INFINITE );
    
    const Pt::ssize_t   handleIndex  = ( result - WAIT_OBJECT_0 );
    
    MutexLock lock( _mutex );        
        
    if( handleIndex != InternalWake )
    {            
        try
        {  
            const HANDLE waitHandle = _waitHandles[ handleIndex ];
            
            DeviceItem*         item         = _waitHandleMap[ waitHandle ];
            const IOEvent&      ev           = item->device->event( waitHandle );
            item->signal->send( ev );    
            
            item->device->resetEvent( waitHandle );

         }
         catch(const std::exception& e )
         {
            std::cerr<< e.what()<<std::endl;
         }
    }
}

void IOMonitorImpl::wake()
{
    SetEvent( _wakeHandle ); 
}

}//namespace System
}//namespace Pt
