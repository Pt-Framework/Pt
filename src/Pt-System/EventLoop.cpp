/*
 * Copyright (C) 2007- 2010 Marc Boris Duerner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
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
#include "Pt/System/EventLoop.h"
#include "Pt/System/Timer.h"
#include "Pt/System/Clock.h"

namespace Pt {

namespace System {

EventLoopBase::EventLoopBase()
: _timeout(WaitInfinite)
{
}


EventLoopBase::~EventLoopBase()
{
    while( _timers.size() )
    {
       Timer* timer = _timers.begin()->second;
        timer->setSelector(0);
    }

    //while( _selectables.size() )
    //{
    //   Selectable* sel = *_selectables.begin();
    //    sel->setSelector(0);
    //}
}


void EventLoopBase::add(Selectable& s)
{
    s.setSelector(this);
}


void EventLoopBase::remove(Selectable& s)
{
    if(s.selector() == this)
        s.setSelector(0);
}


void EventLoopBase::add(Timer& timer)
{
    timer.setSelector(this);
}


void EventLoopBase::remove( Timer& timer )
{
    if(timer.selector() == this)
        timer.setSelector(0);
}


void EventLoopBase::onAddTimer(Timer& timer)
{
    if( timer.active() )
    {
        TimerMap::value_type elem(timer.finished(), &timer);
        _timers.insert(elem);
        //_timers.insert( std::make_pair(timer.finished(), &timer) );
    }
}


void EventLoopBase::onRemoveTimer( Timer& timer )
{
    std::multimap<Timespan, Timer*>::iterator it;
    for(it = _timers.begin(); it != _timers.end(); ++it)
    {
        if(it->second == &timer)
        {
            _timers.erase(it);
            return;
        }
    }
}


void EventLoopBase::onTimerChanged(Timer& timer)
{
    if( timer.active() )
    {
        TimerMap::value_type elem(timer.finished(), &timer);
        _timers.insert(elem);
        //_timers.insert( std::make_pair(timer.finished(), &timer) );
    }
    else
    {
        EventLoopBase::onRemoveTimer(timer);
    }
}


bool EventLoopBase::updateTimer(std::size_t& lowestTimeout)
{
    if( _timers.empty() )
        return false;

    Timespan now = Clock::getSystemTicks();
    Timer* timer = _timers.begin()->second;
    bool timerActive = now >= timer->finished();

    while( ! _timers.empty() )
    {
        timer = _timers.begin()->second;

        if( now < timer->finished() )
        {
            Pt::int64_t remaining = (timer->finished() - now).toUSecs();
            lowestTimeout = (remaining / 1000);
            if(remaining % 1000 > 0) ++lowestTimeout;
            break;
        }

        timer->update(now);

        if( ! _timers.empty() )
        {
            timer = _timers.begin()->second;
            _timers.erase( _timers.begin() );
            TimerMap::value_type elem(timer->finished(), timer);
            _timers.insert(elem);
            //_timers.insert( std::make_pair(timer->finished(), timer) );
        }
    }

    return timerActive;
}

} // namespace System

} // namespace Pt
