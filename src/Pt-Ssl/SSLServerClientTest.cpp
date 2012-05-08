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

#include "Utils.h"
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Ssl/Server.h>
#include <Pt/Ssl/Client.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////
log_define(PT_SSL_LOGGER_CATEGORY);
#define PT_SSL_LOG_S(CODE) PT_SSL_LOG_INFO("@@ Server @@", CODE)
#define PT_SSL_LOG_C(CODE) PT_SSL_LOG_INFO("@@ Client @@", CODE)
#define PT_SSL_LOG_M(CODE) PT_SSL_LOG_INFO("@@ main() @@", CODE)
////////////////////////////////////////////////////////////////////////////////////////////////////

class Server : public Pt::Connectable 
{
    public:
        Server(Pt::System::EventLoop& loop, Pt::Ssl::Context& ctx,
               const std::string& addr, unsigned short port)
        : _sslContext(ctx)
        , _ssl(0)
        , _ios(8192, true)
        , _loop(loop)
        , _socket(0)
        {
            PT_SSL_LOG_S("Waiting connection from client");
            _server.listen(addr, port);
            _server.connectionPending() += Pt::slot(*this, &Server::onAccept);
            _server.setActive(_loop);
            _server.beginAccept();
        }

        ~Server()
        {
            delete _socket;
            delete _ssl;
        }

   private:
        void onAccept(Pt::Net::TcpServer& server)
        {
            PT_SSL_LOG_S("Accepting connection from client");
            _socket = new Pt::Net::TcpSocket(server);
            _socket->setActive(_loop);
            _ios.attach(*_socket);

            PT_SSL_LOG_S("Starting handshake");
            _ssl = new Pt::Ssl::Server(_ios, _sslContext, 0);
            _ssl->handshakeFinished += Pt::slot(*this, &Server::onHandshakeFinished);
            _ssl->shutdownFinished += Pt::slot(*this, &Server::onShutdownFinished);
            _ssl->beginHandshake(true, true);
        }

        void onHandshakeFinished(Pt::Ssl::Server& ssl)
        {
            try 
            {
                _ssl->endHandshake();
            }
            catch(...) 
            {
                PT_SSL_LOG_S("*** HANDSHAKE FAILED ***");
                _loop.exit();
                return;
            }

            PT_SSL_LOG_S("peer name = " << _ssl->buffer().peerName());
            PT_SSL_LOG_S("current cipher = \n" << _ssl->buffer().currentCipher().name());

            _ios.buffer().inputReady() += Pt::slot(*this, &Server::onInput);
            _ios.buffer().outputReady() += Pt::slot(*this, &Server::onOutput);
            _ios.buffer().beginRead();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            PT_SSL_LOG_S("Received raw = " << sb.in_avail());

            while(true) 
            {
                std::streamsize result = _ssl->buffer().import();
                PT_SSL_LOG_S("Received decoded = " << _ssl->buffer().in_avail());
                
                if(result == 0)
                    break;

                if(result < 0) 
                {                   
                    PT_SSL_LOG_S("*** The stream has been shutdown by the other peer ***");
                    _ssl->beginShutdown();
                    return;
                }  

                char buf[512];
                while( _ssl->readsome(buf, 512) > 0 ) 
                {
                    std::cerr << "SERVER RECEIVED: " << _ssl->gcount() << std::endl;
                    std::cerr.write(buf, _ssl->gcount() ); 
                }
            }

            // Send reply
            *_ssl << "Hello world from server!";
            for(int i = 0; i < 1024; ++i) 
                *_ssl << "_12345678X";
            *_ssl << "!!!" << std::flush;

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

        void onShutdownFinished(Pt::Ssl::Server& ssl)
        {
            PT_SSL_LOG_S("*** SHUTDOWN FINISHED ***");
            _loop.exit();
        }

    private:
        Pt::Ssl::Context&    _sslContext;
        Pt::Ssl::Server*     _ssl;
        Pt::System::IOStream    _ios;
        Pt::System::EventLoop&  _loop;
        Pt::Net::TcpServer      _server;
        Pt::Net::TcpSocket*     _socket;
};




class Client : public Pt::Connectable {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port,
               Pt::Ssl::Context& sslClientContext)
        : _sslContext(sslClientContext), _ssl(0), _ios(8192, true), _loop(loop), _msgCnt(0)
        {
            PT_SSL_LOG_C("Connecting to server");

            _socket.setActive(_loop);
            _socket.connected() += Pt::slot(*this, &Client::onTCPConnect);
            _socket.beginConnect(addr, port);
        }

