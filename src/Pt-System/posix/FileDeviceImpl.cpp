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

void FileDeviceImpl::open( const char* path, std::ios_base::openmode mode, IODevice::ReadWriteMode rwMode )
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

    if( rwMode == IODevice::Asynchronous)
        flags |= O_NONBLOCK;
    
    if(mode & std::ios::trunc)
        flags |= O_TRUNC;

    _fd = ::open(path, flags, 0644);
    
    _openMode = mode;

    if(_fd == -1) {
        throw IOError("Could not open file handle", PT_SOURCEINFO);
    }

    try {
        if(mode & std::ios::ate)
            this->seek(0, IODevice::SeekEnd);
    }
    catch(...) {
        this->close();
        throw;
    }
}

void FileDeviceImpl::close()
{
    if(_fd != -1)
    {
        if( ::close(_fd) != 0 )
            throw IOError("Could not close file handle", PT_SOURCEINFO);

        _fd = -1;
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

FileDeviceImpl::pos_type FileDeviceImpl::seek(off_type offset, IODevice::SeekMode mode )
{
    int whence = IODevice::SeekCurrent;
    switch(mode)
    {
        case IODevice::SeekBegin:
            whence = SEEK_SET;
            break;

        case IODevice::SeekCurrent:
            whence = SEEK_CUR;
            break;

        case IODevice::SeekEnd:
            whence = SEEK_END;
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

size_t FileDeviceImpl::read(char* buffer, size_t count, bool& eof)
{
    eof = false;

    retry:

    ssize_t ret = ::read(_fd, (void*)buffer, count);
    if(ret == -1) 
    {
        if(errno == EINTR) // signal interrupt
            goto retry;

        if(errno == EAGAIN) // non-blocking and no data yet
            return 0;

        throw IOError("Could not read from file handle", PT_SOURCEINFO);
    }

    if(ret == 0)
        eof = true;

    return ret;
}

size_t FileDeviceImpl::write(const char* buffer, size_t count)
{
    retry:

    ssize_t ret = ::write(_fd, (const void*)buffer, count);
    if(ret == -1) {
        if(errno == EINTR) // signal interrupt
            goto retry;

        if(errno == EAGAIN) // non-blocking and no data yet
            return 0;

        throw IOError("Could not write to file handle", PT_SOURCEINFO);
    }

    return ret;
}

size_t FileDeviceImpl::peek(char* buffer, size_t count)
{
    bool eof;
    size_t ret = this->read(buffer, count, eof);

    // if we could read data seek back
    if(ret > 0)
        this->seek(-((off_type)ret), IODevice::SeekCurrent);

    return ret;
}

void FileDeviceImpl::sync() const
{
    int ret = fsync(_fd);
    if(ret != 0)
        throw IOError("Could not sync handle", PT_SOURCEINFO);
}

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


bool FileDeviceImpl::wait(IODevice::WaitMode mode, unsigned int msec)
{
    fd_set rfds;
    fd_set wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_SET(_fd, &rfds);
    FD_SET(_fd, &wfds);

    struct timeval* timeout = NULL;
    struct timeval tv;
    if(msec != IODevice::WaitTimeInfinite)
    {
        tv.tv_sec = msec / 1000;
        tv.tv_usec = (msec % 1000) * 1000;
        timeout = &tv;
    }

    retry:
    int ret = -1;

    if(mode & IODevice::WaitInput)
        ret = ::select(_fd + 1, &rfds, 0, 0, &tv);
    else if(mode & IODevice::WaitOutput)
        ret = ::select(_fd + 1, 0, &wfds, 0, &tv);
    else
        ret = ::select(_fd + 1, &rfds, &wfds, 0, &tv);    

    if(ret == -1)
    {
        if(errno == EINTR)
            goto retry;

        throw IOError("Could not select on socket", PT_SOURCEINFO);
    }

    if(ret == 1)
        return true;

    return false;
}

} //namespace System 
} //namespace Pt
