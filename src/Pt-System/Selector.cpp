#include "SelectorImpl.h"
#include "Pt/System/Selector.h"
#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"

namespace Pt {

namespace System {

void SelectorBase::add(Selectable& s)
{
    if(s.selector() != 0)
    {
        throw std::logic_error("Selectable already added");
    }

    s.setSelector(this);
}

void SelectorBase::remove(Selectable& s)
{
    if(s.selector() != this)
    {
        throw std::logic_error("Selectable not added");
    }

    s.setSelector(0);
}


void SelectorBase::add(Timer& timer)
{
    if(timer.selector() != 0)
    {
        throw std::logic_error("Selectable already added");
    }

    timer.setSelector(this);
}


void SelectorBase::remove( Timer& timer )
{
    if(timer.selector() != this)
    {
        throw std::logic_error("Selectable not added");
    }

    timer.setSelector(0);
}


Selector::Selector()
: _impl( 0 )
{
    _impl = new SelectorImpl();
}


Selector::~Selector()
{
    while( _timers.size() )
    {
        Timer* timer = *_timers.begin();
        timer->setSelector(0);
    }

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


void Selector::onEnabled(Selectable& s)
{
    _impl->onEnabled(s);
}


void Selector::onDisabled(Selectable& s)
{
    _impl->onDisabled(s);
}


void Selector::onAdd(Timer& timer)
{
    _timers.push_back(&timer);
}


void Selector::onRemove( Timer& timer )
{
    _timers.remove( &timer );
}


void Selector::onStarted(Timer& timer)
{

}


void Selector::onStopped(Timer& timer)
{

}


void Selector::setApp(Application* app)
{
    _impl->setApp(app);
}


bool Selector::updateTimer(size_t& lowestTimeout)
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
        lowestTimeout = std::min( lowestTimeout, timer->remaining() );
    }
    return timerActive;
}


bool Selector::onWait(unsigned int msecs)
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
    bool b = _impl->wait(msecs);
return b;
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
