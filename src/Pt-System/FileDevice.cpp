/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
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
#include "Pt/System/FileDevice.h"
#include "FileDeviceImpl.h"

namespace Pt {

namespace System {

FileDevice::FileDevice()
{
    _impl = new FileDeviceImpl(*this);
}


FileDevice::FileDevice( const char* path, std::ios_base::openmode mode, bool isAsync )
: _mode(mode)
{
    _impl = new FileDeviceImpl(*this);

    this->open( path, mode, isAsync);
}


FileDevice::~FileDevice()
{
    if( this->enabled() ) {
        try { this->close(); } catch(...) { }
    }

    delete _impl;
}


void FileDevice::open( const char* path, std::ios_base::openmode mode, bool isAsync )
{
    if( this->enabled() ) {
        this->close();
    }

    _impl->open(path, mode, isAsync );

    _mode = mode;
    _path = path;

    IODevice::setEnabled(true);
    IODevice::setEof(false);
    IODevice::setAsync(isAsync);
}


void FileDevice::onClose()
{
    _impl->close();
    IODevice::setEnabled(false);
    IODevice::setEof(false);
}


void FileDevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    _impl->beginRead(buffer, n, eof);
}


size_t FileDevice::onEndRead(bool& eof)
{
    return _impl->endRead(eof);
}


void FileDevice::onBeginWrite(const char* buffer, size_t n)
{
    _impl->beginWrite(buffer, n);
}


size_t FileDevice::onEndWrite()
{
    return _impl->endWrite();
}


size_t FileDevice::size() const
{
    return _impl->size();
}


bool FileDevice::onWait(unsigned n)
{
    return _impl->wait(n);
}


FileDevice::pos_type FileDevice::onSeek(off_type offset, std::ios::seekdir sd)
{
    return _impl->seek(offset, sd);
}


size_t FileDevice::onRead( char* buffer, size_t count, bool& eof )
{
    //if(count > SSIZE_MAX)
    //    count = SSIZE_MAX;

    size_t ret = _impl->read( buffer, count, eof );
    return ret;
}


size_t FileDevice::onWrite(const char* buffer, size_t count)
{
    if( _mode & std::ios_base::out )
        return _impl->write(buffer, count);

    return 0;
}


size_t FileDevice::onPeek(char* buffer, size_t count)
{
    return _impl->peek(buffer, count);
}


void FileDevice::onSync() const
{
    if( _mode & std::ios_base::out )
        _impl->sync();
}


IODeviceImpl& FileDevice::ioimpl()
{ 
    return *_impl; 
}


SelectableImpl& FileDevice::simpl()
{ 
    return *_impl; 
}


void FileDevice::onAttach(SelectorBase& s)
{
    _impl->attach(s);
}


void FileDevice::onDetach(SelectorBase& s)
{
    _impl->detach(s);
}

} // namespace System

} // namespace Pt
