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
#include "Pt/System/EventSink.h"
#include <algorithm>

namespace Pt {

namespace System {

void EventDispatcher::dispatch(const Pt::Event& ev)
{
    const std::type_info& ti = ev.typeInfo();
    EventHandlerMap::iterator hit = _handlers.lower_bound(&ti);
    while(hit != _handlers.end() && *(hit->first) == ti)
    {
        Invokable<const Pt::Event&>* handler = hit->second;

        if(handler)
            handler->invoke(ev);

        ++hit;
    }
}


/*EventSource::Sentry::Sentry(const EventSource* es)
: _es(es)
{
    _es->_sentry = this;
    _es->_sending = true;
    _es->_dirty = false;
}


EventSource::Sentry::~Sentry()
{
    if( _es )
        this->detach();
}


void EventSource::Sentry::detach()
{
    _es->_sending = false;

    if( _es->_dirty == false )
    {
        _es->_sentry = 0;
        _es = 0;
        return;
    }

    std::list<Connection>::iterator it = _es->_connections.begin();
    while( it != _es->_connections.end() )
    {
        if( it->valid() )
        {
            ++it;
        }
        else
        {
            it = _es->_connections.erase(it);
        }
    }

    _es->_dirty = false;
    _es->_sentry = 0;
    _es = 0;
}


bool EventSource::Sentry::operator!() const
{
     return _es == 0;
}*/


EventSource::EventSource()
: _mutex(Pt::System::Mutex::Recursive)
, _dmutex(Pt::System::Mutex::Recursive)
{ }


EventSource::~EventSource()
{
    MutexLock dlock(_dmutex);

    while( true )
    {
        MutexLock lock( _mutex );

        if( _sinks.empty() )
            return;

        EventSink* sink = _sinks.front();
        MutexLock block( sink->_mutex );

       _sinks.remove(sink);
        sink->_sources.remove(this);
    }
}


void EventSource::connect(EventSink& sink)
{
    MutexLock lock1( sink._mutex );
    MutexLock lock2( _mutex );

    _sinks.push_back(&sink);
    sink._sources.push_back(this);
}


void EventSource::disconnect(EventSink& sink)
{
    MutexLock lock1( sink._mutex );
    MutexLock lock2( _mutex );

   _sinks.remove(&sink);
    sink._sources.remove(this);
}


void EventSource::send(const Pt::Event& ev)
{
    MutexLock lock(_mutex);

    std::list<EventSink*>::iterator it;
    for(it = _sinks.begin(); it != _sinks.end(); ++it)
    {
        EventSink* sink = *it;
        sink->commitEvent(ev);
    }
}

} // namespace System

} // namespace Pt
