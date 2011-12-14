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
#include "MainLoopImpl.h"
#include "Pt/System/IOError.h"
#include <cerrno>
#include <cassert>

namespace Pt {

namespace System {

IODeviceImpl::IODeviceImpl(IODevice& device)
: _device(device)
, _fd(-1)
, _timeout(System::Selectable::WaitInfinite)
, _rfds(0)
, _wfds(0)
, _efds(0)
, _sentry(0)
, _errorPending(false)
{ }


IODeviceImpl::~IODeviceImpl()
{
    assert(_rfds == 0);
    assert(_wfds == 0);
    assert(_efds == 0);

    if(_sentry)
        _sentry->detach();

    //this->exitSelect();
}


void IODeviceImpl::open(int fd, bool isAsync, bool inherit)
{
    _fd = fd;

    if (isAsync)
    {
        int flags = fcntl(_fd, F_GETFL);
        flags |= O_NONBLOCK ;
        int ret = fcntl(_fd, F_SETFL, flags);
        if(-1 == ret)
            throw IOError(PT_ERROR_MSG("Could not set fd to non-blocking"));
    }

    if (!inherit)
    {
        int flags = fcntl(_fd, F_GETFD);
        flags |= FD_CLOEXEC ;
        int ret = fcntl(_fd, F_SETFD, flags);
        if(-1 == ret)
            throw IOError(PT_ERROR_MSG("Could not set FD_CLOEXEC"));
    }

}


void IODeviceImpl::close()
{
    if(_fd != -1)
    {
        int fd = _fd;
        _fd = -1;

        while ( ::close(fd) != 0 )
        {
            if( errno != EINTR )
                throw IOError( PT_ERROR_MSG("close failed") );
        }
    }
}


size_t IODeviceImpl::beginRead(char* buffer, size_t n, bool&)
{
    EventLoop* loop = _device.parent();
    if( loop )
    {
        loop->selector().impl().beginRead( _device, this->fd() );
    }

    if(_rfds)
    {
        FD_SET( this->fd(), _rfds );
    }

    return 0;
}


size_t IODeviceImpl::endRead(bool& eof)
{
    EventLoop* loop = _device.parent();
    if( loop )
    {
        loop->selector().impl().endRead( _device, this->fd() );
    }

    if(_rfds)
    {
        FD_CLR( this->fd(), _rfds );
    }

    if (_errorPending)
    {
        _errorPending = false;
        throw IOError("read error", PT_SOURCEINFO);
    }

    return this->read( _device.rbuf(), _device.rbuflen(), eof );
}


size_t IODeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    ssize_t ret = 0;

    while(true)
    {
        ret = ::read( _fd, (void*)buffer, count);
        if(ret > 0)
            break;

        if(ret == 0 || errno == ECONNRESET)
        {
            eof = true;
            return 0;
        }

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw IOError("read failed", PT_SOURCEINFO);

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(this->fd(), &rfds);
        bool ret = this->wait(_timeout, &rfds, 0, 0);
        if(false == ret)
        {
            throw System::IOTimeout();
        }
    }

