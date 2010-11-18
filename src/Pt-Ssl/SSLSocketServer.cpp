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
#include "SSLSocketServer.h"

namespace Pt {
namespace Ssl {

SSLSocketServer::SSLSocketServer(System::EventLoop& loop, const std::string& addr, unsigned short port, SSLContext& sslContext, const char* sessionID)
: SSLConnector(sslContext, sessionID), _loop(loop)
{
    _server.listen(addr, port);
    _server.connectionPending += Pt::slot(*this, &SSLSocketServer::_onTCPAccept);
    _loop.add(_server);

    _client.inputReady  += Pt::slot(*this, &SSLSocketServer::_onTCPInput );
    _client.outputReady += Pt::slot(*this, &SSLSocketServer::_onTCPOutput);
    _loop.add(_client);
}

SSLSocketServer::~SSLSocketServer()
{}

int SSLSocketServer::write(const char* buff, int len)
{
    const int bytesWritten = SSLConnector::write(buff, len);

//    SSLSocketConnector::processMessage(*this, *_client);

    return bytesWritten;
}

void SSLSocketServer::_onTCPAccept(Pt::Net::TcpServer& server)
{
}

void SSLSocketServer::_onTCPOutput(Pt::System::IODevice& socket)
{
}

void SSLSocketServer::_onTCPInput(Pt::System::IODevice& socket)
{
}

void SSLSocketServer::_doSSL()
{
}

} // namespace Pt
} // namespace Ssl
