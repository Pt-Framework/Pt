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

EventSource::Sentry::Sentry(const EventSource* es)
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

    if( _es->_dirty )
    {
        SinkMap::iterator it = _es->_handlers.begin();
        while( it != _es->_handlers.end() )
        {
            if( it->second )
            {
                ++it;
            }
            else
            {
                _es->_handlers.erase(it++);
            }
        }
    }

    _es->_dirty = false;
    _es->_sentry = 0;
    _es = 0;
}


bool EventSource::Sentry::operator!() const
{
     return _es == 0;
}


EventSource::EventSource()
: _mutex(Pt::System::Mutex::Recursive)
, _dmutex(Pt::System::Mutex::Recursive)
, _sentry(0)
, _sending(false)
, _dirty(false)
{ }


EventSource::~EventSource()
{
    MutexLock dlock(_dmutex);

    while( true )
    {
        MutexLock lock( _mutex );

        if(_sentry)
            _sentry->detach();

        if( _handlers.empty() )
            return;

        EventSink* sink = _handlers.begin()->second;
        this->disconnect(*sink);
    }
}


void EventSource::connect(EventSink& sink)
{
    MutexLock lock1( sink._mutex );
    MutexLock lock2( _mutex );

    const std::type_info* ti = 0;
    _handlers.insert( std::make_pair(ti, &sink) );
    sink._sources.push_back(this);
}


void EventSource::disconnect(EventSink& sink)
{
    MutexLock lock1( sink._mutex );
    MutexLock lock2( _mutex );

    SinkMap::iterator it = _handlers.begin();
    while( it != _handlers.end() )
    {
        if(it->second == &sink)
        {
            if(_sending)
            {
                _dirty = true;
                it->second = 0;
            }
            else
            {
                _handlers.erase(it);
            }
        }
        else
            ++it;
    }

    sink._sources.remove(this);
}


void EventSource::send(const Pt::Event& ev)
{
    MutexLock lock(_mutex);
    EventSource::Sentry sentry(this);

    SinkMap::iterator it;
    for(it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        EventSink* sink = it->second;

        if(sink)
            sink->commitEvent(ev);

        if( ! sentry )
            return;
    }
}

} // namespace System

} // namespace Pt
