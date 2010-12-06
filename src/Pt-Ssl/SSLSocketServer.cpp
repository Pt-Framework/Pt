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

#include <iostream>

#include "SSLSocketServer.h"

namespace Pt {
namespace Ssl {

#define SSL_CALL_INFO _getFuncName(PT_FUNCTION)

static const std::string _getFuncName(const std::string& funcName)
{
    size_t      a = funcName.find_first_of("(");
    std::string f = (a == std::string::npos) ? funcName : funcName.substr(0, a);
    a = f.find_last_of("::");
    if(a != std::string::npos) f = f.substr(a + 1);

    char buff[1024];
    sprintf(buff, "[%17s]", f.c_str());

    return buff;
}

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
    _outBuff += std::string(buff, len);
    std::cerr << "[Server-SSL  ] " << SSL_CALL_INFO << " Wrote " << len << " bytes to the output buffer" << std::endl;

    _doSSL();
    return len;
}

void SSLSocketServer::_onTCPAccept(Pt::Net::TcpServer& server)
{
    _client.accept(server);
    std::cout << "[Server-TCP  ] " << SSL_CALL_INFO << "Accepting client connection" << std::endl;

    _client.beginRead(_tcpbuff, sizeof(_tcpbuff));
}

void SSLSocketServer::_onTCPOutput(Pt::System::IODevice& socket)
{
    const int byteCount = _client.endWrite();
    std::cout << "[Server-TCP  ] " << SSL_CALL_INFO << "Wrote " << byteCount << " bytes" << std::endl;

    _client.beginRead(_tcpbuff, sizeof(_tcpbuff));
}

void SSLSocketServer::_onTCPInput(Pt::System::IODevice& socket)
{
    const int byteCount = _client.endRead();
    std::cout << "[Server-TCP  ] " << SSL_CALL_INFO << "Read " << byteCount << " bytes" << std::endl;

    if(byteCount > 0) _inBuff += std::string(_tcpbuff, byteCount);
    _doSSL();
}

void SSLSocketServer::_doSSL()
{
    int byteCount = 0;

    if(_outBuff.length()) {
        byteCount = SSLConnector::write(_outBuff.data(), _outBuff.length());
        if(byteCount > 0) _outBuff.erase(0, byteCount);
    }

    if(!_inBuff.length()) {
        byteCount = SSLConnector::pullData(_sslbuff, sizeof(_sslbuff));
        if(byteCount > 0) _client.beginWrite(_sslbuff, byteCount);
    }

    while(_inBuff.length()) {
        byteCount = SSLConnector::pushData(_inBuff.data(), _inBuff.length());
        if(byteCount > 0) _inBuff.erase(0, byteCount);

        byteCount = SSLConnector::pullData(_sslbuff, sizeof(_sslbuff));
        if(byteCount > 0) _client.beginWrite(_sslbuff, byteCount);
    }

}

} // namespace Pt
} // namespace Ssl
