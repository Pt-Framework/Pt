/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
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
#include <Pt/System/SystemError.h>
#include <chrono>
#include <system_error>

namespace Pt {

namespace System {

void ThreadImplFunction(const Callable<void>& cb)
{
    try
    {
        cb.invoke();
    }
    catch(const ThreadImpl::ThreadExit&)
    {
    }
}


void ThreadImpl::init(const Callable<void>& cb)
{
    delete _cb;
    _cb = cb.clone();
}


void ThreadImpl::start() 
{
    try
    {
        _thread = new std::thread( &ThreadImplFunction, std::ref(*_cb) );
    }
    catch(const std::system_error&)
    {
        throw SystemError("thread creation failed");
    }
}


void ThreadImpl::detach()
{ 
    if(_thread)
        _thread->detach();
}


void ThreadImpl::join()
{
    if(_thread)
        _thread->join();
}


void ThreadImpl::exit()
{
    throw ThreadExit();
}


void ThreadImpl::yield()
{ 
    std::this_thread::yield();
}


void ThreadImpl::sleep(unsigned int ms)
{
    std::chrono::milliseconds msecs(ms);
    std::this_thread::sleep_for(msecs);
}

} // namespace System

} // namespace Pt
