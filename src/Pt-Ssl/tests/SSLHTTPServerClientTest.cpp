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
#include <Pt/Ssl/Server.h>
#include <Pt/Ssl/IOStream.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>
#include <fstream>

log_define("Pt.Ssl.HttpCServerClientTest")

class Server : public Pt::Connectable {
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::Context& sslServerContext)
        : _sslContext(sslServerContext), _ssl(0), _ios(8192, true), _loop(loop), _client(0)
        {
            log_debug("server Waiting connection from client");

            _server.listen(addr, port);
            _server.connectionPending() += Pt::slot(*this, &Server::onTCPAccept);

            _server.setActive(_loop);
        }

        ~Server()
        {
            delete _client;
            delete _ssl;
        }

   private:
        void onTCPAccept(Pt::Net::TcpServer& server)
        {
            log_debug("server Accepting connection from client");
            _client = new Pt::Net::TcpSocket;
            _client->accept(server);

            _client->setActive(_loop);;
            _ios.attach(*_client);

            log_debug("server Starting handshake");
            _ssl = new Pt::Ssl::Server(_sslContext, _ios, 0);
            _ssl->beginHandshake(true, true);

            _ssl->handshakeFinished() += Pt::slot(*this, &Server::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::Server& ssl)
        {
            try {
                ssl.endHandshake();
            }
            catch(...) {
                log_debug("server *** HANDSHAKE FAILED ***");
                _loop.exit();
                return;
            }

            log_debug("server Peer CN = " << _ssl->buffer().peerName());
            log_debug("server Current cipher = \n" << _ssl->buffer().currentCipher().name());

            _ios.buffer().inputReady() += Pt::slot(*this, &Server::onInput);
            _ios.buffer().outputReady() += Pt::slot(*this, &Server::onOutput);

            _ios.buffer().beginRead();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            log_debug("server Received raw = " << sb.in_avail());

            std::string msg;
            while(true)
            {
                const std::streamsize importResult = _ssl->buffer().import();
                if(importResult == -1) {
                    log_debug("server *** The stream has been shutdown by the other peer ***");
                    _ssl->buffer().shutdown();
                    _ios.buffer().inputReady() -= Pt::slot(*this, &Server::onInput);
                    _ios.buffer().outputReady() -= Pt::slot(*this, &Server::onOutput);
                    return;
                }
                if( ! importResult )
                    break;

                log_debug("server Received decoded = " << _ssl->buffer().in_avail());

                while(true) {
                    char buf[512];
                    std::streamsize n =_ssl->readsome(buf, 512);
                    if(n <= 0) 
                      break;
                    msg += std::string(buf, static_cast<size_t>(n));
                }
            }

            std::cerr
                << "############################################################################################# SERVER RECEIVED: "
                << std::endl << msg << std::endl;

            // Send reply
            std::string   lmsg;
            std::ifstream ifs;
            char          rbuf[4096];
#ifdef WIN32
            ifs.open("..\\..\\src\\Pt-Ssl\\long_html.html", std::ios::binary);
#else
            ifs.open("../../src/Pt-Ssl/long_html.html", std::ios::binary);
#endif
            while(ifs) {
                ifs.read( rbuf, sizeof(rbuf) );
                lmsg += std::string( rbuf, static_cast<size_t>(ifs.gcount()) );
            }

            log_debug("server Sending response to the client ... body size = " << lmsg.length());
            *_ssl << "HTTP/1.1 200 OK\r\n"
                     "Date: Fri, 18 Feb 2011 05:36:00 GMT\r\n"
                     "Server: Apache/2.2.13 (Fedora)\r\n"
                     "Last-Modified: Wed, 09 Feb 2011 14:01:41 GMT\r\n"
                     "ETag: \"c024e-1504a-49bd9e7805b40\"\r\n"
                     "Accept-Ranges: bytes\r\n"
                     "Content-Length: " << lmsg.length() << "\r\n"
                     "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                  << lmsg
                  << std::flush;

            _ios.buffer().beginWrite();
            log_debug("server Sending response to the client ... done");
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            log_debug("server Sent raw; remaining = " << sb.out_avail());

            if(sb.out_avail() > 0) {
                sb.beginWrite();
                return;
            }

            log_debug("server Done sending response to the client");

            _ios.buffer().inputReady() -= Pt::slot(*this, &Server::onInput);
            _ios.buffer().outputReady() -= Pt::slot(*this, &Server::onOutput);

            // NOTE: If we uncomment this, the client will get the shutdown notification before receiving the full HTML body
            //       that will cause the client to never prints the full HTML body to the console.
            // _ssl->buffer().shutdown();
        }

    private:
        Pt::Ssl::Context&    _sslContext;
        Pt::Ssl::Server*     _ssl;
        Pt::System::IOStream    _ios;
        Pt::System::EventLoop&  _loop;
        Pt::Net::TcpServer      _server;
        Pt::Net::TcpSocket*     _client;
};

