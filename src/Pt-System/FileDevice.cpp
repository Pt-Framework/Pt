/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
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
#include <Pt/System/FileDevice.h>
#include <Pt/System/EventLoop.h>

namespace Pt {

namespace System {

FileDevice::FileDevice()
: _opening(false)
, _isOpen(false)
{
    _impl = new FileDeviceImpl(*this);
}


FileDevice::FileDevice(const std::string& path, std::ios::openmode mode)
: _opening(false)
, _isOpen(false)
{
    _impl = new FileDeviceImpl(*this);

    this->open( path, mode);
}


FileDevice::FileDevice(const char* path, std::ios::openmode mode)
: _opening(false)
, _isOpen(false)
{
    _impl = new FileDeviceImpl(*this);

    this->open( path, mode);
}


FileDevice::~FileDevice()
{
    try 
    { 
        this->close(); 
    } 
    catch(...) 
    { }

    delete _impl;
}


void FileDevice::open(const std::string& path, std::ios::openmode mode)
{
    open( path.c_str(), mode );
}


void FileDevice::open(const char* path, std::ios::openmode mode)
{
    this->close();

    _impl->open(path, mode);
    _path = path;
}


void FileDevice::beginOpen(const std::string& path, std::ios::openmode mode)
{
    beginOpen( path.c_str(), mode );
}


void FileDevice::beginOpen(const char* path, std::ios::openmode mode)
{
    EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("I/O device not active" );

    this->close();

    _isOpen = _impl->beginOpen(*loop, path, mode);
    _opening = true;

    if(_isOpen)
    {
        loop->setReady(*this); 
    }
}


void FileDevice::endOpen()
{
    if(_isOpen)
    {
        _opening = false;
        return;
    }

    if(_opening)
    {
        _opening = false;
        _impl->endOpen( *loop() );
        _isOpen = true;
    }
}


void FileDevice::onClose()
{
    _impl->close();
    _isOpen = false;
    _opening = false;
}


void FileDevice::onCancel()
{
    EventLoop* loop = this->loop();
    if( loop )
    {
        _impl->cancel(*loop);
        _opening = false;
    }

    IODevice::onCancel();
}


void FileDevice::onSetTimeout(std::size_t timeout)
{
    _impl->setTimeout(timeout);
}


bool FileDevice::onSeekable() const
{ 
    return true; 
}

std::size_t FileDevice::onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl->beginRead(loop, buffer, n, eof);
}


std::size_t FileDevice::onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl->endRead(loop, buffer, n, eof);
}


std::size_t FileDevice::onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl->beginWrite(loop, buffer, n);
}


std::size_t FileDevice::onEndWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl->endWrite(loop, buffer, n );
}


FileDevice::pos_type FileDevice::onSeek(off_type offset, std::ios::seekdir sd)
{
    if( _opening || this->isReading() || this->isWriting() )
        throw IOPending( PT_ERROR_MSG("I/O operation pending") );
    
    return _impl->seek(offset, sd);
}


std::size_t FileDevice::onRead( char* buffer, std::size_t count, bool& eof )
{
    std::size_t ret = _impl->read( buffer, count, eof );
    return ret;
}


std::size_t FileDevice::onWrite(const char* buffer, std::size_t count)
{
    return _impl->write(buffer, count);
}


std::size_t FileDevice::onPeek(char* buffer, std::size_t count)
{
    return _impl->peek(buffer, count);
}


void FileDevice::onSync() const
{
    _impl->sync();
}


bool FileDevice::onRun()
{
    if( _opening )
    {
        if( _isOpen || _impl->runOpen( *loop() ) )
        {
            opened().send(*this);
            return true;
        }

        return false;
    }

    if( this->isReading() )
    {
        if( _ravail || isEof() || _impl->runRead( *loop() ) )
        {
            inputReady().send(*this);
            return true;
        }
    }

    if( this->isWriting() )
    {
        if( _wavail || _impl->runWrite( *loop() ) )
        {
            outputReady().send(*this);
            return true;
        }
    }

    return false;
}

} // namespace System

} // namespace Pt
