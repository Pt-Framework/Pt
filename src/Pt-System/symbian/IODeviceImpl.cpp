/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
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

#include <Pt/System/IOError.h>
#include <cerrno>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>


namespace Pt{

namespace System{

IODeviceImpl::IODeviceImpl()
: _fd(-1)
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
        fcntl(_fd, F_SETFL, flags);
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


IOResult& IODeviceImpl::beginRead(char* buffer, std::size_t n, bool& eof)
{
    _readResult.setFd(_fd);
    _readResult.attach(buffer, n);
    return _readResult;
}


std::size_t IODeviceImpl::endRead(IOResult& result, bool& eof)
{
    assert( &result == &_readResult );

    std::size_t n = this->read( result.impl()->buffer(), result.impl()->bufferSize(), eof );
    return n;
}


std::size_t IODeviceImpl::read( char* buffer, std::size_t count, bool& eof )
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


IOResult& IODeviceImpl::beginWrite(const char* buffer, std::size_t n)
{
    _writeResult.setFd(_fd);
    _writeResult.attach( (char*)buffer, n);
    return _writeResult;
}


std::size_t IODeviceImpl::endWrite(IOResult& result)
{
    assert( &result == &_writeResult );

    std::size_t n = this->write( result.impl()->buffer(), result.impl()->bufferSize() );
    return n;
}


std::size_t IODeviceImpl::write( const char* buffer, std::size_t count )
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


}//namespaec System

}//namespace Pt
