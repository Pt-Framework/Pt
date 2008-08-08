#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Selector.h"
#include <limits>

namespace Pt {

namespace System {

Timer::Timer()
: _selector(0)
, _active(false)
, _started(0)
, _interval(0)
, _elapsed(0)
{ }


Timer::~Timer()
{
    try
    {
        if(_selector)
            _selector->remove(*this);
    }
    catch(...) {}
}


bool Timer::active() const
{
    return _active;
}


std::size_t Timer::interval() const
{
    return _interval;
}


void Timer::setInterval(std::size_t msecs)
{
    _interval = msecs;

    if( _active )
        this->update();
}


void Timer::start(std::size_t interval)
{
    _active = true;
    _interval = interval;
    _started = Clock::getSystemTime();
    _elapsed = 0;
}


void Timer::stop()
{
    _active = false;
    _started = 0;
    _elapsed = 0;
}


bool Timer::update()
{
    if(_active == false)
        return false;

    Timespan now = Clock::getSystemTime();
    Pt::int64_t elapsedMSecs = (now - _started).totalMSecs();

    _elapsed = static_cast<std::size_t>(-1);
    if( elapsedMSecs < static_cast<std::size_t>(-1) )
    {
        _elapsed = static_cast<std::size_t>(elapsedMSecs);
    }

    if(_elapsed >= _interval )
    {
        _elapsed -= _interval;
        _started = now - (_elapsed * 1000);
        timeout.send();
        return true;
    }

    return false;
}


std::size_t Timer::remaining() const
{
    if( ! _active )
        return std::numeric_limits<std::size_t>::max();

    return _interval - _elapsed;
}


void Timer::setSelector(SelectorBase* selector)
{
    if(_selector)
    {
        _selector->onRemove(*this);
    }

    if(selector)
    {
        selector->onAdd(*this);
    }

    _selector = selector;
}

}

}
