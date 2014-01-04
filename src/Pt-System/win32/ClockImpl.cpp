/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "ClockImpl.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/SystemError.h"
#include <stdexcept>
#include <time.h>

namespace Pt {

namespace System {

ClockImpl::ClockImpl()
{
    //DWORD procAffinity;
    //DWORD sysAffinity;
    DWORD_PTR cpuMask = 0x01;

#ifndef _WIN32_WCE
    // HANDLE currentProcessHandle = GetCurrentProcess();

    // if( ! GetProcessAffinityMask( currentProcessHandle,  &procAffinity, &sysAffinity ))
    //     throw SystemError( PT_ERROR_MSG("GetProcessAffinityMask failed") );

    // if( ! SetProcessAffinityMask( currentProcessHandle, 0x01 ) )
    //     throw SystemError( PT_ERROR_MSG("SetProcessAffinityMask failed") );

    DWORD_PTR threadAffinity = SetThreadAffinityMask( GetCurrentThread(), cpuMask );
    if( ! threadAffinity )
        throw SystemError( PT_ERROR_MSG("SetProcessAffinityMask failed") );
#endif

    if( ! QueryPerformanceFrequency( &_frequency ) )
        throw SystemError( PT_ERROR_MSG("QueryPerformanceFrequency failed") );

#ifndef _WIN32_WCE
    // if( ! SetProcessAffinityMask( currentProcessHandle, procAffinity ) )
    //     throw SystemError( PT_ERROR_MSG("SetProcessAffinityMask failed") );

    if( ! SetThreadAffinityMask( GetCurrentThread(), threadAffinity ) )
        throw SystemError( PT_ERROR_MSG("SetProcessAffinityMask failed") );
#endif
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

    const long secs = static_cast<long>(delta.QuadPart / _frequency.QuadPart);
    const long usecs = static_cast<long>(((delta.QuadPart * 1000000) / _frequency.QuadPart ) % 1000000);
           
    return Timespan(secs, usecs);
}


Pt::DateTime ClockImpl::getSystemTime()
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


Pt::DateTime ClockImpl::getLocalTime()
{
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);

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
    FILETIME ft;

    // win32 only : GetSystemTimeAsFileTime(&ft);
    SYSTEMTIME st;
    GetSystemTime( &st );
    SystemTimeToFileTime( &st, &ft );

    // number of 100-nanosecond intervals since January 1, 1601 (UTC)
    Pt::uint64_t tmpres = 0;
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    // convert to microseconds
    tmpres /= 10;

    return Timespan( static_cast<Pt::int64_t>(tmpres) );

    //return Timespan( Pt::int64_t(1000) * GetTickCount() );
}

} // namespace Pt

} // namespace System



