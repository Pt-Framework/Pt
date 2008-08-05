#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Selector.h"
#include <limits>

namespace {

Pt::size_t getCurrentMSecs()
{
    return  Pt::System::Clock::getTime();
}

}

namespace Pt {

namespace System {

const unsigned Timer::InvalidTime = std::numeric_limits<unsigned>::max();


Timer::Timer()
: _selector(0)
, _active(false)
, _started(InvalidTime)
, _interval(InvalidTime)
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


size_t Timer::interval() const
{
    return _interval;
}


void Timer::setInterval(unsigned msecs)
{
    _interval = msecs;

    if( _active )
        this->update();
}


void Timer::start(unsigned interval)
{
    _active = true;
    _interval = interval;
    _started = getCurrentMSecs();
    _elapsed = 0;
}


void Timer::stop()
{
    _active = false;
    _started = InvalidTime;
    _elapsed = 0;
}


bool Timer::update()
{
    if(_active == false)
        return false;

    size_t current = getCurrentMSecs();
    _elapsed = current - _started;

    if(_elapsed >= _interval)
    {
        _elapsed -= _interval;
        _started = current - _elapsed;
        timeout();
        return true;
    }

    return false;
}


size_t Timer::remaining() const
{
    return _active ? _interval - _elapsed : InvalidTime;
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
