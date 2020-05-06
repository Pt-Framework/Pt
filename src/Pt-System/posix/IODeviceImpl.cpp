/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
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

#include "IODeviceImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/Logger.h"
#include "Pt/System/EventLoop.h"
#include <cerrno>
#include <cassert>

PT_LOG_DEFINE("Pt.System.IODevice")

namespace Pt {

namespace System {

IODeviceImpl::IODeviceImpl(IODevice& device)
: _ioh(device)
, _timeout(System::EventLoop::WaitInfinite)
, _errorPending(false)
{ 
}


IODeviceImpl::~IODeviceImpl()
{
    // make sure all operations were cancelled
    assert( ! _ioh.isActive() );
}


bool IODeviceImpl::isOpen() const
{
    return this->fd() != -1;
}


void IODeviceImpl::open(int fd, bool inherit)
{
    PT_LOG_DEBUG("opening fd:" << fd);

    // TODO: we do not need to enable the i/o handle now, but defer it
    // until we call impl().beginRead or impl().beginWrite on the i/o handle.
    // The EventLoopImpl can check internally...

    _ioh.fd = fd;

    int flags = fcntl(this->fd(), F_GETFL);
    flags |= O_NONBLOCK ;
    int ret = fcntl(this->fd(), F_SETFL, flags);
    if(-1 == ret)
        throw IOError("fcntl O_NONBLOCK failed");
    
    if ( ! inherit)
    {
        int flags = fcntl(this->fd(), F_GETFD);
        flags |= FD_CLOEXEC ;
        int ret = fcntl(this->fd(), F_SETFD, flags);
        if(-1 == ret)
            throw IOError("fcntl FD_CLOEXEC failed");
    }

    //if( loop )
    //    loop->impl().enable(_ioh);
}


void IODeviceImpl::close()
{
    // TODO: cancel() is always called before close(), so we do not need
    // a loop to disable the i/o handle

    // make sure all operations were cancelled
    assert( ! _ioh.isActive() );

    if( this->isOpen() )
    {
        _errorPending = false;

        //if(loop)
        //    loop->impl().disable(_ioh);

        int fd = _ioh.fd;
        _ioh.fd = -1;

        while ( ::close(fd) != 0 )
        {
            if( errno != EINTR )
                throw IOError( PT_ERROR_MSG("close failed") );
        }

        PT_LOG_DEBUG("closed fd:" << fd);
    }
}


void IODeviceImpl::cancel(EventLoop& loop)
{
    // disable the handle, we know cancel is always called before close
    // if this is attached to a loop

    if( this->isOpen() )
    {
        loop.selector().cancel(_ioh);
        PT_LOG_DEBUG("cancelling fd:" << _ioh.fd);
    }
}


std::size_t IODeviceImpl::beginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    PT_LOG_DEBUG("begin read on fd:" << _ioh.fd);
    
    for(;;)
    {
        ssize_t ret = ::read( _ioh.fd, (void*)buffer, n);
        if (ret > 0)
        {
            PT_LOG_DEBUG("read:" << ret << " bytes");
            return static_cast<std::size_t>(ret);
        }

        if(ret == 0 || errno == ECONNRESET)
        {
            eof = true;
            PT_LOG_DEBUG("read: EOF");
            return 0;
        }

        if(errno == EAGAIN)
            break;

        if(errno != EINTR)
            throw IOError("read failed");
    }

    loop.selector().beginRead( &_ioh );
    return 0;
}


std::size_t IODeviceImpl::endRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    PT_LOG_DEBUG("end read on fd:" << _ioh.fd);

    loop.selector().endRead( &_ioh );

    if (_errorPending)
    {
        _errorPending = false;
        throw IOError("read error");
    }

    return this->read( buffer, n, eof );
}


std::size_t IODeviceImpl::read( char* buffer, std::size_t count, bool& eof )
{
    ssize_t ret = 0;

    while(true)
    {
        ret = ::read( _ioh.fd, (void*)buffer, count);
        if(ret > 0)
            break;

        if(ret == 0 || errno == ECONNRESET)
        {
            eof = true;
            PT_LOG_DEBUG("read: EOF");
            return 0;
        }

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw IOError("read failed");

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(this->fd(), &rfds);
        bool avail = this->wait(_timeout, &rfds, 0, 0);
        if( ! avail )
            throw System::IOError("read");
    }

    PT_LOG_DEBUG("read: " << ret << " bytes");
    return ret;
}


std::size_t IODeviceImpl::beginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    PT_LOG_DEBUG("begin write on fd:" << _ioh.fd);

    for(;;)
    {
        ssize_t ret = ::write(_ioh.fd, (const void*)buffer, n);
        if (ret > 0)
        {
            PT_LOG_DEBUG("wrote:" << ret << " bytes");
            return static_cast<std::size_t>(ret);
        }

        if (ret == 0 || errno == ECONNRESET || errno == EPIPE)
            throw System::IOError("lost connection to peer");

        if(errno == EAGAIN)
            break;

        if(errno != EINTR)
            throw System::IOError("write failed");
    }
    
    loop.selector().beginWrite( &_ioh );
    return 0;
}


std::size_t IODeviceImpl::endWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    PT_LOG_DEBUG("end write on fd:" << _ioh.fd);

    loop.selector().endWrite( &_ioh );

    if (_errorPending)
    {
        _errorPending = false;
        throw IOError("write error");
    }

    return this->write( buffer, n );
}


std::size_t IODeviceImpl::write( const char* buffer, std::size_t count )
{
    ssize_t ret = 0;

    while(true)
    {
        ret = ::write(_ioh.fd, (const void*)buffer, count);
        if(ret > 0)
            break;

        if(ret == 0 || errno == ECONNRESET || errno == EPIPE)
            throw IOError("lost connection to peer");

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw IOError("Could not write to file handle");

        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(this->fd(), &wfds);
        bool avail = this->wait(_timeout, 0, &wfds, 0);
        if( ! avail )
            throw System::IOError("write");
    }

    PT_LOG_DEBUG("wrote: " << ret << " bytes");
    return ret;
}


bool IODeviceImpl::wait(std::size_t msecs, fd_set* rfds, fd_set* wfds, fd_set* efds)
{
    struct timeval* timeout = 0;
    struct timeval tv;
    if(msecs != EventLoop::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    int ret = -1;
    do
    {
        ret = ::select(FD_SETSIZE, rfds, wfds, efds, timeout);
    } 
    while (ret == -1 && errno == EINTR);

    if (ret == -1)
        throw IOError( PT_ERROR_MSG("select failed") );

    return ret > 0;
}


void IODeviceImpl::sync() const
{
    int ret = fsync(_ioh.fd);
    if(ret != 0)
        throw IOError( PT_ERROR_MSG("sync failed") );
}


bool IODeviceImpl::runRead(EventLoop& loop)
{
    if( ! this->isOpen() )
        return false;

    PT_LOG_DEBUG("run read on fd:" << _ioh.fd);

    Selector& selector = loop.selector();

    if ( selector.isError(&_ioh) )
    {
        _errorPending = true;
        return true;
    }

    return selector.isReadable(&_ioh);
}


bool IODeviceImpl::runWrite(EventLoop& loop)
{

    if( ! this->isOpen() )
        return false;

    PT_LOG_DEBUG("run write on fd:" << _ioh.fd);

    Selector& selector = loop.selector();

    if ( selector.isError(&_ioh) )
    {
        _errorPending = true;
        return true;
    }

    return selector.isWritable(&_ioh);
}

}//namespace System

}//namespace Pt
