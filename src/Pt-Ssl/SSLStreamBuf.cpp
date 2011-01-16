/*
 * Copyright (C) 2010-2010 by Marc Boris Duerner
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

#include <Pt/SourceInfo.h>
#include <iostream>

#include "SSLStreamBuf.h"

namespace Pt {
namespace Ssl {

///// JUST FOR TESTING /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SSL_CALL_INFO SSLStreamBuf::_call_info("SSLStreamBuf", PT_FUNCTION)
const std::string SSLStreamBuf::_call_info(const char* className, const std::string& funcName)
{
    static int count = 0;

    size_t      a = funcName.find_first_of("(");
    std::string f = (a == std::string::npos) ? funcName : funcName.substr(0, a);
    a = f.find_last_of("::");
    if(a != std::string::npos) f = f.substr(a + 1);
    a = f.find_last_of(" ");
    if(a != std::string::npos) f = f.substr(a + 1);

    char buff[1024];
    sprintf(buff, "[%s] %06d [%22s] ", className, count++, f.c_str());

    return buff;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SSLStreamBuf::SSLStreamBuf(std::iostream& ios, SSLContext& ctx, const char* sessionID, size_t bufferSize)
: _in (0),
  _out(0),
  _ssl(0),
  _ios(&ios),
  _ibufferSize(bufferSize+4),
  _ibuffer(0),
  _obufferSize(bufferSize),
  _obuffer(0),
  _pbmax(4)
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
}

SSLStreamBuf::~SSLStreamBuf()
{ SSL_free(_ssl); }

bool SSLStreamBuf::connected() const
{ return SSL_get_state(_ssl) == SSL_ST_OK; }

const char* SSLStreamBuf::getStatusString() const
{ return SSL_state_string_long(_ssl); }

const std::string SSLStreamBuf::getPeerCN() const
{
    if(SSL_get_verify_result(_ssl) != X509_V_OK) return "";

    X509* peer = SSL_get_peer_certificate(_ssl);
    if(!peer) return "";

    char peerCN[256];
    int  ret = X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peerCN, sizeof(peerCN));
    return (ret > 0) ? peerCN : "";
}

void SSLStreamBuf::disconnect()
{
    SSL_shutdown(_ssl);

    while(BIO_pending(_out) > 0)
    {
        char buff[100];
        const int n = BIO_read(_out, buff, sizeof(buff) );
        std::cerr << SSL_CALL_INFO << "BIO_read = " << n << std::endl;

        if(n <= 0)
            throw std::runtime_error("BIO_read failed");

        _ios->write(buff, n);

        const int ret = SSL_do_handshake(_ssl);
        std::cerr << SSL_CALL_INFO << "SSL_do_handshake = " << ret << std::endl;

        if(ret <= 0)
        {
            const int sslerr = SSL_get_error(_ssl, ret);
            if(sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }
}

void SSLStreamBuf::reset()
{
    BIO_reset(_in);
    BIO_reset(_out);
    SSL_clear(_ssl);
}

void SSLStreamBuf::startServerHandshake()
{
    SSL_set_accept_state(_ssl);
}

void SSLStreamBuf::startClientHandshake()
{
    SSL_set_connect_state(_ssl);
    this->writeHandshake();
}

bool SSLStreamBuf::writeHandshake()
{
    std::cerr << SSL_CALL_INFO << "getStatusString = " << getStatusString() << std::endl;

    if(!SSL_want_read(_ssl))
    {
        int ret = SSL_do_handshake(_ssl);
        std::cerr << SSL_CALL_INFO << "SSL_do_handshake = " << ret << std::endl;

        if(ret <= 0)
        {
            int sslerr = SSL_get_error(_ssl, ret);
            if( sslerr == SSL_ERROR_WANT_READ )
                std::cerr << SSL_CALL_INFO << "SSL_ERROR_WANT_READ" << std::endl;
            else if ( sslerr == SSL_ERROR_WANT_WRITE)
                std::cerr << SSL_CALL_INFO << "SSL_ERROR_WANT_WRITE" << std::endl;
            else
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }

    if(BIO_pending(_out))
    {
        char buff[1000]; // Will be the steambufs buffer area later
        const int n = BIO_read(_out, buff, sizeof(buff));
        std::cerr << SSL_CALL_INFO << "BIO_read = " << n << std::endl;

        if(n <= 0)
            throw std::runtime_error("BIO_read failed");

        _ios->write(buff, n);

        return true;
    }

    return false;
}

bool SSLStreamBuf::readHandshake()
{
    std::cerr << SSL_CALL_INFO << "getStatusString = " << getStatusString() << std::endl;

    char buf[1000]; // Will be the steambufs buffer area later

    // Block until data can be read from the stream
    _ios->rdbuf()->sgetc();

    while(true)
    {
        unsigned n = _ios->readsome(buf, sizeof(buf));
        std::cerr << SSL_CALL_INFO << "readsome = " << n << std::endl;

        if(n == 0)
            break;

        while(n)
        {
            const int written = BIO_write(_in, buf, n);
            std::cerr << SSL_CALL_INFO << "BIO_write = " << written << std::endl;

            if(written <= 0)
                throw std::runtime_error("BIO_write failed");

            n -= written;
            if(n > 0)
                std::memcpy(buf, buf + written, n);

            int ret = SSL_do_handshake(_ssl);
            if( ret <= 0 )
            {
                int sslerr = SSL_get_error(_ssl, ret);
                if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                    throw std::runtime_error("SSL_do_handshake failed");
            }
        }
    }

    if( BIO_pending(_out) > 0 || SSL_get_state(_ssl) == SSL_ST_OK )
        return false;

    return true;
}

int SSLStreamBuf::sync()
{ return 0; }

SSLStreamBuf::int_type SSLStreamBuf::underflow()
{ return 0; }

SSLStreamBuf::int_type SSLStreamBuf::overflow(int_type ch)
{
    //
    // We are being called when _obuffer, the output buffer area of the
    // i/o stream is full, or needs to be flushed. In case of a flush,
    // the eof character is passed to overflow(). When SSLStreamBuf is
    // constructed no output buffer area exists, therefore when overflow
    // is called for the first time, we need to set it up.
    //

    if( ! _ios )
        return traits_type::eof();

    if( ! _obuffer )
    {
        _obuffer = new char[_obufferSize];
        this->setp(_obuffer, _obuffer + _obufferSize);
    }
    else if (traits_type::eq_int_type( ch, traits_type::eof() ) )
    {
        // normal blocking overflow case
        size_t avail = this->pptr() - _obuffer;

        // TODO: bytes in _obuffer are not encrypted we need to do that
        //       before we write them to the underlying i/o stream

        // feed _obuffer to openssl
        int written = SSL_write(_ssl, _obuffer, avail);

        // - move leftover in _obuffer to the front
        size_t leftover = avail - written;
        if(leftover > 0)
        {
            traits_type::move(_obuffer, _obuffer + written, leftover);
        }
        this->setp(_obuffer, _obuffer + _obufferSize);
        this->pbump( leftover );

        // write encoded bytes to _ios
        while(true)
        {
            char buf[255];

            int n = BIO_read( _out, buf, sizeof(buf) );
            if(n < 0)
                break;

            _ios->write(buf, n);
        }
    }

    // if the overflow char is not EOF, so put it in buffer
    if( traits_type::eq_int_type(ch, traits_type::eof()) ==  false )
    {
        *(this->pptr()) = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}

} // namespace Pt
} // namespace Ssl
