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

} // namespace Pt

} // namespace System



