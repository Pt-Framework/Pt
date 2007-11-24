#if !defined(PT_ClockImpl_H)
#define PT_ClockImpl_H

#include <windows.h>
#include "Pt/DateTime.h"
#include "Pt/System/TimeValue.h"


namespace Pt {

namespace System {

class ClockImpl
{
public:
    ClockImpl();

    ~ClockImpl();

    void start();

    TimeValue stop();

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


#endif // PT_SysTimeImpl_H
