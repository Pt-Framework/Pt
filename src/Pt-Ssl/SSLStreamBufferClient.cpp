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

    SSL_set_connect_state(_ssl);
    //
    //setg(_sbBuffer + SB_PUTB, _sbBuffer + SB_PUTB, _sbBuffer + SB_PUTB);
}


SSLStreamBuffer2::~SSLStreamBuffer2()
{
    // FIXME free the BIO's ???
    // The do says that the atatched BIOs will be freed automatically.

    SSL_free(_ssl);
}


void SSLStreamBuffer2::handshake()
{
    if( _ios->rdbuf() && _ios->rdbuf()->in_avail() > 0 )
    {
        char buff[600];

        while(true)
        {
            unsigned inavail = _ios->readsome( buff, sizeof(buff) );
            std::cerr << "[SSLStreamBuffer2::handshake] inavail=" << inavail << std::endl;

            if(inavail == 0)
                break;

            int n = BIO_write(_in, buff, inavail);
            std::cerr << "[SSLStreamBuffer2::handshake] BIO_write=" << n << std::endl;

            if(n <= 0)
                throw std::runtime_error("BIO_write failed");

            int ret = SSL_do_handshake(_ssl);
            int err = SSL_get_error(_ssl, ret);

            std::cerr << "[SSLStreamBuffer2::handshake] pending out=" << BIO_pending(_out) << std::endl;

            if( ret == -1 && SSL_ERROR_WANT_READ != SSL_get_error(_ssl, ret) )
            {
                throw std::runtime_error("SSL_do_handshake failed");
            }
        }
    }

    int ret = SSL_do_handshake(_ssl);
    std::cerr << "[SSLStreamBuffer2::handshake] SSL_do_handshake=" << ret << " "
              << SSL_get_error(_ssl, ret) << std::endl;

    if( ret <= 0 && SSL_get_error(_ssl, ret) != SSL_ERROR_WANT_READ )
        throw std::runtime_error("SSL_do_handshake failed");

    int pendingOut = BIO_pending(_out);
    if(pendingOut > 0)
    {
        char buff[100];
        std::cerr << "[SSLStreamBuffer2::handshake] pending out=" << pendingOut << std::endl;
        while(pendingOut > 0)
        {
            int n = BIO_read(_out, buff, sizeof(buff) );

            if( n <= 0)
                throw std::runtime_error("BIO_read failed");

            pendingOut -= n;
            std::cerr << "[SSLStreamBuffer2::handshake] BIO_read=" << n << std::endl;
            _ios->write(buff, n);
        }
    }

    std::cerr << "[SSLStreamBuffer2::handshake] return" << std::endl;
}


void SSLStreamBuffer2::writeHandshake()
{
    //
    // The idea is to write the complete first block of the handshake
    // to the underlying iostream. If it has a StreamBuffer it will never
    // block, but extend as neccessary
    //
    char buff[255];

    int ret = SSL_do_handshake(_ssl);

    assert(ret == -1);
    assert(SSL_get_error(_ssl, ret) == SSL_ERROR_WANT_READ);

    while( true )
    {
        int n = BIO_read(_out, buff, sizeof(buff) );

        if(n == 0)
            break;

        if(n < 0)
        {
            if( BIO_should_retry(_out) )
                break;

            throw std::runtime_error("BIO_read failed");
        }

        std::cerr << "[SSLStreamBuffer2::writeHandskake]"
                  << " wrote " << n << " bytes" << std::endl;

        _ios->write(buff, n);
    }

    //
    // The user has data in the underlying iostream/StreamBuffer now
    // and can use non-blocking i/o to write it to the server and
    // receive the answer. Once data arrives the user can call
    // readHandshake() to let openssl consume the handshake data
    //
}


bool SSLStreamBuffer2::readHandshake()
{
    //
    // After we sent handshake data and the server has answered we
    // feed the bytes received from the server to openssl until
    // either the complete answer was read or all bytes of the
    // partial answer were consumed.
    //

    char buff[255];

    while(true)
    {
        unsigned len = _ios->readsome( buff, sizeof(buff) );
        std::cerr << "[SSLStreamBuffer2::readHandshake] read " << len << " bytes" << std::endl;
        if( len == 0)
            break;

        while(len > 0)
        {
            int n = BIO_write(_in, buff, len);
            std::cerr << "[SSLStreamBuffer2::readHandshake] BIO_write " << n << " bytes" << std::endl;
            len -= n;

            if(n <= 0)
                throw std::runtime_error("BIO_write failed");

            int ret = SSL_do_handshake(_ssl);
            int err = SSL_get_error(_ssl, ret);

            if(ret <= 0 && err != SSL_ERROR_WANT_READ)
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }

    std::cerr << "[SSLStreamBuffer2::readHandshake] " << " SSL finished = " << SSL_is_init_finished(_ssl) << std::endl;
    std::cerr << "[SSLStreamBuffer2::readHandshake] " << " connected = " << connectionEstablished() << std::endl;

    if( connectionEstablished() )
        return false;
    //
    // indicate whether the complete handshake data was written or if we expect
    // more data from the server. If we didn't receive the complete answer we
    // return false, so the user can wait for more data and call this method
    // again. If we return true, the user has to call writeHandshake again...
    // After some iterations we should get in the connected state.
    //
    // if SSL_do_handshake produced data in the out BIO we know we have to
    // go to writing mode
    //
    std::cerr << "[SSLStreamBuffer2::readHandshake] BIO_pending: " << BIO_pending(_out) << std::endl;
    return 0 < BIO_pending(_out);
}


bool SSLStreamBuffer2::connectionEstablished() const
{
    return SSL_get_state(_ssl) == SSL_ST_OK;
}


void SSLStreamBuffer2::disconnect()
{
    SSL_shutdown(_ssl);
    //_doSSL();
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
