#include "ClockImpl.h"
#include "Pt/Exception.h"
#include "time.h"
#include <mmsystem.h>


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
        throw std::runtime_error("GetProcessAffinityMask failed" + PT_SOURCEINFO);

    if( !SetProcessAffinityMask( _currentProcessHandle, 0x01 ) )
        throw std::runtime_error("SetProcessAffinityMask failed" + PT_SOURCEINFO);

    if( !SetThreadAffinityMask( GetCurrentThread(), 0x01 ) )
        throw std::runtime_error("SetProcessAffinityMask failed" + PT_SOURCEINFO);
#endif

    if( !QueryPerformanceFrequency( &_frequency ) )
        throw std::runtime_error("QueryPerformanceFrequency failed" + PT_SOURCEINFO);
}

ClockImpl::~ClockImpl()
{
}

void ClockImpl::start()
{
    _secondStartValue =   timeGetTime();
    QueryPerformanceCounter( &_startValue );
}

TimeValue ClockImpl::stop()
{
    QueryPerformanceCounter( &_stopValue );
    _secondStopValue = timeGetTime();

    LARGE_INTEGER delta;
    delta.QuadPart      = _stopValue.QuadPart - _startValue.QuadPart;
    DWORD secondDelta   = _secondStopValue - _secondStartValue;

    TimeValue result;

    if( secondDelta > 100 )
    {
        result.setSeconds( secondDelta / 1000 );
        result.setMicroSeconds ( ( secondDelta * 1000 ) % 1000000 );
    }
    else
    {
        size_t seconds = static_cast<size_t>( delta.QuadPart / _frequency.QuadPart );
        result.setSeconds( seconds );

        size_t microSeconds = static_cast<size_t>( ( ( delta.QuadPart * 1000000 ) / _frequency.QuadPart ) % 1000000 );
        result.setMicroSeconds( microSeconds );
    }

    return result;
}

Pt::DateTime ClockImpl::getCurrentTime()
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

Pt::size_t ClockImpl::getTime()
{
    return timeGetTime();
}

} // namespace Pt

} // namespace System



