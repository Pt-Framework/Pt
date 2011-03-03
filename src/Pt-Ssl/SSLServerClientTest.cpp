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

#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>

#include <Pt/Ssl/SSLServer.h>
#include <Pt/Ssl/SSLClient.h>

#include <Pt/System/Thread.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/IOStream.h>

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
log_define("Pt.SSL.Logger");
#define PT_SSL_LOG_S(CODE) log_info(Pt::Ssl::SSLContext::pt_ssl_gen_call_info("@@ Server @@", PT_FUNCTION) << CODE << Pt::System::endlog)
#define PT_SSL_LOG_C(CODE) log_info(Pt::Ssl::SSLContext::pt_ssl_gen_call_info("@@ Client @@", PT_FUNCTION) << CODE << Pt::System::endlog)
#define PT_SSL_LOG_M(CODE) log_info(Pt::Ssl::SSLContext::pt_ssl_gen_call_info("@@ main() @@", PT_FUNCTION) << CODE << Pt::System::endlog)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Server : public Pt::Connectable {
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslServerContext)
        : _sslContext(sslServerContext), _ssl(0), _ios(8192, true), _loop(loop), _client(0)
        {
            PT_SSL_LOG_S("Waiting connection from client");

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
            PT_SSL_LOG_S("Accepting connection from client");
            _client = new Pt::Net::TcpSocket;
            _client->accept(server);

            _loop.add(*_client);
            _ios.attachDevice(*_client);

            PT_SSL_LOG_S("Starting handshake");
            _ssl = new Pt::Ssl::SSLServer(_ios, _sslContext, 0);
            _ssl->beginHandshake(true, true);

            _ssl->handshakeFinished += Pt::slot(*this, &Server::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::SSLServer& ssl)
        {
            try {
                ssl.endHandshake();
            }
            catch(...) {
                PT_SSL_LOG_S("*** HANDSHAKE FAILED ***");
                _loop.exit();
                return;
            }
            
            PT_SSL_LOG_S("Peer CN = " << _ssl->buffer().getPeerCN());

            _ios.buffer().inputReady += Pt::slot(*this, &Server::onInput);
            _ios.buffer().outputReady += Pt::slot(*this, &Server::onOutput);

            _ios.buffer().beginRead();
        }
        
        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            PT_SSL_LOG_S("Received raw = " << sb.in_avail());

            std::string msg;
            while(true)
            {
                const int importResult = _ssl->buffer().import();
                if(importResult == -1) {
                    PT_SSL_LOG_S("*** The stream has been shutdown by the other peer ***");
                    _ios.buffer().inputReady -= Pt::slot(*this, &Server::onInput);
                    _ios.buffer().outputReady -= Pt::slot(*this, &Server::onOutput);
                    return;
                }
                if( ! importResult )
                    break;

                PT_SSL_LOG_S("Received decoded = " << _ssl->buffer().in_avail());

                while(true) {
                    char buf[512];
                    unsigned n =_ssl->readsome(buf, 512);
                    if(n <= 0) break;
                    msg += std::string(buf, n);
                }
            }

            std::cerr << "############################################################################################# SERVER RECEIVED: "
                      << std::endl << msg << std::endl;

            // Send reply
            std::string lmsg = "Hello world from server!";
            for(int i = 0; i < 1024; ++i) lmsg += "_12345678X";
            lmsg += "!!!";

            *_ssl << lmsg << std::flush;

            PT_SSL_LOG_S("Sending message to the client ... out_avail = " << _ios.buffer().out_avail());
            _ios.buffer().beginWrite();
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            PT_SSL_LOG_S("Sent raw; remaining = " << sb.out_avail());

            if( sb.out_avail() > 0 )
            {
                sb.beginWrite();
                return;
            }

            _ios.buffer().beginRead();
        }

    private:
        Pt::Ssl::SSLContext&    _sslContext;
        Pt::Ssl::SSLServer*     _ssl;
        Pt::System::IOStream    _ios;
        Pt::System::EventLoop&  _loop;
        Pt::Net::TcpServer      _server;
        Pt::Net::TcpSocket*     _client;
};

