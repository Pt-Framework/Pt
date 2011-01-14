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
#include <cstring>
#include <cassert>

#include "SSLStreamBufClient.h"

namespace Pt {

namespace Ssl {

SSLStreamBufClient::SSLStreamBufClient(std::iostream& ios, SSLContext& ctx, const char* sessionID)
: SSLStreamBuf(ios, ctx, sessionID)
{}

SSLStreamBufClient::~SSLStreamBufClient()
{}

void SSLStreamBufClient::startClientHandshake()
{
    SSL_set_connect_state(_ssl);
    this->writeHandshake();
}

void SSLStreamBufClient::disconnect()
{
    SSL_shutdown(_ssl);

    while(BIO_pending(_out) > 0)
    {
        char buff[100];
        int n = BIO_read(_out, buff, sizeof(buff) );

        if( n <= 0)
            throw std::runtime_error("BIO_read failed");

        std::cerr << "[SSLStreamBufClient::handshake] BIO_read=" << n << std::endl;
        _ios->write(buff, n);

        int ret = SSL_do_handshake(_ssl);
        std::cerr << "[SSLStreamBufClient::handshake] SSL_do_handshake=" << ret << " "
                  << SSL_get_error(_ssl, ret) << std::endl;

        if( ret <= 0 )
        {
            int sslerr = SSL_get_error(_ssl, ret);
            if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }
}

const std::string SSLStreamBufClient::getPeerCN() const
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
