#include "Pt/System/IOMonitor.h"
#include "Pt/System/Clock.h"
#include "IOMonitorImpl.h"


namespace Pt{

namespace System{

size_t getCurrentMSecs()
{
    DateTime currentTime = Clock::getCurrentTime();
    size_t msecs = currentTime.msecs();
    msecs += currentTime.seconds() * 1000;
    msecs += currentTime.minutes() * 60 * 1000;
    msecs += currentTime.hours()   * 60 * 60 * 1000;
    msecs += currentTime.days()    * 24 * 60 * 60 * 1000;
    return msecs;
}




Timer::Timer()
: _started(-1)
, _interval(-1)
, _elapsed(0)
{ }


Timer::~Timer()
{}


void Timer::start(unsigned interval)
{
    _interval = interval;
    _started = getCurrentMSecs();
    _elapsed = 0;
}




IOMonitor::IOMonitor()
: _impl( 0 )
//, _timer(0)
{
    _impl = new IOMonitorImpl();
}


IOMonitor::~IOMonitor()
{
    delete _impl;
}


Signal<const IOEvent&>&  IOMonitor::addDevice( IODevice& device, size_t waitMode )
{
    return _impl->addDevice( device, waitMode );
}


void IOMonitor::removeDevice( IODevice& device )
{
    _impl->removeDevice( device );
}

/*
    if(_timer)
    {
        size_t current = getCurrentMSecs();
        _timer->_elapsed = current - _timer->_started;
        timerTimeout = _timer->_interval - _timer->_elapsed;

        while(_timer->_elapsed >= _timer->_interval)
        {
            _timer->_elapsed %= _timer->_interval;
            _timer->_started = current - _timer->_elapsed;
            _timer->timeout();

            current = getCurrentMSecs();
            _timer->_elapsed = current - _timer->_started;
            timerTimeout = _timer->_interval - _timer->_elapsed;
        }

        //std::cerr << timerTimeout << std::endl;
    }
*/
bool IOMonitor::wait(unsigned int msecs)
{
    size_t timerTimeout = IOMonitor::WaitInfinite;

	std::list<Timer*>::iterator it;
    for(it = _timers.begin(); it != _timers.end(); ++it)
    {
		Timer* timer = *it;
		
		// update elapsed time in each timer
        size_t current = getCurrentMSecs();
        timer->_elapsed = current - timer->_started;
        
        // determine lowest timer timeout
        timerTimeout = std::min(timerTimeout, timer->_interval - timer->_elapsed);

        if(timer->_elapsed >= timer->_interval)
        {
            timer->_elapsed -= timer->_interval;
            timer->_started = current - timer->_elapsed;
            timer->timeout();
			return true;
        }

        //std::cerr << timerTimeout << std::endl;
    }

    // If a timer will become active before the
    // wait timeout we return true
    if(timerTimeout <= msecs)
    {
        _impl->wait(timerTimeout);
        return true;
    }

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
