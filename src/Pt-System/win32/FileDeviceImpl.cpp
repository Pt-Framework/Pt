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
#include "Pt/System/Selector.h"
#include "FileDeviceImpl.h"

namespace Pt {
namespace System {

FileDeviceImpl::FileDeviceImpl()
: _handle(INVALID_HANDLE_VALUE)
, _state( Idle )
{
    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
}

FileDeviceImpl::~FileDeviceImpl()
{ }

void FileDeviceImpl::open( const char* path, std::ios_base::openmode mode, IODevice::ReadWriteMode rwMode )
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

    if( rwMode == IODevice::Asynchronous )
    {
        flags |= FILE_FLAG_OVERLAPPED;
        _readOv.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
        _writeOv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    }
    
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
    _handle = ::CreateFile(tpath.c_str(), access, share, NULL, create, flags, NULL);

    if(_handle == INVALID_HANDLE_VALUE)
        throw IOError("Could not open file handle", PT_SOURCEINFO);    

    try 
    {
        if(mode & std::ios_base::end )
            this->seek(0, IODevice::SeekEnd);
    }
    catch(...) 
    {
        this->close();
        throw;
    }
}

void FileDeviceImpl::close()
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

FileDeviceImpl::pos_type FileDeviceImpl::seek(off_type offset, IODevice::SeekMode mode)
{
    DWORD whence = FILE_BEGIN;
    switch(mode)
    {
        case IODevice::SeekBegin:
            whence = FILE_BEGIN;
            break;

        case IODevice::SeekCurrent:
            whence = FILE_CURRENT;
            break;

        case IODevice::SeekEnd:
            whence = FILE_END;
            break;
    }

    DWORD ret = SetFilePointer(_handle, offset, NULL, whence);

    if(ret == INVALID_SET_FILE_POINTER)
        throw IOError("Could not set file pointer", PT_SOURCEINFO);

    _readOv.Offset = ret;
    _writeOv.Offset = ret;

    return ret;
}

/*
void FileDeviceImpl::resize(off_type size) throw(IOError)
{
    // remember current position
    off_type current = ::SetFilePointer(_handle, 0, NULL, FILE_CURRENT);

    // under Win32 resizeing is done by moving to the desired position
    // and then call SetEndOfFile on the handle.
    DWORD ret = ::SetFilePointer(_handle, size, NULL, FILE_BEGIN);
    if(ret == INVALID_SET_FILE_POINTER)
        throw IOError("Could not set file pointer", PT_SOURCEINFO);

    if( false == ::SetEndOfFile(_handle) )
    {
        // go back to where we were before
        ::SetFilePointer(_handle, current, NULL, FILE_CURRENT);
        throw IOError("Could not truncate file", PT_SOURCEINFO);
    }

    _readOv.Offset = size;
    _writeOv.Offset = size;
}
*/

size_t FileDeviceImpl::size()
{
    DWORD sz = GetFileSize(_handle, NULL);
    if(sz == INVALID_FILE_SIZE)
        throw IOError("Could not get file size", PT_SOURCEINFO);

    return sz;
}


size_t FileDeviceImpl::read(char* buffer, size_t count, bool& eof)
{
    eof = false;
    DWORD readBytes = 0;
    _state = Reading;

    if( FALSE == ReadFile(_handle, (void*)buffer, count, &readBytes, &_readOv) )
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

        #ifndef _WIN32_WCE
        else if (GetOverlappedResult(_handle, &_readOv, &readBytes, FALSE) == FALSE )
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
    _state = Writing;
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(_handle, (void*)buffer, count, &writtenBytes, &_writeOv) )
    {
        if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IOError("Could not write to file handle", PT_SOURCEINFO);
        }

        #ifndef _WIN32_WCE
        if(GetOverlappedResult(_handle, &_readOv, &writtenBytes, FALSE) == FALSE )
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
        this->seek(-((off_type)ret), IODevice::SeekCurrent);

    return ret;
}


void FileDeviceImpl::sync() const
{
    if( false == ::FlushFileBuffers(_handle) ) {
        throw IOError("Could not flush file buffer", PT_SOURCEINFO);
    }
}

void FileDeviceImpl::eventHandles( std::vector<HANDLE>& handles, size_t waitMode )
{
    handles.clear();
    
    if(waitMode & Selector::WaitInput)
        handles.push_back( _readOv.hEvent );
    
    if(waitMode & Selector::WaitOutput)
        handles.push_back( _writeOv.hEvent );                
}
        
IODeviceImpl::WaitResult FileDeviceImpl::waitResult( HANDLE handle )
{
    if( handle == _readOv.hEvent )
        return IODeviceImpl::ReadyRead;
    else if( handle == _writeOv.hEvent ) 
        return IODeviceImpl::ReadyWrite;
    
    throw std::logic_error( "Unkonw event handle" + PT_SOURCEINFO );
}


bool FileDeviceImpl::waitable() const
{
#ifndef _WIN32_WCE
    return true;
#else
    return false;
#endif
}

} //namespace System
} //namespace Pt
