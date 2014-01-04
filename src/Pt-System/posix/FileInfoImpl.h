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

#include <Pt/System/FileInfo.h>
#include <Pt/System/IOError.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

namespace Pt {

namespace System {

class FileInfoImpl
{
    public:
        static void createFile(const std::string& path)
        {
            int fd = open(path.c_str(), O_RDWR|O_EXCL|O_CREAT, 0777);
            if( fd < 0 )
                throw AccessFailed(path);

            ::close(fd);
        }

        static void createDirectory(const std::string& path)
        {
            if( -1 == ::mkdir(path.c_str(), 0777) )
            {
                throw AccessFailed(path);
            }
        }

        static FileInfo::Type getType(const std::string& path)
        {
            struct stat st;
            if( 0 != ::stat(path.c_str(), &st) )
            {
                return FileInfo::Invalid;
            }

            if( S_ISREG(st.st_mode) )
            {
                return FileInfo::File;
            }
            else if( S_ISDIR(st.st_mode) ) 
            {
                return FileInfo::Directory;
            }

            return FileInfo::File;
        }

        static FileInfo::Type getType(const struct stat& st)
        {
            if( S_ISREG(st.st_mode) )
            {
                return FileInfo::File;
            }
            else if( S_ISDIR(st.st_mode) ) 
            {
                return FileInfo::Directory;
            }

            return FileInfo::File;
        }

        static std::size_t size(const std::string& path)
        {
            struct stat buff;

            if( 0 != stat(path.c_str(), &buff) )
            {
                throw AccessFailed(path);
            }

            return buff.st_size;
        }

        static void resize(const std::string& path, std::size_t newSize)
        {
            int ret = 0;
            do
            {
                ret = truncate(path.c_str(), newSize);
            }
            while ( ret == EINTR );

            if(ret != 0)
                throw AccessFailed(path);
        }

        static void remove(const std::string& path)
        {
            if( 0 != ::remove(path.c_str()) )
                throw AccessFailed(path);
        }

        static void move(const std::string& path, const std::string& to)
        {
            if (0 != ::rename(path.c_str(), to.c_str()))
            {
                throw AccessFailed(path);
            }
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
            return "/";
        }
};

} // namespace System

} // namespace Pt
