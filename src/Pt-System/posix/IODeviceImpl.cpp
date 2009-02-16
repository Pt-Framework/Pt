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
#include <cerrno>
#include <cassert>

namespace Pt {

namespace System {

IODeviceImpl::IODeviceImpl(IODevice& device)
: _device(device)
, _fd(-1)
, _rfds(0)
, _wfds(0)
, _efds(0)
{ }


IODeviceImpl::~IODeviceImpl()
{
    assert(_rfds == 0);
    assert(_wfds == 0);
    assert(_efds == 0);
    //this->exitSelect();
}


void IODeviceImpl::open(int fd, bool isAsync)
{
    _fd = fd;

    if(isAsync)
    {
        int flags = fcntl(_fd, F_GETFL);
        flags |= O_NONBLOCK ;
        fcntl(_fd, F_SETFL, O_NONBLOCK);
    }
}


void IODeviceImpl::close()
{
    if(_fd != -1)
    {
        if( ::close(_fd) != 0 )
            throw IOError( PT_ERROR_MSG("close failed") );

        _fd = -1;
    }
}


void IODeviceImpl::attach(SelectorBase& s)
{
    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }
}


size_t IODeviceImpl::endRead(bool& eof)
{
	if(_rfds)
	{
		FD_CLR( this->fd(), _rfds );
	}

    size_t n = this->read( _device.rbuf(), _device.rbuflen(), eof );
    return n;
}


size_t IODeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    eof = false;
    ssize_t ret = 0;

    while(true)
    {
        ret = ::read(_fd, (void*)buffer, count);
        eof = (ret == 0) ;

        if(ret >= 0)
            break;

        if(errno == EINTR) // signal interrupt
            continue;

        if(errno == EAGAIN) // non-blocking and no data yet
        {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(this->fd(), &fds);
            while( true )
            {
                int r = ::select(_fd+1, &fds, 0, 0, 0);
                if( r != -1 )
                    break;

                if( errno != EINTR )
                    throw IOError(  PT_ERROR_MSG("select failed")  );
            }

            continue;
        }

        throw IOError( PT_ERROR_MSG("read failed") );
    }

    return ret;
}


size_t IODeviceImpl::beginWrite(const char* buffer, size_t n)
{
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

    size_t n = this->write( _device.wbuf(), _device.wbuflen() );
    return n;
}


size_t IODeviceImpl::write( const char* buffer, size_t count )
{
    ssize_t ret = 0;

    while(true)
    {
        ret = ::write(_fd, (const void*)buffer, count);

        if(ret >= 0)
            break;

        if(errno == EINTR) // signal interrupt
            continue;

        if(errno == EAGAIN) // non-blocking and no data yet
        {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(this->fd(), &fds);
            while( true )
            {
                int r = ::select(_fd+1, 0, &fds, 0, 0);
                if( r != -1 )
                    break;

                if( errno != EINTR )
                    throw IOError( PT_ERROR_MSG("select failed") );
            }

            continue;
        }

        throw IOError( PT_ERROR_MSG("write failed") );
    }

    return ret;
}


void IODeviceImpl::sync() const
{
    int ret = fsync(_fd);
    if(ret != 0)
        throw IOError( PT_ERROR_MSG("sync failed") );
}



void IODeviceImpl::detach(SelectorBase& s)
{
    this->exitSelect();
}


size_t IODeviceImpl::beginRead(char* buffer, size_t n, bool&)
{
    if(_rfds)
    {
        FD_SET( this->fd(), _rfds );
    }

    return 0;
}


bool IODeviceImpl::wait(std::size_t msecs)
{
    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }

    struct timeval* timeout = 0;
    struct timeval tv;
    if(msecs != Selector::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    this->initWait(rfds, wfds, efds);

    while( true )
    {
        int ret = ::select(FD_SETSIZE, &rfds, &wfds, &efds, timeout);
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( PT_ERROR_MSG("select failed") );
    }

    return this->checkEvent(rfds, wfds, efds);
}


bool IODeviceImpl::wait(std::size_t msecs, fd_set* rfds, fd_set* wfds, fd_set* efds)
{
    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }

    struct timeval* timeout = 0;
    struct timeval tv;
    if(msecs != Selector::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    int ret = -1;
    while( true )
    {
        ret = ::select(FD_SETSIZE, rfds, wfds, efds, timeout);
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( PT_ERROR_MSG("select failed") );
    }

    return ret > 0;
}


void IODeviceImpl::initWait(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    if( this->fd() > 0 )
    {
        if( _device.rbuf() )
        {
            FD_SET(this->fd(), &rfds);
        }
        if( _device.wbuf() )
        {
            FD_SET(this->fd(), &wfds);
        }
    }
}


int IODeviceImpl::initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    _rfds = &rfds;
    _wfds = &wfds;
    _efds = &efds;
    this->initWait(rfds, wfds, efds);

    return this->fd();
}


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


int IODeviceImpl::checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    int avail = 0;

    if( this->fd() < 0)
        return 0;

    if ( FD_ISSET(this->fd(), &efds) )
    {
        _device.errorOccured(_device);
        ++avail;
    }

    if( _device.wbuf() && FD_ISSET(this->fd(), &wfds) )
    {
        _device.outputReady(_device);
        ++avail;
    }

    if( _device.rbuf() && FD_ISSET(this->fd(), &rfds) )
    {
        _device.inputReady(_device);
        ++avail;
    }

    return avail;
}

}//namespaec System

}//namespace Pt
