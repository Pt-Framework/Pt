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
{}


PipeIODevice::~PipeIODevice()
{
    try
    {
        IODevice::close();
    }
    catch(...)
    {}
}


void PipeIODevice::open(int fd)
{
    IODeviceImpl::open(fd);
    this->setValid(true);
}


IOResult& PipeIODevice::_beginRead(char* buffer, size_t n, bool& eof)
{
    return IODeviceImpl::beginRead(buffer, n, eof);
}


size_t PipeIODevice::_endRead(IOResult& result, bool& eof)
{
    return IODeviceImpl::endRead(result, eof);
}


size_t PipeIODevice::_read(char* buffer, size_t count, bool& eof)
{
    return IODeviceImpl::read(buffer, count, eof);
}


size_t PipeIODevice::_write(const char* buffer, size_t count)
{
    return IODeviceImpl::write(buffer, count);
}


void PipeIODevice::_sync() const
{
    fsync( IODeviceImpl::fd() );
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
