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

#include "SSLServer.h"
#include "SSLClient.h"

///// JUST FOR TESTING /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SSL_CALL_INFO_CLIENT Pt::Ssl::SSLStreamBuf::_call_info("@@ Client @@", PT_FUNCTION)
#define SSL_CALL_INFO_SERVER Pt::Ssl::SSLStreamBuf::_call_info("@@ Server @@", PT_FUNCTION)
#define SSL_CALL_INFO_MAIN   Pt::Ssl::SSLStreamBuf::_call_info("@@ main() @@", PT_FUNCTION)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Server : public Pt::Connectable {
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslServerContext)
        : _sslContext(sslServerContext), _ssl(0), _loop(loop), _client(0), _msgCnt(0)
        {
            std::cerr << SSL_CALL_INFO_SERVER << "Waiting connection from client" << std::endl;

            _server.listen(addr, port);
            _server.connectionPending += Pt::slot(*this, &Server::onTCPAccept);

            _loop.add(_server);
        }

        ~Server()
        {
            delete _client;
            delete _ssl;
        }

   private:
        void onTCPAccept(Pt::Net::TcpServer& server)
        {
            std::cerr << SSL_CALL_INFO_SERVER << "Accepting connection from client" << std::endl;
            _client = new Pt::Net::TcpSocket;
            _client->accept(server);

            _loop.add(*_client);
            _ios.attachDevice(*_client);

            std::cerr << SSL_CALL_INFO_SERVER << "Starting handshake" << std::endl;
            _ssl = new Pt::Ssl::SSLServer(_ios, _sslContext, 0);
            _ssl->beginHandshake();
            _ssl->handshakeFinished += Pt::slot(*this, &Server::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::SSLServer& ssl)
        {
            std::cerr << SSL_CALL_INFO_SERVER << "Peer CN = " << _ssl->buffer().getPeerCN() << std::endl;

            _ios.buffer().inputReady += Pt::slot(*this, &Server::onInput);
            _ios.buffer().outputReady += Pt::slot(*this, &Server::onOutput);

            _ios.buffer().beginRead();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            std::cerr << SSL_CALL_INFO_SERVER << "Received raw = " << sb.in_avail() << std::endl;

            _ssl->buffer().import();
            std::cerr << SSL_CALL_INFO_SERVER << "Received decoded = " << _ssl->buffer().in_avail() << std::endl;

            char buf[512];
            unsigned n =_ssl->readsome(buf, 512);
            if(n <= 0) return;

            std::cerr << SSL_CALL_INFO_SERVER << "SERVER RECEIVED: ";
            std::cerr.write(buf, n) << std::endl;

            // NOTE: Production code would call import() again until no further data can be imported.

            // Send reply
            std::cerr << SSL_CALL_INFO_CLIENT << "Sending message to the client ..." << std::endl;
            *_ssl << "Hello world from server!" << std::flush;
            _ios.buffer().beginWrite();
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            _ios.buffer().beginRead();
        }

    private:
        Pt::Ssl::SSLContext&    _sslContext;
        Pt::Ssl::SSLServer*     _ssl;
        Pt::System::IOStream    _ios;
        Pt::System::EventLoop&  _loop;
        Pt::Net::TcpServer      _server;
        Pt::Net::TcpSocket*     _client;
        int                     _msgCnt;
};

class Client : public Pt::Connectable {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslClientContext)
        : _sslContext(sslClientContext), _ssl(0), _loop(loop), _msgCnt(0)
        {
            std::cerr << SSL_CALL_INFO_CLIENT << "Connecting to server" << std::endl;

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

            std::cerr << SSL_CALL_INFO_CLIENT << "Starting handshake" << std::endl;
            _ssl = new Pt::Ssl::SSLClient(_ios, _sslContext, 0);
            _ssl->beginHandshake();
            _ssl->handshakeFinished += Pt::slot(*this, &Client::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::SSLClient& ssl)
        {
            std::cerr << SSL_CALL_INFO_CLIENT << "Peer CN = " << _ssl->buffer().getPeerCN() << std::endl;

            _ios.buffer().inputReady += Pt::slot(*this, &Client::onInput);
            _ios.buffer().outputReady += Pt::slot(*this, &Client::onOutput);

            std::cerr << SSL_CALL_INFO_CLIENT << "Sending message to the server ..." << std::endl;
            *_ssl << "Hello world from client!" << std::flush;
            _ios.buffer().beginWrite();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            std::cerr << SSL_CALL_INFO_CLIENT << "Received raw = " << sb.in_avail() << std::endl;

            _ssl->buffer().import();
            std::cerr << SSL_CALL_INFO_CLIENT << "Received decoded = " << _ssl->buffer().in_avail() << std::endl;

            // TODO: Why this readsome() return 0 but the above in_avail() does not ???
            char buf[512];
            unsigned n =_ssl->readsome(buf, 512);
            if(n <= 0) return;

            std::cerr << SSL_CALL_INFO_CLIENT << "CLIENT RECEIVED: ";
            std::cerr.write(buf, n) << std::endl;

            // NOTE: Production code would call import() again until no further data can be imported.

            std::cerr << SSL_CALL_INFO_CLIENT << "Sending another message to the server ..." << std::endl;
            *_ssl << "Good morning from client!" << std::flush;
            _ios.buffer().beginWrite();
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            _ios.buffer().beginRead();
        }

    private:
        Pt::Ssl::SSLContext&   _sslContext;
        Pt::Ssl::SSLClient*    _ssl;
        Pt::System::IOStream   _ios;
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpSocket     _socket;
        int                    _msgCnt;
};



int main(int argc, char** argv)
{
    try {
        std::cerr << SSL_CALL_INFO_MAIN << "OpenSSL test progam started" << std::endl;

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

        std::cerr << SSL_CALL_INFO_MAIN << "OpenSSL test progam ended" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << SSL_CALL_INFO_MAIN << "Error: " << ex.what() << std::endl;
    }
    catch(const char* ex)
    {
        std::cerr << SSL_CALL_INFO_MAIN << "Error: " << ex << std::endl;
    }
    return 1;
}
