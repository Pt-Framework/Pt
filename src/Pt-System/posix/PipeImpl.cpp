/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2009 Tommi Maekitalo
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
#include "PipeImpl.h"
#include "MainLoopImpl.h"
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


void PipeIODevice::open(int fd)
{
    _impl.open(fd, false);
}


void PipeIODevice::onClose()
{ 
    _impl.close();
}


void PipeIODevice::onSetTimeout(std::size_t timeout)
{
    _impl.setTimeout(timeout);
}


bool PipeIODevice::onRun()
{ 
    if( this->isReading() )
    {
        if( _ravail || isEof() || _impl.runRead( *loop() ) )
        {
            inputReady().send(*this);
            return true;
        }
    }

    if( this->isWriting() )
    {
        if( _wavail || _impl.runWrite( *loop() ) )
        {
            outputReady().send(*this);
            return true;
        }
    }

    return false;
}


void PipeIODevice::onCancel()
{
    if( loop() )
    {
        _impl.cancel( *loop() );
    }

    IODevice::onCancel();
}


std::size_t PipeIODevice::onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl.beginRead(loop, buffer, n, eof);
}


std::size_t PipeIODevice::onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl.endRead(loop, buffer, n, eof);
}


std::size_t PipeIODevice::onRead(char* buffer, std::size_t count, bool& eof)
{
    return _impl.read(buffer, count, eof);
}


std::size_t PipeIODevice::onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl.beginWrite(loop, buffer, n);
}


std::size_t PipeIODevice::onEndWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl.endWrite(loop, buffer, n);
}


std::size_t PipeIODevice::onWrite(const char* buffer, std::size_t count)
{
    return _impl.write(buffer, count);
}


void PipeIODevice::onSync() const
{
    _impl.sync();
}


void PipeIODevice::redirect(int newFd, bool close)
{
    int ret = ::dup2(fd(), newFd);
    if (ret < 0)
        throw SystemError("dup2");

    if (close)
    {
        IODevice::close();
        // second arg is true, because FD_CLOEXEC should not be set on fds 0,1,2
        _impl.open(newFd, true);
    }
}


void PipeIODevice::sigwrite( int signo )
{
    ::write(fd(), (const void*)&signo, sizeof(int));
}


PipeImpl::PipeImpl()
{
    int fds[2];
    if(-1 == ::pipe(fds) )
        throw SystemError( PT_ERROR_MSG("pipe failed") );

    _out.open( fds[0] );
    _in.open( fds[1] );
}


PipeIODevice& PipeImpl::out()
{
    return _out;
}

const PipeIODevice& PipeImpl::out() const
{
    return _out;
}

PipeIODevice& PipeImpl::in()
{
    return _in;
}

void PipeImpl::redirectStdin(bool close)
{
    out().redirect(0, close);
}

void PipeImpl::redirectStdout(bool close)
{
    in().redirect(1, close);
}

void PipeImpl::redirectStderr(bool close)
{
    in().redirect(2, close);
}

} // namespace System

} // namespace Pt
