/***************************************************************************
 *   Copyright (C) 2005-2008 by Marc Boris Duerner                         *
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
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace Pt {

namespace System {

FileImpl::FileImpl()
{
}


FileImpl::~FileImpl()
{
}


std::size_t FileImpl::size(const char* path)
{
    struct stat buff;

    if( 0 != stat(path, &buff) )
        throw SystemError("Could not stat file", PT_SOURCEINFO);

    return buff.st_size;
}


void FileImpl::resize(const char* path, std::size_t newSize)
{
    int ret = 0;
    do {
        ret = truncate(path, newSize);
    } while ( ret == EINTR );

    if(ret != 0)
        throw SystemError("Could not truncate file", PT_SOURCEINFO);
}


void FileImpl::remove(const char* path)
{
    if(0 != ::remove(path))
        throw SystemError("Could not remove file", PT_SOURCEINFO);
}


void FileImpl::copy(const char* path, const char* to)
{
    int sd = open(path, O_RDONLY);
    if (sd == -1) 
        throw SystemError("Could not copy file" + std::string(path) + " to " + std::string(to),
                          PT_SOURCEINFO);

    struct stat st;
    if (fstat(sd, &st) != 0)
    {
        close(sd);
        throw SystemError("Could not copy file" + std::string(path) + " to " + std::string(to),
                          PT_SOURCEINFO);
    }
    const long blockSize = st.st_blksize;

    int dd = open(to, O_CREAT | O_TRUNC | O_WRONLY, st.st_mode & S_IRWXU);
    if (dd == -1)
    {
        close(sd);
        throw SystemError("Could not copy file" + std::string(path) + " to " + std::string(to),
                          PT_SOURCEINFO);
    }

    char buffer[blockSize];
    try
    {
        int n;
        while ((n = read(sd, buffer, blockSize)) > 0)
        {
            if (write(dd, buffer, n) != n)
                throw SystemError("Could not copy file" + std::string(path) + " to " + std::string(to),
                                  PT_SOURCEINFO);
        }
        if (n < 0)
            throw SystemError("Could not copy file" + std::string(path) + " to " + std::string(to),
                              PT_SOURCEINFO);
    }
    catch (...)
    {
        close(sd);
        close(dd);
        throw SystemError("Could not copy file" + std::string(path) + " to " + std::string(to),
                          PT_SOURCEINFO);;
    }

    close(sd);
    if (fsync(dd) != 0)
    {
        close(dd);
        throw SystemError("Could not copy file" + std::string(path) + " to " + std::string(to),
                          PT_SOURCEINFO);
    }

    close(dd);
}


void FileImpl::move(const char* path, const char* to)
{
    if (0 != ::rename(path, to))
        throw SystemError("Could not move file " + std::string(path) + " to " + std::string(to),
                          PT_SOURCEINFO);
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
