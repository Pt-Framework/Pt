/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
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

namespace Pt {

namespace System {

IODevice::IODevice()
: _eof(false)
, _async(false)
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


void IODevice::beginRead(char* buffer, size_t n)
{
    if ( ! async() )
        throw std::logic_error("Device not in async mode." + PT_SOURCEINFO);

    size_t r = this->onBeginRead(buffer, n, _eof);

    if(r > 0 || _eof || _wavail)
        this->setState(Selectable::Avail);
    else
        this->setState(Selectable::Busy);

    _rbuf = buffer;
    _rbuflen = n;
    _ravail = r;
}


size_t IODevice::endRead()
{
    size_t n = this->onEndRead(_eof);

    if(_wavail > 0)
        this->setState(Selectable::Avail);
    else if(_wbuf)
        this->setState(Selectable::Busy);
    else
        this->setState(Selectable::Idle);

    _rbuf = 0;
    _rbuflen = 0;
    _ravail = 0;
    return n;
}


size_t IODevice::read(char* buffer, size_t n)
{
    if ( async() )
    {
        this->beginRead(buffer, n);
        this->wait();
        return endRead();
    }

    return this->onRead(buffer, n, _eof);
}


void IODevice::beginWrite(const char* buffer, size_t n)
{
    if ( ! async() )
        throw std::logic_error("Device not in async mode." + PT_SOURCEINFO);

    size_t r = this->onBeginWrite(buffer, n);

    if(r > 0 || _ravail)
        this->setState(Selectable::Avail);
    else
        this->setState(Selectable::Busy);

    _wbuf = buffer;
    _wbuflen = n;
    _wavail = r;
}


size_t IODevice::endWrite()
{
    size_t n =  onEndWrite();

    if(_ravail > 0 || (_rbuf && _eof) )
        this->setState(Selectable::Avail);
    else if(_rbuf)
        this->setState(Selectable::Busy);
    else
        this->setState(Selectable::Idle);

    _wbuf = 0;
    _wbuflen = 0;
    _wavail = 0;
    return n;
}


size_t IODevice::write(const char* buffer, size_t n)
{
    if ( async() )
    {
        this->beginWrite(buffer, n);
        this->wait();
        return endWrite();
    }

    return this->onWrite(buffer, n);
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


bool IODevice::async() const
{
    return _async; 
}


void IODevice::setEof(bool eof)
{ 
    _eof = eof; 
}


void IODevice::setAsync(bool async)
{
    _async = async; 
}

}

}