class Client : public Pt::Connectable {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::Context& sslClientContext)
        : _sslContext(sslClientContext), _ssl(0), _loop(loop), _header(""), _result(""), _httpSize(0)
        {
            log_debug("client Connecting to server");

            _socket.connected() += Pt::slot(*this, &Client::onTCPConnect);
            _socket.beginConnect(addr, port);
            _socket.setActive(_loop);
        }

        ~Client()
        { delete _ssl; }

    private:
        void onTCPConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();
            _ios.attach(socket);

            log_debug("client Starting handshake");
            _ssl = new Pt::Ssl::IOStream(_sslContext, _ios, 0);
            _ssl->beginConnectHandshake(true);

            _ssl->handshakeFinished() += Pt::slot(*this, &Client::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::IOStream& ssl)
        {
            try {
                ssl.endHandshake();
            }
            catch(...) {
                log_debug("client *** HANDSHAKE FAILED ***");
                _loop.exit();
                return;
            }

            log_debug("client Peer CN = " << _ssl->buffer().peerName());
            log_debug("client Current cipher = \n" << _ssl->buffer().currentCipher().name());
            
            _ios.buffer().inputReady() += Pt::slot(*this, &Client::onInput);
            _ios.buffer().outputReady() += Pt::slot(*this, &Client::onOutput);

            log_debug("client Sending request to the server ...");
            *_ssl <<
                "GET /long_html.html HTTP/1.1\r\n"
                "Host: localhost:443\r\n"
                "User-Agent: Platinum\r\n"
                "Accept: text/html\r\n"
                "Accept-Language: en-us,en;q=0.5\r\n"
                "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                "Connection: close\r\n"
                "Cache-Control: max-age=0\r\n\r\n"
            << std::flush;

            _ios.buffer().beginWrite();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            log_debug("client Received raw = " << sb.in_avail());

            while(true)
            {
                std::streamsize avail = _ssl->buffer().import();

                if(avail == -1) {
                    log_debug("client *** The stream has been shutdown by the other peer ***");
                    _ssl->buffer().shutdown();
                    _ios.buffer().inputReady() -= Pt::slot(*this, &Client::onInput);
                    _ios.buffer().outputReady() -= Pt::slot(*this, &Client::onOutput);
                    std::cerr
                        << "############################################################################################# CLIENT RECEIVED BEFORE SHUTDOWN: "
                        << std::endl << _result << std::endl;
                    return;
                }

                if( ! avail )
                    break;

                log_debug("client Received decoded = " << _ssl->buffer().in_avail());

                while(true) {
                    char buf[512];
                    std::streamsize n =_ssl->readsome(buf, 512);
                    if(n <= 0) 
                      break;
                    _result += std::string(buf, static_cast<size_t>(n));
                }
            }

            if(_header.empty()) {
                size_t pos = _result.find("\r\n\r\n");
                if(pos != std::string::npos) {
                    _header = _result.substr(0, pos);
                    _result = _result.substr(pos);
                }
                std::cerr
                    << "############################################################################################# CLIENT RECEIVED HEADER: "
                    << std::endl << _header << std::endl;

                pos = _header.find("Content-Length:");
                if(pos != std::string::npos) {
                    size_t start = _header.find(" ", pos);
                    size_t end   = _header.find("\r\n", start);
                    _httpSize = atol(_header.substr(start + 1, end - start - 1).c_str());
                    log_debug("client EXPECTED Content-Length = " << _httpSize);
                }
            }

            if(_httpSize && _result.length() < _httpSize) {
                log_debug("client Message not complete; current size = " << _result.length());
                _ios.buffer().beginRead();
                return;
            }

            std::cerr
                << "############################################################################################# CLIENT RECEIVED CONTENT: "
                << std::endl << _result << std::endl;
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            log_debug("client Sent raw; remaining = " << sb.out_avail());

            if(sb.out_avail() > 0) {
                sb.beginWrite();
                return;
            }

            log_debug("client Done sending request to the server");

            _ios.buffer().beginRead();
        }

    private:
        Pt::Ssl::Context&   _sslContext;
        Pt::Ssl::IOStream*    _ssl;
        Pt::System::IOStream   _ios;
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpSocket     _socket;
        std::string            _header;
        std::string            _result;
        size_t                 _httpSize;
};

int main(int argc, char** argv)
{
    try {
        log_info("OpenSSL test progam started");

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 8000;

        Pt::Ssl::CertificateList trustedCACert;
        trustedCACert.fromPem(caPemData, sizeof(caPemData));

        Pt::Ssl::CertificateList serverCertChain;
        Pt::Ssl::PrivateKey serverPrivKey("abc123");
        Pt::Ssl::Context serverContext(Pt::Ssl::Context::Default);
        serverCertChain.fromPem(serverCertPemData, sizeof(serverCertPemData));
        serverPrivKey.fromPem(serverKeyData, sizeof(serverKeyData));
        serverContext.setCACertificates(trustedCACert);
        serverContext.setCertificateChain    (serverCertChain);
        serverContext.setPrivateKey          (serverPrivKey);

        Pt::Ssl::CertificateList clientCertChain;
        Pt::Ssl::PrivateKey clientPrivKey("");
        Pt::Ssl::Context clientContext(Pt::Ssl::Context::Default);
        clientCertChain.fromPem(clientCertPemData, sizeof(clientCertPemData));
        clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));
        clientContext.setCACertificates(trustedCACert);
        clientContext.setCertificateChain    (clientCertChain);
        clientContext.setPrivateKey          (clientPrivKey);

        Server server(loop, addr, port, serverContext);
        Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(2000);
        loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
        loop.run();

        log_info("OpenSSL test progam ended");
        return 0;
    }
    catch(const std::exception& ex)
    {
        log_info("Error: " << ex.what());
    }
    catch(const char* ex)
    {
        log_info("Error: " << ex);
    }
    return 1;
}
