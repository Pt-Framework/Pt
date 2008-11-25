/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#include "IODeviceImpl.h"
#include "Pt/System/IOError.h"
#include <cerrno>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>

namespace Pt{

namespace System{

const short IODeviceImpl::POLLERR_MASK= POLLERR | POLLHUP | POLLNVAL;
const short IODeviceImpl::POLLIN_MASK= POLLIN;
const short IODeviceImpl::POLLOUT_MASK= POLLOUT;

IODeviceImpl::IODeviceImpl()
: _dev(0)
, _fd(-1)
, _rbuf(0)
, _rbuflen(0)
, _wbuf(0)
, _wbuflen(0)
{ }



IODeviceImpl::~IODeviceImpl()
{ }


void IODeviceImpl::open(const std::string& path, std::ios_base::openmode mode, bool isAsync)
{
    int flags = O_RDONLY;

    if( (mode & std::ios_base::in ) && (mode & std::ios_base::out) )
    {
        flags |= O_RDWR;
    }
    else if(mode & std::ios_base::out)
    {
        flags |= O_WRONLY;
    }
    else if(mode & std::ios_base::in  )
    {
        flags |= O_RDONLY;
    }

    if(isAsync)
        flags |= O_NONBLOCK;

    if(mode & std::ios::trunc)
        flags |= O_TRUNC;

    flags |=  O_NOCTTY;

    _fd = ::open( path.c_str(), flags );
    if(_fd == -1)
        throw AccessFailed("open failed", PT_SOURCEINFO);
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
            throw IOError("Could not close file handle", PT_SOURCEINFO);

        _fd = -1;
    }
}


bool IODeviceImpl::wait(unsigned int msecs)
{
    pollfd pfd;
    this->initializePoll(&pfd, 1);

    while( true )
    {
        int ret = ::poll(&pfd, 1, msecs);
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError( "Could not select on file descriptors", PT_SOURCEINFO );
    }

    return this->checkPollEvent();
}


void IODeviceImpl::beginRead(char* buffer, size_t n, bool&)
{
    _rbuf = buffer;
    _rbuflen = n;
}


size_t IODeviceImpl::endRead(bool& eof)
{
    size_t n = this->read( _rbuf, _rbuflen, eof );
    _rbuf = 0;
    _rbuflen = 0;
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
            return 0;

        throw IOError("Could not read from file handle", PT_SOURCEINFO);
    }

    return ret;
}


void IODeviceImpl::beginWrite(const char* buffer, size_t n)
{
    _wbuf = buffer;
    _wbuflen = n;
}


size_t IODeviceImpl::endWrite()
{
    size_t n = this->write( _wbuf, _wbuflen );
    _wbuf = 0;
    _wbuflen = 0;
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
            return 0;

        throw IOError("Could not read from file handle", PT_SOURCEINFO);
    }

    return ret;
}


void IODeviceImpl::sync() const
{
    int ret = fsync(_fd);
    if(ret != 0)
        throw IOError("Could not sync handle", PT_SOURCEINFO);
}


size_t IODeviceImpl::initializePoll(pollfd* pfd, size_t pollSize)
{
    assert(pfd != 0);
    assert(pollSize >= 1);

    pfd->fd = this->fd();
    pfd->revents = 0;
    pfd->events = 0;

    if(_rbuf)
        pfd->events |= POLLIN;
    if(_wbuf)
        pfd->events |= POLLOUT;

    _pfd = pfd;

	return 1;
}


bool IODeviceImpl::checkPollEvent()
{
    bool avail = false;

    if (_pfd->revents & POLLERR_MASK)
    {
        _dev->errorOccured(*_dev);
        avail = true;
    }

    if( _pfd->revents & POLLOUT_MASK )
    {
        _dev->outputReady(*_dev);
        avail = true;
    }

    if( _pfd->revents & POLLIN_MASK )
    {
        _dev->inputReady(*_dev);
        avail = true;
    }

    return avail;
}

}//namespaec System

}//namespace Pt
