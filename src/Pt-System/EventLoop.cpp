/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Duerner                                 *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "SelectorImpl.h"
#include "Pt/System/EventLoop.h"

namespace Pt {

namespace System {

EventLoop::EventLoop()
: _exitLoop(false)
, _allocator(/*255, 64*/)
{
    _selector = new SelectorImpl();
}


EventLoop::~EventLoop()
{
    try
    {
        while ( ! _eventQueue.empty() )
        {
            Event* ev = _eventQueue.front();
            _eventQueue.pop_front();
            ev->destroy(_allocator);
        }
    }
    catch(...)
    {}

    delete _selector;
}


void EventLoop::onAdd( Selectable& s )
{
    return _selector->add( s );
}


void EventLoop::onRemove( Selectable& s )
{
    _selector->remove( s );
}


void EventLoop::onChanged(Selectable& s)
{
    _selector->changed(s);
}


void EventLoop::onRun()
{
    while( true )
    {
        MutexLock lock(_queueMutex);

        if(_exitLoop)
            break;

        if( !_eventQueue.empty() )
        {
            lock.unlock();
            this->processEvents();
        }

        lock.unlock();

        bool active = this->wait( this->idleTimeout() );
        if( ! active )
            timeout.send();
    }
}


bool EventLoop::onWait(std::size_t msecs)
{
    if( _selector->wait(msecs) )
    {
        MutexLock lock(_queueMutex);

        if( !_eventQueue.empty() )
        {
            lock.unlock();
            this->processEvents();
        }

        return true;
    }

    return false;
}


void EventLoop::onWake()
{
    _selector->wake();
}


void EventLoop::onExit()
{
    MutexLock lock(_queueMutex);
    _exitLoop = true;
    lock.unlock();

    this->wake();
}


void EventLoop::onCommitEvent(const Event& ev)
{
    {
        MutexLock lock( _queueMutex );

        // TODO: use a continuous block of memory to store events
        // this avoids new/delete
        Event& clonedEvent = ev.clone(_allocator);

        try
        {
            _eventQueue.push_back(&clonedEvent);
        }
        catch(...)
        {
            clonedEvent.destroy(_allocator);
            throw;
        }
    }

    this->wake();
}


void EventLoop::onProcessEvents()
{
    while( false == _exitLoop )
    {
        MutexLock lock(_queueMutex);

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
            ev->destroy(_allocator);
            throw;
        }

        ev->destroy(_allocator);
    }
}

} // namespace System

} // namespace Pt
