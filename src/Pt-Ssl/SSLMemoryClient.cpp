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
#include "SSLMemoryClient.h"

namespace Pt {
namespace Ssl {

SSLMemoryClient::SSLMemoryClient(SSLContext& sslContext, const char* sessionID)
: SSLMemoryConnector(sslContext, sessionID), _server(0)
{}

SSLMemoryClient::~SSLMemoryClient()
{}

void SSLMemoryClient::connect(SSLMemoryServer& server)
{
    // Connect
    SSLConnector::connect();

    // Ensure that the client and server reference each other
    server._client = this;
    _server = &server;

    // Perform message passing until the connection is established and then pass possible remaining messages
    while(!connectionEstablished()) {
        SSLMemoryConnector::processMessage(*this,    *_server);
        SSLMemoryConnector::processMessage(*_server, *this   );
    }
    SSLMemoryConnector::processMessage(*this,    *_server);
    SSLMemoryConnector::processMessage(*_server, *this   );
}

void SSLMemoryClient::disconnect()
{
    // Disconnect
    SSLConnector::disconnect();

    // Perform message passing once
    SSLMemoryConnector::processMessage(*this,    *_server);
    SSLMemoryConnector::processMessage(*_server, *this   );

    // Ensure that the client and server no longer reference each other
    _server->_client = 0;
    _server = 0;
}

int SSLMemoryClient::write(const char* buff, int len)
{
    const int bytesWritten = SSLConnector::write(buff, len);

    SSLMemoryConnector::processMessage(*this, *_server);

    return bytesWritten;
}


} // namespace Pt
} // namespace Ssl
