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
#include <cassert>

log_define("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

Connection::Connection(Context& ctx, std::ios& ios, OpenMode omode)
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

    if(omode == Accept)
        SSL_set_accept_state(_ssl);
    else
        SSL_set_connect_state(_ssl);

    assert(_ssl);
}


Connection::~Connection()
{
    if(_ssl)
        SSL_free(_ssl); 
}


//std::string Connection::peerName() const
//{
//    if( ! _ssl )
//        return std::string();
//
//    if(SSL_get_verify_result(_ssl) != X509_V_OK) 
//        return std::string();
//
//    X509* peer = SSL_get_peer_certificate(_ssl);
//    if( ! peer) 
//        return std::string();
//
//    char peerCN[256];
//    int  ret = X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peerCN, sizeof(peerCN));
//    return (ret > 0) ? peerCN : "";
//}

//CipherList Connection::ciphers() const
//{
//    if( ! _ssl )
//        return CipherList();
//
//    // TODO: possibly cache the available ciphers in the context
//    STACK_OF(SSL_CIPHER)* ciphers = SSL_get_ciphers(_ssl);
//    return CipherList(ciphers);
//}


//void Connection::setCiphers(const std::vector<SSLCipherInfo>& ciphers)
//{
//    std::string str;
//    for(std::size_t i = 0; i < ciphers.size(); ++i) {
//        if(!str.empty()) str += ":";
//        str += ciphers[i].name;
//    }
//
//    if( ! SSL_set_cipher_list(_ssl, str.c_str()))
//        throw SSLError("invalid cipher");
//
//    _enabledCiphers = ciphers;
//}

const char* Connection::currentCipher() const
{
    //char desc[512];
    //SSL_CIPHER_description(c, desc, sizeof(desc));
    //bits = SSL_CIPHER_get_bits(c, &usedBits);
    //name = SSL_CIPHER_get_name(c);
    //version = SSL_CIPHER_get_version(c);

    const SSL_CIPHER* c = SSL_get_current_cipher(_ssl);
    const char* name = SSL_CIPHER_get_name(c);
    return name;
}


bool Connection::writeHandshake()
{
    log_trace("Connection::writeHandshake");

    std::streambuf* sb = _ios->rdbuf();
    if( ! sb)
        return false;

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
            throw SslError("BIO_read");

        sb->sputn(buff, n);
        return true;
    }

    return SSL_want_write(_ssl);   
}


bool Connection::readHandshake()
{
    log_trace("Connection::readHandshake");

    std::streambuf* sb = _ios->rdbuf();
    if( ! sb)
        return true;

    while(_ios->rdbuf()->in_avail() > 0)
    {
        const std::streamsize bufsize = 2000;
        char buf[bufsize];

        std::streamsize gsize = std::min( sb->in_avail(), bufsize );
        std::streamsize n = sb->sgetn(buf, gsize);

        const int written = BIO_write(_in, buf, static_cast<int>(n));
        assert(written == n);

        if(written <= 0 || written != n)
            throw SslError("BIO_write");

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


bool Connection::shutdown()
{
    log_debug("Connection::shutdown");

    if( ! _connected )
        return true;

    std::streambuf* sb = _ios->rdbuf();
    if( ! sb)
        return false;

    int state = SSL_get_shutdown(_ssl);
    log_debug("SSL_get_shutdown() = " << state);

    bool shutdownSent = (SSL_SENT_SHUTDOWN & state) == SSL_SENT_SHUTDOWN;

    if( ! shutdownSent )
    {
        // write shutdown notify
        log_debug("write shutdown notify");

        int r = SSL_shutdown(_ssl);
        log_debug("SSL_shutdown() = " << r);

        char buf[1000];
        const int n = BIO_read(_out, buf, sizeof(buf));
        if(n <= 0)
            throw SslError("BIO_read");

        sb->sputn(buf, n);
        log_debug("wrote " << n << " bytes to output");

        if(r == 1)
        {
            log_debug("shutdown complete");
            SSL_clear(_ssl);
            _connected = false;
            return true;
        }
    }

    // read shutdown notify
    log_debug("read shutdown notify");

    BUF_MEM* bm = 0;
    BIO_get_mem_ptr(_in, &bm);

    std::streamsize avail = sb->in_avail();
    std::streamsize refill = std::min(static_cast<std::streamsize>(bm->max - bm->length), avail);
    log_debug("refill " << refill << " bytes");
        
    std::streamsize gcount = sb->sgetn(bm->data + bm->length, refill);
    bm->length += static_cast<int>( gcount );
    log_debug("got " << gcount << " bytes from input stream");

    int r = SSL_shutdown(_ssl);
    log_debug("SSL_shutdown() = " << r);

    if(r == 1)
    {
        log_debug("shutdown complete");
        SSL_clear(_ssl);
        _connected = false;
        return true;
    }

    return false;
}


bool Connection::isShutdown() const
{
    int state = SSL_get_shutdown(_ssl);
    log_debug("SSL_get_shutdown() = " << state);
    
    return state != 0;
}


bool Connection::isClosed() const
{
    return ! _connected;
}


std::streamsize Connection::write(const char* buf, std::size_t n)
{
    std::streambuf* sb = _ios->rdbuf();
    if( ! sb)
        return 0;

    std::streamsize written = SSL_write(_ssl, buf, n);
    log_debug("encrypted " << written << " bytes");

    BUF_MEM* bm = 0;
    BIO_get_mem_ptr(_out, &bm);
    if(bm->length > 0)
    {
        sb->sputn(bm->data, bm->length);
        log_debug("wrote " << bm->length << " bytes to output");
        bm->length = 0;
    }

    return written;
}


std::streamsize Connection::read(char* buf, std::size_t n, std::streamsize maxImport)
{
    std::streambuf* sb = _ios->rdbuf();
    if( ! sb)
        return 0;

    if(maxImport == 0) 
        maxImport = sb->in_avail();

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

        // happens when the peer has send the shutdown alert
        if(sslerr == SSL_ERROR_ZERO_RETURN)
        {
            log_debug("SSL_ERROR_ZERO_RETURN");
            return 0;
        }

        if(sslerr != SSL_ERROR_WANT_READ)
        {
            log_debug("ssl error occured");
            while( sslerr = ERR_get_error() ) 
            {
                log_debug("ERR_error_string = " << ERR_error_string(sslerr, 0));
            }
            
            throw SslError("SSL_read");
        }

        if(maxImport == 0)
            return 0;

        // Refill the BIO with encoded bytes for decoding
        BUF_MEM* bm = 0;
        BIO_get_mem_ptr(_in, &bm);

        if(bm->max == bm->length)
            continue;

        const std::streamsize refill = std::min(static_cast<std::streamsize>(bm->max - bm->length), maxImport);
        log_debug("get " << refill << " bytes from _ios");
        
        std::streamsize gcount = sb->sgetn(bm->data + bm->length, refill);
        if(gcount <= 0)
            return 0;

        bm->length += static_cast<int>( gcount );
        log_debug("Wrote " << gcount << " bytes from _ios to _in BUF_MEM");

        maxImport -= gcount;
    }

    return 0;
}

} // namespace Ssl

} // namespace Pt
