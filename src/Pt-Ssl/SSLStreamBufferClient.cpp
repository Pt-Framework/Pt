/*
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

#include "SSLStreamBufferClient.h"
#include "SSLStreamBufferServer.h"
#include <Pt/SourceInfo.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include "openssl/err.h"

namespace Pt {

namespace Ssl {

SSLStreamBufferClient::SSLStreamBufferClient(System::IODevice& ioDevice, SSLContext& sslContext, const char* sessionID)
: SSLStreamBuffer(ioDevice, sslContext, sessionID)
{ }

SSLStreamBufferClient::SSLStreamBufferClient(System::StreamBuffer& streamBuffer, SSLContext& sslContext, const char* sessionID)
: SSLStreamBuffer(streamBuffer, sslContext, sessionID)
{ }

SSLStreamBufferClient::~SSLStreamBufferClient()
{ }

void SSLStreamBufferClient::connect()
{
    SSL_set_connect_state(_ssl);
    SSL_do_handshake(_ssl);
    _doSSL();
}

void SSLStreamBufferClient::disconnect()
{
    SSL_shutdown(_ssl);
    _doSSL();
}

const std::string SSLStreamBufferClient::getPeerCN() const
{
    if(SSL_get_verify_result(_ssl) != X509_V_OK) return "";

    X509* peer;
    peer = SSL_get_peer_certificate(_ssl);

    char peerCN[256];
    X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peerCN, sizeof(peerCN));
    return peerCN;
}



//
//
//

SSLStreamBuffer2::SSLStreamBuffer2(std::iostream& ios, SSLContext& ctx, const char* sessionID)
: _ios(&ios)
{
    // Create the SSL objects
    _in  = BIO_new( BIO_s_mem() );
    _out = BIO_new (BIO_s_mem() );
    _ssl = SSL_new( ctx._ctx );


    // Connect the BIO
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);
    SSL_set_verify(_ssl, SSL_VERIFY_NONE, NULL);

    // Set session ID
    if(sessionID)
        SSL_set_session_id_context(_ssl, reinterpret_cast<const unsigned char*>(sessionID), strlen(sessionID));

    //
    //setg(_sbBuffer + SB_PUTB, _sbBuffer + SB_PUTB, _sbBuffer + SB_PUTB);
}


SSLStreamBuffer2::~SSLStreamBuffer2()
{
    // FIXME free the BIO's ???
    // The do says that the atatched BIOs will be freed automatically.

    SSL_free(_ssl);
}


void SSLStreamBuffer2::initHandshake()
{
    SSL_set_connect_state(_ssl);

    int ret = SSL_do_handshake(_ssl);
    std::cerr << "[SSLStreamBuffer2::handshake] SSL_do_handshake=" << ret << " "
              << SSL_get_error(_ssl, ret) << std::endl;

    if( ret <= 0 )
    {
        int sslerr = SSL_get_error(_ssl, ret);
        if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
            throw std::runtime_error("SSL_do_handshake failed");
    }

    while(BIO_pending(_out) > 0)
    {
        char buff[100];
        int n = BIO_read(_out, buff, sizeof(buff) );

        if( n <= 0)
            throw std::runtime_error("BIO_read failed");

        std::cerr << "[SSLStreamBuffer2::handshake] BIO_read=" << n << std::endl;
        _ios->write(buff, n);

        int ret = SSL_do_handshake(_ssl);
        std::cerr << "[SSLStreamBuffer2::handshake] SSL_do_handshake=" << ret << " "
                  << SSL_get_error(_ssl, ret) << std::endl;

        if( ret <= 0 )
        {
            int sslerr = SSL_get_error(_ssl, ret);
            if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }
}


void SSLStreamBuffer2::initServerHandshake()
{
    SSL_set_accept_state(_ssl);

    // block until data can be read from the stream
    _ios->rdbuf()->sgetc();

    char buf[600];
    while(true)
    {
        unsigned n = _ios->readsome( buf, sizeof(buf) );
        std::cerr << "[SSLStreamBuffer2::handshake] readsome=" << n << std::endl;

        if(n == 0)
            break;

        while(n)
        {
            int written = BIO_write(_in, buf, n);
            std::cerr << "[SSLStreamBuffer2::handshake] BIO_write=" << written << std::endl;

            if(written <= 0)
                throw std::runtime_error("BIO_write failed");

            n -= written;
            if(n > 0)
            {
                std::memcpy(buf, buf + written, n);
            }

            int ret = SSL_do_handshake(_ssl);

            if( ret <= 0 )
            {
                int sslerr = SSL_get_error(_ssl, ret);
                if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                    throw std::runtime_error("SSL_do_handshake failed");
            }
        }
    }
}


void SSLStreamBuffer2::handshake()
{
    char buf[600];

    // block until data can be read from the stream
    _ios->rdbuf()->sgetc();

    while(true)
    {
        unsigned n = _ios->readsome( buf, sizeof(buf) );
        std::cerr << "[SSLStreamBuffer2::handshake] readsome=" << n << std::endl;

        if(n == 0)
            break;

        while(n)
        {
            int written = BIO_write(_in, buf, n);
            std::cerr << "[SSLStreamBuffer2::handshake] BIO_write=" << written << std::endl;

            if(written <= 0)
                throw std::runtime_error("BIO_write failed");

            n -= written;
            if(n > 0)
            {
                std::memcpy(buf, buf + written, n);
            }

            int ret = SSL_do_handshake(_ssl);

            if( ret <= 0 )
            {
                int sslerr = SSL_get_error(_ssl, ret);
                if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                    throw std::runtime_error("SSL_do_handshake failed");
            }
        }
    }

    while(BIO_pending(_out) > 0)
    {
        char buff[100];
        int n = BIO_read(_out, buff, sizeof(buff) );

        if( n <= 0)
            throw std::runtime_error("BIO_read failed");

        std::cerr << "[SSLStreamBuffer2::handshake] BIO_read=" << n << std::endl;
        _ios->write(buff, n);

        int ret = SSL_do_handshake(_ssl);
        std::cerr << "[SSLStreamBuffer2::handshake] SSL_do_handshake=" << ret << " "
                  << SSL_get_error(_ssl, ret) << std::endl;

        if( ret <= 0 )
        {
            int sslerr = SSL_get_error(_ssl, ret);
            if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }
}


bool SSLStreamBuffer2::connected() const
{
    return SSL_get_state(_ssl) == SSL_ST_OK;
}


void SSLStreamBuffer2::disconnect()
{
    SSL_shutdown(_ssl);

    while(BIO_pending(_out) > 0)
    {
        char buff[100];
        int n = BIO_read(_out, buff, sizeof(buff) );

        if( n <= 0)
            throw std::runtime_error("BIO_read failed");

        std::cerr << "[SSLStreamBuffer2::handshake] BIO_read=" << n << std::endl;
        _ios->write(buff, n);

        int ret = SSL_do_handshake(_ssl);
        std::cerr << "[SSLStreamBuffer2::handshake] SSL_do_handshake=" << ret << " "
                  << SSL_get_error(_ssl, ret) << std::endl;

        if( ret <= 0 )
        {
            int sslerr = SSL_get_error(_ssl, ret);
            if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }
}


std::string SSLStreamBuffer2::getPeerCN() const
{
    if(SSL_get_verify_result(_ssl) != X509_V_OK) return "";

    X509* peer;
    peer = SSL_get_peer_certificate(_ssl);

    char peerCN[256];
    X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peerCN, sizeof(peerCN));
    return peerCN;
}

} // namespace Ssl

} // namespace Pt
