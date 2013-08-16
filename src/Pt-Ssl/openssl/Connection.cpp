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
#include "ContextImpl.h"
#include "OpenSsl.h"
#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <Pt/System/IOError.h>
#include <cassert>
#include <cstring>

#include <openssl/err.h>

log_define("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

Connection::Connection(Context& ctx, std::streambuf& ios, int mode)
: _ios(&ios)
, _connected(false)
, _in(0)
, _out(0)
, _ssl(0)
{
    // Create the SSL objects
    _in  = BIO_new( BIO_s_mem() );
    _out = BIO_new( BIO_s_mem() );
    _ssl = SSL_new( ctx.impl()->ctx() );

    // Connect the BIO
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);

    if(mode == StreamBuffer::Accept)
        SSL_set_accept_state(_ssl);
    else
        SSL_set_connect_state(_ssl);
}


Connection::~Connection()
{
    if(_ssl)
        SSL_free(_ssl); 
}


bool Connection::writeHandshake()
{
    log_trace("Connection::writeHandshake");

    if( ! _ios || ! _ssl)
        throw System::IOError("SSL Buffer not initialized");

    int ret = SSL_do_handshake(_ssl);
    log_debug("SSL_do_handshake returns " << ret);

    if(ret <= 0)
    {
        const int sslerr = SSL_get_error(_ssl, ret);
        if(sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) 
        {
            if(sslerr == SSL_ERROR_SSL)
            {
                char buf[255];
                ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
                log_warn("handshake failed: " << buf);
            }
            
            throw HandshakeFailed("SSL handshake failed");
        }
    }

    if(ret == 1)
    {
        _connected = true;
    }

    if( BIO_pending(_out) )
    {
        char buff[1000];
        const int n = BIO_read(_out, buff, sizeof(buff));
        log_debug("wrote " << n << " bytes to output");

        if(n <= 0)
            throw System::IOError("BIO_read");

        _ios->sputn(buff, n);
        return true;
    }

    return SSL_want_write(_ssl);   
}


bool Connection::readHandshake()
{
    log_trace("Connection::readHandshake");

    if( ! _ios || ! _ssl)
        throw System::IOError("SSL Buffer not initialized");

    while(_ios->in_avail() > 0)
    {
        const std::streamsize bufsize = 2000;
        char buf[bufsize];

        std::streamsize gsize = std::min( _ios->in_avail(), bufsize );
        std::streamsize n = _ios->sgetn(buf, gsize);

        const int written = BIO_write(_in, buf, static_cast<int>(n));
        assert(written == n);

        if(written <= 0 || written != n)
            throw System::IOError("BIO_write");

        log_debug("read " << n << " bytes from input");
    }

    int ret = SSL_do_handshake(_ssl);
    log_debug("SSL_do_handshake returns " << ret);

    if( ret <= 0 )
    {
        int sslerr = SSL_get_error(_ssl, ret);
        if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) 
        {
            if(sslerr == SSL_ERROR_SSL)
            {
                char buf[255];
                ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
                log_warn("handshake failed: " << buf);
            }

            throw HandshakeFailed("SSL handshake failed");
        }
    }

    if( ret == 1 && BIO_pending(_out) <= 0 )
    {
        _connected = true;
    }

    return BIO_pending(_out) <= 0 && SSL_want_read(_ssl);   
}


void Connection::shutdown()
{
    if( ! _ios || ! _ssl)
        throw System::IOError("SSL Buffer not initialized");

    const int res = SSL_shutdown(_ssl);
    log_debug("SSL_shutdown() = " << res);

    // Send shutdown message to the other peer
    char buff[1000];
    const int n = BIO_read(_out, buff, sizeof(buff));
    if(n <= 0)
        throw SslError("BIO_read");

    _ios->sputn(buff, n);
    log_debug("Wrote " << n << " bytes to output");

    // Reset all
    BIO_reset(_in);
    BIO_reset(_out);
    SSL_clear(_ssl);

    _connected = false;
}


bool Connection::isShutdown() const
{
    if( ! _ssl)
        return false;

    int state = SSL_get_shutdown(_ssl);
    
    return (SSL_RECEIVED_SHUTDOWN & state) == SSL_RECEIVED_SHUTDOWN;
}


bool Connection::isClosed() const
{
    if( ! _ssl)
        return false;

    int state = SSL_get_shutdown(_ssl);
    
    return (SSL_SENT_SHUTDOWN & state) == SSL_SENT_SHUTDOWN;
}


std::streamsize Connection::write(const char* buf, size_t n)
{
    if( ! _ios || ! _ssl )
        return 0;

    std::streamsize written = SSL_write(_ssl, buf, n);
    log_debug("encrypted " << written << " bytes");

    BUF_MEM* bm = 0;
    BIO_get_mem_ptr(_out, &bm);
    if(bm->length > 0)
    {
        _ios->sputn(bm->data, bm->length);
        log_debug("wrote " << bm->length << " bytes to output");
        bm->length = 0;
    }

    return written;
}


std::streamsize Connection::read(char* buf, size_t n, std::streamsize isize)
{
    if( ! _ios || ! _ssl) 
        return 0;

    if(isize == 0) 
        isize = _ios->in_avail();

    while(true) 
    {
        // even if we could not refill the BIO, we might still get data from the SSL
        const int readSize = SSL_read(_ssl, buf, n);
        log_debug("Read " << readSize << " bytes from _ssl");
        log_debug("SSL_get_shutdown() = " << SSL_get_shutdown(_ssl));

        if(readSize > 0)
        {           
            return readSize;
        }

        long sslerr = SSL_get_error(_ssl, readSize);
        if(sslerr != SSL_ERROR_WANT_READ)
        {
            // This error may indicate that the other peer has send shutdown message
            if(sslerr == SSL_ERROR_ZERO_RETURN)
            {
                log_debug("SSL_ERROR_ZERO_RETURN");
                return 0;
            }

            log_debug("ssl error occured");
            while( ( sslerr = ERR_get_error() ) ) 
            {
                log_debug("ERR_error_string = " << ERR_error_string(sslerr, 0));
            }
            
            throw System::IOError("Failed reading decrypted data from OpenSSL!");
        }

        if(isize == 0)
            return 0;

        // Refill the BIO with encoded bytes for decoding
        BUF_MEM* bm = 0;
        BIO_get_mem_ptr(_in, &bm);

        if(bm->max == bm->length)
            continue;

        const std::streamsize refill = std::min(static_cast<std::streamsize>(bm->max - bm->length), isize);
        log_debug("get " << refill << " bytes from _ios");
        
        std::streamsize gcount = _ios->sgetn(bm->data + bm->length, refill);
        if(gcount <= 0)
            return 0;

        bm->length += static_cast<int>( gcount );
        log_debug("Wrote " << gcount << " bytes from _ios to _in BUF_MEM");

        isize -= gcount;
    }

    return 0;
}

} // namespace Ssl

} // namespace Pt
