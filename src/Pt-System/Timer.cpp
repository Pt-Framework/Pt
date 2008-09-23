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

    if( _active )
        this->update();
}


void Timer::start(std::size_t interval)
{
    _active = true;
    _interval = interval;
    _remaining = _interval * 1000;
    _finished = Clock::getSystemTicks() + _remaining;
}


void Timer::stop()
{
    _active = false;
    _remaining = 0;
    _finished = 0;
}


bool Timer::update()
{
    if(_active == false)
        return false;

    Timespan now = Clock::getSystemTicks();

    if( _finished <= now )
    {
        _finished += (_interval * 1000);
        _remaining = _finished - now;

        if(_remaining < 0)
            _remaining = 0;

        timeout.send();
        return true;
    }

    return false;
}


std::size_t Timer::remaining() const
{
    if( ! _active )
        return std::numeric_limits<std::size_t>::max();

    return static_cast<std::size_t>( _remaining.toUSecs() / 1000 );
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
