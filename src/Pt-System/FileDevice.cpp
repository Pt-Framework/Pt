
/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Dürner                                  *
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

#include "Pt/System/FileDevice.h"

#include "FileDeviceImpl.h"


namespace Pt {

namespace System {


FileDevice::FileDevice()
{
    _impl = new FileDeviceImpl();
}


FileDevice::FileDevice(const char* path, std::ios_base::openmode mode) throw(IO::IOError)
: _mode(mode)
{
    _impl = new FileDeviceImpl();
    this->open(path, mode);
}


FileDevice::~FileDevice() throw()
{
    if( this->valid() ) {
        try { this->close(); } catch(...) { }
    }

    delete _impl;
}


void FileDevice::open(const char* path, std::ios_base::openmode mode) throw(IO::IOError)
{
    if( this->valid() ) {
        this->close();
    }

    _impl->open(path, mode);

    _mode = mode;
    _path = path;

    IO::IODevice::setValid(true);
    IO::IODevice::setEof(false);
}


void FileDevice::_close() throw(IO::IOError)
{
    _impl->close();
    IO::IODevice::setValid(false);
    IO::IODevice::setEof(false);
}


size_t FileDevice::size() const throw(IO::IOError)
{
    return _impl->size();
}


FileDevice::pos_type FileDevice::_seek(off_type offset, SeekMode mode) throw(IO::IOError)
{
    return _impl->seek(offset, mode);
}


size_t FileDevice::_read( char* buffer, size_t count, bool& eof ) throw(IO::IOError)
{
    //if(count > SSIZE_MAX)
    //    count = SSIZE_MAX;

    size_t ret = _impl->read( buffer, count, eof );
    return ret;
}


size_t FileDevice::_write(const char* buffer, size_t count) throw(IO::IOError)
{
    if( _mode & std::ios_base::out ) 
        return _impl->write(buffer, count);

    return 0;
}


size_t FileDevice::_peek(char* buffer, size_t count) throw(IO::IOError)
{
    return _impl->peek(buffer, count);
}


void FileDevice::_sync() const throw(IO::IOError)
{
    if( _mode & std::ios_base::out ) 
        _impl->sync();
}


bool FileDevice::_wait(WaitMode mode, unsigned int msec) throw(IO::IOError)
{
    return _impl->wait(mode, msec);
}


} // namespace System

} // namespace Pt
