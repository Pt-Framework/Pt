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
#ifndef PT_SYSTEM_SELECTOR_H
#define PT_SYSTEM_SELECTOR_H

#include <Pt/Signal.h>
#include <Pt/Timespan.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <map>

namespace Pt {

namespace System {

    class Timer;
    class Selectable;
    class Application;
    class SelectorImpl;

    /** @brief Reports activity on a set of devices.

        A Selector can be used to monitor a set of Selectables and Timers
        and wait for activity on them. The wait call can be performed with
        a timeout and the respective timeout signal is sent if it occurs.
        Clients can be notified about Timer and Selectable activity by
        connecting to the appropriate signals of the Timer and Selectable
        classes.

        The following example uses a %Selector to wait on acitvity on
        a %Timer, which is set to time-out after 1000 msecs.

        @code
        // slot to handle timer activity
        void onTimer();

        int main()
        {
            using cxxtools::System;

            Timer timer;
            timer.start(1000);
            connect(timer.timeout, ontimer);

            Selector selector;
            selector.addTimer(timer);
            selector wait();

            return 0;
        }
        @endcode

        A Selector is the heart of the EventLoop, which calls Selector::wait
        continously. The %EventLoop and %Application classes provide the same API
        as the Selector itself.
    */
    class PT_SYSTEM_API SelectorBase : public Connectable
                                     , public NonCopyable
    {
        friend class Selectable;
        friend class Timer;

        public:
            static const unsigned int WaitInfinite = static_cast<const unsigned int>(-1);

            //! @brief Destructor
            virtual ~SelectorBase();

            /** @brief Adds an IOResult

                Adds an IOResult to the selector. IOResult are removed
                automatically when they get destroyed.
            */
            void add(Selectable& s);

            /** @brief Cancel an IOResult.
            */
            void remove(Selectable& s);

            /** @brief Adds a Timer

                Adds a Timer to the selector. Timers are removed
                automatically when they get destroyed.

                @param timer The device to add
            */
            void add(Timer& timer);

            /** @brief Removes a Timer

                @param timer The timer to remove
            */
            void remove(Timer& timer);

            /** @brief Wait for activity

                This method will wait for activity on the registered
                Selectables and Timers. Use Selector::WaitInfinite to
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

        protected:
            //! @brief Default constructor
            SelectorBase();

            void onAddTimer(Timer& timer);

            void onRemoveTimer( Timer& timer );

            void onTimerChanged( Timer& timer );

            virtual void onAdd(Selectable&) = 0;

            virtual void onRemove(Selectable&) = 0;

            virtual void onChanged(Selectable& s) = 0;

            virtual bool onWait(unsigned int msecs) = 0;

            virtual void onWake() = 0;

        private:
            /** @internal Update all timers and return true if a timer fired

                @param timeout interval to next expiring timer
            */
            bool updateTimer(size_t& timeout);

            //! @internal
            std::multimap<Timespan, Timer*> _timers;

            void* _reserved;
    };

    class PT_SYSTEM_API Selector : public SelectorBase
    {
        public:
            Selector();

            virtual ~Selector();

            void setParent(Application* app);

            SelectorImpl& impl();

        protected:
            void onAdd( Selectable& dev );

            void onRemove( Selectable& dev );

            void onChanged(Selectable&);

            bool onWait(unsigned int msecs = WaitInfinite);

            void onWake();

        private:
            //! @internal
            class SelectorImpl* _impl;
    };

} //namespace System

} //namespace Pt

#endif
