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
#ifndef PT_SYSTEM_IOMONITOR_H
#define PT_SYSTEM_IOMONITOR_H

#include <Pt/System/IODevice.h>
#include <Pt/System/IOEvent.h>
#include <Pt/Signal.h>

namespace Pt{
namespace System{

class PT_SYSTEM_API IOMonitor
{
    public:
        IOMonitor();
        virtual ~IOMonitor();
        
        
        Signal<const IOEvent&>&  addDevice( IODevice& device );
        void removeDevice( IODevice& device );
        void wait();
        void wake();

    private:
        class IOMonitorImpl* _impl;
};

} //namespace System
} //namespace Pt

#endif
