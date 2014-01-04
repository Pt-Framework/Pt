#ifndef PT_SYSTEM_CLOCK_IMPL_H
#define PT_SYSTEM_CLOCK_IMPL_H

#include "Pt/WinVer.h"
#include "Pt/DateTime.h"
#include "Pt/Timespan.h"
#include "Pt/System/Api.h"
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
        LARGE_INTEGER   _frequency;
        LARGE_INTEGER   _startValue;
        LARGE_INTEGER   _stopValue;
        DWORD           _secondStartValue;
        DWORD           _secondStopValue;
};

} // namespace Pt

} // namespace System

#endif
