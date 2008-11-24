/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
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

void throwError(DWORD error, const std::string& path, const Pt::SourceInfo& si)
{
    switch(error)
    {
        case ERROR_READ_FAULT:
        case ERROR_WRITE_FAULT:
            throw IOError(path, si);

        case ERROR_WRITE_PROTECT:
        case ERROR_ACCESS_DENIED:
            throw PermissionDenied(path, si);

        case ERROR_BAD_UNIT:
        case ERROR_INVALID_DRIVE:
        case ERROR_FILE_NOT_FOUND:
        case ERROR_BAD_PATHNAME:
            throw FileNotFound(path, si);

        case ERROR_NOT_READY:
        case ERROR_OPEN_FAILED:
        case ERROR_INVALID_NAME:
            throw AccessFailed(path, si);

        default:
            throw SystemError(path, si);
    }
}

std::size_t FileImpl::size(const std::string& path)
{
    WIN32_FIND_DATA data;
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);

    HANDLE h = FindFirstFile(tpath.c_str(), &data);
    if(h == INVALID_HANDLE_VALUE)
        throwError(GetLastError(), path, PT_SOURCEINFO);

    FindClose(h);

    LARGE_INTEGER li;
    li.HighPart = data.nFileSizeHigh;
    li.LowPart = data.nFileSizeLow;

    return static_cast<std::size_t>(li.QuadPart);
}


void FileImpl::resize(const std::string& path, std::size_t newSize)
{
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);

    HANDLE fileHandle = ::CreateFile(tpath.c_str(),
                                     GENERIC_READ|GENERIC_WRITE,
                                     FILE_SHARE_READ|FILE_SHARE_WRITE,
                                     NULL,
                                     OPEN_EXISTING,
                                     0,
                                     NULL);

    if(fileHandle == INVALID_HANDLE_VALUE)
    {
        throw SystemError(PT_ERROR_MSG("Could not open file") );
    }

    // under Win32 resizing is done by moving to the desired position
    // and then calling SetEndOfFile on the handle.
    DWORD ret = ::SetFilePointer(fileHandle, newSize, NULL, FILE_BEGIN);
    if(ret == INVALID_SET_FILE_POINTER) {
        ::CloseHandle(fileHandle);
        throw SystemError( PT_ERROR_MSG("Could not set file pointer") );
    }

    if( FALSE == ::SetEndOfFile(fileHandle) ) {
        ::CloseHandle(fileHandle);
        throw SystemError( PT_ERROR_MSG("Could not truncate file") );
    }

    if( FALSE == ::CloseHandle(fileHandle) )
        throw SystemError( PT_ERROR_MSG("Could not close file handle") );
}


void FileImpl::remove(const std::string& path)
{
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);

    if(FALSE == ::DeleteFile( tpath.c_str() ))
        throw SystemError( PT_ERROR_MSG("Could not unlink file") );
}


void FileImpl::move(const std::string& path, const std::string& to)
{
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
    std::basic_string<TCHAR> tto = win32::fromMultiByte(to);

#ifdef _WIN32_WCE
    if( FALSE == ::MoveFile(tpath.c_str(), tto.c_str()) )
        throw SystemError( PT_ERROR_MSG("Could not move file") );
#else
    if( FALSE == ::MoveFileEx(tpath.c_str(), tto.c_str(), MOVEFILE_COPY_ALLOWED) )
        throw SystemError( PT_ERROR_MSG("Could not move file") );
#endif
}


void FileImpl::create(const std::string& path)
{
    HANDLE hFile;
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);


    hFile = CreateFile(tpath.c_str(),   // file to create
            GENERIC_WRITE,          // open for writing
            0,                      // do not share
            NULL,                   // default security
            CREATE_ALWAYS,          // overwrite existing
            FILE_ATTRIBUTE_NORMAL | // normal file
            NULL,                   // asynchronous I/O
            NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        throw SystemError( PT_ERROR_MSG("Could not create file") );
    }

    CloseHandle(hFile);
}

} // namespace System

} // namespace Pt
