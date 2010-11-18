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

// Build using: ./jam.sh -q --with-openssl

#include <Pt/Connectable.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <iostream>
#include <stdexcept>

#include "SSLConnector.h"

class Client : public Pt::Connectable, public Pt::Ssl::SSLConnector {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslClientContext)
        : SSLConnector(sslClientContext, 0), _loop(loop)
        {
            _loop.add(_socket);

            _socket.connected += Pt::slot(*this, &Client::onConnect);
            _socket.inputReady += Pt::slot(*this, &Client::onInput);
            _socket.outputReady += Pt::slot(*this, &Client::onOutput);

            _socket.beginConnect(addr, port);
        }

    protected:
        void onConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();
            std::cout << "[CLIENT-TCP] Connected to server" << std::endl;

            std::cout << "[CLIENT-SSL] Status = " << Pt::Ssl::SSLConnector::getStatusString() << std::endl;
            Pt::Ssl::SSLConnector::connect();
            std::cout << "[CLIENT-SSL] Status = " << Pt::Ssl::SSLConnector::getStatusString() << std::endl;

            const int bytesRead = Pt::Ssl::SSLConnector::pullData(_sslbuff, sizeof(_sslbuff));
            _socket.beginWrite(_sslbuff, bytesRead);
        }

        void onOutput(Pt::System::IODevice& socket)
        {
            std::size_t n = _socket.endWrite();
            std::cout << "[CLIENT-TCP] Wrote " << n << " bytes" << std::endl;

            _socket.beginRead(_tcpbuff, sizeof(_tcpbuff));
        }

        void onInput(Pt::System::IODevice& socket)
        {
            std::size_t n = _socket.endRead();
            std::cout << "[CLIENT-TCP] Read " << n << " bytes" << std::endl;

            Pt::Ssl::SSLConnector::pushData(_tcpbuff, n);
            std::cout << "[CLIENT-SSL] Status = " << Pt::Ssl::SSLConnector::getStatusString() << std::endl;

            if(Pt::Ssl::SSLConnector::connectionEstablished()) Pt::Ssl::SSLConnector::write("Hello world from client!", 25);

            const int bytesRead = Pt::Ssl::SSLConnector::pullData(_sslbuff, sizeof(_sslbuff));
            _socket.beginWrite(_sslbuff, bytesRead);
        }

        virtual void onRecvData(const char* buff, int len)
        {
            std::cout << "[CLIENT-SSL] " + std::string(buff, len) << std::endl;
            _loop.exit();
        }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpSocket     _socket;
        char                   _tcpbuff[8192];
        char                   _sslbuff[8192];
};


class Server : public Pt::Connectable, public Pt::Ssl::SSLConnector {
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslServerContext)
        : SSLConnector(sslServerContext, 0), _loop(loop)
        {
            _server.listen(addr, port);
            _server.connectionPending += Pt::slot(*this, &Server::onAccept);
            _loop.add(_server);

            _client.inputReady += Pt::slot(*this, &Server::onInput);
            _client.outputReady += Pt::slot(*this, &Server::onOutput);
            _loop.add(_client);
        }

    protected:
        void onAccept(Pt::Net::TcpServer& server)
        {
            _client.accept(server);
            std::cout << "[SERVER-TCP] Accepting client connection" << std::endl;

            std::cout << "[SERVER-SSL] Status = " << Pt::Ssl::SSLConnector::getStatusString() << std::endl;

            _client.beginRead(_tcpbuff, sizeof(_tcpbuff));
        }

        void onOutput(Pt::System::IODevice& socket)
        {
            std::size_t n = _client.endWrite();
            std::cout << "[SERVER-TCP] Wrote " << n << " bytes" << std::endl;

            _client.beginRead(_tcpbuff, sizeof(_tcpbuff));
        }

        void onInput(Pt::System::IODevice& socket)
        {
            std::size_t n = _client.endRead();
            std::cout << "[SERVER-TCP] Read " << n << " bytes" << std::endl;

            Pt::Ssl::SSLConnector::pushData(_tcpbuff, n);
            std::cout << "[SERVER-SSL] Status = " << Pt::Ssl::SSLConnector::getStatusString() << std::endl;

            if(Pt::Ssl::SSLConnector::connectionEstablished()) Pt::Ssl::SSLConnector::write("Hello world from server!", 25);

            const int bytesRead = Pt::Ssl::SSLConnector::pullData(_sslbuff, sizeof(_sslbuff));
            _client.beginWrite(_sslbuff, bytesRead);
        }

        virtual void onRecvData(const char* buff, int len)
        { std::cout << "[SERVER-SSL] " + std::string(buff, len) << std::endl; }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpServer     _server;
        Pt::Net::TcpSocket     _client;
        char                   _tcpbuff[8192];
        char                   _sslbuff[8192];
};


int main(int argc, char** argv)
{
    try
    {
        std::cout << "[## MAIN ##] OpenSSL test progam started..." << std::endl;

        Pt::Ssl::SSLContext serverContext("root.pem", "server.pem", "password", 0);
        Pt::Ssl::SSLContext clientContext("root.pem", "client.pem", "password", 0);

        Pt::System::MainLoop loop;

        std::string addr("127.0.0.1");
        unsigned short port = 8000;

        Server server(loop, addr, port, serverContext);
        Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(2000);
        loop.run();

        std::cout << "[## MAIN ##] OpenSSL test progam finished..." << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[## MAIN ##] Error: " << ex.what() << std::endl;
    }
    catch(const char* ex)
    {
        std::cerr << "[## MAIN ##] Error: " << ex << std::endl;
    }
    return 1;
}
