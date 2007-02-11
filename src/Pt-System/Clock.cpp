#include "Pt/System/Clock.h"
#include "ClockImpl.h"


namespace Pt {

namespace System {

Clock::Clock()
{
    _impl = new ClockImpl();
}

Clock::~Clock()
{
    delete _impl;
}

void Clock::start()
{
    _impl->start();
}

TimeValue Clock::stop()
{
    return _impl->stop();
}

DateTime Clock::getCurrentTime()
{
    return ClockImpl::getCurrentTime();
}

} //namespace System

} //namespace Pt
