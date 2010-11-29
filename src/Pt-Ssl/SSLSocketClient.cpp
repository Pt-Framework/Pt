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

#include "SSLSocketClient.h"

namespace Pt {
namespace Ssl {

SSLSocketClient::SSLSocketClient(System::EventLoop& loop, const std::string& addr, unsigned short port, SSLContext& sslContext, const char* sessionID)
: SSLConnector(sslContext, sessionID), _loop(loop), _connected(false)
{
    _loop.add(_socket);

    _socket.connected   += Pt::slot(*this, &SSLSocketClient::_onTCPConnect);
    _socket.inputReady  += Pt::slot(*this, &SSLSocketClient::_onTCPInput  );
    _socket.outputReady += Pt::slot(*this, &SSLSocketClient::_onTCPOutput );

    _socket.beginConnect(addr, port);
}

SSLSocketClient::~SSLSocketClient()
{}

void SSLSocketClient::disconnect()
{
    SSLConnector::disconnect();
    _doSSL();
    _loop.exit();
}

int SSLSocketClient::write(const char* buff, int len)
{
    _outBuff += std::string(buff, len);
    _doSSL();
    return len;
}

void SSLSocketClient::_onTCPConnect(Pt::Net::TcpSocket& socket)
{
    _socket.endConnect();
    std::cout << "[CLIENT-TCP] Connected to server" << std::endl;

    std::cout << "[CLIENT-SSL] Status = " << Pt::Ssl::SSLConnector::getStatusString() << std::endl;
    SSLConnector::connect();
    std::cout << "[CLIENT-SSL] Status = " << Pt::Ssl::SSLConnector::getStatusString() << std::endl;

    _doSSL();
}

void SSLSocketClient::_onTCPOutput(Pt::System::IODevice& socket)
{
    const int byteCount = _socket.endWrite();
    std::cout << "[CLIENT-TCP] Wrote " << byteCount << " bytes" << std::endl;

    _socket.beginRead(_tcpbuff, sizeof(_tcpbuff));
}

void SSLSocketClient::_onTCPInput(Pt::System::IODevice& socket)
{
    const int byteCount = _socket.endRead();
    std::cout << "[CLIENT-TCP] Read " << byteCount << " bytes" << std::endl;

    if(byteCount > 0) _inBuff += std::string(_tcpbuff, byteCount);
    _doSSL();
}

void SSLSocketClient::_doSSL()
{
    int byteCount = 0;

    if(_outBuff.length()) {
        byteCount = SSLConnector::write(_outBuff.data(), _outBuff.length());
        if(byteCount > 0) _outBuff.erase(0, byteCount);
    }

    if(!_inBuff.length()) {
        byteCount = SSLConnector::pullData(_sslbuff, sizeof(_sslbuff));
        if(byteCount > 0) _socket.beginWrite(_sslbuff, byteCount);
    }

    while(_inBuff.length()) {
        byteCount = SSLConnector::pushData(_inBuff.data(), _inBuff.length());
        if(byteCount > 0) _inBuff.erase(0, byteCount);
        std::cout << "[CLIENT-SSL] Status = " << Pt::Ssl::SSLConnector::getStatusString() << std::endl;

        byteCount = SSLConnector::pullData(_sslbuff, sizeof(_sslbuff));
        if(byteCount > 0) _socket.beginWrite(_sslbuff, byteCount);
    }

    if(!_connected && SSLConnector::connectionEstablished()) {
        _connected = true;
        connected(*this);

        //SSL_SESSION* sess = SSL_get1_session(_ssl);
        //SSL_set_session(_ssl, sess);
        //SSL_SESSION_free(sess);
    }
}

} // namespace Pt
} // namespace Ssl
