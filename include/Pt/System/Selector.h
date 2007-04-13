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

        A Selector can be used to monitor a set of IODevices and Timers
        and wait for activity on them. The wait call can be performed with
        a timeout and the respective timeout signal is sent if it occurs.
        Clients can be notified about Timer and IODevice activity by
        connecting to the appropriate signals of the Timer and IODevice
        classes. A Selector is the heart of the event loops in Pt and
        the event loop and application classes provide the same API
        as the Selector itself.
    */
    class PT_SYSTEM_API Selector : public Connectable, public NonCopyable
    {
        public:
            static const unsigned int WaitInfinite = static_cast<size_t>(-1);

            /** @brief Wait for input or output

                The WaitMode is passed to Selector::wait to indicate
                if to wait for input, output or both.
            */
            enum WaitMode
            {
                WaitInput  = 1,
                WaitOutput = 2
            };

            //! @brief Default constructor
            Selector();

            //! @brief Destructor
            virtual ~Selector();

            /** @brief Adds an IODevice

                Adds an IODevice to the selector. IODevices are removed
                automatically when they get destroyed.

                @param device The device to add
                @param wm WaitMode used for the device
            */
            void addDevice( IODevice& dev, WaitMode wm );

            /** @brief Removes an IODevice

                @param device The device to remove
            */
            void removeDevice( IODevice& dev );

            /** @brief Adds a Timer

                Adds a Timer to the selector. Timers are removed
                automatically when they get destroyed.

                @param timer The device to add
            */
            void addTimer(Timer& timer);

            /** @brief Removes a Timer

                @param timer The timer to remove
            */
            void removeTimer( Timer& timer );

            /** @brief Wait for activity

                This method will wait for activity on the registered
                IODevices and Timers. Use Selector::WaitInfinite to
                wait without timeout.

                @param msecs timeout in miliseconds
            */
            bool wait(unsigned int msecs = WaitInfinite);

            /** @brief Wakes the selctor from waiting

                This method can be used to end a Selector::wait call
                before the timeout expires. It is supposed to be used from
                another thread and thus is thread-safe.
            */
            void wake();

            /** @brief Notifies about wait timeouts
                This signal is send when the timeout given to a wait
                call of the selector expires and no activity occured.
            */
            Signal<> timeout;

        private:
            /** @brief Update all registered timers
                @internal
                @param timeout interval to next expiring timer
                @return true if a timer fired
            */
            bool updateTimer(size_t& timeout);

        private:
            //! @internal
            class SelectorImpl* _impl;

            //! @internal
            std::list<Timer*> _timers;
    };

} //namespace System

} //namespace Pt

#endif
