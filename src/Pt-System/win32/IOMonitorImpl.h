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
#ifndef PT_SYSTEM_IOMONITORIMPL_H
#define PT_SYSTEM_IOMONITORIMPL_H

#include <Pt/Signal.h>
#include <Pt/System/IOEvent.h>
#include <windows.h>

namespace Pt{
namespace System{

class IODeviceImpl;

class IOMonitorImpl
{
    public:
        IOMonitorImpl();
        ~IOMonitorImpl();
        
        Signal<const IOEvent&>& addDevice( IODeviceImpl& device );
        void removeDevice( IODeviceImpl& device );
        void wait();
        void wake();    
    
    private:     
    
        enum{ InternalWake = 0 };
        struct DeviceItem
        {
            IODeviceImpl*               device;
            Signal<const IOEvent&>*     signal;
        };
        
        std::map<HANDLE,DeviceItem> _devices;
        std::vector<HANDLE>         _waitHandles;
        HANDLE                      _wakeHandle;
};

}//namespace System 
}//namespace Pt

#endif
