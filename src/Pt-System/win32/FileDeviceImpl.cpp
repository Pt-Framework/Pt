/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
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
#include "win32.h"
#include "FileDeviceImpl.h"
#include "MainLoopImpl.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/IOError.h"
#include <cassert>

namespace Pt {

namespace System {

#ifdef _WIN32_WCE

FileDeviceImpl::FileDeviceImpl(FileDevice& dev)
: IODeviceImpl()
, _device(dev)
{
    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;
    _readOv.hEvent = NULL;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
    _writeOv.hEvent = NULL;
}

#else

FileDeviceImpl::FileDeviceImpl(FileDevice& dev)
: OverlappedIODeviceImpl(dev)
{
}

#endif

FileDeviceImpl::~FileDeviceImpl()
{ 
}


void FileDeviceImpl::open( const char* path, std::ios::openmode mode)
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

    if( mode & std::ios::in )
        access |= GENERIC_READ;

    if( mode & std::ios::out )
    {
        access |= GENERIC_WRITE;
        create = OPEN_ALWAYS;
    }

    if( mode & std::ios::trunc )
        create |= TRUNCATE_EXISTING;

#ifndef _WIN32_WCE
    flags |= FILE_FLAG_OVERLAPPED;
#endif

    std::basic_string<TCHAR> tpath;
    win32::fromMultiByte(path, tpath);
    HANDLE h = ::CreateFile(tpath.c_str(), access, share, NULL, create, flags, NULL);

    if(h == INVALID_HANDLE_VALUE)
        throw AccessFailed(path);

    this->setHandle(h);

    try
    {
        if(mode & std::ios::ate )
            this->seek(0, std::ios::end);
    }
    catch(...)
    {
        this->close();
        throw;
    }
}


bool FileDeviceImpl::beginOpen(EventLoop& loop, const char* path, std::ios::openmode mode)
{
    this->open(path, mode);
    return true;
}


void FileDeviceImpl::endOpen(EventLoop& loop)
{
}


bool FileDeviceImpl::runOpen(EventLoop& loop)
{
    return false;
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

    LARGE_INTEGER li;
    li.QuadPart = offset;

    DWORD ret = SetFilePointer(handle(), li.LowPart, &li.HighPart, whence);

    if(ret == INVALID_SET_FILE_POINTER)
        throw IOError( PT_ERROR_MSG("Could not set file pointer") );

    _readOv.Offset = ret;
    _writeOv.Offset = ret;

    return ret;
}


std::size_t FileDeviceImpl::peek(char* buffer, std::size_t count)
{
    bool eof;
    std::size_t ret = this->read(buffer, count, eof);
    if(ret > 0)
        this->seek(-((off_type)ret), std::ios::cur);

    return ret;
}

#ifdef _WIN32_WCE

void FileDeviceImpl::close()
{
    IODeviceImpl::close();

    _readOv.Offset = 0;
    _readOv.OffsetHigh = 0;

    _writeOv.Offset = 0;
    _writeOv.OffsetHigh = 0;
}


void FileDeviceImpl::cancel(EventLoop& loop)
{
}


void FileDeviceImpl::setTimeout(std::size_t)
{
    // wince file systems never block...
}


bool FileDeviceImpl::runRead(EventLoop& loop)
{
    return false;
}

bool FileDeviceImpl::runWrite(EventLoop& loop)
{
    return false;
}


std::size_t FileDeviceImpl::beginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return this->read(buffer, n, eof);
}


std::size_t FileDeviceImpl::endRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return 0;
}


std::size_t FileDeviceImpl::read(char* buffer, std::size_t count, bool& eof)
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
        else
        {
            throw IOError( PT_ERROR_MSG("Could not read from file handle") );
        }
    }

    _readOv.Offset += readBytes;
    _writeOv.Offset += readBytes;
    return readBytes;
}


std::size_t FileDeviceImpl::beginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return this->write(buffer, n);
}


std::size_t FileDeviceImpl::endWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return 0;
}


std::size_t FileDeviceImpl::write(const char* buffer, std::size_t count)
{
    DWORD writtenBytes = 0;

    if( FALSE == WriteFile(handle(), (void*)buffer, count, &writtenBytes, &_writeOv) )
    {
        throw IOError(PT_ERROR_MSG("Could not write to file handle") );
    }

    _readOv.Offset += writtenBytes;
    _writeOv.Offset += writtenBytes;
    return writtenBytes;
}

#endif

} //namespace System

} //namespace Pt
