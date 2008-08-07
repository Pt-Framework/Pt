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
#include <memory>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>

namespace Pt {

namespace System {

PipeIODevice::PipeIODevice()
{
    _impl.setParent(*this);
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


bool PipeIODevice::onWait(unsigned int msecs)
{
    return _impl.wait(msecs);
}


void PipeIODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    _impl.beginRead(buffer, n, eof);
}


size_t PipeIODevice::onEndRead(bool& eof)
{
    return _impl.endRead(eof);
}


size_t PipeIODevice::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl.read(buffer, count, eof);
}


void PipeIODevice::onBeginWrite(const char* buffer, size_t n)
{
    _impl.beginWrite(buffer, n);
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
        throw OpenFailed("pipe", PT_SOURCEINFO);

    _input.open( fds[0] ,isAsync );
    _output.open( fds[1], isAsync );
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
