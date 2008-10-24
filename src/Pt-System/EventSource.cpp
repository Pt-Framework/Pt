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

bool EventSourceCmp::operator()(const std::type_info* t1, const std::type_info* t2) const
{
    if(t2 == 0)
        return false;

    if(t1 == 0)
        return true;

    return t1->before(*t2) != 0;
}


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
    MutexLock lock( _mutex );

    sink.onConnect(*this);

    const std::type_info* ti = 0;
    _handlers.insert( std::make_pair(ti, &sink) );
}


void EventSource::disconnect(EventSink& sink)
{
    MutexLock lock( _mutex );

    sink.onDisconnect(*this);

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
                _handlers.erase(it++);
                continue;
            }
        }

        ++it;
    }
}


void EventSource::subscribe(EventSink& sink, const std::type_info& ti)
{
    MutexLock lock( _mutex );

    sink.onConnect(*this);
    _handlers.insert( std::make_pair(&ti, &sink) );
}


void EventSource::unsubscribe(EventSink& sink, const std::type_info& ti)
{
    MutexLock lock( _mutex );

    sink.onUnsubscribe(*this);

    SinkMap::iterator it = _handlers.lower_bound(&ti);
    while( it != _handlers.end() && *(it->first) == ti )
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
                _handlers.erase(it++);
                continue;
            }
        }

        ++it;
    }
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