    return ret;
}


size_t IODeviceImpl::beginWrite(const char* buffer, size_t n)
{
    ssize_t ret = ::write(_fd, (const void*)buffer, n);

    if (ret > 0)
        return static_cast<size_t>(ret);

    if (ret == 0 || errno == ECONNRESET || errno == EPIPE)
        throw System::IOError("lost connection to peer");

    if(_wfds)
    {
        FD_SET( this->fd(), _wfds );
    }

    return 0;
}


size_t IODeviceImpl::endWrite()
{
    if(_wfds)
    {
        FD_CLR( this->fd(), _wfds );
    }

    if (_errorPending)
    {
        _errorPending = false;
        throw IOError("write error", PT_SOURCEINFO);
    }

    if (_device.wavail() > 0)
    {
        size_t n = _device.wavail();
        return n;
    }

    return this->write( _device.wbuf(), _device.wbuflen() );
}


size_t IODeviceImpl::write( const char* buffer, size_t count )
{
    ssize_t ret = 0;

    while(true)
    {
        ret = ::write(_fd, (const void*)buffer, count);
        if(ret > 0)
            break;

        if(ret == 0 || errno == ECONNRESET || errno == EPIPE)
            throw IOError("lost connection to peer");

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw IOError("Could not write to file handle", PT_SOURCEINFO);

        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(this->fd(), &wfds);
        bool ret = this->wait(_timeout, 0, &wfds, 0);
        if(false == ret)
        {
            throw System::IOTimeout();
        }
    }

    return ret;
}


void IODeviceImpl::sigwrite( int signo )
{
    ::write(_fd, (const void*)&signo, sizeof(int));
}


void IODeviceImpl::cancel()
{
    if(_rfds)
    {
        FD_CLR( this->fd(), _rfds );
        _rfds = 0;
    }

    if(_wfds)
    {
        FD_CLR( this->fd(), _wfds );
        _wfds = 0;
    }
}


void IODeviceImpl::sync() const
{
    int ret = fsync(_fd);
    if(ret != 0)
        throw IOError( PT_ERROR_MSG("sync failed") );
}


void IODeviceImpl::attach(EventLoop& s)
{
    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }
}


void IODeviceImpl::detach(EventLoop& s)
{
    this->exitSelect();
}


bool IODeviceImpl::wait(std::size_t msecs)
{
    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    this->initWait(rfds, wfds, efds);
    this->wait(msecs, &rfds, &wfds, &efds);
    return this->checkEvent(rfds, wfds, efds);
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
    } while (ret == -1 && errno == EINTR);

    if (ret == -1)
        throw IOError( PT_ERROR_MSG("select failed") );

    return ret > 0;
}


void IODeviceImpl::initWait(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }

    if( this->fd() > 0 )
    {
        if( _device.rbuf() )
        {
            EventLoop* loop = _device.parent();
            if( loop )
            {
                loop->selector().impl().beginRead( _device, this->fd() );
            }

            FD_SET(this->fd(), &rfds);
        }

        if( _device.wbuf() )
        {
            FD_SET(this->fd(), &wfds);
        }
    }
}


// TODO: move to enable
int IODeviceImpl::initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    _rfds = &rfds;
    _wfds = &wfds;
    _efds = &efds;
    this->initWait(rfds, wfds, efds);

    return this->fd();
}


// TODO: move to detach / disable
void IODeviceImpl::exitSelect()
{
    if( this->fd() > 0)
    {
        if(_rfds)
            FD_CLR(this->fd(), _rfds);
        if(_wfds)
            FD_CLR(this->fd(), _wfds);
        if(_efds)
            FD_CLR(this->fd(), _efds);
    }

    _rfds = 0;
    _wfds = 0;
    _efds = 0;
}


bool IODeviceImpl::avail(Selector& s)
{
    // get selector impl
    // get fd_sets
    // check descriptor
    return false;
}


int IODeviceImpl::checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    int avail = 0;

    if( this->fd() < 0)
        return 0;

    DestructionSentry sentry(_sentry);

    if ( FD_ISSET(this->fd(), &efds) )
    {
        _errorPending = true;

        try
        {
            bool reading = _device.reading();
            bool writing = _device.writing();

            if (reading)
            {
                ++avail;
                _device.inputReady(_device);
            }

            if( ! _sentry )
                return avail;

            if (writing)
            {
                ++avail;
                _device.outputReady(_device);
            }

            if( ! _sentry )
                return avail;

            if (!reading && !writing)
            {
                avail = true;
                _device.close();
            }
        }
        catch (...)
        {
            _errorPending = false;
            throw;
        }
        _errorPending = false;

        return avail;
    }


    if( _device.wavail() > 0 || FD_ISSET(this->fd(), &wfds) )
    {
        _device.outputReady(_device);
        ++avail;
    }

    if( ! sentry )
        return avail;

    if( _device.rbuf() && FD_ISSET(this->fd(), &rfds) )
    {
        _device.inputReady(_device);
        ++avail;
    }

    return avail;
}

}//namespace System

}//namespace Pt
