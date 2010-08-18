/*
 * Copyright (C) 2007 Marc Boris Duerner
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

namespace Pt {

namespace System {

EventLoop::EventLoop()
: _exitLoop(false)
, _allocator(/*255, 64*/)
, _usedalloc(&_allocator)
{
    _impl = new EventLoopImpl();
}

EventLoop::EventLoop(Allocator& a)
:_exitLoop(false)
, _allocator(/*255, 64*/)
, _usedalloc(&a)
{
	_impl = new EventLoopImpl();
}


EventLoop::~EventLoop()
{
    try
    {
        while ( ! _eventQueue.empty() )
        {
            Event* ev = _eventQueue.front();
            _eventQueue.pop_front();
            ev->destroy(*_usedalloc);
        }
    }
    catch(...)
    {}

    delete _impl;
}


void EventLoop::onAdd( Selectable& s )
{
    return _impl->add( s );
}


void EventLoop::onRemove( Selectable& s )
{
    _impl->remove( s );
}


void EventLoop::onReinit(Selectable& s)
{
}


void EventLoop::onChanged(Selectable& s)
{
    _impl->changed(s);
}


// void EventLoop::onRun()
// {
//     while( true )
//     {
//         RecursiveLock lock(_queueMutex);

//         if(_exitLoop)
//         {
//             _exitLoop = false;
//             break;
//         }

//         if( !_eventQueue.empty() )
//         {
//             lock.unlock();
//             this->processEvents();
//         }

//         lock.unlock();

//         bool active = this->wait( this->idleTimeout() );
//         if( ! active )
//             timeout.send();
//     }

//     exited();
// }


void EventLoop::onRun()
{
    while( true )
    {
        WaitResult result = this->waitNext( this->idleTimeout() );

        if( result.isTimeout() )
        {
            timeout.send();
            continue;
        }

        if( result.isEvent() )
        {
            RecursiveLock lock(_queueMutex);

            if(_exitLoop)
            {
                _exitLoop = false;
                break;
            }

            if( ! _eventQueue.empty() )
            {
                lock.unlock();
                this->processEvents();
            }

            lock.unlock();
        }
    }

    exited();
}


WaitResult EventLoop::waitNext(std::size_t msecs)
{
    size_t timerTimeout = EventLoop::WaitInfinite;

    // If a timer is immediately ready, still check for an
    // active selectable to avoid timer preemption
    if ( updateTimer(timerTimeout) )
    {
         return _impl->waitNext(0).setTimer();
    }

    // This handles the case when no timer will become
    // active in the given timeout. The result of the
    // wait call indicates activity
    if(timerTimeout > msecs || timerTimeout == EventLoop::WaitInfinite)
    {
        return _impl->waitNext(msecs);
    }

    // A timer will become active before the timeout expires
    while(true)
    {
        WaitResult result = _impl->waitNext(timerTimeout);

        if( result.isActive() )
            return result;

        if( updateTimer(timerTimeout) )
            return result.setTimer();
    }

    return WaitResult();
}


bool EventLoop::onWait(std::size_t msecs)
{
    // if( _impl->wait(msecs) )
    // {
    //     RecursiveLock lock(_queueMutex);

    //     if( !_eventQueue.empty() )
    //     {
    //         lock.unlock();
    //         this->processEvents();
    //     }

    //     return true;
    // }

    return false;
}


void EventLoop::onWake()
{
    _impl->wake();
}


void EventLoop::onExit()
{
    RecursiveLock lock(_queueMutex);
    _exitLoop = true;
    lock.unlock();

    this->wake();
}


void EventLoop::onCommitEvent(const Event& ev)
{
    {
        RecursiveLock lock( _queueMutex );

        // TODO: use a continuous block of memory to store events
        // this avoids new/delete
        Event& clonedEvent = ev.clone(*_usedalloc);

        try
        {
            _eventQueue.push_back(&clonedEvent);
        }
        catch(...)
        {
            clonedEvent.destroy(*_usedalloc);
            throw;
        }
    }

    this->wake();
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
            ev->destroy(*_usedalloc);
            throw;
        }

        ev->destroy(*_usedalloc);
    }
}

} // namespace System

} // namespace Pt
