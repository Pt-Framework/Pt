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
#include "EventLoopImpl.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"

namespace Pt {

namespace System {

EventLoop::EventLoop()
: _allocator(/*255, 64*/)
, _usedalloc(&_allocator)
, _timeout(WaitInfinite)
, _exitLoop(false)
{
}


EventLoop::EventLoop(Allocator& a)
: _allocator(/*255, 64*/)
, _usedalloc(&a)
, _timeout(WaitInfinite)
, _exitLoop(false)
{
}


EventLoop::~EventLoop()
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
        timer->setSelector(0);
    }

    //while( _selectables.size() )
    //{
    //   Selectable* sel = *_selectables.begin();
    //    sel->setSelector(0);
    //}
}


void EventLoop::add(Selectable& s)
{
    s.setSelector(this);
}


void EventLoop::remove(Selectable& s)
{
    if(s.selector() == this)
        s.setSelector(0);
}


void EventLoop::add(Timer& timer)
{
    timer.setSelector(this);
}


void EventLoop::remove( Timer& timer )
{
    if(timer.selector() == this)
        timer.setSelector(0);
}


void EventLoop::run()
{
    _exitLoop = false;
    this->onRun();
    exited();
}


void EventLoop::exit()
{
    RecursiveLock lock(_queueMutex);
    _exitLoop = true;
    lock.unlock();

    this->onExit();
}


void EventLoop::onAddTimer(Timer& timer)
{
    if( timer.active() )
    {
        TimerMap::value_type elem(timer.finished(), &timer);
        _timers.insert(elem);
        //_timers.insert( std::make_pair(timer.finished(), &timer) );
    }
}


void EventLoop::onRemoveTimer( Timer& timer )
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


void EventLoop::onTimerChanged(Timer& timer)
{
    if( timer.active() )
    {
        TimerMap::value_type elem(timer.finished(), &timer);
        _timers.insert(elem);
        //_timers.insert( std::make_pair(timer.finished(), &timer) );
    }
    else
    {
        EventLoop::onRemoveTimer(timer);
    }
}


bool EventLoop::updateTimer(std::size_t& lowestTimeout)
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
            TimerMap::value_type elem(timer->finished(), timer);
            _timers.insert(elem);
            //_timers.insert( std::make_pair(timer->finished(), timer) );
        }
    }

    return timerActive;
}


size_t EventLoop::runNext(WaitResult& result)
{
    if( result.isTimeout() )
    {
        timeout.send();
    }

    if( result.isEvent() )
    {
        RecursiveLock lock(_queueMutex);

        if(_exitLoop)
        {
            result.clear();
            result.setExit();
            return 0;
        }

        if( ! _eventQueue.empty() )
        {
            lock.unlock();
            this->processEvents();
        }

        lock.unlock();
    }

    result.clear();

    size_t timerTimeout = EventLoop::WaitInfinite;

    // Check for active timers and process them
    updateTimer(timerTimeout);

    // no timer will become active within the idle timeout
    if(timerTimeout > this->idleTimeout() || timerTimeout == EventLoop::WaitInfinite)
    {
        return this->idleTimeout();
    }

    // A timer will become active before the timeout expires
    result.setTimer();
    return timerTimeout;
}


void EventLoop::onCommitEvent(const Event& ev)
{
    {
        RecursiveLock lock( _queueMutex );

        // TODO: use a continuous block of memory to store events
        // this avoids new/delete
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


void EventLoop::onQueueEvent(const Event& ev)
{
    RecursiveLock lock( _queueMutex );

    // TODO: use a continuous block of memory to store events
    // this avoids new/delete
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


void EventLoop::onProcessEvents()
{
    while( false == _exitLoop )
    {
        RecursiveLock lock(_queueMutex);

        if ( _eventQueue.empty() || _exitLoop )
            break;

        Event* ev = _eventQueue.front();
        _eventQueue.pop_front();

        try
        {
            lock.unlock();
            event.send(*ev);
        }
        catch(...)
        {
            ev->destroy( this->allocator() );
            throw;
        }

        ev->destroy( this->allocator() );
    }
}



} // namespace System

} // namespace Pt
