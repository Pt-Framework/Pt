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

#include "SSLStreamBufferClient.h"
#include "SSLStreamBufferServer.h"

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
            std::cout << "[@@ Client-TCP  ] ################################### Connecting to server" << std::endl;
            _socket.connected += Pt::slot(*this, &Client::_onTCPConnect);
            _socket.beginConnect(addr, port);
            _loop.add(_socket);
        }

        ~Client()
        { delete _ssl; }

        void sendMessage(const char* msg)
        {
            if(_msgCnt >= 3) {
                _loop.exit();
                return;
            }

            std::cout << "[@@ Client-SSL  ] ################################### Sending message to server (" << _msgCnt << ")" << std::endl;

            std::ostream os(_ssl);
            os << msg;
            os.flush();

            ++_msgCnt;
        }

    private:
        void _onTCPConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();

            std::cout << "[@@ Client-SSL  ] ################################### Initializing SSL" << std::endl;
            _ssl = new Pt::Ssl::SSLStreamBufferClient(_socket, _sslContext, 0);
            _ssl->connected              += Pt::slot(*this, &Client::_onSSLConnect            );
            _ssl->decryptedDataAvailable += Pt::slot(*this, &Client::_onDecryptedDataAvailable);

            _ssl->connect();
            std::cout << "[@@ Client-SSL  ] ################################### Status = " << _ssl->getStatusString() << std::endl;
        }

        void _onSSLConnect(Pt::Ssl::SSLStreamBuffer& ssl)
        {
            std::cout << "[@@ Client-SSL  ] ################################### Peer CN = " + _ssl->getPeerCN() << std::endl;

            sendMessage("Hello world from client!");
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
            std::cout << "[@@ Client-SSL  ] ################################### Receiving message from server: " << cum << std::endl;

            sendMessage("Hello world from client!");
        }

    private:
        Pt::Ssl::SSLContext&            _sslContext;
        Pt::Ssl::SSLStreamBufferClient* _ssl;
        Pt::System::EventLoop&          _loop;
        Pt::Net::TcpSocket              _socket;
        int                             _msgCnt;
};



class Client2 : public Pt::Connectable {
    public:
        Client2(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port,
                Pt::Ssl::SSLContext& sslClientContext)
        : _sslContext(sslClientContext), _ssl(0), _loop(loop), _msgCnt(0)
        {
            std::cout << "[@@ Client2 ] ### Connecting to server" << std::endl;

            _socket.connected += Pt::slot(*this, &Client2::onTCPConnect);
            _socket.beginConnect(addr, port);
            _loop.add(_socket);
        }

        ~Client2()
        { delete _ssl; }


    private:
        void onTCPConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();
            _ios.attachDevice(socket);

            std::cout << "[@@ Client2::onTCPConnect ] ### starting handshake" << std::endl;

            _ssl = new Pt::Ssl::SSLStreamBuffer2(_ios, _sslContext, 0);
            _ssl->handshake();

            std::cout << "[@@ Client2::onTCPConnect] ### out_avail:"
                      << _ios.buffer().out_avail() << std::endl;

            std::cout << "[@@ Client2::onTCPConnect] ### beginWrite" << std::endl;
            _ios.buffer().beginWrite();
            _ios.buffer().outputReady += Pt::slot(*this, &Client2::onWriteHandshake);
            _ios.buffer().inputReady += Pt::slot(*this, &Client2::onReadHandshake);
        }

        void onWriteHandshake(Pt::System::StreamBuffer& sb)
        {
            _ios.buffer().endWrite();

            std::cout << "[@@ Client2::onWriteHandshake] ### out_avail:"
                      << _ios.buffer().out_avail() << std::endl;

            this->handshake();
        }

        void onReadHandshake(Pt::System::StreamBuffer& sb)
        {
            _ios.buffer().endRead();

            std::cout << "[@@ Client2::onReadHandshake] ### in_avail:"
                      << _ios.buffer().in_avail() << std::endl;

            this->handshake();
        }

        void handshake()
        {
            _ssl->handshake();

            // we have read all bytes, need to write now
            if( _ios.buffer().out_avail() )
            {
                std::cout << "[@@ Client2::handshake] ### beginWrite" << std::endl;
                _ios.buffer().beginWrite();
                return;
            }

            std::cout << "[@@ Client2::handshake ] ### connected " <<  _ssl->connectionEstablished() <<  std::endl;
            if( _ssl->connectionEstablished() )
            {
                std::cout << "[@@ Client2::handshake ] ### SUCCESS - CONNECTED" << std::endl;
                return;
            }

            // more to read
            std::cout << "[@@ Client2::handshake ] ### read more handshake bytes" << std::endl;
            _ios.buffer().beginRead();
        }

        void onSSLConnect(Pt::Ssl::SSLStreamBuffer& ssl)
        {
            std::cout << "[@@ Client-SSL  ] ################################### Peer CN = " + _ssl->getPeerCN() << std::endl;
        }

    private:
        Pt::Ssl::SSLContext&       _sslContext;
        Pt::Ssl::SSLStreamBuffer2* _ssl;
        Pt::System::IOStream       _ios;
        Pt::System::EventLoop&     _loop;
        Pt::Net::TcpSocket         _socket;
        int                        _msgCnt;
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
        Client2 client(loop, addr, port, clientContext);

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
