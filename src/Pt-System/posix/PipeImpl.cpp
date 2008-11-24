/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
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
#include "Pt/System/SystemError.h"
#include <memory>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>

namespace Pt {

namespace System {

PipeIODevice::PipeIODevice()
: _impl(*this)
{
}


PipeIODevice::~PipeIODevice()
{
    try
    {
        IODevice::close();
    }
    catch(...)
    {}
}


void PipeIODevice::open(int fd, bool isAsync)
{
    _impl.open(fd, isAsync);
    this->setEnabled(true);
    this->setAsync(isAsync);
}


bool PipeIODevice::onWait(std::size_t msecs)
{
    return _impl.wait(msecs);
}


size_t PipeIODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    return _impl.beginRead(buffer, n, eof);
}


size_t PipeIODevice::onEndRead(bool& eof)
{
    return _impl.endRead(eof);
}


size_t PipeIODevice::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl.read(buffer, count, eof);
}


size_t PipeIODevice::onBeginWrite(const char* buffer, size_t n)
{
    return _impl.beginWrite(buffer, n);
}


size_t PipeIODevice::onEndWrite()
{
    return _impl.endWrite();
}


size_t PipeIODevice::onWrite(const char* buffer, size_t count)
{
    return _impl.write(buffer, count);
}


void PipeIODevice::onSync() const
{
    _impl.sync();
}


PipeImpl::PipeImpl(bool isAsync)
{
    int fds[2];
    if(-1 == ::pipe(fds) )
        throw SystemError( PT_ERROR_MSG("pipe failed") );

    _out.open( fds[0] ,isAsync );
    _in.open( fds[1], isAsync );
}


PipeImpl::~PipeImpl()
{
}


IODevice& PipeImpl::out()
{
    return _out;
}

IODevice& PipeImpl::in()
{
    return _in;
}

} // namespace System

} // namespace Pt
