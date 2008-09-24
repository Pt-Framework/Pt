#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Selector.h"
#include <limits>

namespace Pt {

namespace System {

Timer::Timer()
: _selector(0)
, _active(false)
, _interval(0)
, _remaining(0)
, _finished(0)
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

    if( ! _active )
        return;

    this->update();
}


void Timer::start(std::size_t interval)
{
    _active = true;
    _interval = interval;
    _remaining = _interval * 1000;
    _finished = Clock::getSystemTicks() + _remaining;

    if(_selector)
        _selector->onChanged(*this);
}


void Timer::stop()
{
    _active = false;
    _remaining = 0;
    _finished = 0;

    if(_selector)
        _selector->onChanged(*this);
}


bool Timer::update()
{
    if(_active == false)
        return false;

    Timespan now = Clock::getSystemTicks();
    return this->update(now);
}


bool Timer::update(const Timespan& now)
{
    if(_active == false)
        return false;

    bool hasElapsed = now >= _finished;

    while( now >= _finished )
    {
        _finished += (_interval * 1000);
        timeout.send();
    }

    _remaining = _finished - now;
    return hasElapsed;
}


void Timer::setSelector(SelectorBase* selector)
{
    if(_selector == selector)
        return;

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
