#include "ClockImpl.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/SystemError.h"
#include <stdexcept>
#include <time.h>

namespace Pt {

namespace System {

ClockImpl::ClockImpl()
: _procAffinity(0)
, _sysAffinity(0)
, _currentProcessHandle(0)
{
    _currentProcessHandle = GetCurrentProcess();

#ifndef _WIN32_WCE

    if( !GetProcessAffinityMask( _currentProcessHandle,  &_procAffinity, &_sysAffinity ))
        throw SystemError( PT_ERROR_MSG("GetProcessAffinityMask failed") );

    if( !SetProcessAffinityMask( _currentProcessHandle, 0x01 ) )
        throw SystemError( PT_ERROR_MSG("SetProcessAffinityMask failed") );

    if( !SetThreadAffinityMask( GetCurrentThread(), 0x01 ) )
        throw SystemError( PT_ERROR_MSG("SetProcessAffinityMask failed") );
#endif

    if( !QueryPerformanceFrequency( &_frequency ) )
        throw SystemError( PT_ERROR_MSG("QueryPerformanceFrequency failed") );
}


ClockImpl::~ClockImpl()
{
}


void ClockImpl::start()
{
    _secondStartValue = GetTickCount();
    QueryPerformanceCounter( &_startValue );
}


Timespan ClockImpl::stop()
{
    QueryPerformanceCounter( &_stopValue );
    _secondStopValue = GetTickCount();

    LARGE_INTEGER delta;
    delta.QuadPart      = _stopValue.QuadPart - _startValue.QuadPart;
    DWORD secondDelta   = _secondStopValue - _secondStartValue;

    if( secondDelta > 100 )
    {
        return Timespan(secondDelta / 1000 , ( secondDelta * 1000 ) % 1000000 );
    }
    else
    {
        return Timespan(delta.QuadPart / _frequency.QuadPart,
                        ((delta.QuadPart * 1000000) / _frequency.QuadPart ) % 1000000 );
    }

    return Timespan();
}


Pt::DateTime ClockImpl::getLocalTime()
{
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);

    return DateTime (    systemTime.wYear,
                systemTime.wMonth,
                systemTime.wDay,
                systemTime.wHour,
                systemTime.wMinute,
                systemTime.wSecond,
                systemTime.wMilliseconds    );
}


Timespan ClockImpl::getSystemTicks()
{
    return Timespan( 1000 * GetTickCount() );
}

} // namespace Pt

} // namespace System



