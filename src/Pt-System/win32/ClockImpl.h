#include "Pt/DateTime.h"
#include "Pt/Timespan.h"
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

        static DateTime getCurrentTime();

        static Pt::size_t getTime();

    private:
        DWORD           _procAffinity;
        DWORD           _sysAffinity;
        HANDLE          _currentProcessHandle;
        LARGE_INTEGER   _frequency;
        LARGE_INTEGER   _startValue;
        LARGE_INTEGER   _stopValue;
        DWORD           _secondStartValue;
        DWORD           _secondStopValue;
};

} // namespace Pt

} // namespace System

