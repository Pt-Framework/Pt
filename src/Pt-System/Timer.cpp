#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"
#include <climits>


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
: _active(false)
, _started(InvalidTime)
, _interval(InvalidTime)
, _elapsed(0)
{ }


Timer::~Timer()
{
    this->destroyed(*this);
}


bool Timer::active() const
{
    return _active;
}


size_t Timer::interval() const
{
    return _interval;
}


void Timer::setInterval(size_t msecs)
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
    return _interval - _elapsed;
}



/*
size_t Timer::_resolution = 10000;

Timer::Timer()
: _timerThread( *this )
, _eventCounter( 0 )
, _run( false )
{
}

Timer::~Timer()
{ }

size_t Timer::resolution() 
{ 
    return _resolution / 1000; 
}

void Timer::start()
{  
    _timerThread.start(); 
}

void Timer::stop()
{
    _run = false;
    _timerThread.wait();
    _eventCounter = 0;
}

void Timer::run()
{
    _run = true;

    size_t            events      = 0;
    size_t            deltaRest = 0;
    Clock            stopWatch;
    TimeValue        deltaTime;

    size_t sleepTime = _resolution / 1000;
    stopWatch.start();

    Thread::sleep( sleepTime );

    while( _run )
    {
        deltaTime = stopWatch.stop();

        stopWatch.start();

        deltaRest  +=  ( deltaTime.seconds() / 1000000.0 + ( deltaTime.microSeconds() ) ); 

        events = ( deltaRest / _resolution  );

        if( events == 0 )
            events = 1;

        deltaRest  = ( deltaRest %  _resolution ); 

        for( size_t i = 0; i < events; ++i )
            onTime.send( ++_eventCounter );

        Thread::sleep( sleepTime );
    }
}
*/

}

}
