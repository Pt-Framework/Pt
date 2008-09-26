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
#include "Pt/Signal.h"
#include "Pt/Event.h"
#include "Pt/System/EventLoop.h"

namespace Pt {

namespace System {

EventLoopBase::EventLoopBase()
: _timeout(WaitInfinite)
{
    connect(event, *this, &EventLoopBase::dispatchEvent);
}


EventLoopBase::~EventLoopBase()
{
    DispatchTable::iterator it;
    for( it =_dispatchTable.begin(); it != _dispatchTable.end(); ++it)
    {
        delete it->second;
    }
}


/*void EventLoopBase::setParent(Application* app)
{
    this->onSetParent(app);
}*/


void EventLoopBase::run()
{
    this->onRun();
}


void EventLoopBase::exit()
{
    this->onExit();
}

void EventLoopBase::setIdleTimeout(unsigned int msecs)
{ 
    _timeout = msecs; 
}


unsigned int EventLoopBase::idleTimeout() const
{ 
    return _timeout; 
}


void EventLoopBase::commitEvent(const Event& event)
{
    this->onCommitEvent(event);
}


void EventLoopBase::queueEvent(const Event& event)
{
    this->onQueueEvent(event);
}


void EventLoopBase::processEvents()
{
    this->onProcessEvents();
}


void EventLoopBase::dispatchEvent(const Event& ev)
{
    const std::type_info& ti = ev.typeInfo();
    DispatchTable::iterator it = _dispatchTable.find(&ti);
    if( it != _dispatchTable.end() )
    {
        it->second->send(ev);
    }
}


bool CompareTypeInfo::operator()(const std::type_info* t1, const std::type_info* t2) const
{
    return t1->before(*t2) != 0;
}


EventLoop::EventLoop()
: _exitLoop(false)
, _connectionMutex(Mutex::Normal)
, _allocator(/*255, 64*/)
, _queueMutex(Mutex::Normal)
{
    _selector = new SelectorImpl();
}


EventLoop::~EventLoop()
{
    try
    {
        Connection connection;
        while( true )
        {
            {
                MutexLock lock( _connectionMutex );

                if( _connections.empty() )
                    break;

                connection = _connections.front();
                _connections.remove( connection );
            }

            connection.close();
        }
    }
    catch(...)
    {}

    delete _selector;
}


/*void EventLoop::onSetParent(Application* app)
{
    _selector->setParent(app);
}*/


bool EventLoop::opened(const Connection& c)
{
    MutexLock lock(_connectionMutex);
    bool accept = Connectable::opened(c);
    return accept;
}


void EventLoop::closed(const Connection& c)
{
    MutexLock lock(_connectionMutex);
    Connectable::closed(c);
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


bool EventLoop::onWait(unsigned int msecs)
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
    //MutexLock lock(_mutex);
    _selector->wake();
}


void EventLoop::onExit()
{
    MutexLock lock(_queueMutex);
    _exitLoop = true;
    lock.unlock();

    this->wake();
}


void EventLoop::onQueueEvent(const Event& ev)
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


void EventLoop::onCommitEvent(const Event& ev)
{
    queueEvent(ev);
    this->wake();
}


void EventLoop::onProcessEvents()
{
    while( false == _exitLoop )
    {
        MutexLock lock(_queueMutex);

        Event* ev = 0;

        if ( !_eventQueue.empty() )
        {
            ev = _eventQueue.front();
            _eventQueue.pop_front();
        }
        else
        {
            break;
        }

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
