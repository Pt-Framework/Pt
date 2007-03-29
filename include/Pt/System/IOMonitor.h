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

/** @brief The IOMonitor implements the wait functionality for a device event.

    On the IOMonitor can one or more devices registered for event monitoring. 
    At the registration time the client gets a signal object specific for this device.
    This signal is emited if on the registered device an event occurred.
    The registration, device removing, wait and wake are thread save. The client 
    can implements an event loop by using the wait and wake methodes. The wait 
    method waits until an IO event on device occurred. The wake methode call, 
    signalize the IOMonitor to wake up from the wait state. The wake call 
    doesn't emit an event.
*/
class PT_SYSTEM_API IOMonitor
{
    public:
        static const unsigned int WaitTimeInfinite = static_cast<unsigned int>(-1);

        //! @brief Default constructor
        IOMonitor();
        
        //! @brief Destructor
        virtual ~IOMonitor();

        /** @brief Adds a device to the monitor
            
            @param device The device to add
            @return A signal which signalize the device events            
        */
        Signal<const IOEvent&>& addDevice( IODevice& device );

        /** @brief Removes a device from the monitor
            
            @param device The device to remove
        */
        void removeDevice( IODevice& device );

        //! @brief Wait until an event occurred
        bool wait(unsigned int msecs = WaitTimeInfinite);
        
        //! @brief Wake the monitor from wait state
        void wake();

    private:
        class IOMonitorImpl* _impl;
};

} //namespace System

} //namespace Pt

#endif
