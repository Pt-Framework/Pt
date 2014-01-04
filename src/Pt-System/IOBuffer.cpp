/*
 * Copyright (C) 2005-2013 Marc Boris Duerner
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

#include <Pt/System/IOBuffer.h>
#include <Pt/System/IOError.h>
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace Pt {

namespace System {

IOBuffer::IOBuffer(std::size_t bufferSize, bool extend)
: _ioDevice   (0)
, _ibufferSize(0)
, _ibuffer    (0)
, _obufferSize(0)
, _obuffer    (0)
, _oextend    (false)
{
    init(bufferSize, extend);
}

IOBuffer::IOBuffer(IODevice& ioDevice, std::size_t bufferSize, bool extend)
: _ioDevice   (0)
, _ibufferSize(0)
, _ibuffer    (0)
, _obufferSize(0)
, _obuffer    (0)
, _oextend    (false)
{
    init(bufferSize, extend);
    attach(ioDevice); 
}


IOBuffer::~IOBuffer()
{
    delete _ibuffer;
    delete _obuffer;
}


void IOBuffer::init(std::size_t bufferSize, bool extend)
{
    _ibufferSize = bufferSize + 4;
    _ibuffer = 0;
    _obufferSize = bufferSize;
    _obuffer = 0;
    _oextend = extend;

    if( gptr() )
        setg(_ibuffer, _ibuffer + _ibufferSize, _ibuffer + _ibufferSize);

    if( pptr() )
        setp(_obuffer, _obuffer + _obufferSize);
}


void IOBuffer::attach(IODevice& ioDevice)
{ 
    if( ioDevice.isReading() || ioDevice.isWriting() )
        throw IOPending("IODevice in use");

    this->detach();

    _ioDevice = &ioDevice;
    ioDevice.inputReady() += slot(*this, &IOBuffer::onRead);
    ioDevice.outputReady() += slot(*this, &IOBuffer::onWrite);
}


void IOBuffer::detach()
{ 
    if( ! _ioDevice)
        return;

    if( _ioDevice->isReading() || _ioDevice->isWriting() )
        throw IOPending("IODevice in use");

    _ioDevice->inputReady() -= slot(*this, &IOBuffer::onRead);
    _ioDevice->outputReady() -= slot(*this, &IOBuffer::onWrite);
    _ioDevice = 0;
}


void IOBuffer::reset()
{
    discard();
    detach();
}


void IOBuffer::discard()
{
    if(_ioDevice && (_ioDevice->isReading() || _ioDevice->isWriting()))
        throw IOPending("IOBuffer in use");

    setg(0, 0, 0);

    if(_obuffer)
        setp(_obuffer, _obuffer + _obufferSize);
    else
        setp(0, 0);
}


void IOBuffer::beginRead()
{
    if(_ioDevice == 0 || _ioDevice->isReading())
        return;

    if( ! _ibuffer)
        _ibuffer = new char[_ibufferSize];

    std::size_t putback = _pbmax;
    std::size_t leftover = 0;

    // Keep chars for putback
    if( gptr() )
    {
        putback    = std::min<std::size_t>(gptr() - eback(), _pbmax);
        char* to   = _ibuffer + _pbmax - putback;
        char* from = gptr() - putback;

        leftover = egptr() - gptr();
        std::memmove(to, from, putback + leftover);
    }

    std::size_t used = _pbmax + leftover;

    if(_ibufferSize == used)
        throw std::logic_error(PT_ERROR_MSG("IOBuffer is full"));

    _ioDevice->beginRead(_ibuffer + used, _ibufferSize - used);

    setg(_ibuffer + (_pbmax - putback), // start of get area
         _ibuffer + used,               // gptr position
         _ibuffer + used);              // end of get area
}


void IOBuffer::onRead(IODevice& dev)
{ 
    _inputReady.send(*this); 
}


std::size_t IOBuffer::endRead()
{
    std::size_t readSize = _ioDevice->endRead();

    setg(eback(),             // start of get area
         gptr(),              // gptr position
         egptr() + readSize); // end of get area

    return readSize;
}


void IOBuffer::beginWrite()
{
    if(_ioDevice == 0 || _ioDevice->isWriting())
        return;

    if( pptr() )
    {
        std::size_t avail = pptr() - pbase();
        if(avail > 0)
        {
            _ioDevice->beginWrite(_obuffer, avail);
        }
    }
}


void IOBuffer::onWrite(IODevice& dev)
{ 
    _outputReady.send(*this); 
}


std::size_t IOBuffer::endWrite()
{
    typedef IOBuffer::traits_type traits_type;

    std::size_t leftover = 0;
    std::size_t written  = 0;

    if( pptr() )
    {
        std::size_t avail = pptr() - pbase();
        written      = _ioDevice->endWrite();

        leftover = avail - written;
        if(leftover > 0)
            traits_type::move(_obuffer, _obuffer + written, leftover);
    }

    setp(_obuffer, _obuffer + _obufferSize);
    pbump( static_cast<int>(leftover) );

    return written;
}


bool IOBuffer::isReading() const
{
    return _ioDevice ? _ioDevice->isReading() : false;
}


bool IOBuffer::isWriting() const
{
    return _ioDevice ? _ioDevice->isWriting() : false;
}


std::streamsize IOBuffer::showmanyc()
{
    if( ! _ioDevice || _ioDevice->isEof() )
    {
        return -1;
    }

    return 0;
}


std::streamsize IOBuffer::showfull()
{ 
    return 0; 
}


int IOBuffer::sync()
{
    typedef IOBuffer::traits_type traits_type;

    if(!_ioDevice )
        return 0;

    if( pptr() )
    {
        while(pptr() > pbase())
        {
            const IOBuffer::int_type ch = overflow(traits_type::eof());
            if(ch == traits_type::eof())
                return -1;

            _ioDevice->sync();
        }
    }

    return 0;
}


std::streambuf::int_type IOBuffer::underflow()
{
    typedef IOBuffer::traits_type traits_type;

    if(!_ioDevice)
        return traits_type::eof();

    if(_ioDevice->isReading())
        endRead();

    if( gptr() < egptr() )
        return traits_type::to_int_type( *gptr() );

    if(_ioDevice->isEof())
        return traits_type::eof();

    if(!_ibuffer)
        _ibuffer = new char[_ibufferSize];

    std::size_t putback = _pbmax;

    if( gptr() )
    {
        putback = std::min<std::size_t>(gptr() - eback(), _pbmax);
        std::memmove(_ibuffer + (_pbmax - putback),
                     gptr() - putback,
                     putback );
    }

    std::size_t readSize = _ioDevice->read(_ibuffer + _pbmax, _ibufferSize - _pbmax);

    setg(_ibuffer + _pbmax - putback,    // start of get area
         _ibuffer + _pbmax,              // gptr position
         _ibuffer + _pbmax + readSize ); // end of get area

    if(_ioDevice->isEof())
        return traits_type::eof();

    return traits_type::to_int_type( *gptr() );
}


std::streambuf::int_type IOBuffer::overflow(std::streambuf::int_type ch)
{
    typedef IOBuffer::traits_type traits_type;

    if(!_ioDevice)
        return traits_type::eof();

    if(!_obuffer)
    {
        _obuffer = new char[_obufferSize];
        setp(_obuffer, _obuffer + _obufferSize);
    }
    else if(_ioDevice->isWriting()) // beginWrite is unfinished
    {
        endWrite();
    }
    else if(traits_type::eq_int_type(ch, traits_type::eof()) || ! _oextend)
    {
        // normal blocking overflow case
        std::size_t avail    = pptr() - _obuffer;
        std::size_t written  = _ioDevice->write(_obuffer, avail);
        std::size_t leftover = avail - written;

        if(leftover > 0)
            traits_type::move(_obuffer, _obuffer + written, leftover);

        setp(_obuffer, _obuffer + _obufferSize);
        pbump( static_cast<int>(leftover) );
    }
    else
    {
        // if the buffer area is extensible and overflow is not called by
        // sync/flush we copy the output buffer to a larger one
        std::size_t bufsize = _obufferSize + (_obufferSize / 2);
        char* buf = new char[ bufsize ];
        traits_type::copy(buf, _obuffer, _obufferSize);
        std::swap(_obuffer, buf);
        setp(_obuffer, _obuffer + bufsize);
        pbump( static_cast<int>(_obufferSize) );
        _obufferSize = bufsize;
        delete [] buf;
    }

    // if the overflow char is not EOF put it in buffer
    if(traits_type::eq_int_type(ch, traits_type::eof()) == false)
    {
        *pptr() = traits_type::to_char_type(ch);
        pbump(1);
    }

    return traits_type::not_eof(ch);
}


std::streambuf::pos_type IOBuffer::seekoff(std::streambuf::off_type off, std::ios::seekdir dir, std::ios::openmode)
{
    typedef IOBuffer::pos_type pos_type;
    typedef IOBuffer::off_type off_type;

    pos_type ret = pos_type(off_type(-1));

    if( ! _ioDevice || ! _ioDevice->seekable() || off == 0)
        return ret;

    if(_ioDevice->isWriting())
        endWrite();

    if(_ioDevice->isReading())
        endRead();

    ret = _ioDevice->seek(off, dir);

    // Eliminate currently buffered sequence
    discard();

    return ret;
}


std::streambuf::pos_type IOBuffer::seekpos(std::streambuf::pos_type p, std::ios::openmode mode)
{ 
    return seekoff(p, std::ios::beg, mode); 
}


IOBuffer::int_type IOBuffer::pbackfail(std::streambuf::int_type)
{
    typedef IOBuffer::traits_type traits_type;
    return traits_type::eof();
}

} // namespace System

} // namespace Pt
