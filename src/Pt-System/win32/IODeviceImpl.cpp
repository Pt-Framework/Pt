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
#include "Pt/System/SystemError.h"
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


void IODeviceImpl::close()
{
    if(_handle != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseHandle(_handle) )
            throw IOError("Could not close file handle");

        _handle = INVALID_HANDLE_VALUE;
    }
}

#ifndef _WIN32_WCE

/////////////////////////////////////////////////////////////////////
// OverlappedIODeviceImpl
/////////////////////////////////////////////////////////////////////

OverlappedIODeviceImpl::OverlappedIODeviceImpl()
: _ioh()
, _ioEvent(NULL)
, _timeout(INFINITE)
{
    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
}


OverlappedIODeviceImpl::OverlappedIODeviceImpl(IODevice& dev)
: _ioh(dev)
, _ioEvent(NULL)
, _timeout(INFINITE)
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
    if(_ioEvent)
        CloseHandle(_ioEvent);
}


void OverlappedIODeviceImpl::init(IODevice& dev)
{
    _ioh.init(dev);
}


void OverlappedIODeviceImpl::close()
{
    IODeviceImpl::close();

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

    if(_readOv.hEvent)
        GetOverlappedResult( handle(), &_readOv, &bytes, TRUE );

    if(_writeOv.hEvent)
        GetOverlappedResult( handle(), &_writeOv, &bytes, TRUE );

    if(_readOv.hEvent != NULL)
    {
        loop.selector().disableOverlapped(_ioh);
    }

    _readOv.hEvent = NULL;
    _writeOv.hEvent = NULL;

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


std::size_t OverlappedIODeviceImpl::beginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    if(_readOv.hEvent == NULL)
    {
        loop.selector().enableOverlapped(_ioh);
        _readOv.hEvent = _ioh.handle();
        _writeOv.hEvent = _ioh.handle();
    }

    DWORD bufsize = n > std::numeric_limits<DWORD>::max() ? std::numeric_limits<DWORD>::max()
                                                          : static_cast<DWORD>(n);

    // if we can can read data immediately, we return the number of bytes
    // that were read, so the EventLoop calls onAvail, even if the event 
    // in the overlapped struct is not fired
    DWORD readBytes = 0;
    if( FALSE == ReadFile(handle(), (void*)buffer, bufsize, &readBytes, &_readOv) )
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

        loop.selector().disableOverlapped(_ioh);
        _readOv.hEvent = NULL;
        _writeOv.hEvent = NULL;

        throw IOError("ReadFile failed");
    }

    return readBytes;
}


std::size_t OverlappedIODeviceImpl::endRead(EventLoop& loop, char* buffer, std::size_t, bool& eof)
{
    // finishes the overlapped operation. Blocks until data is available,
    // so beginRead can be ended by endRead without a wait step.
    DWORD readBytes = 0;
    if( FALSE == GetOverlappedResult(handle(), &_readOv, &readBytes, TRUE) )
    {
        DWORD err = GetLastError();
        if( ERROR_BROKEN_PIPE == err )
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


std::size_t OverlappedIODeviceImpl::read(char* buffer, std::size_t n, bool& eof)
{
    if( ! _ioEvent)
    {
        _ioEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
        if( _ioEvent == NULL )
            throw SystemError("CreateEvent failed");
    }

    OVERLAPPED ov;
    ov.Offset = _readOv.Offset;
    ov.OffsetHigh = _readOv.OffsetHigh;
    ov.hEvent = _ioEvent;

    DWORD bufsize = n > std::numeric_limits<DWORD>::max() ? std::numeric_limits<DWORD>::max()
                                                          : static_cast<DWORD>(n);

    DWORD readBytes = 0;
    if( FALSE == ReadFile(handle(), (void*)buffer, bufsize, &readBytes, &ov) )
    {
        DWORD err = GetLastError();
        if(ERROR_HANDLE_EOF == err || ERROR_BROKEN_PIPE == err)
        {
            eof = true;
            return 0;
        }
        
        if(ERROR_IO_PENDING != err)
        {
            throw IOError("ReadFile failed");
        }

        DWORD result = WaitForSingleObject(ov.hEvent, _timeout);
        if(result != WAIT_OBJECT_0)
        {
            throw IOError("ReadFile timeout");
        }
    
        if(FALSE == GetOverlappedResult(handle(), &ov, &readBytes, TRUE) )
        {
            throw IOError("GetOverlappedResult failed");
        }
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


std::size_t OverlappedIODeviceImpl::beginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    if(_readOv.hEvent == NULL)
    {
        loop.selector().enableOverlapped(_ioh);
        _readOv.hEvent = _ioh.handle();
        _writeOv.hEvent = _ioh.handle();
    }

    DWORD bufsize = n > std::numeric_limits<DWORD>::max() ? std::numeric_limits<DWORD>::max()
                                                          : static_cast<DWORD>(n);

    DWORD writtenBytes = 0;
    if( FALSE == WriteFile(handle(), (void*)buffer, bufsize, &writtenBytes, &_writeOv) )
    {
        DWORD err = GetLastError();
        if( ERROR_IO_PENDING == err )
        {
            return 0;
        }

        loop.selector().disableOverlapped(_ioh);
        _readOv.hEvent = NULL;
        _writeOv.hEvent = NULL;

        throw IOError("WriteFile failed");
    }

    return writtenBytes;
}


std::size_t OverlappedIODeviceImpl::endWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    DWORD writtenBytes = 0;
    if (GetOverlappedResult( handle(), &_writeOv, &writtenBytes, FALSE) == FALSE )
    {
        throw IOError( PT_ERROR_MSG("GetOverlappedResult failed") );
    }

    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}


std::size_t OverlappedIODeviceImpl::write(const char* buffer, std::size_t n)
{
    if( ! _ioEvent)
    {
        _ioEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
        if( _ioEvent == NULL )
            throw SystemError("CreateEvent failed");
    }

    OVERLAPPED ov;
    ov.Offset = _writeOv.Offset;
    ov.OffsetHigh = _writeOv.OffsetHigh;
    ov.hEvent = _ioEvent;

    DWORD bufsize = n > std::numeric_limits<DWORD>::max() ? std::numeric_limits<DWORD>::max()
                                                          : static_cast<DWORD>(n);

    DWORD writtenBytes = 0;
    if( FALSE == WriteFile(handle(), (void*)buffer, bufsize, &writtenBytes, &ov) )
    {
        DWORD err = GetLastError();
        if( ERROR_IO_PENDING != err )
        {
            throw IOError("WriteFile");
        }
        
        DWORD result = WaitForSingleObject(ov.hEvent, _timeout);
        if(result != WAIT_OBJECT_0)
        {
            throw IOError("ReadFile timeout");
        }

        if(FALSE == GetOverlappedResult(handle(), &ov, &writtenBytes, TRUE) )
        {
            throw IOError("GetOverlappedResult");
        }
    }

    _readOv.Offset += writtenBytes;
    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}

#endif

}//namespaec System

}//namespace Pt

