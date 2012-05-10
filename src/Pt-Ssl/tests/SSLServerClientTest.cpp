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

#include "PemData.h"
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Ssl/Server.h>
#include <Pt/Ssl/Client.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>

log_define("Pt.Ssl.ServerClientTest")

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
            log_debug("server waiting for connect");
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
            log_debug("server accepting connection");
            _socket = new Pt::Net::TcpSocket(server);
            _socket->setActive(_loop);
            _ios.attach(*_socket);

            log_debug("server starts handshake");
            _ssl = new Pt::Ssl::Server(_sslContext, _ios, 0);
            _ssl->handshakeFinished() += Pt::slot(*this, &Server::onHandshakeFinished);
            _ssl->shutdownFinished() += Pt::slot(*this, &Server::onShutdownFinished);
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
                log_debug("server *** HANDSHAKE FAILED ***");
                _loop.exit();
                return;
            }

            log_debug("server peer name = " << _ssl->buffer().peerName());
            log_debug("server current cipher = \n" << _ssl->buffer().currentCipher().name());

            _ios.buffer().inputReady() += Pt::slot(*this, &Server::onInput);
            _ios.buffer().outputReady() += Pt::slot(*this, &Server::onOutput);
            _ios.buffer().beginRead();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            log_debug("server received raw = " << sb.in_avail());

            while(true) 
            {
                std::streamsize result = _ssl->buffer().import();
                log_debug("server received decoded = " << _ssl->buffer().in_avail());
                
                if(result == 0)
                    break;

                if(result < 0) 
                {                   
                    log_debug("server *** The stream has been shutdown by the other peer ***");
                    _ssl->beginShutdown();
                    return;
                }  

                char buf[512];
                while( _ssl->readsome(buf, 512) > 0 ) 
                {
                    log_debug("server received: " << _ssl->gcount());
                    std::cerr.write(buf, _ssl->gcount() ); 
                }
            }

            // Send reply
            *_ssl << "Hello world from server!";
            for(int i = 0; i < 1024; ++i) 
                *_ssl << "_12345678X";
            *_ssl << "!!!" << std::flush;

            log_debug("server sends message to the client ... out_avail = " << _ios.buffer().out_avail());
            _ios.buffer().beginWrite();
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            log_debug("server sent raw; remaining = " << sb.out_avail());

            if( sb.out_avail() > 0 )
            {
                sb.beginWrite();
                return;
            }

            _ios.buffer().beginRead();
        }

        void onShutdownFinished(Pt::Ssl::Server& ssl)
        {
            _ssl->endShutdown();
            log_debug("server *** SHUTDOWN FINISHED ***");
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
            log_debug("client connecting");

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

            log_debug("client starting handshake");
            _ssl = new Pt::Ssl::Client(_sslContext, _ios, 0);
            _ssl->beginHandshake(true);

            _ssl->handshakeFinished() += Pt::slot(*this, &Client::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::Client& ssl)
        {
            try {
                ssl.endHandshake();
            }
            catch(...) {
                log_debug("client *** HANDSHAKE FAILED ***");
                _loop.exit();
                return;
            }

            const Pt::Ssl::Session& sess = _ssl->buffer().session();

            log_debug("client Peer CN = " << _ssl->buffer().peerName());
            log_debug("client Current cipher = \n" << _ssl->buffer().currentCipher().name());

            _ios.buffer().inputReady() += Pt::slot(*this, &Client::onInput);
            _ios.buffer().outputReady() += Pt::slot(*this, &Client::onOutput);

            std::string lmsg = "Hello world from client!";

            log_debug("client sending message... size = " << lmsg.length());
            *_ssl << lmsg << std::flush;
            _ios.buffer().beginWrite();
        }

        void onShutdownFinished(Pt::Ssl::Client& ssl)
        {
            _ssl->endShutdown();
            log_debug("client *** SHUTDOWN FINISHED ***");
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            log_debug("client received raw = " << sb.in_avail());

            std::string result;
            while(true)
            {
                std::streamsize avail = _ssl->buffer().import();

                if(avail == -1) {
                    log_debug("client *** The stream has been shutdown by the other peer ***");
                    _ssl->buffer().shutdown();
                    _ios.buffer().inputReady() -= Pt::slot(*this, &Client::onInput);
                    _ios.buffer().outputReady() -= Pt::slot(*this, &Client::onOutput);
                    return;
                }

                if( ! avail )
                    break;

                log_debug("client received decoded = " << _ssl->buffer().in_avail());

                while(true) {
                    char buf[512];
                    std::streamsize n =_ssl->readsome(buf, 512);
                    if(n <= 0) break;
                    result += std::string(buf, static_cast<size_t>(n));
                }
            }

            log_debug("client received: " << result);

            if( result.find("!!!") == std::string::npos )
            {
                log_debug("client message not complete ");
                _ios.buffer().beginRead();
                return;
            }

            // Send more messages
            if(_msgCnt < 2) {
                ++_msgCnt;
                log_debug("client sending another message to the server ...");
                *_ssl << "Good morning from client!" << std::flush;
                _ios.buffer().beginWrite();
            }
            // Shutdown
            else {
                log_debug("client shutting down the stream");
                _ios.buffer().inputReady() -= Pt::slot(*this, &Client::onInput);
                _ios.buffer().outputReady() -= Pt::slot(*this, &Client::onOutput);

                _ssl->beginShutdown();
                _ssl->shutdownFinished() += Pt::slot(*this, &Client::onShutdownFinished);
            }
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            log_debug("client sent raw; remaining = " << sb.out_avail());

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
        log_debug("OpenSSL test progam started");

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 6000;

        Pt::Ssl::CertificateList trustedCACert;
        trustedCACert.fromPem(caPemData, sizeof(caPemData));

        Pt::Ssl::CertificateList serverCertChain;
        Pt::Ssl::PrivateKey      serverPrivKey("abc123");
        Pt::Ssl::Context         serverContext(Pt::Ssl::Context::Default);
        serverCertChain.fromPem(serverCertPemData, sizeof(serverCertPemData));

        serverPrivKey.fromPem(serverKeyData, sizeof(serverKeyData));
        serverContext.setCACertificates(trustedCACert);
        serverContext.setCertificateChain(serverCertChain);
        serverContext.setPrivateKey(serverPrivKey);

        Pt::Ssl::CertificateList clientCertChain;
        Pt::Ssl::PrivateKey      clientPrivKey("");
        Pt::Ssl::Context         clientContext(Pt::Ssl::Context::Default);
        clientCertChain.fromPem(clientCertPemData, sizeof(clientCertPemData));
        clientPrivKey  .fromPem(clientKeyData, sizeof(clientKeyData));
        clientContext  .setCACertificates(trustedCACert);
        clientContext  .setCertificateChain    (clientCertChain);
        clientContext  .setPrivateKey          (clientPrivKey);

        Server server(loop, serverContext, addr, port);
        Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(30000);
        loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
        loop.run();

        log_debug("OpenSSL test progam ended");
        return 0;
    }
    catch(const std::exception& ex)
    {
        log_error("Error: " << ex.what());
    }
    catch(const char* ex)
    {
        log_error("Error: " << ex);
    }

    return 1;
}
