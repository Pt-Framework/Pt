#ifndef PT_SYSTEM_CLOCK_H
#define PT_SYSTEM_CLOCK_H

#include <Pt/Types.h>
#include <Pt/DateTime.h>
#include <Pt/Timespan.h>
#include <Pt/System/Api.h>

namespace Pt {

namespace System {

class PT_SYSTEM_API Clock
{
    public:
        /** @brief Contructs a Clock
        */
        Clock();
        /** @brief Destructor
        */
        ~Clock();

        /** @brief Start the clock.
        */
        void start();

        /** @brief Stop the clock.
            Returns the elapsed time since start was called.
        */
        Timespan stop();

        /** @brief Returns the current time
        */
        static DateTime getCurrentTime();

        /** @brief Gets the system time
            The getTime function retrieves the system time, in milliseconds. 
            The system time is the time elapsed since the system was started.

            TODO: getElapsedTime, getSystemTime, getLocalTime
        */
        static Timespan getSystemTime();

    private:
        class ClockImpl *_impl;
};

} //namespace Pt

} //namespace System

#endif // PT_SYSTEM_CLOCK_H
