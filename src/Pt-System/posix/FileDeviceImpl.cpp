/*
 * Copyright (C) 2005 by Marc Boris Drner
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

#include "FileDeviceImpl.h"
#include "PathImpl.h"
#include <Pt/System/Path.h>
#include <Pt/System/IODevice.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits>

namespace Pt {

namespace System {

FileDeviceImpl::FileDeviceImpl(FileDevice& device)
: IODeviceImpl(device)
, _pos(0)
, _readPos(0)
, _writePos(0)
{ }


FileDeviceImpl::~FileDeviceImpl()
{ }


std::uint64_t FileDeviceImpl::advance(std::size_t count)
{
    if(count > std::numeric_limits<std::uint64_t>::max() - _pos)
        throw IOError("I/O position overflow");

    std::uint64_t position = _pos;
    _pos += count;
    return position;
}


void FileDeviceImpl::setPosition(std::uint64_t position)
{
    if(position > static_cast<std::uint64_t>(std::numeric_limits<off_t>::max()))
        throw IOError("I/O position out of range");

    off_t target = static_cast<off_t>(position);
    if( ::lseek(fd(), target, SEEK_SET) != target )
        throw IOError( PT_ERROR_MSG("lseek failed") );
}


void FileDeviceImpl::open(const Path& path, std::ios::openmode mode)
{
    int flags = O_RDONLY;

    if( (mode & std::ios::in ) && (mode & std::ios::out) ) {
        flags |= O_RDWR;
        flags |= O_CREAT;
    }
    else if(mode & std::ios::out) {
        flags |= O_WRONLY;
        flags |= O_CREAT;
    }
    else if(mode & std::ios::in) {
        flags |= O_RDONLY;
    }

    flags |= O_NONBLOCK;

    if(mode & std::ios::trunc)
        flags |= O_TRUNC;

    int fd = ::open(path.impl()->c_str(), flags, 0644);
    if(fd == -1)
    {
        throw AccessFailed(path.toLocal());
    }

    IODeviceImpl::open(fd, false);

    _pos = 0;
    _readPos = 0;
    _writePos = 0;

    try {
        if(mode & std::ios::ate)
            this->seek(0, std::ios::end);
    }
    catch(...) {
        this->close();
        throw;
    }
}


bool FileDeviceImpl::beginOpen(EventLoop& loop, const Path& path, std::ios::openmode mode)
{
    this->open(path, mode);
    return true;
}


void FileDeviceImpl::endOpen(EventLoop& loop)
{
}


bool FileDeviceImpl::runOpen(EventLoop& loop)
{
    return false;
}


bool FileDeviceImpl::seekable() const
{
    struct stat s;

    int ret = fstat(fd(), &s);
    if(ret == 0)
    {
        if(S_ISREG(s.st_mode) || S_ISBLK(s.st_mode))
            return true;
    }

    return false;
}


FileDeviceImpl::pos_type FileDeviceImpl::seek(off_type offset, std::ios::seekdir sd )
{
    if(sd == std::ios::cur && offset == 0)
        return pos_type( static_cast<off_type>(_pos) );

    int whence = SEEK_SET;
    off_t target = static_cast<off_t>(offset);
    switch(sd)
    {
        case std::ios::beg:
            whence = SEEK_SET;
            break;

        case std::ios::cur:
            if(_pos > static_cast<std::uint64_t>(std::numeric_limits<off_t>::max()))
                throw IOError("I/O position out of range");

            target += static_cast<off_t>(_pos);
            break;

        case std::ios::end:
            whence = SEEK_END;
            break;

        default:
            throw std::invalid_argument("Unknown seekdir");
    }

    off_t ret = lseek(fd(), target, whence);
    if( ret == (off_t)-1 )
        throw IOError( PT_ERROR_MSG("lseek failed") );

    _pos = static_cast<std::uint64_t>(ret);
    _readPos = _pos;
    _writePos = _pos;
    return pos_type( static_cast<off_type>(_pos) );
}


void FileDeviceImpl::resize(off_type size)
{
    int ret = ::ftruncate(fd(), size);
    if(ret != 0)
        throw IOError( PT_ERROR_MSG("ftruncate failed") );

}


std::size_t FileDeviceImpl::peek(char* buffer, std::size_t count)
{
    if(_pos > static_cast<std::uint64_t>(std::numeric_limits<off_t>::max()))
        throw IOError("I/O position out of range");

    off_t position = static_cast<off_t>(_pos);
    ssize_t ret;
    do
    {
        ret = ::pread(fd(), static_cast<void*>(buffer), count, position);
    }
    while(ret == -1 && errno == EINTR);

    if(ret == -1)
        throw IOError( PT_ERROR_MSG("pread failed") );

    return static_cast<std::size_t>(ret);
}


std::size_t FileDeviceImpl::onBeginRead(EventLoop& loop, char* buffer,
                                        std::size_t count, bool& eof)
{
    _readPos = advance(count);
    setPosition(_readPos);
    return IODeviceImpl::onBeginRead(loop, buffer, count, eof);
}


std::size_t FileDeviceImpl::onEndRead(EventLoop& loop, char* buffer,
                                      std::size_t count, bool& eof)
{
    setPosition(_readPos);
    return IODeviceImpl::onEndRead(loop, buffer, count, eof);
}


std::size_t FileDeviceImpl::onRead(char* buffer, std::size_t count, bool& eof)
{
    _readPos = advance(count);
    setPosition(_readPos);
    return IODeviceImpl::onRead(buffer, count, eof);
}


std::size_t FileDeviceImpl::onBeginWrite(EventLoop& loop, const char* buffer,
                                         std::size_t count)
{
    _writePos = advance(count);
    setPosition(_writePos);
    return IODeviceImpl::onBeginWrite(loop, buffer, count);
}


std::size_t FileDeviceImpl::onEndWrite(EventLoop& loop, const char* buffer,
                                       std::size_t count)
{
    setPosition(_writePos);
    return IODeviceImpl::onEndWrite(loop, buffer, count);
}


std::size_t FileDeviceImpl::onWrite(const char* buffer, std::size_t count)
{
    _writePos = advance(count);
    setPosition(_writePos);
    return IODeviceImpl::onWrite(buffer, count);
}

} //namespace System

} //namespace Pt
