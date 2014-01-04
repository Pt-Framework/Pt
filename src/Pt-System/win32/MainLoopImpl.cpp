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
#include "MainLoopImpl.h"

namespace Pt {

namespace System {

MainLoopImpl::MainLoopImpl(Signal<const Pt::Event&>& eventSignal)
: _event(&eventSignal)
{
}

MainLoopImpl::MainLoopImpl(Signal<const Pt::Event&>& eventSignal, Allocator& a)
: _eventQueue(a)
, _event(&eventSignal)
{
}


MainLoopImpl::~MainLoopImpl()
{
}


void MainLoopImpl::avail(Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);
    _avail.push_back(&s);
}


void MainLoopImpl::idle(Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);

    std::vector<Selectable*>::iterator it = _avail.begin();
    while(it != _avail.end())
    {
        if(*it == &s)
            it = _avail.erase(it);
        else
            ++it;
    }
}


void MainLoopImpl::run()
{
    while( this->waitNext() )
        ;
}


void MainLoopImpl::exit()
{
    _eventQueue.exit();
    wake();
}


void MainLoopImpl::commitEvent(const Event& event)
{ 
    _eventQueue.pushEvent(event); 
    wake();
}


void MainLoopImpl::queueEvent(const Event& event)
{ 
    _eventQueue.pushEvent(event); 
}


bool MainLoopImpl::waitNext()
{
    std::size_t timeout = _timerQueue.processTimers();

    // check all selectables that did not require waiting
    while( true )
    {
        Pt::System::MutexLock lock(_mutex);

        if( _avail.empty() )
            break;

        timeout = 0;
        Selectable* s = _avail.back();
        _avail.pop_back();
        lock.unlock();

        s->run();
    }

    bool isActive = true;
    if( _selector.waitForWake(timeout) )
        isActive = _eventQueue.processEvents(*_event);

    return isActive;
}

} //namespace System

} //namespace Pt
