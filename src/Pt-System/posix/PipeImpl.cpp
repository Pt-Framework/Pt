/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#include "PipeImpl.h"
#include <memory>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>


namespace Pt {

namespace System {

PipeIODevice::PipeIODevice()
: _fd(-1)
{
    _result.init(*this);
}


PipeIODevice::~PipeIODevice()
{
    try
    {
        this->close();
    }
    catch(...)
    {}
}


void PipeIODevice::open(int fd)
{
    _fd = fd;
    fcntl(_fd, F_SETFL, O_NONBLOCK);
    this->setValid(true);
}


IOResult& PipeIODevice::beginRead(char* buffer, size_t n)
{
    _result.setFd(_fd);
    _result.attach(buffer, n);
    return _result;
}


size_t PipeIODevice::endRead(IOResult& result)
{
    size_t n = this->read( result.data(), result.capacity() );
    return n;
}


size_t PipeIODevice::_read(char* buffer, size_t count, bool& eof)
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


size_t PipeIODevice::_write(const char* buffer, size_t count)
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


void PipeIODevice::_sync() const
{
    fsync(_fd);
}


PipeImpl::PipeImpl()
{
    int fds[2];
    if(-1 == ::pipe(fds) )
        throw OpenFailed("pipe", PT_SOURCEINFO);

    _input.open( fds[0] );
    _output.open( fds[1] );
}


PipeImpl::~PipeImpl()
{
}


IODevice& PipeImpl::input()
{
    return _input;
}

IODevice& PipeImpl::output()
{
    return _output;
}

} // namespace System

} // namespace Pt
