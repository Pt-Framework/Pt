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
#include "Pt/System/SystemError.h"
#include "Pt/SourceInfo.h"
#include <sys/time.h>
#include <time.h>

#if defined(WITH_MACH_CLOCK)
#include <mach/mach_time.h>
#endif

namespace Pt {

namespace System {

ClockImpl::ClockImpl()
{}


ClockImpl::~ClockImpl()
{}


void ClockImpl::start()
{
   gettimeofday( &_startTime, 0 );
}


Timespan ClockImpl::stop()
{
    gettimeofday( &_stopTime, 0 );

    time_t secs = _stopTime.tv_sec - _startTime.tv_sec;
    suseconds_t usecs = _stopTime.tv_usec - _startTime.tv_usec;

    return Timespan(secs, usecs);
}


DateTime ClockImpl::getSystemTime()
{
    struct ::tm tim;
    struct timeval tod;

    gettimeofday(&tod, NULL);

    time_t sec = tod.tv_sec;
    gmtime_r(&sec, &tim);

    return DateTime( tim.tm_year + 1900,
                     tim.tm_mon + 1,
                     tim.tm_mday,
                     tim.tm_hour,
                     tim.tm_min,
                     tim.tm_sec,
                     tod.tv_usec / 1000 );
}


DateTime ClockImpl::getLocalTime()
{
    struct timeval tod;
    gettimeofday(&tod, NULL);

    struct tm tim;
    time_t sec = tod.tv_sec;
    localtime_r(&sec, &tim);

    return DateTime( tim.tm_year + 1900,
                     tim.tm_mon + 1,
                     tim.tm_mday,
                     tim.tm_hour,
                     tim.tm_min,
                     tim.tm_sec,
                     tod.tv_usec / 1000 );
}


#if defined(WITH_MACH_CLOCK)

Timespan ClockImpl::getSystemTicks()
{
    mach_timebase_info_data_t info;
    kern_return_t ret = mach_timebase_info(&info);
    if(ret != 0)
        throw SystemError("mach_timebase_info");
        
    uint64_t time = mach_absolute_time();
    time /= info.denom;
    time *= info.numer;

    return Timespan(time / uint64_t(1000));
}

#else // WITH_POSIX_CLOCK

Timespan ClockImpl::getSystemTicks()
{
    timespec tp;
    int r = clock_gettime(CLOCK_MONOTONIC, &tp);
    if(r != 0)
        throw System::SystemError("clock_gettime");

    return Timespan(tp.tv_sec, tp.tv_nsec/1000);
}

#endif

} // namespace Pt

} // namespace System



