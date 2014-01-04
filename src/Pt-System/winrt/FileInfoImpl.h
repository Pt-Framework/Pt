/*
 * Copyright (C) 2013 Marc Boris Duerner
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

#include "../win32/win32.h"
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
            std::wstring wpath;
            win32::fromMultiByte(path, wpath);

            HANDLE h = CreateFile2(wpath.c_str(), // file to create
                                   GENERIC_WRITE, // open for writing
                                   0, // do not share
                                   CREATE_NEW,
                                   NULL);

            if (h == INVALID_HANDLE_VALUE)
                throw AccessFailed(path);

            if( FALSE == ::CloseHandle(h) )
                throw IOError("CloseHandle");
        }

        static void createDirectory(const std::string& path)
        {
            std::wstring wpath;
            win32::fromMultiByte( path, wpath );

            if( FALSE == ::CreateDirectoryW(wpath.c_str(), NULL) )
                throw AccessFailed(path);
        }

        static FileInfo::Type getType(const std::string& path)
        {
            std::wstring wpath;
            win32::fromMultiByte(path, wpath);

            WIN32_FILE_ATTRIBUTE_DATA info;
            BOOL ret = GetFileAttributesExW( wpath.c_str(), 
                                             GetFileExInfoStandard, 
                                             &info );
            if(ret == 0)
            {
                return FileInfo::Invalid;
            }

            DWORD attr = info.dwFileAttributes;

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
            std::wstring wpath;
            win32::fromMultiByte(path, wpath);

            WIN32_FILE_ATTRIBUTE_DATA info;
            BOOL ret = GetFileAttributesExW( wpath.c_str(), 
                                             GetFileExInfoStandard, 
                                             &info );
            if(ret == 0)
            {
                throw AccessFailed(path);
            }

            LARGE_INTEGER li;
            li.HighPart = info.nFileSizeHigh;
            li.LowPart = info.nFileSizeLow;
            return static_cast<Pt::uint64_t>(li.QuadPart);
        }

        static void resize(const std::string& path, Pt::uint64_t newSize)
        {
            std::wstring wpath;
            win32::fromMultiByte(path, wpath);

            HANDLE h = ::CreateFile2(wpath.c_str(),
                                     GENERIC_READ|GENERIC_WRITE,
                                     FILE_SHARE_READ|FILE_SHARE_WRITE,
                                     OPEN_EXISTING,
                                     NULL);

            if(h == INVALID_HANDLE_VALUE)
                throw AccessFailed(path);

            LARGE_INTEGER liSize;
            liSize.QuadPart = newSize;

            if( INVALID_SET_FILE_POINTER == ::SetFilePointerEx(h, liSize, NULL, FILE_BEGIN) ||
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
            std::wstring wpath;
            win32::fromMultiByte(path, wpath);

            WIN32_FILE_ATTRIBUTE_DATA info;
            BOOL ret = GetFileAttributesExW( wpath.c_str(), 
                                             GetFileExInfoStandard, 
                                             &info );
            if(ret == 0)
                return;

            DWORD attr = info.dwFileAttributes;

            if(attr & FILE_ATTRIBUTE_DIRECTORY)
            {
                if( FALSE == ::RemoveDirectoryW( wpath.c_str() ) )
                    throw AccessFailed(path);
            }
            else
            {
                if( FALSE == ::DeleteFileW( wpath.c_str() ) )
                    throw AccessFailed(path);
            }
        }

        static void move(const std::string& from, const std::string& to)
        {
            std::wstring wfrom;
            win32::fromMultiByte( from, wfrom );
    
            std::wstring wto;
            win32::fromMultiByte( to, wto );

            if( FALSE == ::MoveFileExW( wfrom.c_str(), wto.c_str(), 0) )
                throw AccessFailed(from);
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


