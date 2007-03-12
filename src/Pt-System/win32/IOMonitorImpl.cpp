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

namespace Pt{
namespace System{

IOMonitorImpl::IOMonitorImpl()
{ 
    _wakeHandle = CreateEvent( NULL, FALSE, FALSE, NULL );
    _waitHandles.push_back( _wakeHandle );
}

IOMonitorImpl::~IOMonitorImpl()
{ 
    std::map<HANDLE,DeviceItem>::iterator it = _devices.begin();

    for( ; it != _devices.end(); ++it)
        delete it->second.signal;

    _devices.clear(); 
    _waitHandles.clear();
    CloseHandle( _wakeHandle );
}
 
const Signal<const IOEvent&>& IOMonitorImpl::addDevice( IODeviceImpl& device )
{
    DeviceItem item;
    
    item.signal = new Signal<const IOEvent&>();
    item.device = &device;
    
    _devices.insert( std::make_pair( device.handle(), item ) );
    _waitHandles.push_back( device.handle() );
    
    return *item.signal;
}

void IOMonitorImpl::removeDevice( IODeviceImpl& device )
{
    DeviceItem& item = _devices[ device.handle() ];
    delete item.signal;
    
    _devices.erase( device.handle() );
    
    std::vector<HANDLE>::iterator it = _waitHandles.begin();
    
    for( ; it != _waitHandles.end(); ++it )
    {
        if( *it != device.handle() )
            continue;
        
        _waitHandles.erase( it );            
        break;        
    }
}

void IOMonitorImpl::wait()
{      
    DWORD result = WaitForMultipleObjects( _waitHandles.size(), &_waitHandles[0], false, INFINITE );
    
    const Pt::ssize_t   handleIndex  = ( result - WAIT_OBJECT_0 );
    DeviceItem&         item         = _devices[ _waitHandles[ handleIndex ] ];
    const IOEvent&      ev           = item.device->waitEvent();
    
    item.signal->send( ev );    
}

void IOMonitorImpl::wake()
{
    SetEvent( _wakeHandle ); 
}

}//namespace System
}//namespace Pt
