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
#include "win32.h"
#include "FileDeviceImpl.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/Selector.h"
#include "Pt/System/SystemError.h"
#include <cassert>

namespace Pt {

namespace System {

FileDeviceImpl::FileDeviceImpl(FileDevice& dev)
: _device(dev)
, _waitHandle(INVALID_HANDLE_VALUE)
{
#ifndef _WIN32_WCE
    _waitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if( _waitHandle == NULL )
        throw SystemError("CreateEvent failed", PT_SOURCEINFO);
#endif

    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
}


FileDeviceImpl::~FileDeviceImpl()
{ 
#ifndef _WIN32_WCE
    ::CloseHandle(_waitHandle);
#endif
}


void FileDeviceImpl::open( const char* path, std::ios_base::openmode mode, bool isAsync )
{
    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;

    DWORD access = GENERIC_READ;
    DWORD share  = FILE_SHARE_READ|FILE_SHARE_WRITE;
    DWORD create = OPEN_EXISTING;
    DWORD flags  = 0;

    if( mode & std::ios_base::in )
        access |= GENERIC_READ;

    if( mode & std::ios_base::out )
    {
        access |= GENERIC_WRITE;
        create = OPEN_ALWAYS;
    }

    if( mode & std::ios_base::trunc )
        create |= TRUNCATE_EXISTING;

    if( isAsync )
    {
#ifndef _WIN32_WCE
        flags |= FILE_FLAG_OVERLAPPED;
#endif
    }

    std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
    HANDLE h = ::CreateFile(tpath.c_str(), access, share, NULL, create, flags, NULL);

    if(h == INVALID_HANDLE_VALUE)
        throw IOError("Could not open file handle", PT_SOURCEINFO); // OpenFailed

    this->setHandle(h);

    try
    {
        if(mode & std::ios_base::ate )
            this->seek(0, std::ios::end);
    }
    catch(...)
    {
        this->close();
        throw;
    }
}


void FileDeviceImpl::attach(SelectorBase& s)
{
}


void FileDeviceImpl::detach(SelectorBase& s)
{
    this->setWaitHandle(_waitHandle);
}


bool FileDeviceImpl::wait(unsigned int msecs)
{
#ifndef _WIN32_WCE
    if( _device.avail() )
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

#else
    return true;
#endif
}


bool FileDeviceImpl::setWaitHandle(HANDLE h)
{
#ifndef _WIN32_WCE

    HANDLE prevHandle = _readOv.hEvent;

    if( prevHandle != h )
    {
        if(_device._rbuf && _readOv.hEvent)
            HasOverlappedIoCompleted(&_readOv) ? _device.setState(Selectable::Avail) 
                                               : CancelIo( handle() );

        if(_device._wbuf && _writeOv.hEvent)
            HasOverlappedIoCompleted(&_writeOv)  ? _device.setState(Selectable::Avail)
                                                 : CancelIo( handle() );

        _readOv.hEvent = h;
        _writeOv.hEvent = h;
    }
    
    if( ! prevHandle && _device._rbuf )
    {
        bool eof = _device.eof();
        this->beginRead(_device._rbuf, _device._rbuflen, eof);
        _device.setEof(eof);
    }

    if( ! prevHandle && _device._wbuf)
    {
        this->beginWrite(_device._wbuf, _device._wbuflen);
    }
 
    return true;
    
#else

    if(_device._rbuf || _device._wbuf)
    {
        SetEvent(h);
    }

    return true;
    
#endif
}


bool FileDeviceImpl::checkEvent()
{
#ifndef _WIN32_WCE

    bool avail = false;

    if( _device._wbuf && HasOverlappedIoCompleted(&_writeOv) )
    {
        _device.outputReady.send( _device );
        avail = true;
    }
    
    if( _device._rbuf && HasOverlappedIoCompleted(&_readOv) )
    {
        _device.inputReady.send( _device );
        avail = true;
    }

    return avail;
    
#else

    bool avail = false;

    if(_device._wbuf)
    {
        _device.outputReady.send( _device );
        avail = true;
    }
    
    if(_device._rbuf)
    {
        _device.inputReady.send( _device );
        avail = true;
    }

    return avail;
    
#endif
}


void FileDeviceImpl::beginRead(char* buffer, size_t n, bool& eof)
{
    if(_readOv.hEvent == NULL)
        return;

    DWORD readBytes = 0;
    if( FALSE == ReadFile(handle(), (void*)buffer, n, &readBytes, &_readOv) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() )
        {
            eof = true;
        }
        else if( ERROR_BROKEN_PIPE == GetLastError() )
        {
            _device.setState(Selectable::Avail);
            eof = true;
        }
        else if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not begin read from file handle", PT_SOURCEINFO);
        }
    }
    else
    {
        _device.setState(Selectable::Avail);
    }
}

    
size_t FileDeviceImpl::endRead(bool& eof)
{
    DWORD readBytes = 0;

    if( _device.eof() )
    { 
        eof = true;
        return 0;
    }
    
#ifndef _WIN32_WCE
   
    if (GetOverlappedResult(handle(), &_readOv, &readBytes, FALSE) == FALSE )
    {
        DWORD err = GetLastError();
        if( ERROR_HANDLE_EOF == err )
        {
            eof = true;
        }
        else if( ERROR_BROKEN_PIPE == GetLastError() )
        {
            _device.setState(Selectable::Avail);
            eof = true;
        }
        else
        {
            throw IOError("Could not end read from file handle", PT_SOURCEINFO);
        }
    }
    
#else

    this->read(_device._rbuf, _device._rbuflen, eof);

#endif

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


void FileDeviceImpl::beginWrite(const char* buffer, size_t n)
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
            _device.setState(Selectable::Avail);
        }
    }
}

    
size_t FileDeviceImpl::endWrite()
{
    DWORD writtenBytes = 0;
  
#ifndef _WIN32_WCE
 
    if (GetOverlappedResult( handle(), &_writeOv, &writtenBytes, FALSE) == FALSE )
    {
        throw IOError("GetOverlappedResult failed", PT_SOURCEINFO);
    }

#else

    this->write(_device._wbuf, _device._wbuflen);

#endif

    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}	


