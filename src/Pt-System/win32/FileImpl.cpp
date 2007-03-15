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
#include "win32.h"
#include <string>
#include <iostream>
#include <windows.h>


namespace Pt {

namespace System {


FileImpl::FileImpl(const std::string& path)
: _path(path)
{
}


FileImpl::~FileImpl()
{
}



std::size_t FileImpl::size() const
{
    WIN32_FIND_DATA data;
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(_path);

    HANDLE h = FindFirstFile(tpath.c_str(), &data);
    if(h == INVALID_HANDLE_VALUE)
        throw SystemError("Could not get file size.", PT_SOURCEINFO);

    FindClose(h);

    LARGE_INTEGER li;
    li.HighPart = data.nFileSizeHigh;
    li.LowPart = data.nFileSizeLow;
    
    return static_cast<std::size_t>(li.QuadPart);
}


void FileImpl::resize(std::size_t newSize)
{
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(_path);

    HANDLE fileHandle = ::CreateFile(tpath.c_str(),
                                     GENERIC_READ|GENERIC_WRITE,
                                     FILE_SHARE_READ|FILE_SHARE_WRITE,
                                     NULL,
                                     OPEN_EXISTING,
                                     0,
                                     NULL);

    if(fileHandle == INVALID_HANDLE_VALUE)
    {
        throw SystemError("Could not open file.", PT_SOURCEINFO);
    }

    // under Win32 resizing is done by moving to the desired position
    // and then calling SetEndOfFile on the handle.
    DWORD ret = ::SetFilePointer(fileHandle, newSize, NULL, FILE_BEGIN);
    if(ret == INVALID_SET_FILE_POINTER) {
        ::CloseHandle(fileHandle);
        throw SystemError("Could not set file pointer", PT_SOURCEINFO);
    }

    if( FALSE == ::SetEndOfFile(fileHandle) ) {
        ::CloseHandle(fileHandle);
        throw SystemError("Could not truncate file", PT_SOURCEINFO);
    }

    if( FALSE == ::CloseHandle(fileHandle) )
        throw SystemError("Could not close file handle", PT_SOURCEINFO);
}


void FileImpl::remove()
{
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(_path);

    if(FALSE == ::DeleteFile( tpath.c_str() ))
        throw SystemError("Could not unlink file", PT_SOURCEINFO);
}


void FileImpl::copy(const std::string& to) const
{
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(_path);
    std::basic_string<TCHAR> tto = win32::fromMultiByte(to);

    if(FALSE == ::CopyFile( tpath.c_str(), tto.c_str(), FALSE ))
        throw SystemError("Could not copy file", PT_SOURCEINFO);
}


void FileImpl::move(const std::string& to)
{
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(_path);
    std::basic_string<TCHAR> tto = win32::fromMultiByte(to);

    #ifdef _WIN32_WCE
        if( FALSE == ::MoveFile(tpath.c_str(), tto.c_str()) )
            throw SystemError("Could not move file", PT_SOURCEINFO);
    #else
        if( FALSE == ::MoveFileEx(tpath.c_str(), tto.c_str(), MOVEFILE_COPY_ALLOWED) )
            throw SystemError("Could not move file", PT_SOURCEINFO);
    #endif

    _path = to;
}


bool FileImpl::exists() const
{
    DWORD file_attr;
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(_path);

    file_attr = ::GetFileAttributes( tpath.c_str() );

    return file_attr != 0xffffffff;
}


void FileImpl::create()
{
    HANDLE hFile;
    std::basic_string<TCHAR> tpath = win32::fromMultiByte(_path);

// WinCE does not support overlapped I/O
#ifdef _WIN32_WCE
    hFile = CreateFile(tpath.c_str(),   // file to create
            GENERIC_WRITE,          // open for writing
            0,                      // do not share
            NULL,                   // default security
            CREATE_ALWAYS,          // overwrite existing
            FILE_ATTRIBUTE_NORMAL | // normal file
            NULL,                   // asynchronous I/O
            NULL);
#else
    hFile = CreateFile(tpath.c_str(),   // file to create
            GENERIC_WRITE,          // open for writing
            0,                      // do not share
            NULL,                   // default security
            CREATE_ALWAYS,          // overwrite existing
            FILE_ATTRIBUTE_NORMAL | // normal file
            FILE_FLAG_OVERLAPPED,   // asynchronous I/O
            NULL);                  // no attr. template
#endif

    if (hFile == INVALID_HANDLE_VALUE)
    {
        throw SystemError( "Could not create file" , PT_SOURCEINFO);

    }

    CloseHandle(hFile);
}


} // namespace System

} // namespace Pt
