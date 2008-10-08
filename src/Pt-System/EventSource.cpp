/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
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
#include "Pt/System/EventSource.h"

namespace Pt {

namespace System {

static Mutex pt_evt_mtx(Mutex::Normal);


EventSource::EventSource()
: _mutex(Pt::System::Mutex::Recursive)
{ }


EventSource::~EventSource()
{
    while( true )
    {
        MutexLock lock1( pt_evt_mtx );
        MutexLock lock2( _mutex );

        if( _sinks.empty() )
            break;

        EventSink* sink = _sinks.front();
        _sinks.remove(sink);

        sink->removeSource(*this);
    }
}


void EventSource::connect(EventSink& sink)
{
    MutexLock lock(_mutex);
    sink.addSource(*this);
    _sinks.push_back(&sink);
}


void EventSource::disconnect(EventSink& sink)
{
    MutexLock lock(_mutex);
    _sinks.remove(&sink);
    sink.removeSource(*this);
}


void EventSource::send(const Pt::Event& ev)
{
    MutexLock lock(_mutex);

    std::list<EventSink*>::const_iterator it = _sinks.begin();
    for(; it != _sinks.end(); ++it)
    {
        EventSink* sink = *it;
        sink->commitEvent(ev);
    }

//     const std::type_info& ti = ev.typeInfo();
//     DispatchTable::iterator it2 = _dispatchTable.find(&ti);
//     if( it2 != _dispatchTable.end() )
//     {
//         it2->second->send(ev);
//     }
}


void EventSource::removeSink(EventSink& sink)
{
    MutexLock lock(_mutex);
    _sinks.remove(&sink);
}

/*
EventSource::EventSource()
: _mutex(Pt::System::Mutex::Normal)
{ }


EventSource::~EventSource()
{
    Connection connection;
    while( true )
    {
        {
            MutexLock lock( _mutex );

                if( _connections.empty() )
                break;

                connection = _connections.front();
                _connections.remove( connection );
        }

        connection.close();
    }
}


Connection EventSource::connect( EventLoopBase& receiver )
{
    // Do not lock here, the Connection will call
    // Connectable::opened on this object
    return Connection( *this, slot(receiver, &EventLoopBase::commitEvent).clone() );
}


Connection EventSource::connect( const Slot& s )
{
    // Do not lock here, the Connection will call
    // Connectable::opened on this object
    return Connection( *this, s.clone() );
}


bool EventSource::opened( const Connection& c )
{
    MutexLock lock(_mutex);
    bool accept = Connectable::opened(c);
    return accept;
}


void EventSource::closed( const Connection& c )
{
    MutexLock lock(_mutex);
    Connectable::closed(c);
}


void EventSource::send(const Pt::Event& ev) const
{
    typedef Pt::Invokable<const Pt::Event&, Pt::Void, Pt::Void> InvokableT;
    MutexLock lock(_mutex);

    std::list<Connection>::const_iterator it = Connectable::connections().begin();
    for(; it != _connections.end(); ++it)
    {
        if( false == it->valid() || &( it->sender() ) != this  )
            continue;

        const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
        invokable->invoke(ev);
    }
}
*/

} // namespace System

} // namespace Pt
