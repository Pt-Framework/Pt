/*
 * Copyright (C) 2006-2012 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "EventLoopImpl.h"

namespace Pt {

namespace System {

EventLoopImpl::EventLoopImpl(Signal<const Event&>& eventSignal)
: _event(&eventSignal)
{ }


EventLoopImpl::~EventLoopImpl()
{ }


void EventLoopImpl::attach(Selectable& s)
{
    _selector.attach(s);
}


void EventLoopImpl::detach(Selectable& s)
{
    _selector.detach(s);
}


void EventLoopImpl::avail(Selectable& s)
{
    MutexLock lock(_mutex);
    _avail.push_back(&s);
}


void EventLoopImpl::idle(Selectable& s)
{
    MutexLock lock(_mutex);

    std::vector<Selectable*>::iterator it = _avail.begin();
    while( it != _avail.end() )
    {
        if(*it == &s)
            it = _avail.erase(it);
        else
            ++it;
    }
}


void EventLoopImpl::run()
{
    while( this->waitNext() )
        ;
}


void EventLoopImpl::exit()
{
    _eventQueue.exit();
    wake();
}


void EventLoopImpl::wake()
{
    _selector.wake();
}


void EventLoopImpl::commitEvent(const Event& event)
{ 
    _eventQueue.pushEvent(event); 
    wake();
}


void EventLoopImpl::queueEvent(const Event& event)
{ 
    _eventQueue.pushEvent(event);  
}


bool EventLoopImpl::processEvents()
{ 
    return _eventQueue.processEvents(*_event);
}


bool EventLoopImpl::waitNext()
{
    bool isActive = true;
    size_t msecs = _timerQueue.processTimers();

    while(true)
    {
        MutexLock lock(_mutex);
        if( _avail.empty() )
            break;

        Selectable* selectable = _avail.back();
        _avail.pop_back();
        lock.unlock();

        msecs = 0;
        selectable->run();
    }

    if( _selector.waitForWake(msecs) )
        isActive = this->processEvents();

    return isActive;
}

} //namespace System

} //namespace Pt

