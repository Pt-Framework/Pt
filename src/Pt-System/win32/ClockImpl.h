#ifndef PT_SYSTEM_CLOCK_IMPL_H
#define PT_SYSTEM_CLOCK_IMPL_H

#include <Pt/WinVer.h>
#include <Pt/DateTime.h>
#include <Pt/Timespan.h>
#include <Pt/System/Api.h>
#include <windows.h>

namespace Pt {

namespace System {

class ClockImpl
{
    public:
        ClockImpl();

        ~ClockImpl();

        void start();

        Timespan stop();

        static DateTime getSystemTime();

        static DateTime getLocalTime();

        static Timespan getSystemTicks();

    private:
        Timespan _startValue;
};

} // namespace Pt

} // namespace System

#endif
