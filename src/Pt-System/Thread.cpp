/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr Marc Boris Duerner                       *
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
#include "ThreadImpl.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/Thread.h"
#include <Pt/System/EventLoop.h>
#include <stdexcept>

namespace Pt {

namespace System {

Thread::Thread(const Callable<void>& cb)
: _state(Thread::Ready)
, _impl(0)
{
    _impl = new ThreadImpl(cb);
}


Thread::Thread(EventLoopBase& loop)
: _state(Thread::Ready)
, _impl(0)
{
    _impl = new ThreadImpl( callable(loop, &EventLoopBase::run) );
}


Thread::~Thread()
{
    delete _impl;
}


void Thread::start()
{
    if( this->state() == Ready )
    {
        _impl->start();
        _state = Thread::Running;
    }
}


void Thread::exit()
{
    ThreadImpl::exit();
}


void Thread::yield()
{
    ThreadImpl::yield();
}


void Thread::sleep(unsigned int ms)
{
    ThreadImpl::sleep(ms);
}


void Thread::detach()
{
    _impl->detach();
}


void Thread::join()
{
    if( this->state() == Running )
    {
        _impl->join();
        _state = Thread::Finished;
    }
}


bool Thread::joinNoThrow()
{
    bool ret = true;
    try
    {
        _impl->join();
    }
    catch(...)
    {
        ret = false;
    }

    _state = Thread::Finished;
    return ret;
}


void Thread::terminate()
{
    _impl->terminate();
    _state = Thread::Finished;
}

} // namespace System

} // !namespace Pt

