#ifndef PT_SYSTEM_CLOCK_H
#define PT_SYSTEM_CLOCK_H

#include <Pt/Types.h>
#include <Pt/DateTime.h>
#include <Pt/Timespan.h>
#include <Pt/System/Api.h>

namespace Pt {

namespace System {

/** @brief Measures time intervals

    The clock class can be used like a stop-watch by calling Clock::start()
    and Clock::stop(). The latter method returns the elapsed time.
*/
class PT_SYSTEM_API Clock
{
    public:
        /** @brief Contructs a Clock
        */
        Clock();

        /** @brief Destructor
        */
        ~Clock();

        /** @brief Start the clock
        */
        void start();

        /** @brief Stop the clock

            Returns the elapsed time since start was called.
        */
        Timespan stop();

        /** @brief Returns the current local time
        */
        static DateTime getLocalTime();

        /** @brief Returns the timespan since a fixed point in the past

            The getSystemTicks function retrieves the system ticks, in milliseconds.
            The system time is the time elapsed since i.e. the system was started, or
            the unix epoch or some other fixed point in the past.
        */
        static Timespan getSystemTicks();

    private:
        class ClockImpl *_impl;
};

} //namespace Pt

} //namespace System

#endif // PT_SYSTEM_CLOCK_H
