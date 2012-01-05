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

FileDeviceImpl::FileDeviceImpl(FileDevice& dev)
: OverlappedIODeviceImpl(dev)
{
}


FileDeviceImpl::~FileDeviceImpl()
{ 
//#ifndef _WIN32_WCE
//#endif
}


void FileDeviceImpl::open( const char* path, IODevice::OpenMode mode, EventLoop* loop)
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

    if( mode & IODevice::Read )
        access |= GENERIC_READ;

    if( mode & IODevice::Write )
    {
        access |= GENERIC_WRITE;
        create = OPEN_ALWAYS;
    }

    if( mode & IODevice::Trunc )
        create |= TRUNCATE_EXISTING;

#ifndef _WIN32_WCE
    flags |= FILE_FLAG_OVERLAPPED;
#endif

    std::basic_string<TCHAR> tpath;
    win32::fromMultiByte(path, tpath);
    HANDLE h = ::CreateFile(tpath.c_str(), access, share, NULL, create, flags, NULL);

    if(h == INVALID_HANDLE_VALUE)
        throw FileNotFound(path, PT_SOURCEINFO);

    this->setHandle(h);

    try
    {
        if(mode & IODevice::AtEnd )
            this->seek(0, std::ios::end);
    }
    catch(...)
    {
        this->close(loop);
        throw;
    }
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
        throw IOError( PT_ERROR_MSG("Could not set file pointer") );

    _readOv.Offset = ret;
    _writeOv.Offset = ret;

    return ret;
}


size_t FileDeviceImpl::size()
{
    DWORD sz = GetFileSize(handle(), NULL);
    if(sz == INVALID_FILE_SIZE)
        throw IOError( PT_ERROR_MSG("Could not get file size") );

    return sz;
}


size_t FileDeviceImpl::peek(char* buffer, size_t count)
{
    bool eof;
    size_t ret = this->read(buffer, count, eof);
    if(ret > 0)
        this->seek(-((off_type)ret), std::ios::cur);

    return ret;
}

} //namespace System

} //namespace Pt
