/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Drner                                  *
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
{
}


EventLoop::~EventLoop()
{
    _connectionMutex.lock();

     Connectable::shutDown();

    while( !_connections.empty() )
    {
        Connection connection = _connections.front();
        _connectionMutex.unlock();

        connection.close();
        _connectionMutex.lock();
        _connections.remove( connection );
    }

    _connectionMutex.unlock();
}


int EventLoop::run()
{
    while(false == _exitLoop)
    {
        _mutex.lock();

        if( _eventQueue.empty() )
        {
            _cond.wait( _mutex );
            _mutex.unlock();
        }
        else {
            _mutex.unlock();
        }

        this->processEvents();
    }

    return 0;
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

        if( _eventQueue.empty() ) {
            _mutex.unlock();
            break;
        }

        Pt::Event* ev = _eventQueue.front();
        _eventQueue.remove(ev);

        _mutex.unlock();

        event.send<const Pt::Event&>(*ev);
        delete ev;
    }
}


void EventLoop::wake()
{
    _mutex.lock();
    _cond.signal();
    _mutex.unlock();
}


void EventLoop::exit()
{
    _exitLoop = true;
    this->wake();
}


void EventLoop::queueEvent(const Pt::Event& event)
{
    _mutex.lock();

    Pt::Event* ev = event.clone();
    _eventQueue.push_back(ev);

    _mutex.unlock();
}


} // namespace Gui

} // namespace Pt
