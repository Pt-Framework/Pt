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

static Mutex pt_evt_mtx(Mutex::Normal);


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

	if( _es->_dirty == false )
	{
		_es->_sentry = 0;
		_es = 0;
		return;
	}

	std::list<EventSink*>::iterator it = _es->_sinks.begin();
	while( it != _es->_sinks.end() )
	{
		if( *it )
		{
			++it;
		}
		else
		{
			it = _es->_sinks.erase(it);
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
{ }


EventSource::~EventSource()
{
    while( true )
    {
        MutexLock lock1( pt_evt_mtx );
        MutexLock lock2( _mutex );

        if(_sentry)
            _sentry->detach();

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

    if( _sending )
    {
        _dirty = true;
        EventSink* ns = 0;
        std::replace(_sinks.begin(), _sinks.end(), &sink, ns );
        sink.removeSource(*this);
    }
    else
    {
        _sinks.remove(&sink);
        sink.removeSource(*this);
    }
}


void EventSource::send(const Pt::Event& ev)
{
    MutexLock lock(_mutex);

    Sentry sentry(this);

    std::list<EventSink*>::const_iterator it = _sinks.begin();
    for(; it != _sinks.end(); ++it)
    {
        EventSink* sink = *it;
        if(sink)
            sink->commitEvent(ev);

        if( ! sentry )
            return;
    }

    const std::type_info& ti = ev.typeInfo();
    HandlerMap::iterator hit = _handlers.lower_bound(&ti);
    while(hit != _handlers.end() && *(hit->first) == ti)
    {
        IEventHandler* handler = hit->second;

        if(handler)
            handler->send(ev);

        if( ! sentry )
            return;

        ++hit;
    }
}


void EventSource::removeSink(EventSink& sink)
{
    MutexLock lock(_mutex);
    _sinks.remove(&sink);
}


void EventSource::disconnect(EventSink& sink, const std::type_info& ti)
{
	MutexLock lock(_mutex);

	std::pair<HandlerMap::iterator, HandlerMap::iterator> range;
	range = _handlers.equal_range(&ti);

	HandlerMap::iterator it = range.first;
	for(it = range.first; it != range.second; ++it)
	{
		IEventHandler* handler = it->second;
		if( handler->involves(&sink) )
		{
			if( _sending )
			{
				_dirty = true;
				delete it->second;
				it->second = 0;
			}
			else
			{
				_handlers.erase(it);
			}

			sink.removeSource(*this);
			return;
		}
	}
}

} // namespace System

} // namespace Pt
