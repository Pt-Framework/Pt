/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                                *
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
#include "Pt/System/IODevice.h"
#include "FileDeviceImpl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace Pt{
namespace System{

FileDeviceImpl::FileDeviceImpl()
: _fd(-1)
{ }

FileDeviceImpl::~FileDeviceImpl()
{ }

void FileDeviceImpl::open( const char* path, std::ios_base::openmode mode, bool isAsync )
{
    int flags = O_RDONLY;

    if( (mode & std::ios_base::in ) && (mode & std::ios_base::out) ) {
        flags |= O_RDWR;
        flags |= O_CREAT;
    }
    else if(mode & std::ios_base::out) {
        flags |= O_WRONLY;
        flags |= O_CREAT;
    }
    else if(mode & std::ios_base::in) {
        flags |= O_RDONLY;
    }

    if(isAsync)
        flags |= O_NONBLOCK;

    if(mode & std::ios::trunc)
        flags |= O_TRUNC;

    _fd = ::open(path, flags, 0644);

    if(_fd == -1) {
        throw OpenFailed("open failed", PT_SOURCEINFO);
    }

    try {
        if(mode & std::ios::ate)
            this->seek(0, std::ios::end);
    }
    catch(...) {
        this->close();
        throw;
    }
}



bool FileDeviceImpl::seekable() const
{
    struct stat s;

    int ret = fstat(_fd, &s);
    if(ret == 0)
    {
        if(S_ISREG(s.st_mode) || S_ISBLK(s.st_mode))
            return true;
    }

    return false;
}



FileDeviceImpl::pos_type FileDeviceImpl::seek(off_type offset, std::ios::seekdir sd )
{
    int whence = std::ios::cur;
    switch(sd)
    {
        case std::ios::beg:
            whence = SEEK_SET;
            break;

        case std::ios::cur:
            whence = SEEK_CUR;
            break;

        case std::ios::end:
            whence = SEEK_END;
            break;

        default:
            break;
    }

    off_t ret = lseek(_fd, offset, whence);
    if( ret == (off_t)-1 )
        throw IOError("Could not seek on file handle", PT_SOURCEINFO);

    return ret;
}

void FileDeviceImpl::resize(off_type size)
{
    int ret = ::ftruncate(_fd, size);
    if(ret != 0)
        throw IOError("Could not truncate file", PT_SOURCEINFO);

}

size_t FileDeviceImpl::size()
{
    struct stat buff;
    int ret = fstat(_fd, &buff);
    if(ret != 0)
        throw IOError("Could not stat file", PT_SOURCEINFO);

    return buff.st_size;
}


size_t FileDeviceImpl::peek(char* buffer, size_t count)
{
    bool eof;
    size_t ret = this->read(buffer, count, eof);

    // if we could read data seek back
    if(ret > 0)
        this->seek(-((off_type)ret), std::ios::cur);

    return ret;
}


/*
const IOEvent& FileDeviceImpl::event( FdsType fdsType )
{
    switch( fdsType )
    {
        case ReadFds:
            return _readEvent;
        break;
        case WriteFds:
            return _writeEvent;
        break;
    }
}
*/

} //namespace System 
} //namespace Pt
