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
    HandlerMap::iterator hit = _handlers.lower_bound(&ti);
    while(hit != _handlers.end() && *(hit->first) == ti)
    {
        IEventHandler* handler = hit->second;

        if(handler)
            handler->send(ev);

        ++hit;
    }
}


void EventDispatcher::unsubscribe(const Slot& slot, const std::type_info& ti)
{
	std::pair<HandlerMap::iterator, HandlerMap::iterator> range;
	range = _handlers.equal_range(&ti);

	HandlerMap::iterator it = range.first;
	for(it = range.first; it != range.second; ++it)
	{
		IEventHandler* handler = it->second;
		if( handler->slot().equals(slot) )
		{
			_handlers.erase(it);
			break;
		}
	}
}


void EventDispatcher::unsubscribeAll(const Slot& slot)
{
	HandlerMap::iterator it;
	for(it = _handlers.begin(); it != _handlers.end(); ++it)
	{
		IEventHandler* handler = it->second;
		if( handler->slot().equals(slot) )
		{
			_handlers.erase(it);
		}
	}
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
}


EventSource::EventSource()
: _mutex(Pt::System::Mutex::Recursive)
, _dmutex(Pt::System::Mutex::Recursive)
{ }


EventSource::~EventSource()
{
    while( true )
    {
        if( ! _dmutex.tryLock() )
        {
            Thread::yield();
            continue;
        }

        MutexLock lock( _mutex );

        if( _connections.empty() )
            return;

        _dispatcher.unsubscribeAll( _connections.front().slot() );
        _connections.front().close();
        lock.unlock();
        _dmutex.unlock();
    }
}


void EventSource::send(const Pt::Event& ev)
{
    MutexLock lock(_mutex);
    Sentry sentry(this);

    _dispatcher.dispatch(ev);
}


void EventSource::unsubscribe(EventSink& sink, const std::type_info& ti)
{
	MethodSlot<void, EventSink, const Pt::Event&> eslot = slot(sink, &EventSink::commitEvent);

	std::list<Connection>::iterator iter = Connectable::connections().begin();
	std::list<Connection>::iterator end = Connectable::connections().end();

	for(; iter != end; ++iter)
	{
		if( iter->slot().equals(eslot) )
		{
			_dispatcher.unsubscribe(iter->slot(), ti);
			iter->close();
			return;
		}
	}
}


bool EventSource::opened(const Connection& c)
{
	MutexLock lock1(_dmutex);
	MutexLock lock(_mutex);
	Connectable::opened(c);
	return true;
}


void EventSource::closed(const Connection& c)
{
	MutexLock lock1(_dmutex);
	MutexLock lock2(_mutex);
	Connectable::closed(c);
}

} // namespace System

} // namespace Pt
