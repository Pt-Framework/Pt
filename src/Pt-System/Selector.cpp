#include "SelectorImpl.h"
#include "Pt/System/Selector.h"
#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"

namespace Pt {

namespace System {

SelectorBase::~SelectorBase()
{
    while( _timers.size() )
    {
       Timer* timer = _timers.begin()->second;
        timer->setSelector(0);
    }
}


void SelectorBase::add(Selectable& s)
{
    s.setSelector(this);
}


void SelectorBase::remove(Selectable& s)
{
    if(s.selector() == this)
        s.setSelector(0);
}


void SelectorBase::add(Timer& timer)
{
    timer.setSelector(this);
}


void SelectorBase::remove( Timer& timer )
{
    if(timer.selector() == this)
        timer.setSelector(0);
}


void SelectorBase::onAddTimer(Timer& timer)
{
    if( timer.active() )
    {
        _timers.insert( std::make_pair(timer.finished(), &timer) );
    }
}


void SelectorBase::onRemoveTimer( Timer& timer )
{
    std::multimap<Timespan, Timer*>::iterator it;
    for(it = _timers.begin(); it != _timers.end(); ++it)
    {
        if(it->second == &timer)
        {
            _timers.erase(it);
            return;
        }
    }
}


void SelectorBase::onTimerChanged(Timer& timer)
{
    if( timer.active() )
    {
        _timers.insert( std::make_pair(timer.finished(), &timer) );
    }
    else
    {
        SelectorBase::onRemoveTimer(timer);
    }
}


bool SelectorBase::updateTimer(std::size_t& lowestTimeout)
{
    if( _timers.empty() )
        return false;

    Timespan now = Clock::getSystemTicks();
    Timer* timer = _timers.begin()->second;
    bool timerActive = now >= timer->finished();
	
    while( ! _timers.empty() )
    {
        timer = _timers.begin()->second;

        if( now < timer->finished() )
        {
            Pt::int64_t remaining = (timer->finished() - now).toUSecs();
            lowestTimeout = (remaining / 1000);
            if(remaining % 1000 > 0) ++lowestTimeout;
            break;
        }

        timer->update(now);

        if( ! _timers.empty() )
        {
            timer = _timers.begin()->second;
            _timers.erase( _timers.begin() );
            _timers.insert( std::make_pair(timer->finished(), timer) );
        }
    }

    return timerActive;
}


/*bool SelectorBase::wait(unsigned int msecs)
{
	Timespan now = Clock::getSystemTicks();

	if( ! _timers.empty() )
	{
		Timer* timer = _timers.begin()->second;
		if( now >= timer->finished() )
		{
			msecs = 0;
		}
		else
		{
			Pt::int64_t remaining = (timer->finished() - now).toUSecs();
			size_t timerTimeout = (remaining / 1000);
			if(remaining % 1000 > 0) ++timerTimeout;
			msecs = std::min(msecs, timerTimeout);
		}
	}

	bool active = this->onWait(msecs);
	if( updateTimer(msecs) )
	{
		active = true;
	}

	return active;
}*/


bool SelectorBase::wait(std::size_t msecs)
{
    size_t timerTimeout = Selector::WaitInfinite;

    // If a timer is immediately ready, still check for an
    // active selectable to avoid timer preemption
    if ( updateTimer(timerTimeout) )
    {
        this->onWait(0);
        return true;
    }
	
    // This handles the case when no timer will become
    // active in the given timeout. The result of the
    // wait call indicates activity
    if(timerTimeout > msecs || timerTimeout == Selector::WaitInfinite)
    {
        return this->onWait(msecs);
    }

    // A timer will become active before the timeout expires
    while(true)
    {
        if( this->onWait(timerTimeout) )
            return true;

        if( updateTimer(timerTimeout) )
            return true;
    }

    return false;
}


void SelectorBase::wake()
{ 
    this->onWake();
}


SelectorBase::SelectorBase()
{}


Selector::Selector()
: _impl( 0 )
{
    _impl = new SelectorImpl();
}


Selector::~Selector()
{
    delete _impl;
}


void Selector::onAdd( Selectable& selectable )
{
    _impl->add(selectable);
}


void Selector::onRemove( Selectable& selectable )
{
    _impl->remove(selectable);
}


void Selector::onChanged(Selectable& s)
{
    _impl->changed(s);
}


bool Selector::onWait(std::size_t msecs)
{
    return _impl->wait(msecs);
}


void Selector::onWake()
{
    _impl->wake();
}


SelectorImpl& Selector::impl()
{
	return *_impl;
}

}//namespace System

}//namespace Pt
