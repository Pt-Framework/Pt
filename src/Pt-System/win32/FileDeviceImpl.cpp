/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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
#include "Pt/IO/IODevice.h"
using namespace Pt::IO;

#include "FileDeviceImpl.h"


//LPSTR message;
//FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
//              NULL, exxr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
//printf(message);


namespace Pt {

namespace System {

FileDeviceImpl::FileDeviceImpl()
: _handle(INVALID_HANDLE_VALUE)
{
    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
}


FileDeviceImpl::~FileDeviceImpl() throw()
{
}


void FileDeviceImpl::open(const char* path, std::ios_base::openmode mode) throw(IO::IOError)
{
    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;

    DWORD access = GENERIC_READ; // open for reading
    DWORD share = FILE_SHARE_READ|FILE_SHARE_WRITE; // unix always allows sharing
    DWORD create = OPEN_EXISTING; // fail if not exist
    DWORD flags = 0;

    if(mode & std::ios_base::in ) {
        access |= GENERIC_READ; // open for reading
    }

    if(mode & std::ios_base::out) {
        access |= GENERIC_WRITE; // open for writing
        create = OPEN_ALWAYS;    // create if not exist
    }

    if(mode & std::ios_base::trunc )
        create |= TRUNCATE_EXISTING;

    //if(mode & IODevice::NonBlock)
    //{
    //    #ifdef _WIN32_WCE
    //        throw std::runtime_error("Overlapped I/O not supported under WinCE"+ PT_SOURCEINFO);
    //    #endif
    //    flags |= FILE_FLAG_OVERLAPPED; // open as non-blocking
    //    _readOv.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
    //    _writeOv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    //}

    std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
    _handle = ::CreateFile(tpath.c_str(), access, share, NULL, create, flags, NULL);

    if(_handle == INVALID_HANDLE_VALUE) {
        throw IO::IOError("Could not open file handle", PT_SOURCEINFO);
    }

    try {
        if(mode & std::ios_base::end )
            this->seek(0, IODevice::SeekEnd);
    }
    catch(...) {
        this->close();
        throw;
    }
}


void FileDeviceImpl::close() throw(IO::IOError)
{
    if(_readOv.hEvent != NULL)
        ::CloseHandle(_readOv.hEvent);

    if(_writeOv.hEvent != NULL)
        ::CloseHandle(_writeOv.hEvent);

    if(_handle != INVALID_HANDLE_VALUE)
    {
        if( FALSE == ::CloseHandle(_handle) )
            throw IO::IOError("Could not close file handle", PT_SOURCEINFO);

        _handle = INVALID_HANDLE_VALUE;
    }
}


FileDeviceImpl::pos_type FileDeviceImpl::seek(off_type offset, IO::IODevice::SeekMode mode) throw(IO::IOError)
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
        throw IO::IOError("Could not set file pointer", PT_SOURCEINFO);

    _readOv.Offset = ret;
    _writeOv.Offset = ret;

    return ret;
}

/*
void FileDeviceImpl::resize(off_type size) throw(IO::IOError)
{
    // remember current position
    off_type current = ::SetFilePointer(_handle, 0, NULL, FILE_CURRENT);

    // under Win32 resizeing is done by moving to the desired position
    // and then call SetEndOfFile on the handle.
    DWORD ret = ::SetFilePointer(_handle, size, NULL, FILE_BEGIN);
    if(ret == INVALID_SET_FILE_POINTER)
        throw IO::IOError("Could not set file pointer", PT_SOURCEINFO);

    if( false == ::SetEndOfFile(_handle) )
    {
        // go back to where we were before
        ::SetFilePointer(_handle, current, NULL, FILE_CURRENT);
        throw IO::IOError("Could not truncate file", PT_SOURCEINFO);
    }

    _readOv.Offset = size;
    _writeOv.Offset = size;
}
*/

size_t FileDeviceImpl::size() throw(IO::IOError)
{
    DWORD sz = GetFileSize(_handle, NULL);
    if(sz == INVALID_FILE_SIZE)
        throw IO::IOError("Could not get file size", PT_SOURCEINFO);

    return sz;
}


size_t FileDeviceImpl::read(char* buffer, size_t count, bool& eof) throw(IO::IOError)
{
    eof = false;
    DWORD readBytes = 0;

    if( FALSE == ReadFile(_handle, (void*)buffer, count, &readBytes, &_readOv) )
    {
        if( ERROR_HANDLE_EOF == GetLastError() )
        {
            eof = true;
            readBytes = 0;
        }
        else if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IO::IOError("Could not read from file handle", PT_SOURCEINFO);
        }

        #ifndef _WIN32_WCE
        else if
        ( FALSE == GetOverlappedResult(_handle, &_readOv, &readBytes, FALSE) == TRUE ) {
            readBytes = 0;
        }
        #endif
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


size_t FileDeviceImpl::write(const char* buffer, size_t count) throw(IO::IOError)
{
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(_handle, (void*)buffer, count, &writtenBytes, &_writeOv) )
    {
        if( ERROR_IO_PENDING != GetLastError() )
        {
            throw IO::IOError("Could not write to file handle", PT_SOURCEINFO);
        }

        #ifndef _WIN32_WCE
        if( FALSE == GetOverlappedResult(_handle, &_readOv, &writtenBytes, FALSE) == TRUE )
        {
            writtenBytes = 0;
        }
        #endif
    }

    _readOv.Offset += writtenBytes;
    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}


size_t FileDeviceImpl::peek(char* buffer, size_t count) throw(IO::IOError)
{
    bool eof;
    size_t ret = this->read(buffer, count, eof);
    if(ret > 0)
        this->seek(-((off_type)ret), IODevice::SeekCurrent);

    return ret;
}


void FileDeviceImpl::sync() const throw(IO::IOError)
{
    if( false == ::FlushFileBuffers(_handle) ) {
        throw IO::IOError("Could not flush file buffer", PT_SOURCEINFO);
    }
}


bool FileDeviceImpl::wait(IODevice::WaitMode mode, unsigned int msec) throw(IO::IOError)
{
    DWORD count = 0;
    HANDLE handles[2];

    if(mode & IODevice::WaitInput) {
        handles[count] = _readOv.hEvent;
        count++;
    }
    if(mode & IODevice::WaitOutput) {
        handles[count] = _writeOv.hEvent;
        count++;
    }

    DWORD ret = ::WaitForMultipleObjects(count, handles, FALSE, msec);
    if(ret == WAIT_FAILED) {
        throw IO::IOError ("Could not wait for file handle: ", PT_SOURCEINFO);
    }
    else if(ret == WAIT_TIMEOUT)
        return false;

    return true;
}


}

}
