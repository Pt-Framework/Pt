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
#include <cassert>
#include <cstring>

log_define("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

StreamBuffer::StreamBuffer(size_t bufferSize)
: _connection(0)
, _ibufferSize(bufferSize + 4)
, _ibuffer(0)
, _obufferSize(bufferSize)
, _obuffer(0)
, _pbmax(4)
{
}


StreamBuffer::StreamBuffer(Context& ctx, std::streambuf& sb, OpenMode mode, size_t bufferSize)
: _connection(0)
, _ibufferSize(bufferSize + 4)
, _ibuffer(0)
, _obufferSize(bufferSize)
, _obuffer(0)
, _pbmax(4)
{
    this->open(ctx, sb, mode);
}


StreamBuffer::~StreamBuffer()
{ 
    if(_connection)
        delete _connection; 

    delete [] _ibuffer;
    delete [] _obuffer;
}


void StreamBuffer::open(Context& ctx, std::streambuf& sb, OpenMode mode)
{
    if(_connection)
        delete _connection;

    _connection = 0;
    _connection = new Connection(ctx, sb, mode);
}


//void StreamBuffer::discard()
//{
//    if( ! _ssl)
//        return;
//
//    // Reset all
//    (void) BIO_reset(_in);
//    (void) BIO_reset(_out);
//    SSL_clear(_ssl);
//
//    delete [] _ibuffer; _ibuffer = 0;
//    delete [] _obuffer; _obuffer = 0;
//
//    _connected = false;
//}


//CipherList StreamBuffer::ciphers() const
//{
//    if( ! _ssl )
//        return CipherList();
//
//    // TODO: possibly cache the available ciphers in the context
//    STACK_OF(SSL_CIPHER)* ciphers = SSL_get_ciphers(_ssl);
//    return CipherList(ciphers);
//}
//
//
//Cipher StreamBuffer::currentCipher() const
//{
//    if( ! _ssl )
//        return Cipher();
//
//    // TODO: possibly return a Cipher that has internally a reference to a 
//    //       CipherData in the context cache
//    const SSL_CIPHER* c = SSL_get_current_cipher(_ssl);
//    return Cipher(c);
//}


bool StreamBuffer::isConnected() const
{ 
    return _connection && _connection->connected(); 
}


bool StreamBuffer::writeHandshake()
{
    log_trace("StreamBuffer::writeHandshake");
    
    if( ! _connection )
        throw SslError("no connection");

    return _connection->writeHandshake();
}


bool StreamBuffer::readHandshake()
{
    log_trace("StreamBuffer::readHandshake");
    
    if( ! _connection )
        throw SslError("no connection");

    return _connection->readHandshake();
}


bool StreamBuffer::shutdown()
{
    if( _connection )
        _connection->shutdown();

    delete [] _ibuffer; _ibuffer = 0;
    delete [] _obuffer; _obuffer = 0;

    setg(0, 0, 0);
    setp(0, 0);
    
    // TODO: return shutdown state
    return false;
}


bool StreamBuffer::isShutdown() const
{
    return _connection && _connection->isShutdown(); 
}


bool StreamBuffer::isClosed() const
{
    return _connection && _connection->isClosed(); 
}


void StreamBuffer::import(std::streamsize isize)
{
    log_trace("StreamBuffer::do_underflow");

    if( ! _connection )
        return;

    if( ! _ibuffer ) 
    {
        log_debug("setting up get area: " << _ibufferSize);
        _ibuffer = new char[_ibufferSize];
    }

    // Return 0 if full
    if(_ibufferSize == (this->egptr() - this->gptr() + _pbmax))
    {
        log_debug("get area is full");
        return;
    }

    // Move unread bytes and putback to front
    size_t putback  = _pbmax;
    size_t leftover = 0;
    if( this->gptr() ) 
    {
        putback = std::min<size_t>( this->gptr() - this->eback(), _pbmax);
        char* to = _ibuffer + _pbmax - putback;
        char* from = this->gptr() - putback;

        leftover = this->egptr() - this->gptr();
        std::memmove( to, from, putback + leftover );

        this->setg( _ibuffer + (_pbmax - putback),  // start of get area
                    _ibuffer + _pbmax,              // gptr position
                    _ibuffer + _pbmax + leftover ); // end of get area
    }

    // We only have to make some progress
    size_t used = _pbmax + leftover;
    size_t unused = _ibufferSize - used;

    log_debug("get area free space: " << unused);
    assert(unused);

    std::streamsize readSize = _connection->read(_ibuffer + used, unused, isize);
    log_debug("read " << readSize << " bytes from connection");

    if(readSize > 0)
    {
        this->setg( _ibuffer + (_pbmax - putback), // start of get area
                    _ibuffer + _pbmax,             // gptr position
                    _ibuffer + used + readSize );  // end of get area
    }

    return;
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

    this->import(_ibufferSize);

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


/*
std::vector<CipherInfo> StreamBuffer::availableCiphers() const
{
    std::vector<CipherInfo> availCiphers;

    STACK_OF(SSL_CIPHER)* chp = SSL_get_ciphers(_ssl);
    for(int i = 0; i < sk_SSL_CIPHER_num(chp); ++i)
    {
        // Skip if not valid
        const SSL_CIPHER* c = sk_SSL_CIPHER_value(chp, i);
        if( ! c->valid )
            continue;

        // Get the ID and split it
        const unsigned long id  = c->id;
        const int           id0 = (int) (  id >> 24);
        const int           id1 = (int) ( (id >> 16) & 0xFFL );
        const int           id2 = (int) ( (id >>  8) & 0xFFL );
        const int           id3 = (int) (  id        & 0xFFL );

        // Convert the ID to a readable string
        char strid[64];
        if((id & 0xFF000000L) == 0x02000000L)
            sprintf(strid, "0x%02X,0x%02X,0x%02X", id1, id2, id3);
        else if((id & 0xFF000000L) == 0x03000000L)
            sprintf(strid, "0x%02X,0x%02X", id2, id3);
        else
            sprintf(strid, "0x%02X,0x%02X,0x%02X,0x%02X", id0, id1, id2, id3);

        // Get some information
        char desc[512];
        SSL_CIPHER_description(c, desc, sizeof(desc));
        const int dlen = strlen(desc);
        if(desc[dlen - 1] == '\n')
            desc[dlen - 1] = 0;

        // Store the chiper information
        int usedBits;
        int bits = SSL_CIPHER_get_bits(c, &usedBits);
        CipherInfo cipher(id, strid, SSL_CIPHER_get_name(c), bits, usedBits, 
                          SSL_CIPHER_get_version(c), desc);
        
        availCiphers.push_back(cipher);
    }

    return availCiphers;
}*/

/*
void StreamBuffer::setCiphers(const std::vector<SSLCipherInfo>& ciphers)
{
    std::string str;
    for(size_t i = 0; i < ciphers.size(); ++i) {
        if(!str.empty()) str += ":";
        str += ciphers[i].name;
    }

    if(!SSL_set_cipher_list(_ssl, str.c_str()))
        throw SSLRuntimeError("Failed selecting SSL ciphers!", PT_SOURCEINFO);

    _enabledCiphers = ciphers;
}
*/

} // namespace Ssl

} // namespace Pt
