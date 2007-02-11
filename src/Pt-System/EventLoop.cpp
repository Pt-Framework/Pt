/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Drner                                  *
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
#include "Pt/Signal.h"

#include "Pt/System/Event.h"
#include "Pt/System/EventLoop.h"

#include <string>
#include <iostream>
using namespace std;


namespace Pt {

namespace System {


EventLoop::EventLoop()
: _exitLoop(false)
{
}


EventLoop::~EventLoop()
{
    this->wake();
}


int EventLoop::run()
{
    while(false == _exitLoop)
    {
        _queueMutex.lock();

        if( _eventQueue.empty() )
        {
            _loopMutex.lock();
            _queueMutex.unlock();

            _loopCondition.wait( _loopMutex );

            _loopMutex.unlock();
        }
        else {
            _queueMutex.unlock();
        }

        this->processEvents();
    }

    return 0;
}


void EventLoop::commitEvent(const Pt::Event& event)
{
    queueEvent(event);
    this->wake();
}


void EventLoop::processEvents()
{
    while( false == _exitLoop )
    {
        _queueMutex.lock();

        if( _eventQueue.empty() ) {
            _queueMutex.unlock();
            break;
        }

        Pt::Event* ev = _eventQueue.front();
        _eventQueue.remove(ev);

        _queueMutex.unlock();

        event.send<const Pt::Event&>(*ev);
        delete ev;
    }
}


void EventLoop::wake()
{
    _loopMutex.lock();
    _loopCondition.signal();
    _loopMutex.unlock();
}


void EventLoop::exit()
{
    _exitLoop = true;
    this->wake();
}

void EventLoop::queueEvent(const Pt::Event& event)
{
    _queueMutex.lock();

    Pt::Event* ev = event.clone();
    _eventQueue.push_back(ev);

    _queueMutex.unlock();
}



} // namespace Gui

} // namespace Pt
