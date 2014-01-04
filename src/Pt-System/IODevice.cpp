/*
 * Copyright (C) 2004-2013 Marc Boris Duerner
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

#include <Pt/System/IODevice.h>
#include <Pt/System/EventLoop.h>

namespace Pt {

namespace System {

IODevice::IODevice()
: _loop(0)
, _rbuf(0)
, _rbuflen(0)
, _ravail(0)
, _wbuf(0)
, _wbuflen(0)
, _wavail(0)
, _eof(false)
{ }


IODevice::~IODevice()
{
}


void IODevice::close()
{
    this->cancel();
    this->onClose();
    IODevice::setEof(false);
}


void IODevice::setTimeout(std::size_t timeout)
{
    this->onSetTimeout(timeout);
}


void IODevice::beginRead(char* buffer, std::size_t n)
{
    EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("I/O device not active");

    if (_rbuf || _wbuf)
        throw IOPending("I/O operation pending");

    std::size_t r = this->onBeginRead(*loop, buffer, n, _eof);

    if(r > 0 || _eof)
        loop->setReady(*this); 

    _rbuf = buffer;
    _rbuflen = n;
    _ravail = r;
}


std::size_t IODevice::endRead()
{
    if( ! _rbuf )
        return 0;

    std::size_t n = 0;

    if(_ravail > 0 || _eof)
    {
        n = _ravail;
        _rbuf = 0;
        _rbuflen = 0;
        _ravail = 0;
        return n;
    }

    try
    {
        n = this->onEndRead(*_loop, _rbuf, _rbuflen, _eof);
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


std::size_t IODevice::read(char* buffer, std::size_t n)
{
    if( _rbuf || _wbuf)
        throw IOPending("I/O operation pending");

    return this->onRead(buffer, n, _eof);
}


void IODevice::beginWrite(const char* buffer, std::size_t n)
{
    EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("I/O device not active");

    if (_wbuf || _rbuf)
        throw IOPending("I/O operation pending");

    std::size_t r = this->onBeginWrite(*loop, buffer, n);

    if(r > 0)
        loop->setReady(*this); 

    _wbuf = buffer;
    _wbuflen = n;
    _wavail = r;
}


std::size_t IODevice::endWrite()
{
    if( ! _wbuf )
        return 0;

    std::size_t n = 0;

    if(_wavail > 0)
    {
        n = _wavail;
        _wbuf = 0;
        _wbuflen = 0;
        _wavail = 0;
        return n;
    }

    try
    {
        n = onEndWrite(*_loop, _wbuf, _wbuflen);
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


std::size_t IODevice::write(const char* buffer, std::size_t n)
{
    if( _rbuf || _wbuf)
        throw IOPending("I/O operation pending");

    return this->onWrite(buffer, n);
}


bool IODevice::seekable() const
{
    return onSeekable();
}


IODevice::pos_type IODevice::seek(off_type offset, seekdir sd)
{
    off_type ret = this->onSeek(offset, sd);
    if( ret != off_type(-1) )
        setEof(false);

    return ret;
}


std::size_t IODevice::peek(char* buffer, std::size_t n)
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


bool IODevice::isEof() const
{ 
    return _eof; 
}


void IODevice::setEof(bool eof)
{ 
    _eof = eof; 
}


void IODevice::onAttach(EventLoop& loop)
{ 
    _loop = &loop;
}


void IODevice::onDetach(EventLoop& loop)
{ 
    _loop = 0; 
}


void IODevice::onCancel()
{
    _rbuf = 0;
    _rbuflen = 0;
    _ravail = 0;

    _wbuf = 0;
    _wbuflen = 0;
    _wavail = 0;
}

} // namespace System

} // namespace Pt
