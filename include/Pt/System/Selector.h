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

#include <Pt/NonCopyable.h>
#include <Pt/Signal.h>
#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <list>


namespace Pt {

namespace System {

    class Timer;

    /** @brief Reports activity on a set of devices.

        On the IOMonitor can one or more devices registered for event monitoring. 
        At the registration time the client gets a signal object specific for this device.
        This signal is emited if on the registered device an event occurred.
        The registration, device removing, wait and wake are thread save. The client 
        can implements an event loop by using the wait and wake methodes. The wait 
        method waits until an IO event on device occurred. The wake methode call, 
        signalize the IOMonitor to wake up from the wait state. The wake call 
        doesn't emit an event.
    */
    class PT_SYSTEM_API Selector : public Connectable, public NonCopyable
    {
        public:
            static const unsigned int WaitInfinite = static_cast<size_t>(-1);

            enum WaitMode
            {
                WaitInput  = 1,
                WaitOutput = 2
            };

            //! @brief Default constructor
            Selector();

            //! @brief Destructor
            virtual ~Selector();

            /** @brief Adds a device to the monitor

                @param device The device to add
                @return A signal which signalize the device events
            */
            void addDevice( IODevice& dev, WaitMode wm );

            /** @brief Removes a device from the monitor

                @param device The device to remove
            */
            void removeDevice( IODevice& dev );

            void addTimer(Timer& timer);

            void removeTimer( Timer& timer );

            //! @brief Wait until an event occurred
            bool wait(unsigned int msecs = WaitInfinite);

            //! @brief Wake the monitor from wait state
            void wake();

            Signal<> timeout;

        private:
            class SelectorImpl* _impl;
            std::list<Timer*> _timers;
    };

} //namespace System

} //namespace Pt

#endif
