/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
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
        throw SystemError("CreateEvent failed", PT_SOURCEINFO);

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


void PipeIODevice::open(HANDLE handle, bool isAsync)
{
    this->setHandle(handle);
    this->setEnabled(true);
    this->setEof(false);
    this->setAsync(isAsync);
}


bool PipeIODevice::onWait(unsigned int msecs)
{
    if( this->avail() )
    {
        this->checkEvent();
        return true;
    }

    if(_readOv.hEvent == NULL)
        this->setWaitHandle(_waitHandle);

    DWORD result = WaitForSingleObject(_readOv.hEvent, msecs);
    
    if(result == WAIT_OBJECT_0)
    {
        this->checkEvent();
        return true;
    }

    if(result != WAIT_TIMEOUT)
        throw IOError("WAIT_FAILED on pipe", PT_SOURCEINFO);
          
    return false;
}


bool PipeIODevice::setWaitHandle(HANDLE h)
{
    HANDLE prevHandle = _readOv.hEvent;

    if( prevHandle != h )
    {
        if(_rbuf && _readOv.hEvent)
            HasOverlappedIoCompleted(&_readOv) ? this->setState(Selectable::Avail) 
                                               : CancelIo( handle() );

        if(_wbuf && _writeOv.hEvent)
            HasOverlappedIoCompleted(&_writeOv)  ? this->setState(Selectable::Avail)
                                                 : CancelIo( handle() );

        _readOv.hEvent = h;
        _writeOv.hEvent = h;
    }
    
    if( ! prevHandle && _rbuf )
    {
        bool eof = this->eof();
        this->onBeginRead(_rbuf, _rbuflen, eof);
        this->setEof(eof);
    }

    if( ! prevHandle && _wbuf)
    {
        this->onBeginWrite(_wbuf, _wbuflen);
    }
 
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
    this->setWaitHandle(_waitHandle);
}


void PipeIODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{  
    if(_readOv.hEvent == NULL)
        return;

    DWORD readBytes = 0;
    if( FALSE == ReadFile(handle(), (void*)_rbuf, _rbuflen, &readBytes, &_readOv) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() )
        {
            eof = true;
        }
        else if( ERROR_BROKEN_PIPE == GetLastError() )
        {
            this->setState(Selectable::Avail);
            eof = true;
        }
        else if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not begin read from file handle", PT_SOURCEINFO);
        }
    }
    else
    {
        this->setState(Selectable::Avail);
    }
}


size_t PipeIODevice::onEndRead(bool& eof)
{
    DWORD readBytes = 0;
    
    if( this->eof() )
    { 
        eof = true;
        return 0;
    }

    if (GetOverlappedResult(handle(), &_readOv, &readBytes, FALSE) == FALSE )
    {
        DWORD err = GetLastError();
        if( ERROR_HANDLE_EOF == err )
        {
            eof = true;
        }
        else if( ERROR_BROKEN_PIPE == GetLastError() )
        {
            this->setState(Selectable::Avail);
            eof = true;
        }
        else
        {
            throw IOError("Could not end read from file handle", PT_SOURCEINFO);
        }
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


void PipeIODevice::onBeginWrite(const char* buffer, size_t n)
{
    if(_writeOv.hEvent != NULL)
    {
        DWORD writtenBytes = 0;
        if( FALSE == WriteFile(handle(), (void*)buffer, n, &writtenBytes, &_writeOv) )
        {
            DWORD err = GetLastError();
            if( ERROR_IO_PENDING != err )
            {
                throw IOError("Could not read from file handle", PT_SOURCEINFO);
            }
        }
        else
        {
            this->setState(Selectable::Avail);
        }
    }
}


size_t PipeIODevice::onEndWrite()
{
    DWORD writtenBytes = 0;
    
    if (GetOverlappedResult( handle(), &_writeOv, &writtenBytes, FALSE) == FALSE )
    {
        throw IOError("GetOverlappedResult failed", PT_SOURCEINFO);
    }

    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}


void PipeIODevice::onClose()
{
    IODeviceImpl::close();
}


size_t PipeIODevice::onRead(char* buffer, size_t count, bool& eof)
{
    eof = false;
    DWORD readBytes = 0;

    if( FALSE == ReadFile(handle(), (void*)buffer, count, &readBytes, NULL) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() || 
            ERROR_BROKEN_PIPE == GetLastError() )
        {
            eof = true;
            readBytes = 0;
        }
        else if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not read from file handle", PT_SOURCEINFO);
        }
        else if (GetOverlappedResult(handle(), &_readOv, &readBytes, FALSE) == FALSE )
        {
            readBytes = 0;
        }
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


size_t PipeIODevice::onWrite(const char* buffer, size_t count)
{
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(handle(), (void*)buffer, count, &writtenBytes, NULL) )
    {
        if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not write to file handle", PT_SOURCEINFO);
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
        throw IOError("Could not flush file buffer", PT_SOURCEINFO);
}




PipeImpl::PipeImpl(bool isAsync)
{
    std::stringstream ss;
    ss<<"\\\\.\\pipe\\ptpipe"<<_nameId;

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
        throw OpenFailed("Could not create named pipe", PT_SOURCEINFO);

    HANDLE outputHandle = ::CreateFile(ss.str().c_str(), access, share, NULL, create, flags, NULL);
    if(outputHandle == INVALID_HANDLE_VALUE)
        throw OpenFailed("Could not open file handle", PT_SOURCEINFO);

    _inputDevice.open(inputHandle, isAsync);
    _outputDevice.open(outputHandle, isAsync);

    _nameId++;
}


PipeImpl::~PipeImpl()
{
    _nameId--;
}


IODevice& PipeImpl::input()
{
    return _inputDevice;
}


IODevice& PipeImpl::output()
{
    return _outputDevice;
}


Pt::uint32_t  PipeImpl::_nameId = 0;

} // namespace System

} // namespace Pt