        ~Client()
        { delete _ssl; }

    private:
        void onTCPConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();
            _ios.attach(socket);

            PT_SSL_LOG_C("Starting handshake");
            _ssl = new Pt::Ssl::Client(_ios, _sslContext, 0);
            _ssl->beginHandshake(true);

            _ssl->handshakeFinished += Pt::slot(*this, &Client::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::Client& ssl)
        {
            try {
                ssl.endHandshake();
            }
            catch(...) {
                PT_SSL_LOG_C("*** HANDSHAKE FAILED ***");
                _loop.exit();
                return;
            }

            const Pt::Ssl::Session& sess = _ssl->buffer().session();

            PT_SSL_LOG_C("Peer CN = " << _ssl->buffer().peerName());
            PT_SSL_LOG_C("Current cipher = \n" << _ssl->buffer().currentCipher().name());

            _ios.buffer().inputReady() += Pt::slot(*this, &Client::onInput);
            _ios.buffer().outputReady() += Pt::slot(*this, &Client::onOutput);

            std::string lmsg = "Hello world from client!";

            PT_SSL_LOG_C("Sending message to the server ... size = " << lmsg.length());
            *_ssl << lmsg << std::flush;
            _ios.buffer().beginWrite();
        }

        void onShutdownFinished(Pt::Ssl::Client& ssl)
        {
            PT_SSL_LOG_C("*** SHUTDOWN FINISHED ***");
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
                    _ssl->buffer().shutdown();
                    _ios.buffer().inputReady() -= Pt::slot(*this, &Client::onInput);
                    _ios.buffer().outputReady() -= Pt::slot(*this, &Client::onOutput);
                    return;
                }

                if( ! avail )
                    break;

                PT_SSL_LOG_C("Received decoded = " << _ssl->buffer().in_avail());

                while(true) {
                    char buf[512];
                    std::streamsize n =_ssl->readsome(buf, 512);
                    if(n <= 0) break;
                    result += std::string(buf, static_cast<size_t>(n));
                }
            }

            std::cerr << "########################################################### CLIENT RECEIVED: "
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
                _ios.buffer().inputReady() -= Pt::slot(*this, &Client::onInput);
                _ios.buffer().outputReady() -= Pt::slot(*this, &Client::onOutput);

                _ssl->beginShutdown();
                _ssl->shutdownFinished += Pt::slot(*this, &Client::onShutdownFinished);
            }
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            PT_SSL_LOG_C("Sent raw; remaining = " << sb.out_avail());

            _ios.buffer().beginRead();
        }

    private:
        Pt::Ssl::Context&   _sslContext;
        Pt::Ssl::Client*    _ssl;
        Pt::System::IOStream   _ios;
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpSocket     _socket;
        int                    _msgCnt;
};

int main(int argc, char** argv)
{
    try {
        Pt::System::Logger::getTarget("").setLogLevel(Pt::System::Trace);
        PT_SSL_LOG_M("OpenSSL test progam started");

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 6000;

        Pt::Ssl::CertificateList trustedCACert;
        trustedCACert.fromPemFile("ca.pem");

        Pt::Ssl::CertificateList serverCertChain;
        Pt::Ssl::PrivateKey      serverPrivKey("abc123");
        Pt::Ssl::Context         serverContext(0, Pt::Ssl::Context::Default);
        serverCertChain.fromPemFile("server.pem");
        serverPrivKey.fromPemFile("server.key");
        serverContext.setCACertificates(trustedCACert);
        serverContext.setCertificateChain(serverCertChain);
        serverContext.setPrivateKey(serverPrivKey);

        Pt::Ssl::CertificateList clientCertChain;
        Pt::Ssl::PrivateKey      clientPrivKey("");
        Pt::Ssl::Context         clientContext(0, Pt::Ssl::Context::Default);
        clientCertChain.fromPemFile           ("client.pem");
        clientPrivKey  .fromPemFile           ("client.key");
        clientContext  .setCACertificates(trustedCACert);
        clientContext  .setCertificateChain    (clientCertChain);
        clientContext  .setPrivateKey          (clientPrivKey);

        Server server(loop, serverContext, addr, port);
        Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(30000);
        loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
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
