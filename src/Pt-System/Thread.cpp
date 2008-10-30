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

Thread::Thread(const Callable<void>& cb, Mode mode)
: _impl(0)
, _cb(0)
{
    _impl = new ThreadImpl(*this, mode);
    _cb = cb.clone();
}


Thread::Thread(EventLoopBase& loop, Mode mode)
: _impl(0)
, _cb(0)
{
    _impl = new ThreadImpl(*this, mode);
    _cb = callable(loop, &EventLoopBase::run).clone();
}


Thread::Thread(Mode mode)
: _impl(0)
, _cb(0)
{
    _impl = new ThreadImpl(*this, mode);
}


Thread::~Thread()
{
    if( this->joinable() && this->state() == Running )
        this->join();

    delete _impl;
    delete _cb;
}


Thread::State Thread::state() const
{
    return _impl->state();
}


Thread::Mode Thread::mode() const
{
    return _impl->mode();
}


bool Thread::detached() const
{
    return _impl->mode() == Detached;
}


bool Thread::joinable() const
{
    return _impl->mode() == Joinable;
}


void Thread::start()
{
    if( this->state() != Ready )
        throw std::logic_error(PT_SOURCEINFO + "Thread already started");

    _impl->start( this->mode() );
}


void Thread::join()
{
    if( this->detached() )
        throw std::logic_error(PT_SOURCEINFO + "Can not wait on a detached thread");

    if( this->state() != Running )
        throw std::logic_error(PT_SOURCEINFO + "Thread is not running");

    _impl->wait();
}


void Thread::exit()
{
    ThreadImpl::exit();
}


void Thread::terminate()
{
    if( this->state() != Running )
        throw std::logic_error(PT_SOURCEINFO + "Thread is not running");

    _impl->terminate();
}


void Thread::detach()
{
    if( ! this->joinable() )
        throw std::logic_error(PT_SOURCEINFO + "Thread is not joinable");

    _impl->detach();
}


void Thread::yield()
{
    ThreadImpl::yield();
}


void Thread::sleep(unsigned int ms)
{
    ThreadImpl::sleep(ms);
}


void Thread::run()
{
    if(_cb == 0)
        throw std::logic_error("No runnable given to thread." + PT_SOURCEINFO);

    _cb->invoke();
}

} // namespace System

} // !namespace Pt

