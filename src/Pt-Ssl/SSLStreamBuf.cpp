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

#include "SSLStreamBufServer.h"
#include "SSLStreamBufClient.h"

namespace Pt {
namespace Ssl {

SSLStreamBuf::SSLStreamBuf(std::iostream& ios, SSLContext& ctx, const char* sessionID)
: _in (0),
  _out(0),
  _ssl(0),
  _ios(&ios)
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

bool SSLStreamBuf::writeHandshake()
{
    if( ! SSL_want_read(_ssl) )
    {
        int ret = SSL_do_handshake(_ssl);
        std::cerr << "[SSLStreamBufClient::doHandshake] SSL_do_handshake=" << ret << std::endl;

        if( ret <= 0 )
        {
            int sslerr = SSL_get_error(_ssl, ret);
            if( sslerr == SSL_ERROR_WANT_READ )
            {
                std::cerr << "[SSLStreamBufClient::doHandshake] wants read SSL_ERROR_WANT_READ"  << std::endl;
            }
            else if ( sslerr == SSL_ERROR_WANT_WRITE)
            {
                std::cerr << "[SSLStreamBufClient::doHandshake] wants write SSL_ERROR_WANT_WRITE"  << std::endl;
            }
            else
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }

    if( BIO_pending(_out) )
    {
        char buff[1000]; // will be the steambufs buffer area later
        int n = BIO_read(_out, buff, sizeof(buff) );

        if( n <= 0)
            throw std::runtime_error("BIO_read failed");

        std::cerr << "[SSLStreamBufClient::handshake] BIO_read=" << n << std::endl;
        _ios->write(buff, n);

        return true;
    }

    return false;
}

bool SSLStreamBuf::readHandshake()
{
    char buf[1000]; // will be the steambufs buffer area later

    // block until data can be read from the stream
    _ios->rdbuf()->sgetc();

    while(true)
    {
        unsigned n = _ios->readsome( buf, sizeof(buf) );
        std::cerr << "[SSLStreamBufClient::handshake] readsome=" << n << std::endl;

        if(n == 0)
            break;

        while(n)
        {
            int written = BIO_write(_in, buf, n);
            std::cerr << "[SSLStreamBufClient::handshake] BIO_write=" << written << std::endl;

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

    if( BIO_pending(_out) > 0 || SSL_get_state(_ssl) == SSL_ST_OK )
        return false;

    return true;
}

} // namespace Pt
} // namespace Ssl
