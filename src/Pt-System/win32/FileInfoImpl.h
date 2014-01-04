/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
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
#include <Pt/WinVer.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/IOError.h>
#include <Pt/System/SystemError.h>
#include <string>
#include <cstring>
#include <cstddef>
#include <windows.h>

namespace Pt {

namespace System {

class FileInfoImpl
{
    public:
        static void createFile(const std::string& path)
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

        static void createDirectory(const std::string& path)
        {
            std::basic_string<TCHAR> str;
            win32::fromMultiByte( path, str );

            if( FALSE == ::CreateDirectory(str.c_str(), NULL) )
                throw AccessFailed(path);
        }

        static FileInfo::Type getType(const std::string& path)
        {
            std::basic_string<TCHAR> tpath;
            win32::fromMultiByte(path, tpath);
            DWORD attr = GetFileAttributes( tpath.c_str() );

            if(attr == 0xffffffff)
            {
                if( 0 != strstr(path.c_str(), ".sys") )
                    return FileInfo::File;

                return FileInfo::Invalid;
            }

            if(attr & FILE_ATTRIBUTE_DIRECTORY)
                return FileInfo::Directory;

            return FileInfo::File;
        }

        static FileInfo::Type getType(DWORD attr)
        {
            if(attr == 0xffffffff)
                return FileInfo::Invalid;

            if(attr & FILE_ATTRIBUTE_DIRECTORY)
                return FileInfo::Directory;

            return FileInfo::File;
        }

        static Pt::uint64_t size(const std::string& path)
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
            return static_cast<Pt::uint64_t>(li.QuadPart);
        }

        static void resize(const std::string& path, Pt::uint64_t newSize)
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

            LARGE_INTEGER li;
            li.QuadPart = newSize;

            if( INVALID_SET_FILE_POINTER == ::SetFilePointer(h, li.LowPart, &li.HighPart, FILE_BEGIN) ||
                FALSE == ::SetEndOfFile(h) )
            {
                ::CloseHandle(h);
                throw IOError("SetFilePointer");
            }

            if( FALSE == ::CloseHandle(h) )
                throw IOError("CloseHandle");
        }

        static void remove(const std::string& path)
        {
            std::basic_string<TCHAR> tpath;
            win32::fromMultiByte(path, tpath);

            DWORD attr = GetFileAttributes( tpath.c_str() );
            
            if(attr & FILE_ATTRIBUTE_DIRECTORY)
            {
                if( FALSE == ::RemoveDirectory( tpath.c_str() ) )
                    throw AccessFailed(path);
            }
            else
            {
                if( FALSE == ::DeleteFile( tpath.c_str() ) )
                    throw AccessFailed(path);
            }
        }

        static void move(const std::string& path, const std::string& to)
        {
            std::basic_string<TCHAR> tpath;
            win32::fromMultiByte(path, tpath);

            std::basic_string<TCHAR> tto;
            win32::fromMultiByte(to, tto);

#ifdef _WIN32_WCE
            if( FALSE == ::MoveFile(tpath.c_str(), tto.c_str()) )
            {
                //DWORD error = GetLastError();
                //if(error == ERROR_NOT_SAME_DEVICE)
                //{
                //    if( ! allowCopy )
                //        throw AccessFailed(path);

                //    if( FALSE == CopyFile( tpath.c_str(), tto.c_str(), TRUE ) )
                //        throw AccessFailed(path);

                //    FileImpl::remove(path);
                //    return;
                //}

                throw AccessFailed(path);
            }
#else

            if( FALSE == ::MoveFileEx(tpath.c_str(), tto.c_str(), 0) )
                throw AccessFailed(path);

#endif
        }

        static const char* curdir()
        {
            return ".";
        }

        static const char* updir()
        {
            return "..";
        }

        static const char* sep()
        {
            return "\\";
        }
};

} // namespace System

} // namespace Pt