class Client : public Pt::Connectable {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslClientContext)
        : _sslContext(sslClientContext), _ssl(0), _ios(8192, true), _loop(loop), _msgCnt(0)
        {
            PT_SSL_LOG_C("Connecting to server");

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

            PT_SSL_LOG_C("Starting handshake");
            _ssl = new Pt::Ssl::SSLClient(_ios, _sslContext, 0);
            _ssl->beginHandshake(true);

            _ssl->handshakeFinished += Pt::slot(*this, &Client::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::SSLClient& ssl)
        {
            try {
                ssl.endHandshake();
            }
            catch(...) {
                PT_SSL_LOG_C("*** HANDSHAKE FAILED ***");
                _loop.exit();
                return;
            }

            PT_SSL_LOG_C("Peer CN = " << _ssl->buffer().getPeerCN());

            _ios.buffer().inputReady += Pt::slot(*this, &Client::onInput);
            _ios.buffer().outputReady += Pt::slot(*this, &Client::onOutput);

            std::string lmsg = "Hello world from client!";

            PT_SSL_LOG_C("Sending message to the server ... size = " << lmsg.length());
            *_ssl << lmsg << std::flush;
            _ios.buffer().beginWrite();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            PT_SSL_LOG_C("Received raw = " << sb.in_avail());

            std::string result;
            while(true)
            {
                std::streamsize avail = _ssl->buffer().import();

                if(avail == -1) {
                    PT_SSL_LOG_C("*** The stream has been shutdown by the other peer ***");
                    _ios.buffer().inputReady -= Pt::slot(*this, &Client::onInput);
                    _ios.buffer().outputReady -= Pt::slot(*this, &Client::onOutput);
                    return;
                }

                if( ! avail )
                    break;

                PT_SSL_LOG_C("Received decoded = " << _ssl->buffer().in_avail());

                while(true) {
                    char buf[512];
                    unsigned n =_ssl->readsome(buf, 512);
                    if(n <= 0) break;
                    result += std::string(buf, n);
                }
            }

            std::cerr << "############################################################################################# CLIENT RECEIVED: "
                      << std::endl << result << std::endl;

            if( result.find("!!!") == std::string::npos )
            {
                PT_SSL_LOG_C("Message not complete ");
                _ios.buffer().beginRead();
                return;
            }

            // Send more messages
            if(_msgCnt < 2) {
                ++_msgCnt;
                PT_SSL_LOG_C("Sending another message to the server ...");
                *_ssl << "Good morning from client!" << std::flush;
                _ios.buffer().beginWrite();
            }
            // Shutdown
            else {
                PT_SSL_LOG_C("Shutting down the stream");
                _ios.buffer().inputReady -= Pt::slot(*this, &Client::onInput);
                _ios.buffer().outputReady -= Pt::slot(*this, &Client::onOutput);
                _ssl->buffer().shutdown();
            }
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            PT_SSL_LOG_C("Sent raw; remaining = " << sb.out_avail());

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
        PT_SSL_LOG_M("OpenSSL test progam started");

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 8000;

        Pt::Ssl::SSLContext serverContext("ca.pem", "server.pem", "server.key", "password", 0);
        Pt::Ssl::SSLContext clientContext("ca.pem", "client.pem", "client.key", "password", 0);

        Server server(loop, addr, port, serverContext);
        Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(2000);
        loop.timeout += Pt::slot(loop, &Pt::System::EventLoop::exit);
        loop.run();

        PT_SSL_LOG_M("OpenSSL test progam ended");
        return 0;
    }
    catch(const std::exception& ex)
    {
        PT_SSL_LOG_M("Error: " << ex.what());
    }
    catch(const char* ex)
    {
        PT_SSL_LOG_M("Error: " << ex);
    }
    return 1;
}
