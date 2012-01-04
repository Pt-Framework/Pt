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
    _impl.open(fd, false, parent());

//  _impl.open2(fd, false);
//
//  if( this->isActive() )
//      _impl.enable( *parent() );
}


void PipeIODevice::onClose()
{ 
    _impl.close( parent() );

//  if( _impl.isOpen() )
//  {
//      if( this->isActive() )
//          _impl.disable( *parent() );
//
//      _impl.close2();
//  }
}


void PipeIODevice::onAttach(EventLoop& loop)
{ 
    //if( _impl.isOpen() )
    //{
    //    _impl.enable(loop); 
    //}

    _impl.attach(loop); 
}


void PipeIODevice::onDetach(EventLoop& loop)
{
    //if( _impl.isOpen() ) // isOpen()
    //{
    //    _impl.disable(loop); 
    //}

    _impl.detach(loop);
}


bool PipeIODevice::onRun()
{ 
    //return _impl.run(); 

    if( this->reading() )
    {
        if( _ravail || _impl.runRead( *parent() ) )
        {
            inputReady().send(*this);
            return true;
        }
    }

    if( this->writing() )
    {
        if( _wavail || _impl.runWrite( *parent() ) )
        {
            outputReady().send(*this);
            return false;
        }
    }

    return false;
}


void PipeIODevice::onCancel()
{
    if( isActive() )
    {
        _impl.cancel( *parent() );
    }

    IODevice::onCancel();
}


size_t PipeIODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    return _impl.beginRead(*parent(), buffer, n, eof);
}


size_t PipeIODevice::onEndRead(bool& eof)
{
    return _impl.endRead(*parent(), eof);
}


size_t PipeIODevice::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl.read(buffer, count, eof);
}


size_t PipeIODevice::onBeginWrite(const char* buffer, size_t n)
{
    return _impl.beginWrite(*parent(), buffer, n);
}


size_t PipeIODevice::onEndWrite()
{
    return _impl.endWrite(*parent());
}


size_t PipeIODevice::onWrite(const char* buffer, size_t count)
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
        _impl.open(newFd, true, parent());
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
