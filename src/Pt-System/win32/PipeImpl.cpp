/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
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
#include "Pt/System/Selector.h"
#include <windows.h>
#include <sstream>


namespace Pt {

namespace System {

PipeIODevice::PipeIODevice()
: _handle(INVALID_HANDLE_VALUE)
{
	_readResult.init(*this);

    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;
    _readOv.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
    _writeOv.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
}


PipeIODevice::~PipeIODevice()
{
    try
    {
        close();
    }
    catch(...)
    {
    }
}



void PipeIODevice::open(HANDLE handle, bool isAsync)
{
    _handle = handle;
    this->setValid(true);
    this->setEof(false);
    this->setAsync(isAsync);
}


HANDLE PipeIODevice::deviceHandle() const
{
    return _handle;
}

IOResult& PipeIODevice::_beginRead(char* buffer, size_t n, bool& eof)
{
    DWORD readBytes = 0;

    if( FALSE == ReadFile(_handle, (void*)buffer, n, &readBytes, &_readOv) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() )
        {
			eof = true;            
        }
        else if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not read from file handle", PT_SOURCEINFO);
        }
    }

    _readResult.setHandle(_readOv.hEvent);
    return _readResult;
}


size_t PipeIODevice::_endRead(IOResult& result, bool& eof)
{    
	assert(&result == &_readResult);
    
    DWORD readBytes = 0;
    if (GetOverlappedResult(_handle, &_readOv, &readBytes, FALSE) == FALSE )
    {
        if( ERROR_HANDLE_EOF == GetLastError() )
        {
			eof = true;            
        }
        else
        {
            throw IOError("Could not read from file handle", PT_SOURCEINFO);
        }        
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;

    return readBytes;
}


IOResult& PipeIODevice::_beginWrite(const char* buffer, size_t n)
{ 
	return IODeviceImpl::beginWrite(buffer, n); 
}


size_t PipeIODevice::_endWrite(IOResult& result)
{
	return IODeviceImpl::endWrite(result);
}


void PipeIODevice::_close()
{
    if(_readOv.hEvent != NULL)
        ::CloseHandle(_readOv.hEvent);

    if(_writeOv.hEvent != NULL)
        ::CloseHandle(_writeOv.hEvent);

    if(_handle != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseHandle(_handle) )
            throw IOError("Could not close file handle", PT_SOURCEINFO);

        _handle = INVALID_HANDLE_VALUE;
    }
}


size_t PipeIODevice::_read(char* buffer, size_t count, bool& eof)
{
    eof = false;
    DWORD readBytes = 0;

    if( FALSE == ReadFile(_handle, (void*)buffer, count, &readBytes, NULL) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() )
        {
            eof = true;
            readBytes = 0;
        }
        else if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not read from file handle", PT_SOURCEINFO);
        }
        else if (GetOverlappedResult(_handle, &_readOv, &readBytes, FALSE) == FALSE )
        {
            readBytes = 0;
        }
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


size_t PipeIODevice::_write(const char* buffer, size_t count)
{
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(_handle, (void*)buffer, count, &writtenBytes, NULL) )
    {
        if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not write to file handle", PT_SOURCEINFO);
        }
        if(GetOverlappedResult(_handle, &_readOv, &writtenBytes, FALSE) == FALSE )
        {
            writtenBytes = 0;
        }
    }

    _readOv.Offset += writtenBytes;
    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}


void PipeIODevice::_sync() const
{
    if( FALSE == ::FlushFileBuffers(_handle) ) {
        throw IOError("Could not flush file buffer", PT_SOURCEINFO);
    }
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
