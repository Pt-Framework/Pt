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
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

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
    struct stat buff;

    if( 0 != stat(_path.c_str(), &buff) )
        throw SystemError("Could not stat file", PT_SOURCEINFO);

    return buff.st_size;
}


void FileImpl::resize(std::size_t newSize)
{
    int ret = 0;
    do {
        ret = truncate(_path.c_str(), newSize);
    } while ( ret == EINTR );

    if(ret != 0)
        throw SystemError("Could not truncate file", PT_SOURCEINFO);
}


void FileImpl::remove()
{
    if(0 != ::remove(_path.c_str()) == -1)
        throw SystemError("Could not remove file", PT_SOURCEINFO);
}


void FileImpl::copy(const std::string& to) const
{
    int sd = open(_path.c_str(), O_RDONLY);
    if (sd == -1) 
        throw SystemError("Could not copy file" + _path + " to " + to, PT_SOURCEINFO);

    struct stat st;
    if (fstat(sd, &st) != 0)
    {
        close(sd);
        throw SystemError("Could not copy file" + _path + " to " + to, PT_SOURCEINFO);
    }
    const long blockSize = st.st_blksize;

    int dd = open(to.c_str(), O_CREAT | O_TRUNC | O_WRONLY, st.st_mode & S_IRWXU);
    if (dd == -1)
    {
        close(sd);
        throw SystemError("Could not copy file" + _path + " to " + to, PT_SOURCEINFO);
    }

    char buffer[blockSize];
    try
    {
        int n;
        while ((n = read(sd, buffer, blockSize)) > 0)
        {
            if (write(dd, buffer, n) != n)
                throw SystemError("Could not copy file" + _path + " to " + to, PT_SOURCEINFO);
        }
        if (n < 0)
            throw SystemError("Could not copy file" + _path + " to " + to, PT_SOURCEINFO);
    }
    catch (...)
    {
        close(sd);
        close(dd);
        throw SystemError("Could not copy file" + _path + " to " + to, PT_SOURCEINFO);;
    }

    close(sd);
    if (fsync(dd) != 0)
    {
        close(dd);
        throw SystemError("Could not copy file" + _path + " to " + to, PT_SOURCEINFO);
    }

    close(dd);
}


void FileImpl::move(const std::string& to)
{
    if (0 != ::rename(_path.c_str(), to.c_str()))
        throw SystemError("Could not move file " + _path + " to " + to , PT_SOURCEINFO);
    _path = to;
}


void FileImpl::create()
{
    FILE* f = fopen(_path.c_str(), "w");
    if (!f)
        throw SystemError("Could not create file " + _path, PT_SOURCEINFO);

    fclose(f);
}


bool FileImpl::exists() const
{
    struct stat buff;

    int err = stat(_path.c_str(), &buff);
    if (err == -1 )
    {
        if (errno == ENOENT || errno == ENOTDIR)
            return false;

        throw SystemError("Could not stat file " + _path, PT_SOURCEINFO);
    }

    return true;
}


void FileImpl::create(const char* path)
{
    FILE* f = fopen(path, "w");
    if (!f)
        throw SystemError("Could not create file " + std::string(path), PT_SOURCEINFO);

    fclose(f);
}

}

}
