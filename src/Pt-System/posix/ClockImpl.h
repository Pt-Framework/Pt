#include "Pt/DateTime.h"
#include "Pt/Timespan.h"
#include <sys/time.h>
#include <time.h>

namespace Pt {

namespace System {

class ClockImpl
{
    public:
        ClockImpl();

        ~ClockImpl();

        void start ();

        Timespan stop();

        static DateTime getSystemTime();

        static DateTime getLocalTime();

        static Timespan getSystemTicks();

    private:
        struct timeval  _startTime;
        struct timeval  _stopTime;
        struct timezone _timeZone;
};

} // namespace Pt

} // namespace System
