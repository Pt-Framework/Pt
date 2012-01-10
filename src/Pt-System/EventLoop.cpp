/*
 * Copyright (C) 2007- 2010 Marc Boris Duerner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "Pt/System/EventLoop.h"
#include "Pt/System/Selectable.h"
#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"

namespace Pt {

namespace System {

EventLoop::EventLoop()
{
}


EventLoop::~EventLoop()
{
}


void EventLoop::setIdleTimeout(size_t msecs)
{ 
    if(msecs != WaitInfinite)
        _idleTimer.start(msecs);
    else
        _idleTimer.stop();
}


size_t EventLoop::idleTimeout() const
{ 
    if( ! _idleTimer.started() )
        return WaitInfinite;

    return _idleTimer.interval(); 
}


Signal<>& EventLoop::timeout()
{ 
    return _idleTimer.timeout();
}


Signal<const Event&>& EventLoop::event()
{ 
    return _event; 
}


Signal<>& EventLoop::exited()
{ 
    return _exited; 
}


void EventLoop::run()
{
    _idleTimer.setActive(*this);
    this->onRun();
    _idleTimer.detach();
    exited();
}


void EventLoop::exit()
{
    this->onExit();
}


//////////////////////////////////////////////////////////////////////////
// EventQueue
//////////////////////////////////////////////////////////////////////////

EventQueue::EventQueue()
: _allocator(/*255, 64*/)
, _usedalloc(&_allocator)
{}


EventQueue::EventQueue(Allocator& a)
: _allocator(/*255, 64*/)
, _usedalloc(&a)
{}


EventQueue::~EventQueue()
{
    try
    {
        while ( ! _eventQueue.empty() )
        {
            Event* ev = _eventQueue.front();
            _eventQueue.pop_front();
            ev->destroy( this->allocator() );
        }
    }
    catch(...)
    {}
}


void EventQueue::clear()
{
    try
    {
        while ( ! _eventQueue.empty() )
        {
            Event* ev = _eventQueue.front();
            _eventQueue.pop_front();
            ev->destroy( this->allocator() );
        }
    }
    catch(...)
    {}
}


void EventQueue::pushEvent(const Event& ev)
{
    Event& clonedEvent = ev.clone( this->allocator() );

    try
    {
        _eventQueue.push_back(&clonedEvent);
    }
    catch(...)
    {
        clonedEvent.destroy( this->allocator() );
        throw;
    }
}


Event* EventQueue::front()
{
    return _eventQueue.front();
}


void EventQueue::popFront()
{
    Event* ev = _eventQueue.front();
    _eventQueue.pop_front();
    ev->destroy( this->allocator() );
}


//////////////////////////////////////////////////////////////////////////
// TimerQueue
//////////////////////////////////////////////////////////////////////////

TimerQueue::TimerQueue()
{}


TimerQueue::~TimerQueue()
{
    while( _timers.size() )
    {
       Timer* timer = _timers.begin()->second;
        timer->detach();
    }
}


void TimerQueue::addTimer(Timer& timer)
{
    if( timer.started() )
    {
        TimerMap::value_type elem(timer.finished(), &timer);
        _timers.insert(elem);
    }
}


void TimerQueue::removeTimer( Timer& timer )
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


size_t TimerQueue::processTimers()
{
    size_t lowestTimeout = EventLoop::WaitInfinite;

    if( _timers.empty() )
        return lowestTimeout;

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
            if(remaining % 1000 > 0) 
                ++lowestTimeout;

            break;
        }

        timer->update(now);

        if( ! _timers.empty() )
        {
            timer = _timers.begin()->second;
            _timers.erase( _timers.begin() );

            TimerMap::value_type elem(timer->finished(), timer);
            _timers.insert(elem);
        }
    }

    return lowestTimeout;
}






//////////////////////////////////////////////////////////////////////////
// EventDispatcher
//////////////////////////////////////////////////////////////////////////

/*EventDispatcher::EventDispatcher()
: _allocator() 
, _usedalloc(&_allocator)
, _state(0)
{}


EventDispatcher::EventDispatcher(Allocator& a)
: _allocator() 
, _usedalloc(&a) 
, _state(0) 
{}


EventDispatcher::~EventDispatcher()
{
    try
    {
        while ( ! _eventQueue.empty() )
        {
            Event* ev = _eventQueue.front();
            _eventQueue.pop_front();
            ev->destroy( this->allocator() );
        }
    }
    catch(...)
    {}

    while( _timers.size() )
    {
       Timer* timer = _timers.begin()->second;
        timer->detach();
    }
}


void EventDispatcher::run()
{
    _state = 0;
    this->onRun();
}


void EventDispatcher::exit()
{
    RecursiveLock lock(_queueMutex);
    _state = 1;
    lock.unlock();

    this->wake();
    this->onExit();
}


void EventDispatcher::wake()
{
    this->onWake();
}


void EventDispatcher::commitEvent(const Event& ev)
{
    {
        RecursiveLock lock( _queueMutex );

        Event& clonedEvent = ev.clone( this->allocator() );

        try
        {
            _eventQueue.push_back(&clonedEvent);
        }
        catch(...)
        {
            clonedEvent.destroy( this->allocator() );
            throw;
        }
    }

    this->wake();
}


void EventDispatcher::queueEvent(const Event& ev)
{
    RecursiveLock lock( _queueMutex );

    Event& clonedEvent = ev.clone( this->allocator() );

    try
    {
        _eventQueue.push_back(&clonedEvent);
    }
    catch(...)
    {
        clonedEvent.destroy( this->allocator() );
        throw;
    }
}


bool EventDispatcher::processEvents()
{
    bool isActive = true;

    while( true )
    {
        RecursiveLock lock(_queueMutex);
        isActive = _state == 0;

        if ( _eventQueue.empty() || ! isActive )
            break;

        Event* ev = _eventQueue.front();
        _eventQueue.pop_front();

        try
        {
            lock.unlock();
            event().send(*ev);
        }
        catch(...)
        {
            ev->destroy( this->allocator() );
            throw;
        }

        ev->destroy( this->allocator() );
    }

    return isActive;
}


void EventDispatcher::addTimer(Timer& timer)
{
    if( timer.started() )
    {
        TimerQueue::value_type elem(timer.finished(), &timer);
        _timers.insert(elem);
    }
}


void EventDispatcher::removeTimer( Timer& timer )
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


size_t EventDispatcher::processTimers()
{
    size_t lowestTimeout = EventLoop::WaitInfinite;

    if( _timers.empty() )
        return lowestTimeout;

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
            if(remaining % 1000 > 0) 
                ++lowestTimeout;

            break;
        }

        timer->update(now);

        if( ! _timers.empty() )
        {
            timer = _timers.begin()->second;
            _timers.erase( _timers.begin() );

            TimerQueue::value_type elem(timer->finished(), timer);
            _timers.insert(elem);
        }
    }

    return lowestTimeout;
}*/


/*bool EventLoopImpl::processTimers(size_t& lowestTimeout)
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
            TimerQueue::value_type elem(timer->finished(), timer);
            _timers.insert(elem);
            //_timers.insert( std::make_pair(timer->finished(), timer) );
        }
    }

    return timerActive;
}*/

} // namespace System

} // namespace Pt
