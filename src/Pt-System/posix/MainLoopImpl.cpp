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
#include <Pt/System/Logger.h>
#include <algorithm>

PT_LOG_DEFINE("Pt.System.MainLoop")

namespace Pt {

namespace System {

MainLoopImpl::MainLoopImpl(Signal<const Event&>& eventSignal)
: _event(&eventSignal)
{ }

MainLoopImpl::MainLoopImpl(Signal<const Event&>& eventSignal, Allocator& a)
: _event(&eventSignal)
, _eventQueue(a)
{ }


MainLoopImpl::~MainLoopImpl()
{ }


void MainLoopImpl::avail(Selectable& s)
{
    MutexLock lock(_mutex);

    std::vector<Selectable*>::iterator it = std::lower_bound(_avail.begin(),
                                                             _avail.end(), &s);

    if(it == _avail.end() || *it != &s)
        _avail.insert(it, &s);
}


void MainLoopImpl::idle(Selectable& s)
{
    MutexLock lock(_mutex);

    std::vector<Selectable*>::iterator it = std::lower_bound(_avail.begin(),
                                                             _avail.end(), &s);

    if(it != _avail.end() && *it == &s)
        _avail.erase(it);
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


void MainLoopImpl::commitEvent(const Event& ev)
{
    _eventQueue.pushEvent(ev);
    wake();
}


void MainLoopImpl::queueEvent(const Event& ev)
{
    _eventQueue.pushEvent(ev);
}


void MainLoopImpl::processEvents()
{
    //TODO: should this also check selectables?
    //return
    _eventQueue.processEvents(*_event);
}


// TODO: rename runNext
bool MainLoopImpl::waitNext()
{
    PT_LOG_TRACE("MainLoopImpl::waitNext");

    std::size_t timeout = _timerQueue.processTimers();

    // check all selectables that did not require waiting, but
    // for fairness reasons check only as many selectables as
    // were ready in the first place.

    PT_LOG_DEBUG("next timer expires in: " << timeout << " msecs");

    std::size_t n = 0;
    while( true )
    {
        MutexLock lock(_mutex);

        if( _avail.empty() )
            break;

        if(n == 0)
            n = _avail.size();

        timeout = 0;

        Selectable* selectable = _avail.back();
        _avail.pop_back();
        --n;

        lock.unlock();

        PT_LOG_DEBUG("running selectable");
        selectable->run();

        if(n == 0)
            break;
    }

    PT_LOG_DEBUG("waiting for events");
    bool isActive = true;
    if( _selector.waitForWake(timeout) )
        isActive = _eventQueue.processEvents(*_event);

    PT_LOG_TRACE("returning activity: " << isActive);
    return isActive;
}

} //namespace System

} //namespace Pt
