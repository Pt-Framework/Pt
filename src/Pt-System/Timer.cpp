#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"

namespace Pt
{
namespace System
{

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

	size_t			events	  = 0;
	size_t			deltaRest = 0;
	Clock			stopWatch;
	TimeValue		deltaTime;

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

}
}
