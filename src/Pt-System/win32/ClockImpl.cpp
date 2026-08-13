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
#include <Pt/SourceInfo.h>
#include <Pt/System/SystemError.h>

namespace {

Pt::Timespan queryPerformanceCounter()
{
#ifndef _WIN32_WCE
    DWORD_PTR cpuMask = 0x01;

    DWORD_PTR threadAffinity = SetThreadAffinityMask( GetCurrentThread(), cpuMask );
    if( ! threadAffinity )
        throw Pt::System::SystemError("SetThreadAffinityMask");
#endif

    LARGE_INTEGER value;
    LARGE_INTEGER frequency;

    if( ! QueryPerformanceFrequency( &frequency ) )
        throw Pt::System::SystemError("QueryPerformanceFrequency");

    if( ! QueryPerformanceCounter( &value ) )
        throw Pt::System::SystemError("QueryPerformanceCounter");

#ifndef _WIN32_WCE
    if( ! SetThreadAffinityMask( GetCurrentThread(), threadAffinity ) )
        throw Pt::System::SystemError("SetProcessAffinityMask");
#endif

    LONGLONG seconds = value.QuadPart / frequency.QuadPart;
    LONGLONG remain = value.QuadPart - (seconds * frequency.QuadPart);
    LONGLONG usecs = remain * 1000000 / frequency.QuadPart;
    usecs += seconds * 1000000;

    return Pt::Timespan(usecs);
}

}

namespace Pt {

namespace System {

ClockImpl::ClockImpl()
{
}


ClockImpl::~ClockImpl()
{
}


void ClockImpl::start()
{
    _startValue = queryPerformanceCounter();
}


Timespan ClockImpl::stop()
{
    Pt::Timespan stopValue = queryPerformanceCounter();
    return stopValue - _startValue;
}


Pt::DateTime ClockImpl::getSystemTime()
{
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);

    return DateTime(systemTime.wYear,
                    systemTime.wMonth,
                    systemTime.wDay,
                    systemTime.wHour,
                    systemTime.wMinute,
                    systemTime.wSecond,
                    systemTime.wMilliseconds );
}


Pt::DateTime ClockImpl::getLocalTime()
{
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);

    return DateTime(systemTime.wYear,
                    systemTime.wMonth,
                    systemTime.wDay,
                    systemTime.wHour,
                    systemTime.wMinute,
                    systemTime.wSecond,
                    systemTime.wMilliseconds);
}


Timespan ClockImpl::getSystemTicks()
{
#ifdef __cplusplus_winrt
    ULONGLONG msecs = GetTickCount64();
    return Timespan( Pt::int64_t(1000) * msecs );
#else  
    return queryPerformanceCounter();
#endif
}

} // namespace Pt

} // namespace System
