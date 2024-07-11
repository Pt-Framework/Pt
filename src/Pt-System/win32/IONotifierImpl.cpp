/*
 * Copyright (C) 2006-2018 Marc Boris Duerner
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

#include "IONotifierImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/EventLoop.h"
#include "Selector.h"

namespace Pt {

namespace System {

IONotifierImpl::IONotifierImpl(IONotifier& ioNotifier)
: _flags(0)
{
    _handle.init(ioNotifier);
}


IONotifierImpl::~IONotifierImpl()
{
}


void IONotifierImpl::reset()
{
    _handle.setHandle(INVALID_HANDLE_VALUE);
}


void IONotifierImpl::setFd(int fd)
{
}


void IONotifierImpl::setHandle(void* h)
{
    if(h == INVALID_HANDLE_VALUE)
        throw IOError("invalid i/o handle");

    _handle.setHandle(h);
}


void IONotifierImpl::cancel(EventLoop& loop)
{
    _flags = 0;
    loop.selector().disable(_handle);
}


void IONotifierImpl::beginWait(EventLoop& loop, int flags)
{
    _flags = flags;

    if(_handle.handle() == INVALID_HANDLE_VALUE)
        throw IOError("invalid i/o handle");

    loop.selector().enable(_handle);
}


int IONotifierImpl::endWait(EventLoop& loop)
{
    if(_handle.handle() == INVALID_HANDLE_VALUE)
        throw IOError("invalid i/o handle");

    loop.selector().disable(_handle);
    int flags = _flags;
    _flags = 0;
    return flags;
}


bool IONotifierImpl::runWait(EventLoop& loop)
{
    return true;
}

} // namespace

} // namespace
