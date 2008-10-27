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
#include "Pt/System/EventSink.h"
#include "Pt/System/EventSource.h"

namespace Pt {

namespace System {

EventSink::EventSink()
: _mutex(Pt::System::Mutex::Recursive)
{ }


EventSink::~EventSink()
{
    while( true )
    {
        MutexLock lock( _mutex );

        if( _sources.empty() )
            return;

        EventSource* source = _sources.front();
        if( ! source->tryDisconnect(*this) )
        {
            lock.unlock();
            Thread::yield();
        }
    }
}


void EventSink::commitEvent(const Event& event)
{
    this->onCommitEvent(event);
}


void EventSink::onConnect(EventSource& source)
{
    MutexLock lock1( _mutex );

    _sources.push_back(&source);
}


void EventSink::onDisconnect(EventSource& source)
{
    MutexLock lock1( _mutex );

    _sources.remove(&source);
}


void EventSink::onUnsubscribe(EventSource& source)
{
    MutexLock lock1( _mutex );

    std::list<EventSource*>::iterator it;
    for(it = _sources.begin(); it != _sources.end(); ++it)
    {
        if(&source == *it)
        {
            _sources.erase(it);
            return;
        }
    }
}

} // namespace System

} // namespace Pt
