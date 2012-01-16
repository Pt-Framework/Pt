/*
 * Copyright (C) 2005-2007 by Marc Boris Duerner
 * Copyright (C) 2006-2007 Tobias Mueller
 * Copyright (C) 2006-2007 PTV AG
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
#include "FileImpl.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/IOError.h"
#include "Pt/System/File.h"
#include "win32.h"
#include <string>
#include <iostream>
#include <windows.h>

namespace Pt {

namespace System {

/*void throwError(DWORD error, const std::string& path, const Pt::SourceInfo& si)
{
//INVALID_DATA,		EINVAL
//INVALID_PARAMETER,		EINVAL
//NOT_ENOUGH_MEMORY,		ENOMEM
//OUTOFMEMORY,		ENOMEM
//NOT_SAME_DEVICE,		EXDEV
//DIR_NOT_EMPTY,		ENOTEMPTY
//HANDLE_EOF,		ENODATA
//FILE_EXISTS,		EEXIST
//ALREADY_EXISTS,		EEXIST
//CANNOT_MAKE,		EPERM
//DISK_FULL,			ENOSPC
//HANDLE_DISK_FULL    ENOSPC
    
    switch(error)
    {
        case ERROR_READ_FAULT:
        case ERROR_WRITE_FAULT:
        case ERROR_CRC:
        case ERROR_IO_DEVICE:
        case ERROR_NOT_READY:
        case ERROR_BUSY:
        case ERROR_CANNOT_MAKE:
            throw IOError(path, si);

        case ERROR_WRITE_PROTECT:
        case ERROR_ACCESS_DENIED:
        case ERROR_SHARING_VIOLATION:
        case ERROR_LOCK_VIOLATION:
        case ERROR_NOT_OWNER:
        case ERROR_CURRENT_DIRECTORY:
            throw PermissionDenied(path, si);

        case ERROR_FILE_NOT_FOUND:
        case ERROR_FILENAME_EXCED_RANGE:
            throw FileNotFound(path, si);

        case ERROR_DIRECTORY:
            throw DirectoryNotFound(path, si);

        case ERROR_BAD_UNIT:
        case ERROR_BAD_DEVICE:  
            throw DeviceNotFound(path, si);

        case ERROR_ALREADY_EXISTS:
        case ERROR_FILE_EXISTS:
        case ERROR_BAD_PATHNAME:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_FILE_CORRUPT:
        case ERROR_FILE_INVALID:
        case ERROR_OPEN_FAILED:
            throw AccessFailed(path, si);

        default:
            throw SystemError(path, si);
    }
}*/


namespace {

/*void throwFileError(const std::string& path, const Pt::SourceInfo& si)
{
    DWORD error = GetLastError();
    switch(error)
    {
        case ERROR_BAD_PATHNAME:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_OPEN_FAILED:
            throw FileNotFound(path, si);

        default:
            throwError(error, path, si);
    }
}*/

}


std::size_t FileImpl::size(const std::string& path)
{
    WIN32_FIND_DATA data;
    std::basic_string<TCHAR> tpath;
    win32::fromMultiByte(path, tpath);

    HANDLE h = FindFirstFile(tpath.c_str(), &data);
    if(h == INVALID_HANDLE_VALUE)
        throw AccessFailed(path);

    FindClose(h);

    LARGE_INTEGER li;
    li.HighPart = data.nFileSizeHigh;
    li.LowPart = data.nFileSizeLow;
    return static_cast<std::size_t>(li.QuadPart);
}


void FileImpl::resize(const std::string& path, std::size_t newSize)
{
    std::basic_string<TCHAR> tpath;
    win32::fromMultiByte(path, tpath);

    HANDLE h = ::CreateFile( tpath.c_str(),
                             GENERIC_READ|GENERIC_WRITE,
                             FILE_SHARE_READ|FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL );

    if(h == INVALID_HANDLE_VALUE)
        throw AccessFailed(path);

    if( INVALID_SET_FILE_POINTER == ::SetFilePointer(h, newSize, NULL, FILE_BEGIN) ||
        FALSE == ::SetEndOfFile(h) )
    {
        ::CloseHandle(h);
        throw IOError("SetFilePointer");
    }

    if( FALSE == ::CloseHandle(h) )
        throw IOError("CloseHandle");
}


void FileImpl::remove(const std::string& path)
{
    std::basic_string<TCHAR> tpath;
    win32::fromMultiByte(path, tpath);

    if( FALSE == ::DeleteFile( tpath.c_str() ) )
        throw AccessFailed(path);
}


void FileImpl::move(const std::string& path, const std::string& to, bool allowCopy)
{
    std::basic_string<TCHAR> tpath;
    win32::fromMultiByte(path, tpath);

    std::basic_string<TCHAR> tto;
    win32::fromMultiByte(to, tto);

#ifdef _WIN32_WCE
    if( FALSE == ::MoveFile(tpath.c_str(), tto.c_str()) )
    {
        DWORD error = GetLastError();
        if(error == ERROR_NOT_SAME_DEVICE)
        {
            if( ! allowCopy )
                throw AccessFailed(path);

            if( FALSE == CopyFile( tpath.c_str(), tto.c_str(), TRUE ) )
                throw AccessFailed(path);

            FileImpl::remove(path);
            return;
        }

        throw AccessFailed(path);
    }
#else
    DWORD flags = 0;
    if(allowCopy)
        flags = MOVEFILE_COPY_ALLOWED;

    if( FALSE == ::MoveFileEx(tpath.c_str(), tto.c_str(), flags) )
    {
        throw AccessFailed(path);
    }
#endif
}


void FileImpl::create(const std::string& path)
{
    std::basic_string<TCHAR> tpath;
    win32::fromMultiByte(path, tpath);

    HANDLE h = CreateFile( tpath.c_str(), // file to create
                           GENERIC_WRITE, // open for writing
                           0, // do not share
                           NULL,
                           CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);

    if (h == INVALID_HANDLE_VALUE)
        throw AccessFailed(path);

    if( FALSE == ::CloseHandle(h) )
        throw IOError("CloseHandle");
}

} // namespace System

} // namespace Pt
