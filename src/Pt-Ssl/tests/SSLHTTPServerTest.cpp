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
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>
#include <fstream>

log_define("Pt.Ssl.HttpCServerTest")

class Server : public Pt::Connectable {
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::Context& sslServerContext)
        : _sslContext(sslServerContext), _ssl(0), _ios(0), _loop(loop), _client(0)
        {
            log_debug("server *** Waiting connection from client ***");

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

            _client->setActive(_loop);

            _ios = new Pt::System::IOStream;
            _ios->attach(*_client);

            log_debug("server Starting handshake");
            _ssl = new Pt::Ssl::Server(_sslContext, *_ios, 0);
            _ssl->beginHandshake(true, false);
          //_ssl->beginHandshake(true, true);

            _ssl->handshakeFinished() += Pt::slot(*this, &Server::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::Server& ssl)
        {
            try {
                ssl.endHandshake();
            }
            catch(...) {
                log_debug("server *** HANDSHAKE FAILED ***");
                _client->detach();
                delete _client; _client = 0;
                delete _ios; _ios = 0;
                delete _ssl; _ssl = 0;

                log_debug("server *** Waiting connection from client ***");
                return;
            }

            log_debug("server Peer CN = " << _ssl->buffer().peerName());
            log_debug("server Current cipher = \n" << _ssl->buffer().currentCipher().name());

            _ios->buffer().inputReady() += Pt::slot(*this, &Server::onInput);
            _ios->buffer().outputReady() += Pt::slot(*this, &Server::onOutput);

            _ios->buffer().beginRead();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            log_debug("server Received raw = " << sb.in_avail());

            if(_ssl->buffer().import() == -1) {
                log_debug("server *** The stream has been shutdown by the other peer ***");
                _ssl->buffer().writeShutdown();
                _ios->buffer().inputReady() -= Pt::slot(*this, &Server::onInput);
                _ios->buffer().outputReady() -= Pt::slot(*this, &Server::onOutput);
                return;
            }

            log_debug("server Received decoded = " << _ssl->buffer().in_avail());

            char buf[512];
            std::streamsize n =_ssl->readsome(buf, 512);
            if(n <= 0) return;

            std::cerr
                << "############################################################################################# SERVER RECEIVED: "
                << std::endl;
            std::cerr.write(buf, n);

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

            log_debug("server Sending response to the client ... out_avail = " << _ios->buffer().out_avail());
            _ios->buffer().beginWrite();
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

            _ios->buffer().inputReady() -= Pt::slot(*this, &Server::onInput);
            _ios->buffer().outputReady() -= Pt::slot(*this, &Server::onOutput);

            // NOTE: If we uncomment this, the client will get the shutdown notification before receiving
            //       the full HTML body that will cause the client to never get the full HTML body.
            // _ssl->buffer().shutdown();

            _client->detach();
            delete _client; _client = 0;
            delete _ios; _ios = 0;
            delete _ssl; _ssl = 0;

            log_debug("server *** Waiting connection from client ***");
        }

    private:
        Pt::Ssl::Context&    _sslContext;
        Pt::Ssl::Server*     _ssl;
        Pt::System::IOStream*   _ios;
        Pt::System::EventLoop&  _loop;
        Pt::Net::TcpServer      _server;
        Pt::Net::TcpSocket*     _client;
};

int main(int argc, char** argv)
{
    try {
        log_debug("OpenSSL test progam started");

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 8000;

        Pt::Ssl::CertificateList trustedCACert;
        Pt::Ssl::CertificateList serverCertChain;
        Pt::Ssl::PrivateKey      serverPrivKey("abc123");
        Pt::Ssl::Context         serverContext(Pt::Ssl::Context::Default);
        trustedCACert.fromPem(caPemData, sizeof(caPemData));
        serverCertChain.fromPem(serverCertPemData, sizeof(serverCertPemData));
        serverPrivKey.fromPem(serverKeyData, sizeof(serverKeyData));
        serverContext.setCACertificates(trustedCACert);
        serverContext.setCertificateChain(serverCertChain);
        serverContext.setPrivateKey(serverPrivKey);
        
        Server server(loop, addr, port, serverContext);

        loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
        loop.run();

        log_debug("OpenSSL test progam ended");
        return 0;
    }
    catch(const std::exception& ex)
    {
        log_debug("Error: " << ex.what());
    }
    catch(const char* ex)
    {
        log_debug("Error: " << ex);
    }
    return 1;
}
