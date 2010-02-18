/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 Bjoern Oliver Streule
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
#include "SelectorImpl.h"
#include "Pt/System/Selector.h"
#include "Pt/System/SystemError.h"
#include <windows.h>
#include <sstream>
#include <iostream>
#include <cassert>

namespace Pt {

namespace System {

PipeIODevice::PipeIODevice()
: _waitHandle(INVALID_HANDLE_VALUE)
{
    _waitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if( _waitHandle == NULL )
        throw SystemError( PT_ERROR_MSG("CreateEvent failed") );

    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
}


PipeIODevice::~PipeIODevice()
{
    try
    {
        Selectable::close();
    }
    catch(...)
    {
    }

    ::CloseHandle(_waitHandle);
}


void PipeIODevice::onCancel()
{
    ::CancelIo( handle() );

    DWORD bytes = 0;

    if( this->reading() && ! HasOverlappedIoCompleted(&_readOv) )
    {
        GetOverlappedResult( handle(), &_readOv, &bytes, TRUE );
    }

    if( this->writing() && ! HasOverlappedIoCompleted(&_writeOv) )
    {
        GetOverlappedResult( handle(), &_writeOv, &bytes, TRUE );
    }
}


void PipeIODevice::open(HANDLE handle, bool isAsync)
{
    this->setHandle(handle);
    this->setEnabled(true);
    this->setEof(false);
    this->setAsync(isAsync);
}


bool PipeIODevice::onWait(std::size_t msecs)
{
    if( this->avail() )
    {
        this->checkEvent();
        return true;
    }

    if(_readOv.hEvent == NULL)
    {
        bool active = false;
        this->setWaitHandle(_waitHandle, active);
        if( active )
        {
            this->checkEvent();
            return true;
        }
    }

    DWORD result = WaitForSingleObject(_readOv.hEvent, msecs);

    if(result == WAIT_OBJECT_0)
    {
        this->checkEvent();
        return true;
    }

    if(result != WAIT_TIMEOUT)
        throw IOError( PT_ERROR_MSG("WAIT_FAILED on pipe") );

    return false;
}


bool PipeIODevice::setWaitHandle(HANDLE h, bool& avail)
{
    // set avail to true if data is immediately available. This will
    // let the Selector check the other Selectables with a timeout of
    // 0 and call checkEvent on this object

    // the previous handle might be this objects event handle
    // or the one assigned by the Selector
    HANDLE prevHandle = _readOv.hEvent;

    // if the handle changes, we need to stop any previous I/O operation
    // unless it has already completed. _rbuf is set by an IODevice when
    // beginRead has been called. The event handle is NULL when this
    // method is called for the first time. If either one is not set, then
    // there were no I/O operations scheduled before.
    if( prevHandle != h )
    {
        if(_rbuf && _readOv.hEvent)
            HasOverlappedIoCompleted(&_readOv) ? avail = true
                                               : CancelIo( handle() );

        if(_wbuf && _writeOv.hEvent)
            HasOverlappedIoCompleted(&_writeOv)  ? avail = true
                                                 : CancelIo( handle() );

        _readOv.hEvent = h;
        _writeOv.hEvent = h;
    }

    // If _rbuf is set by IODevice::beginRead but the previous event used in
    // the overlapped structs is NULL, IODevice::beginRead was called before
    // the IODevice was added to a Selector or IODevice::wait was called for
    // the first time.
    if( ! prevHandle && _rbuf )
    {
        bool eof = false;
        size_t n = this->onBeginRead(_rbuf, _rbuflen, eof);
        if(eof || n > 0)
            avail = true;

        this->setEof(eof);
    }

    // see above. onBeginWrite could not do anything when it was called
    if( ! prevHandle && _wbuf)
    {
        size_t n = this->onBeginWrite(_wbuf, _wbuflen);
        if(n > 0)
            avail = true;
    }

    // we accept the HANDLE h
    return true;
}


bool PipeIODevice::checkEvent()
{
    bool avail = false;

    if( _wbuf && HasOverlappedIoCompleted(&_writeOv) )
    {
        outputReady.send(*this);
        avail = true;
    }

    if( _rbuf && HasOverlappedIoCompleted(&_readOv) )
    {
        inputReady.send(*this);
        avail = true;
    }

    return avail;
}


void PipeIODevice::onAttach(SelectorBase& s)
{
}


void PipeIODevice::onDetach(SelectorBase& s)
{
    // handle the case when we were added to a Selector and beginRead
    // was called with data possibly available. setWaitHandle() will
    // cancel the overlapped operation or set the active flag in which
    // case we set Avail so the next waiter knows data is available
    bool active = false;
    this->setWaitHandle(_waitHandle, active);

    if(active)
        this->setState(Selectable::Avail);
}


size_t PipeIODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    // IODevice::beginRead was called before the IODevice was added to
    // a selector or IODevice::wait was called for the first time
    if(_readOv.hEvent == NULL)
        return 0;

