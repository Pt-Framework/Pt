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

#include <Pt/Ssl/SSLServer.h>

#include <Pt/System/Thread.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/IOStream.h>

///// JUST FOR TESTING /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SSL_CALL_INFO_SERVER Pt::Ssl::SSLContext::_call_info("@@ Server @@", PT_FUNCTION)
#define SSL_CALL_INFO_MAIN   Pt::Ssl::SSLContext::_call_info("@@ main() @@", PT_FUNCTION)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Server : public Pt::Connectable {
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslServerContext)
        : _sslContext(sslServerContext), _ssl(0), _ios(0), _loop(loop), _client(0)
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

            _ios = new Pt::System::IOStream;
            _ios->attachDevice(*_client);

            std::cerr << SSL_CALL_INFO_SERVER << "Starting handshake" << std::endl;
            _ssl = new Pt::Ssl::SSLServer(*_ios, _sslContext, 0);
            _ssl->beginHandshake(true, true);
            _ssl->handshakeFinished += Pt::slot(*this, &Server::onSSLHandshakeFinished);
            _ssl->handshakeFailed += Pt::slot(*this, &Server::onSSLHandshakeFailed);
        }

        void onSSLHandshakeFinished(Pt::Ssl::SSLServer& ssl)
        {
            std::cerr << SSL_CALL_INFO_SERVER << "Peer CN = " << _ssl->buffer().getPeerCN() << std::endl;

            _ios->buffer().inputReady += Pt::slot(*this, &Server::onInput);
            _ios->buffer().outputReady += Pt::slot(*this, &Server::onOutput);

            _ios->buffer().beginRead();
        }

        void onSSLHandshakeFailed(Pt::Ssl::SSLServer& ssl)
        {
            std::cerr << SSL_CALL_INFO_SERVER << "Handshake failed!" << std::endl;

            _loop.remove(*_client);
            delete _client; _client = 0;
            delete _ios; _ios = 0;
            delete _ssl; _ssl = 0;

            std::cerr << std::endl << std::endl;
            std::cerr << SSL_CALL_INFO_SERVER << "Waiting connection from client" << std::endl;
            
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            std::cerr << SSL_CALL_INFO_SERVER << "Received raw = " << sb.in_avail() << std::endl;

            if(_ssl->buffer().import() == -1) {
                std::cerr << SSL_CALL_INFO_SERVER << "*** The stream has been shutdown by the other peer ***" << std::endl;
                _ios->buffer().inputReady -= Pt::slot(*this, &Server::onInput);
                _ios->buffer().outputReady -= Pt::slot(*this, &Server::onOutput);
                return;
            }

            std::cerr << SSL_CALL_INFO_SERVER << "Received decoded = " << _ssl->buffer().in_avail() << std::endl;

            char buf[512];
            unsigned n =_ssl->readsome(buf, 512);
            if(n <= 0) return;

            std::cerr << SSL_CALL_INFO_SERVER << "SERVER RECEIVED: ";
            std::cerr.write(buf, n) << std::endl;

            // Send reply
            std::cerr << SSL_CALL_INFO_SERVER << "Sending message to the client ..." << std::endl;
            *_ssl <<
                "HTTP/1.1 200 OK\r\n"
                "Server: Platinum\r\n"
                "Content-Length: 134\r\n"
                "Connection: close\r\n"
                "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                "<html>\r\n"
                "    <head><title>Test HTTPS Server</title></head>\r\n"
                "    <body>\r\n"
                "        <p>Hello world from server!</p>\r\n"
                "    </body>\r\n"
                "</html>\r\n"
            << std::flush;
            _ios->buffer().beginWrite();
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();

            _loop.remove(*_client);
            delete _client; _client = 0;
            delete _ios; _ios = 0;
            delete _ssl; _ssl = 0;

            std::cerr << std::endl << std::endl;
            std::cerr << SSL_CALL_INFO_SERVER << "Waiting connection from client" << std::endl;
        }

    private:
        Pt::Ssl::SSLContext&    _sslContext;
        Pt::Ssl::SSLServer*     _ssl;
        Pt::System::IOStream*   _ios;
        Pt::System::EventLoop&  _loop;
        Pt::Net::TcpServer      _server;
        Pt::Net::TcpSocket*     _client;
};

int main(int argc, char** argv)
{
    try {
        std::cerr << SSL_CALL_INFO_MAIN << "OpenSSL test progam started" << std::endl;

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 8000;

        Pt::Ssl::SSLContext serverContext("ca.pem", "server.pem", "server.key", "password", 0);

        Server server(loop, addr, port, serverContext);

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
