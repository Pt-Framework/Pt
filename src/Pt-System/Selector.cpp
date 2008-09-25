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


void SelectorBase::setParent(Application* app)
{
    this->onSetParent(app);
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


void SelectorBase::onAdd(Timer& timer)
{
    if( timer.active() )
    {
        _timers.insert( std::make_pair(timer.finished(), &timer) );
    }
}


void SelectorBase::onRemove( Timer& timer )
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


void SelectorBase::onChanged(Timer& timer)
{
    if( timer.active() )
    {
        _timers.insert( std::make_pair(timer.finished(), &timer) );
    }
    else
    {
        SelectorBase::onRemove(timer);
    }
}


bool SelectorBase::updateTimer(size_t& lowestTimeout)
{
    if( _timers.empty() )
        return false;

    Timespan now = Clock::getSystemTicks();
    Timer* timer = _timers.begin()->second;
    bool timerActive = now > timer->finished();

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

        if( ! _timers.empty() &&
            timer == _timers.begin()->second &&
            timer->finished() != _timers.begin()->first )
        {
            _timers.erase( _timers.begin() );
            _timers.insert( std::make_pair(timer->finished(), timer) );
        }
    }

    return timerActive;
}


bool SelectorBase::wait(unsigned int msecs)
{
    size_t timerTimeout = Selector::WaitInfinite;

    if ( updateTimer(timerTimeout) )
        return true;

    // This handles the case when no timer will become
    // active in the given timeout. The result of the
    // wait call indicates activity
    if(timerTimeout > msecs)
        return this->onWait(msecs);

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


void Selector::onSetParent(Application* app)
{
    _impl->setParent(app);
}


bool Selector::onWait(unsigned int msecs)
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
