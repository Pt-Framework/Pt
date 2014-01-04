/*
 * Copyright (C) 2007- 2013 Marc Boris Duerner
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

#include <Pt/System/EventLoop.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/Timer.h>
#include <Pt/System/Clock.h>
#include <Pt/System/Logger.h>

log_define("Pt.System.EventLoop")

namespace Pt {

namespace System {

EventLoop::EventLoop()
{
}


EventLoop::~EventLoop()
{
}


void EventLoop::run()
{
    this->onRun();
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
, _exited(false)
{}


EventQueue::EventQueue(Allocator& a)
: _allocator(/*255, 64*/)
, _usedalloc(&a)
, _exited(false)
{}


EventQueue::~EventQueue()
{
    try
    {
        MutexLock lock(_mutex);

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


void EventQueue::exit()
{
    MutexLock lock(_mutex);
    _exited = true;
}


void EventQueue::pushEvent(const Event& ev)
{ 
    MutexLock lock(_mutex);

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


bool EventQueue::processEvents(Signal<const Event&>& eventSignal)
{ 
    bool isActive = true;

    MutexLock lock(_mutex);

    while( true )
    {    
        isActive = ! _exited;

        if ( _eventQueue.empty() || _exited )
            break;

        Event* ev = _eventQueue.front();
        _eventQueue.pop_front();
        lock.unlock();

        try
        {
            eventSignal.send(*ev);
        }
        catch(...)
        {
            lock.lock();
            ev->destroy( this->allocator() );
            throw;
        }

		lock.lock();
        ev->destroy( this->allocator() );
    }

    return isActive;
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
    if( timer.isStarted() )
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


std::size_t TimerQueue::processTimers()
{
    log_trace("TimerQueue::processTimers");

    std::size_t lowestTimeout = EventLoop::WaitInfinite;

    if( _timers.empty() )
    {
        log_trace("no timers, returning: " << lowestTimeout);
        return lowestTimeout;
    }

    Timespan now = Clock::getSystemTicks();
    Timer* timer = _timers.begin()->second;

    log_trace("now: " << now.toMSecs());
    log_trace("first timer at: " << timer->finished().toMSecs());

    while( ! _timers.empty() )
    {
        log_trace("get front of timer queue");
        timer = _timers.begin()->second;

        if( now < timer->finished() )
        {
            Pt::int64_t remaining = (timer->finished() - now).toUSecs();
            
            Pt::uint64_t remainingMSecs = static_cast<Pt::uint64_t>(remaining / 1000);
            if(remaining % 1000 > 0) 
                ++remainingMSecs;

            lowestTimeout = (remainingMSecs <= EventLoop::WaitMax) ? static_cast<std::size_t>(remainingMSecs)
                                                                   : EventLoop::WaitMax ;

            log_trace("no more timer expired: " << timer->finished().toMSecs());
            break;
        }

        log_trace("updating expired timer");
        timer->update(now);

        if( ! _timers.empty() )
        {
            log_trace("resetting timer: " << timer->finished().toMSecs());
            timer = _timers.begin()->second;
            _timers.erase( _timers.begin() );

            TimerMap::value_type elem(timer->finished(), timer);
            _timers.insert(elem);
        }
    }

    log_trace("TimerQueue::processTimers returns: " << lowestTimeout);
    return lowestTimeout;
}

} // namespace System

} // namespace Pt
