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
#include "Pt/System/IODevice.h"
#include "Pt/System/EventLoop.h"

namespace Pt{ 

namespace System{

IODeviceImpl::IODeviceImpl()
: _handle(INVALID_HANDLE_VALUE)
{
}


IODeviceImpl::~IODeviceImpl()
{ 
}


void IODeviceImpl::setHandle(HANDLE h)
{
    _handle = h;
}


void IODeviceImpl::close(EventLoop* loop)
{
    if(_handle != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseHandle(_handle) )
            throw IOError("Could not close file handle", PT_SOURCEINFO);

        _handle = INVALID_HANDLE_VALUE;
    }
}

#ifndef _WIN32_WCE

/////////////////////////////////////////////////////////////////////
// OverlappedIODeviceImpl
/////////////////////////////////////////////////////////////////////

OverlappedIODeviceImpl::OverlappedIODeviceImpl(IODevice& dev)
: _device(dev)
{
    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
}


OverlappedIODeviceImpl::~OverlappedIODeviceImpl()
{
}


void OverlappedIODeviceImpl::close(EventLoop* loop)
{
    IODeviceImpl::close(loop);

    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
}


void OverlappedIODeviceImpl::cancel(EventLoop& loop)
{
    // CancelIO is enough, because we cancel in the same thread where the
    // operation was started
    CancelIo( handle() );

    DWORD bytes = 0;
    GetOverlappedResult( handle(), &_readOv, &bytes, TRUE );
    GetOverlappedResult( handle(), &_writeOv, &bytes, TRUE );

    if(_readOv.hEvent != NULL)
    {
        loop.impl().disable(_device);
    }

    _readOv.hEvent = NULL;
    _writeOv.hEvent = NULL;

}


void OverlappedIODeviceImpl::attach(EventLoop& loop)
{
    /*HANDLE h = loop.impl().enable(_device);

    _readOv.hEvent = h;
    _writeOv.hEvent = h;*/
}


void OverlappedIODeviceImpl::detach(EventLoop& loop)
{
    /*loop.impl().disable(_device);

    _readOv.hEvent = NULL;
    _writeOv.hEvent = NULL;*/
}


bool OverlappedIODeviceImpl::runRead(EventLoop& loop)
{
    if( HasOverlappedIoCompleted(&_readOv) )
    {
        return true;
    }

    return false;
}

bool OverlappedIODeviceImpl::runWrite(EventLoop& loop)
{
    if( HasOverlappedIoCompleted(&_writeOv) )
    {
        return true;
    }

    return false;
}


size_t OverlappedIODeviceImpl::beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    if(_readOv.hEvent == NULL)
    {
        HANDLE h = loop.impl().enable(_device);
        _readOv.hEvent = h;
        _writeOv.hEvent = h;
    }

    // if we can can read data immediately, we return the number of bytes
    // that were read, so the EventLoop calls onAvail, even if the event 
    // in the overlapped struct is not fired
    DWORD readBytes = 0;
    if( FALSE == ReadFile(handle(), (void*)buffer, n, &readBytes, &_readOv) )
    {
        DWORD err = GetLastError();
        if( ERROR_HANDLE_EOF == err || ERROR_BROKEN_PIPE == err )
        {
            eof = true;
            return 0;
        }
        else if( err == ERROR_IO_PENDING )
        {
            return 0;
        }

        throw IOError( PT_ERROR_MSG("Could not begin read from file handle") );
    }

    return readBytes;
}


size_t OverlappedIODeviceImpl::endRead(EventLoop& loop, bool& eof)
{
    // finishes the overlapped operation. Blocks until data is available,
    // so beginRead can be ended by endRead without a wait step.
    DWORD readBytes = 0;
    if( FALSE == GetOverlappedResult(handle(), &_readOv, &readBytes, TRUE) )
    {
        DWORD err = GetLastError();
        if( ERROR_BROKEN_PIPE == err || ERROR_BROKEN_PIPE == err )
        {
            eof = true;
        }
        else
        {
            throw IOError( PT_ERROR_MSG("Could not end read from file handle") );
        }
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


size_t OverlappedIODeviceImpl::read(char* buffer, size_t count, bool& eof)
{
    DWORD readBytes = 0;
    if( FALSE == ReadFile(handle(), (void*)buffer, count, &readBytes, &_readOv) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() || 
            ERROR_BROKEN_PIPE == GetLastError() )
        {
            eof = true;
            readBytes = 0;
        }
        else if( ERROR_IO_PENDING == GetLastError() )
        {
            if(FALSE == GetOverlappedResult(handle(), &_readOv, &readBytes, TRUE) )
            {
                throw IOError( PT_ERROR_MSG("Could not read from file handle") );
            }
        }
        else
        {
            throw IOError( PT_ERROR_MSG("Could not read from file handle") );
        }
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


size_t OverlappedIODeviceImpl::beginWrite(EventLoop& loop, const char* buffer, size_t n)
{
    if(_readOv.hEvent == NULL)
    {
        HANDLE h = loop.impl().enable(_device);
        _readOv.hEvent = h;
        _writeOv.hEvent = h;
    }

    DWORD writtenBytes = 0;
    if( FALSE == WriteFile(handle(), (void*)buffer, n, &writtenBytes, &_writeOv) )
    {
        DWORD err = GetLastError();
        if( ERROR_IO_PENDING == err )
        {
            return 0;
        }
        
        throw IOError( PT_ERROR_MSG("Could not read from file handle") );
    }

    return writtenBytes;
}


size_t OverlappedIODeviceImpl::endWrite(EventLoop& loop)
{
    DWORD writtenBytes = 0;
    if (GetOverlappedResult( handle(), &_writeOv, &writtenBytes, FALSE) == FALSE )
    {
        throw IOError( PT_ERROR_MSG("GetOverlappedResult failed") );
    }

    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}


size_t OverlappedIODeviceImpl::write(const char* buffer, size_t count)
{
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(handle(), (void*)buffer, count, &writtenBytes, &_writeOv) )
    {
        if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError(PT_ERROR_MSG("Could not write to file handle") );
        }
        if(GetOverlappedResult(handle(), &_readOv, &writtenBytes, FALSE) == FALSE )
        {
            writtenBytes = 0;
        }
    }

    _readOv.Offset += writtenBytes;
    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}

#endif

}//namespaec System

}//namespace Pt

