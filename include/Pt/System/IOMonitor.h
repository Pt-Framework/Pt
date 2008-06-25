#ifndef PT_SYSTEM_IOMONITOR_H
#define PT_SYSTEM_IOMONITOR_H

#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <Pt/SystemApi.h>

namespace Pt {

    class Timer;
    class Selectable;
    class IOMonitorImpl;

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
    class IOMonitor : public NonCopyable
    {
        public:
            static const unsigned int WaitInfinite = static_cast<const unsigned int>(-1);

            //! @brief Destructor
            virtual ~IOMonitor()
            {}

            /** @brief Adds an IOResult

                Adds an IOResult to the selector. IOResult are removed
                automatically when they get destroyed.
            */
            void add(Selectable& s);

            /** @brief Cancel an IOResult.
            */
            void remove(Selectable& s);

            void setDirty(Selectable& s);

            /** @brief Adds a Timer

                Adds a Timer to the selector. Timers are removed
                automatically when they get destroyed.

                @param timer The device to add
            */
            void add(Timer& timer)
            { this->onAdd(timer); }

            /** @brief Removes a Timer

                @param timer The timer to remove
            */
            void remove( Timer& timer )
            { this->onRemove(timer); }

            /** @brief Wait for activity

                This method will wait for activity on the registered
                Selectables and Timers. Use Selector::WaitInfinite to
                wait without timeout.

                @param msecs timeout in miliseconds
            */
            bool wait(unsigned int msecs = WaitInfinite)
            {
                bool ret = this->onWait(msecs);
                if(ret == false)
                    timeout.send();

                return ret;
            }

            /** @brief Wakes the selctor from waiting

                This method can be used to end a Selector::wait call
                before the timeout expires. It is supposed to be used from
                another thread and thus is thread-safe.
            */
            void wake()
            { this->onWake(); }

            /** @brief Notifies about wait timeouts
                This signal is send when the timeout given to a wait
                call of the selector expires and no activity occured.
            */
            Signal<> timeout;

            virtual IOMonitorImpl& impl() = 0;

        protected:
            //! @brief Default constructor
            IOMonitor()
            {}

            virtual void onAdd(Selectable&) = 0;

            virtual void onRemove(Selectable&) = 0;

            virtual void onAdd(Timer& timer) = 0;

            virtual void onRemove( Timer& timer ) = 0;

            virtual bool onWait(unsigned int msecs) = 0;

            virtual void onWake() = 0;
    };

} //namespace Pt

#endif
