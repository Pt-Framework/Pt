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
#include "Pt/System/Thread.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>

namespace Pt {

namespace System {

Thread::Thread(Runnable& runnable, Mode mode)
: _impl(0)
, _runnable(&runnable)
{
    _impl = new ThreadImpl(*this, mode);
}


Thread::Thread(Mode mode)
: _impl(0)
, _runnable(0)
{
    _impl = new ThreadImpl(*this, mode);
}


Thread::~Thread()
{
    delete _impl;
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


void Thread::wait()
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
    if(_runnable == 0)
        throw std::logic_error("No runnable given to thread." + PT_SOURCEINFO);

    _runnable->run();
}

} // namespace System

} // !namespace Pt

