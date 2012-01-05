/*
 * Copyright (C) 2004-2008 Marc Boris Duerner
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

#include "Pt/System/IODevice.h"
#include <cstring>
#include <cassert>

namespace Pt {

namespace System {

IODevice::IODevice()
: _eof(false)
, _avail(false)
, _rbuf(0)
, _rbuflen(0)
, _ravail(0)
, _wbuf(0)
, _wbuflen(0)
, _wavail(0)
, _reserved(0)
{ }


IODevice::~IODevice()
{ }


void IODevice::close()
{
    this->cancel();
    this->onClose();
    IODevice::setEof(false);
}


void IODevice::beginRead(char* buffer, size_t n)
{
    if( ! isActive() )
        throw std::logic_error( PT_ERROR_MSG("I/O device not active") );

    if (_rbuf || _wbuf)
        throw IOPending( PT_ERROR_MSG("I/O operation pending") );

    size_t r = this->onBeginRead(buffer, n, _eof);

    if(r > 0 || _eof)
        this->setAvail();

    _rbuf = buffer;
    _rbuflen = n;
    _ravail = r;
}


size_t IODevice::endRead()
{
    if( ! _rbuf )
        return 0;

    size_t n = 0;

    if(_ravail > 0 || _eof)
    {
        n = _ravail;
        _rbuf = 0;
        _rbuflen = 0;
        _ravail = 0;

        this->setIdle();
        return n;
    }

    try
    {
        n = this->onEndRead(_eof);
    }
    catch (...)
    {
        _rbuf = 0;
        _rbuflen = 0;
        _ravail = 0;
        throw;
    }

    _rbuf = 0;
    _rbuflen = 0;
    _ravail = 0;

    return n;
}


size_t IODevice::read(char* buffer, size_t n)
{
    if( _rbuf || _wbuf)
        throw IOPending( PT_ERROR_MSG("I/O operation pending") );

    return this->onRead(buffer, n, _eof);
}


size_t IODevice::beginWrite(const char* buffer, size_t n)
{
    if( ! isActive() )
        throw std::logic_error( PT_ERROR_MSG("I/O device not active") );

    if (_wbuf || _rbuf)
        throw IOPending( PT_ERROR_MSG("I/O operation pending") );

    size_t r = this->onBeginWrite(buffer, n);

    if(r > 0)
        this->setAvail();

    _wbuf = buffer;
    _wbuflen = n;
    _wavail = r;

    return r;
}


size_t IODevice::endWrite()
{
    if( ! _wbuf )
        return 0;

    size_t n = 0;

    if(_wavail > 0)
    {
        n = _wavail;
        _wbuf = 0;
        _wbuflen = 0;
        _wavail = 0;

        this->setIdle();
        return n;
    }

    try
    {
        n = onEndWrite();
    }
    catch (...)
    {
        _wbuf = 0;
        _wbuflen = 0;
        _wavail = 0;
        throw;
    }

    _wbuf = 0;
    _wbuflen = 0;
    _wavail = 0;

    return n;
}


size_t IODevice::write(const char* buffer, size_t n)
{
    if( _rbuf || _wbuf)
        throw IOPending( PT_ERROR_MSG("I/O operation pending") );

    return this->onWrite(buffer, n);
}


void IODevice::setAvail()
{
    System::EventLoop* loop = this->parent();
    if(loop)
        loop->setAvail(*this); 

    _avail = true;
}


void IODevice::setIdle()
{
    System::EventLoop* loop = this->parent();
    if(loop)
        loop->setIdle(*this);

    _avail = false;
}


void IODevice::onCancel()
{
    this->setIdle();

    _rbuf = 0;
    _rbuflen = 0;
    _ravail = 0;

    _wbuf = 0;
    _wbuflen = 0;
    _wavail = 0;
}


bool IODevice::seekable() const
{
    return onSeekable();
}


IODevice::pos_type IODevice::seek(off_type offset, std::ios::seekdir sd)
{
    off_type ret = this->onSeek(offset, sd);
    if( ret != off_type(-1) )
        setEof(false);

    return ret;
}


size_t IODevice::peek(char* buffer, size_t n)
{ 
    return this->onPeek(buffer, n); 
}


void IODevice::sync()
{ 
    return this->onSync(); 
}


IODevice::pos_type IODevice::position()
{ 
    return this->seek(0, std::ios::cur); 
}


bool IODevice::eof() const
{ 
    return _eof; 
}


void IODevice::setEof(bool eof)
{ 
    _eof = eof; 
}


Signal<IODevice&>& IODevice::inputReady()
{
    return _inputReady;
}


Signal<IODevice&>& IODevice::outputReady()
{
    return _outputReady;
}

}

}
