#include "Pt/System/Selector.h"
#include "Pt/System/Timer.h"
#include "Pt/System/IOChannel.h"
#include "SelectorImpl.h"


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


void Selector::addChannel( IOChannel& channel)
{
    _impl->addChannel( channel );
    connect(channel.destroyed, *this, &Selector::removeChannel);
}


void Selector::removeChannel( IOChannel& channel )
{
    _impl->removeChannel( channel );
}


void Selector::addTimer(Timer& timer)
{
    _timers.push_back(&timer);
    connect(timer.destroyed, *this, &Selector::removeTimer);
}


void Selector::removeTimer( Timer& timer )
{
    _timers.remove( &timer );
}


bool Selector::wait(unsigned int msecs)
{
    size_t timerTimeout = Selector::WaitInfinite;

    std::list<Timer*>::iterator it;
    for(it = _timers.begin(); it != _timers.end(); ++it)
    {
        Timer* timer = *it;

        // update timer and return indicating activity
        // if it fires its timout signal
        if( timer->update() )
            return true;

        // determine lowest timer timeout
        timerTimeout = std::min( timerTimeout, timer->remaining() );
    }

    // If a timer will become active before the passed
    // timeout expires we can wait and return true
    if(timerTimeout <= msecs)
    {
        _impl->wait(timerTimeout);
        return true;
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
