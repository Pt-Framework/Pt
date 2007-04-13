#include "ClockImpl.h"
#include "Pt/System/TimeValue.h"
#include "Pt/Exception.h"
#include "time.h"


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

TimeValue ClockImpl::stop()
{
    gettimeofday( &_stopTime, 0 );
    TimeValue stop(_stopTime.tv_sec, _stopTime.tv_usec);
    TimeValue start(_startTime.tv_sec, _startTime.tv_usec);
    return stop - start;
}

DateTime ClockImpl::getCurrentTime()
{
    struct timeval timeValue;
    gettimeofday(&timeValue, NULL);

    struct tm* currentTimeStruct;
    currentTimeStruct = localtime(&timeValue.tv_sec);

    Date date(    currentTimeStruct->tm_year + 1900,
                currentTimeStruct->tm_mon + 1,
                currentTimeStruct->tm_mday    );
    Time time(    currentTimeStruct->tm_hour,
                currentTimeStruct->tm_min,
                currentTimeStruct->tm_sec,
                timeValue.tv_usec / 1000    );
    DateTime dateTime(date, time);

    return dateTime;
}

Pt::size_t ClockImpl::getTime()
{
    DateTime currentTime = ClockImpl::getCurrentTime();
    size_t msecs = currentTime.msecs();
    msecs += currentTime.seconds() * 1000;
    msecs += currentTime.minutes() * 60 * 1000;
    msecs += currentTime.hours()   * 60 * 60 * 1000;
    msecs += currentTime.days()    * 24 * 60 * 60 * 1000;
    return msecs;
}

} // namespace Pt

} // namespace System



