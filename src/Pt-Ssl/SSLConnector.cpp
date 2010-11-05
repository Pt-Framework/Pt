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
#include "SSLConnector.h"

namespace Pt {
namespace Ssl {

SSLConnector::SSLConnector(SSLContext& sslContext, const char* sessionID)
: _in ( BIO_new(BIO_s_mem()) ),
  _out( BIO_new(BIO_s_mem()) ),
  _ssl( SSL_new(sslContext._ctx) )
{
    // Connect the BIO
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);
    SSL_set_verify(_ssl, SSL_VERIFY_NONE, NULL);

    // Star as server by default
    SSL_set_accept_state(_ssl);

    // Set session ID
    if(sessionID) SSL_set_session_id_context(_ssl, reinterpret_cast<const unsigned char*>(sessionID), strlen(sessionID));
}

SSLConnector::~SSLConnector()
{ SSL_free(_ssl); }

const char* SSLConnector::getStatusString() const
{ return SSL_state_string_long(_ssl); }

bool SSLConnector::connectionEstablished() const
{ return SSL_get_state(_ssl) == SSL_ST_OK; }

void SSLConnector::connect()
{
    SSL_set_connect_state(_ssl);
    SSL_do_handshake(_ssl);
}

void SSLConnector::disconnect()
{ SSL_shutdown(_ssl); }

void SSLConnector::reset()
{
    BIO_reset(_in);
    BIO_reset(_out);
    SSL_clear(_ssl);
}

int SSLConnector::write(const char* buff, int len)
{
    int bytesWritten = SSL_write(_ssl, buff, len);

    if(bytesWritten < 0) {
        if(!SSL_want_read(_ssl)) throw "Connection error!";
        return 0;
    }

    return bytesWritten;
}

int SSLConnector::pullData(char* buff, int buffSize) const
{
    const int bytesRead = BIO_read(_out, buff, buffSize);

    if(bytesRead < 0) {
        if(!BIO_should_retry(_out)) throw "Output buffer error!";
        return 0;
    }

    return bytesRead;
}

int SSLConnector::pushData(const char* buff, int len)
{
    const int bytesWritten = BIO_write(_in, buff, len);

    if(!SSL_is_init_finished(_ssl)) {
        SSL_do_handshake(_ssl);
    }
    else {
        char rbuff[8192];
        const int bytesRead = SSL_read(_ssl, rbuff, sizeof(rbuff));
        if(bytesRead > 0) {
            onRecvData(rbuff, bytesRead);
        }
        else if(SSL_get_shutdown(_ssl) & SSL_RECEIVED_SHUTDOWN) {
            SSL_shutdown(_ssl);
        }
    }

    return bytesWritten;
}


} // namespace Pt
} // namespace Ssl
