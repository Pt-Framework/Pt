#include "SelectorImpl.h"
#include "Pt/System/Selector.h"
#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"

namespace Pt {

namespace System {

Selector::Selector()
: _impl( 0 )
{
    _impl = new SelectorImpl();
}


Selector::~Selector()
{
    delete _impl;
}


void Selector::add( IOResult& result )
{
    result.setSelector(this);
    _impl->complete(result);
}


void Selector::remove( IOResult& result )
{
    _impl->cancel(result);
}


void Selector::add(Timer& timer)
{
    void (Selector::*removeTimer)(Timer&);
    removeTimer = &Selector::remove;

    _timers.push_back(&timer);
    connect(timer.destroyed, *this, removeTimer);
}


void Selector::remove( Timer& timer )
{
    _timers.remove( &timer );
}


bool Selector::updateTimer(size_t& timeout)
{
    bool timerActive = false;
    std::list<Timer*>::iterator it;
    for(it = _timers.begin(); it != _timers.end(); ++it)
    {
        Timer* timer = *it;

        // update timer and return indicating activity
        // if it fires its timout signal
        if( timer->update() )
        {
            timerActive = true;
            continue;
        }

        // determine lowest timer timeout
        timeout = std::min( timeout, timer->remaining() );
    }
    return timerActive;
}

bool Selector::wait(unsigned int msecs)
{
    size_t timerTimeout = Selector::WaitInfinite;

    if ( updateTimer(timerTimeout) )
        return true;

    // If a timer will become active before the passed
    // timeout expires we can wait and return true
    if(timerTimeout <= msecs)
    {
        if (_impl->wait(timerTimeout))
            return true;

        return updateTimer(timerTimeout);
    }

    // This handles the case when no timer will become
    // active in the given timeout. The result of the
    // wait call indicates activity
    bool ret = _impl->wait(msecs);
    if(ret == false)
        timeout.send();

    return ret;
}


void Selector::wake()
{
    _impl->wake();
}

}//namespace System

}//namespace Pt