    // if we can can read data immediately, we return the number of bytes
    // that were read, so the Selector calls checkEvent on us even if the
    // event in the overlapped struct is not fired
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


size_t PipeIODevice::onEndRead(bool& eof)
{
    if( this->eof() )
    { 
        eof = true;
        return 0;
    }

    // a IODevice::beginRead outside a Selector was followed by an endRead
    // This happens when the IODevice is async, but used synchronously
    if(_readOv.hEvent == NULL)
    {
        return this->onRead(_rbuf, _rbuflen, eof);
    }

    // finishes the overlapped operation. Blocks until data is available
    // This was beginRead can be ended by endRead without a wait step.
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


size_t PipeIODevice::onRead(char* buffer, size_t count, bool& eof)
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


size_t PipeIODevice::onBeginWrite(const char* buffer, size_t n)
{
    if(_writeOv.hEvent == NULL)
        return 0;

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


size_t PipeIODevice::onEndWrite()
{
    if(_writeOv.hEvent == NULL)
    {
        return this->onWrite(_wbuf, _wbuflen);
    }

    DWORD writtenBytes = 0;
    if (GetOverlappedResult( handle(), &_writeOv, &writtenBytes, FALSE) == FALSE )
    {
        throw IOError( PT_ERROR_MSG("GetOverlappedResult failed") );
    }

    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}


void PipeIODevice::onClose()
{
    IODeviceImpl::close();
}


size_t PipeIODevice::onWrite(const char* buffer, size_t count)
{
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(handle(), (void*)buffer, count, &writtenBytes, NULL) )
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


void PipeIODevice::onSync() const
{
    if( FALSE == ::FlushFileBuffers( handle() ) )
        throw IOError( PT_ERROR_MSG("Could not flush file buffer") );
}

void PipeIODevice::redirect(int newFd, bool close)
{
}

PipeImpl::PipeImpl(bool isAsync)
{
    std::stringstream ss;
    ss<<"\\\\.\\pipe\\pt-" << GetCurrentProcessId() << '-' << _nameId;

    DWORD pflags = PIPE_ACCESS_DUPLEX;
    DWORD access = GENERIC_WRITE;
    DWORD share  = 0;
    DWORD create = OPEN_EXISTING;
    DWORD flags  = 0;
    
    if(isAsync)
    {
        flags  = FILE_FLAG_OVERLAPPED;
        pflags |= FILE_FLAG_OVERLAPPED;
    }

    HANDLE inputHandle = ::CreateNamedPipe(ss.str().c_str(),
                                           pflags,
                                           PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                           1,
                                           256,
                                           256,
                                           1000,
                                           NULL );
    if (inputHandle == INVALID_HANDLE_VALUE)
        throw SystemError("Could not create named pipe", PT_SOURCEINFO);

    HANDLE outputHandle = ::CreateFile(ss.str().c_str(), access, share, NULL, create, flags, NULL);
    if(outputHandle == INVALID_HANDLE_VALUE)
        throw SystemError("Could not open file handle", PT_SOURCEINFO);

    _out.open(inputHandle, isAsync);
    _in.open(outputHandle, isAsync);

    InterlockedIncrement(&_nameId);
}

PipeImpl::~PipeImpl()
{
    _nameId--;
}


PipeIODevice& PipeImpl::out()
{
    return _out;
}


PipeIODevice& PipeImpl::in()
{
    return _in;
}


LONG PipeImpl::_nameId = 0;

} // namespace System

} // namespace Pt
