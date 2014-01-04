/*
 * Copyright (C) 2010-2012 by Marc Boris Duerner
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

#include "Connection.h"
#include <CertificateImpl.h>
#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <Pt/System/IOError.h>
#include <limits>
#include <algorithm>
#include <cassert>
#include <cstring>

log_define("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

StreamBuffer::StreamBuffer(std::size_t bufferSize)
: _connection(0)
, _ibufferSize(bufferSize + _pbmax)
, _ibuffer(0)
, _obufferSize(bufferSize)
, _obuffer(0)
{
}


StreamBuffer::StreamBuffer(Context& ctx, std::ios& ios, OpenMode mode, std::size_t bufferSize)
: _connection(0)
, _ibufferSize(bufferSize + _pbmax)
, _ibuffer(0)
, _obufferSize(bufferSize)
, _obuffer(0)
{
    this->open(ctx, ios, mode);
}


StreamBuffer::~StreamBuffer()
{ 
    close();
}


void StreamBuffer::open(Context& ctx, std::ios& ios, OpenMode mode)
{
    if(_connection)
    {
        delete _connection;
        _connection = 0;
    }
    
    _connection = new Connection(ctx, ios, mode);
}


void StreamBuffer::close()
{
    if(_connection)
    {
        delete _connection;
        _connection = 0;
    }

    // TODO: delete buffers only in dtor
    // setg(0,0,0) and setp(0,0) should be enough
    delete [] _ibuffer; _ibuffer = 0;
    delete [] _obuffer; _obuffer = 0;

    setg(0, 0, 0);
    setp(0, 0);
}


const char* StreamBuffer::currentCipher() const
{
    if(_connection)
        return _connection->currentCipher();

    return "NONE";
}


bool StreamBuffer::isConnected() const
{ 
    return _connection && _connection->connected(); 
}


bool StreamBuffer::writeHandshake()
{
    if( ! _connection )
        throw SslError("no connection");

    return _connection->writeHandshake();
}


bool StreamBuffer::readHandshake(std::streamsize)
{   
    if( ! _connection )
        throw SslError("no connection");

    return _connection->readHandshake();
}


bool StreamBuffer::shutdown()
{
    bool shutdownComplete = true;

    if( _connection )
    {
        sync();
        shutdownComplete = _connection->shutdown();
    }

    // TODO: delete buffers only in dtor
    // setg(0,0,0) and setp(0,0) should be enough
    delete [] _ibuffer; _ibuffer = 0;
    delete [] _obuffer; _obuffer = 0;

    setg(0, 0, 0);
    setp(0, 0);
    
    // TODO: return shutdown state
    return shutdownComplete;
}


bool StreamBuffer::isShutdown() const
{
    return _connection && _connection->isShutdown();
}


bool StreamBuffer::isClosed() const
{
    return _connection && _connection->isClosed(); 
}


void StreamBuffer::import(std::streamsize maxImport)
{
    log_trace("StreamBuffer::import");

    if( ! _connection )
        return;

    if( ! _ibuffer ) 
    {
        log_debug("setting up get area: " << _ibufferSize);
        _ibuffer = new char[_ibufferSize];
    }

    // return if full
    std::size_t leftover = this->egptr() - this->gptr();

    if( leftover == _ibufferSize - _pbmax)
    {
        log_debug("get area is full");
        return;
    }

    // move unread bytes and putback to front
    std::size_t putback  = _pbmax;
    if( this->gptr() ) 
    {
        putback = std::min<std::size_t>( this->gptr() - this->eback(), _pbmax);
        char* to = _ibuffer + _pbmax - putback;
        char* from = this->gptr() - putback;

        leftover = this->egptr() - this->gptr();
        std::memmove( to, from, putback + leftover );

        this->setg( _ibuffer + (_pbmax - putback),  // start of get area
                    _ibuffer + _pbmax,              // gptr position
                    _ibuffer + _pbmax + leftover ); // end of get area
    }

    // We only have to make some progress
    std::size_t used = _pbmax + leftover;
    std::size_t unused = _ibufferSize - used;

    log_debug("get area free space: " << unused);
    assert(unused);

    std::streamsize readSize = _connection->read(_ibuffer + used, unused, maxImport);
    log_debug("read " << readSize << " bytes from connection");

    if(readSize > 0)
    {
        this->setg( _ibuffer + (_pbmax - putback), // start of get area
                    _ibuffer + _pbmax,             // gptr position
                    _ibuffer + used + readSize );  // end of get area
    }

    return;
}


std::streamsize StreamBuffer::showmanyc()
{
    // TODO:
    // return _connection && _connection->rdbuf() ? _target->rdbuf()->in_avail() : -1;

    return 0;
}


std::streamsize StreamBuffer::showfull()
{ 
    return 0; 
}


int StreamBuffer::sync()
{
    if( this->pptr() )
    {
        while( this->pptr() > this->pbase() )
        {
            const int_type ch = this->overflow( traits_type::eof() );
            if( ch == traits_type::eof() )
            {
                return -1;
            }
        }
    }

    return 0;
}


StreamBuffer::int_type StreamBuffer::underflow()
{
    log_trace("StreamBuffer::underflow");

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    // TODO: special value to indicate blocking I/O
    std::streamsize max = std::numeric_limits<std::streamsize>::max();
    this->import(max);

    //if( 0 == this->do_underflow(_ibufferSize) )
    //{
    //    if( isShutdown() ) 
    //    {
    //        log_debug("Received shutdown notification");
    //        return traits_type::eof();
    //    }

    //    if( traits_type::eof() == _ios->sgetc() )
    //    {
    //        log_debug("underlying streambuf is EOF");
    //        return traits_type::eof();
    //    }
    //}

    return this->gptr() < this->egptr() ? traits_type::to_int_type( *gptr() )
                                        : traits_type::eof();
}


StreamBuffer::int_type StreamBuffer::overflow(int_type ch)
{
    // We are being called when _obuffer, the output buffer area of the
    // i/o stream is full, or needs to be flushed. In case of a flush,
    // the eof character is passed to overflow(). When StreamBuffer is
    // constructed no output buffer area exists, therefore when overflow
    // is called for the first time, we need to set it up.

    if( ! _connection )
        return 0;

    // No buffer area etablished yet
    if( ! _obuffer ) 
    {
        log_debug("Allocating _obuffer of size " << _obufferSize);

        _obuffer = new char[_obufferSize];
        this->setp(_obuffer, _obuffer + _obufferSize);
    }
    else
    {
        // Normal blocking overflow case
        std::size_t avail = this->pptr() - _obuffer;

        std::streamsize written = _connection->write(_obuffer, avail);
        if(written == 0)
            return traits_type::eof();

        // Move leftover in _obuffer to the front
        std::size_t leftover = avail - static_cast<std::size_t>(written);
        if(leftover > 0)  
            traits_type::move(_obuffer, _obuffer + written, leftover);
        
        this->setp(_obuffer, _obuffer + _obufferSize);
        this->pbump( leftover );
    }

    // put the overflow char in the buffer area, if not EOF
    if( ! traits_type::eq_int_type( ch, traits_type::eof() ) )
    {
        *(this->pptr()) = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}

} // namespace Ssl

} // namespace Pt
