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

#include <iostream>
#include <stdexcept>

#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/Thread.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/IOStream.h>

#include "SSLStreamBufClient.h"
#include "SSLStreamBufServer.h"
#include "SSLStreamBufferServer.h"

///// JUST FOR TESTING /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SSL_CALL_INFO_CLIENT _printInfo("Client", PT_FUNCTION)
#define SSL_CALL_INFO_SERVER _printInfo("Server", PT_FUNCTION)
static const std::string _printInfo(const char* name, const std::string& funcName)
{
    static int count = 0;

    size_t      a = funcName.find_first_of("(");
    std::string f = (a == std::string::npos) ? funcName : funcName.substr(0, a);
    a = f.find_last_of("::");
    if(a != std::string::npos) f = f.substr(a + 1);

    char buff[1024];
    sprintf(buff, " %06d (%s) [%17s] ", count++, name, f.c_str());

    return buff;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Server : public Pt::Connectable {
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslServerContext)
        : _sslContext(sslServerContext), _ssl(0), _loop(loop), _client(0)
        {
            std::cout << "[@@ Server-TCP  ] ################################### Waiting connection from client" << std::endl;
            _server.listen(addr, port);
            _server.connectionPending += Pt::slot(*this, &Server::_onAccept);
            _loop.add(_server);
        }

        ~Server()
        {
            delete _client;
            delete _ssl;
        }

   private:
        void _onAccept(Pt::Net::TcpServer& server)
        {
            std::cout << "[@@ Server-TCP  ] ################################### Accepting client connection" << std::endl;
            _client = new Pt::Net::TcpSocket;
            _client->accept(server);
            _loop.add(*_client);

            std::cout << "[@@ Server-SSL  ] ################################### Initializing SSL" << std::endl;
            _ssl = new Pt::Ssl::SSLStreamBufferServer(*_client, _sslContext, 0);
            _ssl->decryptedDataAvailable += Pt::slot(*this, &Server::_onDecryptedDataAvailable);

            _ssl->accept();
            std::cout << "[@@ Server-SSL  ] ################################### Status = " << _ssl->getStatusString() << std::endl;
        }

        void _onDecryptedDataAvailable(Pt::Ssl::SSLStreamBuffer& ssl)
        {
            std::string cum;
            char        buff[128];

            std::istream is(_ssl);

            int len = 0;
            do {
                len = is.readsome(buff, sizeof(buff));
                cum += std::string(buff, len);
            } while(len > 0);
            std::cout << "[@@ Server-SSL  ] ################################### Receiving message from client: " << cum << std::endl;

            std::cout << "[@@ Server-SSL  ] ################################### Sending message to client" << std::endl;

            std::ostream os(_ssl);
            os << "Hello world from server!";
            os.flush();
        }

    private:
        Pt::Ssl::SSLContext&            _sslContext;
        Pt::Ssl::SSLStreamBufferServer* _ssl;
        Pt::System::EventLoop&          _loop;
        Pt::Net::TcpServer              _server;
        Pt::Net::TcpSocket*             _client;
};

class Client : public Pt::Connectable {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslClientContext)
        : _sslContext(sslClientContext), _ssl(0), _loop(loop), _msgCnt(0)
        {
            std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "Connecting to server" << std::endl;

            _socket.connected += Pt::slot(*this, &Client::onTCPConnect);
            _socket.beginConnect(addr, port);
            _loop.add(_socket);
        }

        ~Client()
        { delete _ssl; }


    private:
        void onTCPConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();
            _ios.attachDevice(socket);

            std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "Starting handshake" << std::endl;

            _ssl = new Pt::Ssl::SSLStreamBufClient(_ios, _sslContext, 0);
            _ssl->startClientHandshake();

            std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "out_avail = " << _ios.buffer().out_avail() << std::endl;

            std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "Begin write" << std::endl;
            _ios.buffer().beginWrite();
            _ios.buffer().outputReady += Pt::slot(*this, &Client::onWriteHandshake);
            _ios.buffer().inputReady  += Pt::slot(*this, &Client::onReadHandshake);
        }

        void onWriteHandshake(Pt::System::StreamBuffer& sb)
        {
            _ios.buffer().endWrite();
            std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "out_avail = " << _ios.buffer().out_avail() << std::endl;

            if( _ssl->writeHandshake() || _ios.buffer().out_avail() > 0 )
            {
                std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "Begin write" << std::endl;
                _ios.buffer().beginWrite();
                return;
            }

            std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "Begin read" << std::endl;
            _ios.buffer().beginRead();
        }

        void onReadHandshake(Pt::System::StreamBuffer& sb)
        {
            _ios.buffer().endRead();
            std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "in_avail = " << _ios.buffer().in_avail() << std::endl;

            if( _ssl->readHandshake() )
            {
                std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "Read more handshake bytes" << std::endl;
                _ios.buffer().beginRead();
                return;
            }

            if( _ssl->connected() )
            {
                std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "successfully connected" << std::endl;
                _ios.buffer().outputReady -= Pt::slot(*this, &Client::onWriteHandshake);
                _ios.buffer().inputReady  -= Pt::slot(*this, &Client::onReadHandshake);
                return;
            }

            _ssl->writeHandshake();

            std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "Begin write" << std::endl;
            _ios.buffer().beginWrite();
        }

        void onSSLConnect(Pt::Ssl::SSLStreamBuffer& ssl)
        {
            std::cerr << "[@@ TestApp @@]" << SSL_CALL_INFO_CLIENT << "Peer CN = " << _ssl->getPeerCN() << std::endl;
        }

    private:
        Pt::Ssl::SSLContext&         _sslContext;
        Pt::Ssl::SSLStreamBufClient* _ssl;
        Pt::System::IOStream         _ios;
        Pt::System::EventLoop&       _loop;
        Pt::Net::TcpSocket           _socket;
        int                          _msgCnt;
};



int main(int argc, char** argv)
{
    try {
        std::cout << "[@@ main() @@@  ] ################################### OpenSSL test progam started" << std::endl;

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 8000;

        Pt::Ssl::SSLContext serverContext("root.pem", "server.pem", "password", 0);
        Pt::Ssl::SSLContext clientContext("root.pem", "client.pem", "password", 0);

        Server server(loop, addr, port, serverContext);
        Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(2000);
        loop.timeout += Pt::slot(loop, &Pt::System::EventLoop::exit);
        loop.run();

        std::cout << "[@@ main() @@@  ] ################################### OpenSSL test progam ended" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[@@ main() @@@  ] ################################### Error: " << ex.what() << std::endl;
    }
    catch(const char* ex)
    {
        std::cerr << "[@@ main() @@@  ] ################################### Error: " << ex << std::endl;
    }
    return 1;
}
