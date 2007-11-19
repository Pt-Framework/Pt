/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Duerner                                 *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          *
 *   Copyright (C) 2007 Sebastian Pieck                                    *
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
#include "Pt/Signal.h"

#include "Pt/System/EventLoop.h"
#include "Pt/Event.h"

namespace Pt {
namespace System {

EventLoop::EventLoop()
: _exitLoop(false)
, _connectionMutex(Pt::System::Mutex::Normal)
, _mutex(Pt::System::Mutex::Normal)
, _timeout(Selector::WaitInfinite)
{
    connect(_selector.timeout, timeout);
}

EventLoop::~EventLoop()
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


void EventLoop::run()
{
    while( false == _exitLoop )
    {
        _mutex.lock();

        if( _eventQueue.empty() )
        {
            _mutex.unlock();
            _selector.wait(_timeout);
        }
        else
        {
            _mutex.unlock();
        }

        this->processEvents();
    }
}


void EventLoop::commitEvent(const Pt::Event& event)
{
    queueEvent(event);
    this->wake();
}

void EventLoop::processEvents()
{
    while( false == _exitLoop )
    {
        _mutex.lock();

        if( _eventQueue.empty() )
        {
            _mutex.unlock();
            break;
        }

        Pt::Event* ev = _eventQueue.front();
        _eventQueue.remove(ev);

        _mutex.unlock();

        event.send(*ev);
        delete ev;
    }
}

void EventLoop::wake()
{
    MutexLock lock( _mutex );
    _selector.wake();
}

void EventLoop::exit()
{
    _exitLoop = true;
    this->wake();
}

void EventLoop::queueEvent(const Pt::Event& event)
{
    MutexLock lock( _mutex );

    Pt::Event* ev = event.clone();
    _eventQueue.push_back(ev);
}


void EventLoop::add( IOResult& result )
{
    MutexLock lock( _mutex );
    return _selector.add( result );
}


void EventLoop::remove( IOResult& result )
{
    MutexLock lock( _mutex );
    _selector.remove( result );
}


void EventLoop::add( Timer& timer )
{
    MutexLock lock( _mutex );
    return _selector.add( timer );
}

void EventLoop::remove( Timer& timer)
{
    MutexLock lock( _mutex );
    _selector.remove( timer );
}


void EventLoop::setIdleTimeout(unsigned int msecs)
{
    _timeout = msecs;
}

unsigned int EventLoop::idleTimeout() const
{
    return _timeout;
}


} // namespace System
} // namespace Pt
