/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#include "ThreadImpl.h"
#include <Pt/System/Thread.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/SystemError.h>

namespace Pt {

namespace System {

Thread::Thread()
: _state(Thread::Ready)
, _detach(false)
, _impl(0)
{
    _impl = new ThreadImpl();
}


Thread::Thread(const Callable<void>& cb)
: _state(Thread::Ready)
, _detach(false)
, _impl(0)
{
    _impl = new ThreadImpl();
	_impl->init(cb);
}


Thread::Thread(EventLoop& loop)
: _state(Thread::Ready)
, _detach(false)
, _impl(0)
{
    _impl = new ThreadImpl();
    _impl->init( callable(loop, &EventLoop::run) );
}


Thread::~Thread()
{
    if(_state == Running)
        std::terminate();

    delete _impl;
}


void Thread::init(const Callable<void>& cb)
{
    if( this->state() == Ready )
    {
        _impl->init(cb);
    }
}


void Thread::start()
{
    if( _state == Ready )
    {
        _impl->start();
        _state = Thread::Running;

        if(_detach)
            detach();
    }
}


void Thread::detach()
{
    if( _state == Ready )
    {
        _detach = true;
        return;
    }

    if( _state != Running && _state != Detached )
        throw SystemError("thread not detachable");

    _impl->detach();
    _state = Detached;
}


void Thread::join()
{
    if( _state != Running && _state != Joined )
        throw SystemError("thread not joinable");

    _impl->join();
    _state = Thread::Joined;
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

    return ret;
}

} // namespace System

} // namespace Pt
