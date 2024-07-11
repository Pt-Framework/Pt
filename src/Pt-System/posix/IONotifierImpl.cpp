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
#include "Pt/System/Logger.h"
#include "Pt/System/EventLoop.h"
#include <cerrno>
#include <cassert>

PT_LOG_DEFINE("Pt.System.IONotifier")

namespace Pt {

namespace System {

IONotifierImpl::IONotifierImpl(IONotifier& notifier)
: _ioh(notifier)
, _errorPending(false)
{
}


IONotifierImpl::~IONotifierImpl()
{
}


void IONotifierImpl::reset()
{
    assert( ! _ioh.isActive() );
    _ioh.fd = -1;
}


void IONotifierImpl::setFd(int fd)
{
    PT_LOG_DEBUG("set fd:" << fd);

    if(fd < 0)
        throw IOError("invalid i/o descriptor");

    _ioh.fd = fd;

    int flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK ;
    int ret = fcntl(fd, F_SETFL, flags);
    if(-1 == ret)
        throw IOError("fcntl O_NONBLOCK failed");
}


void IONotifierImpl::cancel(EventLoop& loop)
{
    if( _ioh.isActive() )
    {
        loop.selector().cancel(_ioh);
        PT_LOG_DEBUG("canceling fd:" << _ioh.fd);
    }
}


void IONotifierImpl::beginWait(EventLoop& loop, int flags)
{
    PT_LOG_DEBUG("begin wait on fd:" << _ioh.fd);

    if(_ioh.fd < 0)
        throw IOError("invalid i/o descriptor");

    loop.selector().beginWait(&_ioh, flags);
}


int IONotifierImpl::endWait(EventLoop& loop)
{
    PT_LOG_DEBUG("end wait on fd:" << _ioh.fd);

    if(_ioh.fd < 0)
        throw IOError("invalid i/o descriptor");

    int flags = loop.selector().endWait(&_ioh);

    if(_errorPending)
    {
        _errorPending = false;
        throw IOError("I/O error");
    }

    return flags;
}


bool IONotifierImpl::runWait(EventLoop& loop)
{
    if( ! _ioh.isActive() )
        return false;

    PT_LOG_DEBUG("run wait on fd:" << _ioh.fd);

    Selector& selector = loop.selector();

    if ( selector.isError(&_ioh) )
    {
        _errorPending = true;
        return true;
    }

    return selector.isReady(&_ioh);
}

} // namespace

} // namespace