FileDeviceImpl::pos_type FileDeviceImpl::seek(off_type offset, std::ios::seekdir sd)
{
    DWORD whence = FILE_BEGIN;
    switch(sd)
    {
        case std::ios::beg:
            whence = FILE_BEGIN;
            break;

        case std::ios::cur:
            whence = FILE_CURRENT;
            break;

        case std::ios::end:
            whence = FILE_END;
            break;

        default:
            throw std::invalid_argument("Unknown seekdir");
            break;
    }

    DWORD ret = SetFilePointer(handle(), offset, NULL, whence);

    if(ret == INVALID_SET_FILE_POINTER)
        throw IOError("Could not set file pointer", PT_SOURCEINFO);

    _readOv.Offset = ret;
    _writeOv.Offset = ret;

    return ret;
}


size_t FileDeviceImpl::size()
{
    DWORD sz = GetFileSize(handle(), NULL);
    if(sz == INVALID_FILE_SIZE)
        throw IOError("Could not get file size", PT_SOURCEINFO);

    return sz;
}

 
size_t FileDeviceImpl::read(char* buffer, size_t count, bool& eof)
{
    eof = false;
    DWORD readBytes = 0;

    if( FALSE == ReadFile(handle(), (void*)buffer, count, &readBytes, &_readOv) )
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

#ifndef _WIN32_WCE

        else if (GetOverlappedResult(handle(), &_readOv, &readBytes, FALSE) == FALSE )
        {
            readBytes = 0;
        }
        
#endif
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


size_t FileDeviceImpl::write(const char* buffer, size_t count)
{
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(handle(), (void*)buffer, count, &writtenBytes, &_writeOv) )
    {
        if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not write to file handle", PT_SOURCEINFO);
        }

#ifndef _WIN32_WCE
        
        if(GetOverlappedResult(handle(), &_readOv, &writtenBytes, FALSE) == FALSE )
        {
            writtenBytes = 0;
        }
        
#endif
    }

    _readOv.Offset += writtenBytes;
    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}


size_t FileDeviceImpl::peek(char* buffer, size_t count)
{
    bool eof;
    size_t ret = this->read(buffer, count, eof);
    if(ret > 0)
        this->seek(-((off_type)ret), std::ios::cur);

    return ret;
}


void FileDeviceImpl::sync() const
{
    if( false == ::FlushFileBuffers(handle()) ) {
        throw IOError("Could not flush file buffer", PT_SOURCEINFO);
    }
}

} //namespace System

} //namespace Pt
