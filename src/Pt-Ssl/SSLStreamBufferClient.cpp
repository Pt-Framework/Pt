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


void SSLStreamBuffer2::writeHandshake()
{
    //
    // The idea is to write the complete first block of the handshake
    // to the underlying iostream. If it has a StreamBuffer it will never
    // block, but extend as neccessary
    //

    int r = SSL_do_handshake(_ssl);
    std::cerr << "handshake: " << r << std::endl;
    if(r < 0)
    {
        long ec = SSL_get_error(_ssl, r);
        std::cerr << "handshake err: " << ec << std::endl;
    }

    int bytesRead = 1;
    char buff[255];

    if( ! BIO_pending(_out) )
        throw "expected pending data for connect";

    while( bytesRead )
    {
        bytesRead = BIO_read(_out, buff, sizeof(buff) );

        if(bytesRead == 0) break;
        if(bytesRead < 0)
        {
            if( ! BIO_should_retry(_out) )
                throw "Output buffer error!";
            else
                std::cerr << "[SSLStreamBuffer2] "
                          << " Output BIO should retry R=" << BIO_should_read(_out)
                          << " W=" << BIO_should_write(_out) << std::endl;

            break;
        }

        std::cerr << "[SSLStreamBuffer2] " << " Pulled " << bytesRead
                  << " bytes from the output BIO" << std::endl;

        _ios->write(buff, bytesRead);
    }

    r = SSL_do_handshake(_ssl);
    std::cerr << "handshake: " << r << std::endl;
    if(r < 0)
    {
        long ec = SSL_get_error(_ssl, r);
        std::cerr << "handshake err: " << ec << std::endl;
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
    std::cerr << "[SSLStreamBuffer2::readHandshake] "
              << " before BIO R=" << BIO_should_read(_in)
              << " W=" << BIO_should_write(_in) << std::endl;
    char buff[255];

    while(true)
    {
        unsigned len = _ios->readsome( buff, sizeof(buff) );
        std::cerr << "[SSLStreamBuffer2::readHandshake] readsome " << len << std::endl;
        if( len == 0)
            break;

        int bytesWritten = 0;

        while(len > 0)
        {
            bytesWritten = BIO_write(_in, buff, len);
            std::cerr << "[SSLStreamBuffer2::readHandshake] written to BIO " << len << std::endl;
            len -= bytesWritten;
            std::cerr << "[SSLStreamBuffer2] "
                      << " Input BIO should retry R=" << BIO_should_read(_in)
                      << " W=" << BIO_should_write(_in) << std::endl;
            if(bytesWritten < 0)
            {
                if( ! BIO_should_retry(_in ))
                    throw "Output buffer error!";
                else
                    std::cerr << "[SSLStreamBuffer2] "
                              << " Input BIO should retry R=" << BIO_should_read(_in)
                              << " W=" << BIO_should_write(_in) << std::endl;
                continue;
            }

            //std::cerr << "flush: " << BIO_flush(_in) << std::endl;

            //
            // ### ADDITION
            //
            std::cerr << "[Client2] " << " SSL status = " << SSL_state_string_long(_ssl) << std::endl;
            if( ! SSL_is_init_finished(_ssl) ) {
                int r = SSL_do_handshake(_ssl);
                std::cerr << "[Client2] re-calling SSL_do_handshake() " << r << std::endl;
                if( r < 0)
                {
                    long ec = SSL_get_error(_ssl, r);
                    std::cerr << "[Client2] SSL_do_handshake() " << ec << " " << SSL_ERROR_WANT_READ << " " << SSL_ERROR_WANT_WRITE << std::endl;
                }
            }
            else {
                // NOTE: This will never be executed because this function 'return 0 == BIO_should_read(_in);'
                //       The fact is, when the handshaking complete the above statement will be 'true'

                std::cerr << "[Client2] SSL_do_handshake() done" << std::endl;
                 if(connectionEstablished()) std::cerr << "##### Connection established!" << std::endl;

                char _readBuff[4096];
                const int bytesRead = SSL_read(_ssl, _readBuff, sizeof(_readBuff));

                if(bytesRead < 0){
                    long lerr = ERR_get_error();
                    if(lerr) {
                        char buf[255];
                        ERR_error_string_n(lerr, buf, sizeof(buf));
                        // PRINT ERROR
                    }
                }
                else if(bytesRead > 0) {
                    // Use data in _readBuff
                }
                else if(SSL_get_shutdown(_ssl) & SSL_RECEIVED_SHUTDOWN) {
                    SSL_shutdown(_ssl);
                }
            }

        }
    }

    std::cerr << "[SSLStreamBuffer2::readHandshake] "
              << " done with BIO R=" << BIO_should_read(_in)
              << " W=" << BIO_should_write(_in) << std::endl;

    std::cerr << "[Client2] " << " SSL finished = " << SSL_is_init_finished(_ssl) << std::endl;
    std::cerr << "[Client2] " << " connected = " << connectionEstablished() << std::endl;

    //
    // indicate whether the complete handshake data was written or if we expect
    // more data from the server. If we didn't receive the complete answer we
    // return false, so the user can wait for more data and call this method
    // again. If we return true, the user has to call writeHandshake again...
    // After some iterations we should get in the connected state.
    //
    return 1 == BIO_should_read(_in);
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
