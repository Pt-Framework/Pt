#include "Pt/System/IOMonitor.h"
#include "Pt/System/Timer.h"
#include "Pt/System/IOChannel.h"
#include "IOMonitorImpl.h"


namespace Pt {

namespace System {


IOMonitor::IOMonitor()
: _impl( 0 )

{
    _impl = new IOMonitorImpl();
}


IOMonitor::~IOMonitor()
{
    delete _impl;
}


void IOMonitor::addChannel( IOChannel& channel)
{
    return _impl->addChannel( channel );
}


void IOMonitor::removeChannel( IOChannel& channel )
{
    _impl->removeChannel( channel );
}


void IOMonitor::addTimer(Timer& timer)
{
    _timers.push_back(&timer);
    connect(timer.destroyed, *this, &IOMonitor::removeTimer);
}


void IOMonitor::removeTimer( Timer& timer )
{
    _timers.remove( &timer );
}


bool IOMonitor::wait(unsigned int msecs)
{
    size_t timerTimeout = IOMonitor::WaitInfinite;

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


void IOMonitor::wake()
{
    _impl->wake();
}

}//namespace System

}//namespace Pt
