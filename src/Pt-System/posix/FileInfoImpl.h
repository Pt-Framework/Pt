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

#include "PathImpl.h"
#include <Pt/System/FileInfo.h>
#include <Pt/System/Path.h>
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
        static void createFile(const Path& path)
        {
            int fd = open(path.impl()->c_str(), O_RDWR|O_EXCL|O_CREAT, 0777);
            if( fd < 0 )
                throw AccessFailed(path.impl()->c_str());

            ::close(fd);
        }

        static void createSymlink(const Path& target, const Path& link)
        {
            if( 0 != ::symlink(target.impl()->c_str(), link.impl()->c_str()) )
                throw AccessFailed(link.impl()->c_str());
        }

        static void createHardlink(const Path& target, const Path& link)
        {
            if( 0 != ::link(target.impl()->c_str(), link.impl()->c_str()) )
                throw AccessFailed(link.impl()->c_str());
        }

        static void createDirectory(const Path& path)
        {
            if( -1 == ::mkdir(path.impl()->c_str(), 0777) )
            {
                throw AccessFailed(path.impl()->c_str());
            }
        }

        static FileInfo::Type getType(const Path& path)
        {
            struct stat st;
            if( 0 != ::stat(path.impl()->c_str(), &st) )
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

        static FileInfo::Type linkStatus(const Path& path)
        {
            struct stat st;
            if( 0 != ::lstat(path.impl()->c_str(), &st) )
            {
                return FileInfo::Invalid;
            }

            if( S_ISLNK(st.st_mode) )
            {
                return FileInfo::Link;
            }
            else if( S_ISREG(st.st_mode) )
            {
                return FileInfo::File;
            }
            else if( S_ISDIR(st.st_mode) )
            {
                return FileInfo::Directory;
            }

            return FileInfo::File;
        }

        static std::size_t size(const Path& path)
        {
            struct stat s;

            if( 0 != stat(path.impl()->c_str(), &s) )
            {
                throw AccessFailed(path.impl()->c_str());
            }

            return s.st_size;
        }

        static DateTime lastModified(const Path& path)
        {
            struct stat s;

            if( 0 != stat(path.impl()->c_str(), &s) )
            {
                throw AccessFailed(path.impl()->c_str());
            }

            struct tm tim;
            localtime_r(&s.st_mtime, &tim);

            return DateTime( tim.tm_year + 1900,
                             tim.tm_mon + 1,
                             tim.tm_mday,
                             tim.tm_hour,
                             tim.tm_min,
                             tim.tm_sec );
        }

        static void resize(const Path& path, std::size_t newSize)
        {
            int ret = 0;
            do
            {
                ret = truncate(path.impl()->c_str(), newSize);
            }
            while ( ret == EINTR );

            if(ret != 0)
                throw AccessFailed(path.impl()->c_str());
        }

        static void remove(const Path& path)
        {
            if( 0 != ::remove(path.impl()->c_str()) )
                throw AccessFailed(path.impl()->c_str());
        }

        static void move(const Path& path, const Path& to)
        {
            if (0 != ::rename(path.impl()->c_str(), to.impl()->c_str()))
            {
                throw AccessFailed(path.impl()->c_str());
            }
        }

        static FileInfo::Perms permissions(const Path& path)
        {
            struct stat st;
            if( 0 != ::stat(path.impl()->c_str(), &st) )
            {
                throw AccessFailed(path.impl()->c_str());
            }

            return static_cast<FileInfo::Perms>(st.st_mode & 07777);
        }

        static void permissions(const Path& path, FileInfo::Perms prms,
                                FileInfo::PermOptions opts)
        {
            FileInfo::Perms effective = static_cast<FileInfo::Perms>(
                static_cast<int>(prms) & static_cast<int>(FileInfo::PermMask));

            if( static_cast<int>(opts) & static_cast<int>(FileInfo::PermAdd) )
            {
                FileInfo::Perms current = permissions(path);
                effective = static_cast<FileInfo::Perms>(
                    static_cast<int>(current) | static_cast<int>(effective));
            }
            else if( static_cast<int>(opts) & static_cast<int>(FileInfo::PermRemove) )
            {
                FileInfo::Perms current = permissions(path);
                effective = static_cast<FileInfo::Perms>(
                    static_cast<int>(current) & ~static_cast<int>(effective));
            }

            int ret;

#ifdef AT_SYMLINK_NOFOLLOW
            if( static_cast<int>(opts) & static_cast<int>(FileInfo::PermNoFollow) )
            {
                ret = ::fchmodat(AT_FDCWD, path.impl()->c_str(),
                                 static_cast<mode_t>(effective), AT_SYMLINK_NOFOLLOW);
            }
            else
#endif
            {
                ret = ::chmod(path.impl()->c_str(), static_cast<mode_t>(effective));
            }

            if( ret != 0 )
                throw AccessFailed(path.impl()->c_str());
        }
};

} // namespace System

} // namespace Pt
