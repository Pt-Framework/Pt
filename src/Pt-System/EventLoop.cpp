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

EventLoop::EventLoop(EventLoopImpl* impl)
: _impl(impl)
{
    this->add(_idleTimer);
    //_impl = new EventLoopImpl();
}


EventLoop::EventLoop(EventLoopImpl* impl, Allocator& a)
: _impl(impl)
{
    this->add(_idleTimer);
    //_impl = new EventLoopImpl(a);
}


EventLoop::~EventLoop()
{
    //delete _impl;
}

void EventLoop::init(EventLoopImpl* impl)
{
    _impl = impl;
}

Allocator& EventLoop::allocator()
{ 
    return _impl->allocator(); 
}


void EventLoop::setIdleTimeout(size_t msecs)
{ 
    if(msecs != WaitInfinite)
        _idleTimer.start(msecs);
    else
        _idleTimer.stop();

    //_impl->setIdleTimeout(msecs); 
}


size_t EventLoop::idleTimeout() const
{ 
    if( ! _idleTimer.active() )
        return WaitInfinite;

    return _idleTimer.interval();
    //return _impl->idleTimeout(); 
}


Signal<>& EventLoop::timeout()
{ 
    return _idleTimer.timeout();
    //return _impl->timeout(); 
}


Signal<const Event&>& EventLoop::event()
{ 
    return _impl->event(); 
}


Signal<>& EventLoop::exited()
{ 
    return _impl->exited(); 
}

void EventLoop::add(Selectable& s)
{
    s.setParent(this);
}


void EventLoop::remove(Selectable& s)
{
    if(s.parent() == this)
        s.setParent(0);
}


void EventLoop::add(Timer& timer)
{
    timer.setParent(this);
}


void EventLoop::remove( Timer& timer )
{
    if(timer.parent() == this)
        timer.setParent(0);
}


void EventLoop::run()
{
    _impl->run();
}


void EventLoop::exit()
{
    _impl->exit();
}


void EventLoop::onCommitEvent(const Event& ev)
{
    _impl->commitEvent(ev);
}


void EventLoop::onQueueEvent(const Event& ev)
{
    _impl->queueEvent(ev);
}


void EventLoop::onProcessEvents()
{
    _impl->processEvents();
}


void EventLoop::onWake()
{
    _impl->wake();
}


void EventLoop::onAddTimer(Timer& timer)
{
    _impl->addTimer(timer);
}


void EventLoop::onRemoveTimer( Timer& timer )
{
    _impl->removeTimer(timer);
}


//////////////////////////////////////////////////////////////////////////
// EventLoopImpl
//////////////////////////////////////////////////////////////////////////

EventLoopImpl::EventLoopImpl()
: _allocator(/*255, 64*/)
, _usedalloc(&_allocator)
//, _timeout(EventLoop::WaitInfinite)
, _state(0)
{}


EventLoopImpl::EventLoopImpl(Allocator& a)
: _allocator(/*255, 64*/)
, _usedalloc(&a)
//, _timeout(EventLoop::WaitInfinite)
, _state(0)
{}


EventLoopImpl:: ~EventLoopImpl()
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
        timer->setParent(0);
    }
}


void EventLoopImpl::run()
{
    _state = 0;
    this->onRun();
    exited();
}


void EventLoopImpl::exit()
{
    RecursiveLock lock(_queueMutex);
    _state = 1;
    lock.unlock();

    this->wake();
}


void EventLoopImpl::wake()
{
    this->onWake();
}


size_t EventLoopImpl::processTimers()
{
    /*if( result.isTimeout() )
    {
        timeout().send();
    }*/

    /*if( result.isEvent() )
    {
        RecursiveLock lock(_queueMutex);

        if(_state == 1)
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
    }*/

    //result.clear();

    size_t timerTimeout = EventLoop::WaitInfinite;

    // Check for active timers and process them
    processTimers(timerTimeout);

    // no timer will become active within the idle timeout
    /*if(timerTimeout > this->idleTimeout() || timerTimeout == EventLoop::WaitInfinite)
    {
        return this->idleTimeout();
    }*/

    // A timer will become active before the timeout expires
    //result.setTimer();
    return timerTimeout;
}


void EventLoopImpl::commitEvent(const Event& ev)
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


void EventLoopImpl::queueEvent(const Event& ev)
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


bool EventLoopImpl::processEvents()
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


void EventLoopImpl::addTimer(Timer& timer)
{
    if( timer.active() )
    {
        TimerQueue::value_type elem(timer.finished(), &timer);
        _timers.insert(elem);
    }
}


void EventLoopImpl::removeTimer( Timer& timer )
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


bool EventLoopImpl::processTimers(size_t& lowestTimeout)
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
}

} // namespace System

} // namespace Pt
