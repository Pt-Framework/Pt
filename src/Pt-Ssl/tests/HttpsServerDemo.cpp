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
#include <Pt/Ssl/IOBuffer.h>
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
            _server.connectionPending() += Pt::slot(*this, &Server::onAccept);
            _server.setActive(_loop);
        }

        ~Server()
        {
            delete _client;
            delete _ssl;
        }

   private:
        void onAccept(Pt::Net::TcpServer& server)
        {
            log_debug("server Accepting connection from client");
            _client = new Pt::Net::TcpSocket;
            _client->accept(server);

            _client->setActive(_loop);

            _ios = new Pt::System::IOStream;
            _ios->attach(*_client);

            log_debug("server Starting handshake");
            _ssl = new Pt::Ssl::IOBuffer(_sslContext, *_ios, 0);
            _ssl->beginAccept(true, false);
          //_ssl->beginHandshake(true, true);

            _ssl->handshakeFinished() += Pt::slot(*this, &Server::onHandshake);
            //_ssl->shutdownFinished() += Pt::slot(*this, &Server::onShutdown);
            _ssl->inputReady() += Pt::slot(*this, &Server::onInput);
            _ssl->outputReady() += Pt::slot(*this, &Server::onOutput);
        }

        void onHandshake(Pt::Ssl::IOBuffer& ssl)
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

            log_debug("server Peer CN = " << ssl.peerName());
            log_debug("server Current cipher = \n" << ssl.currentCipher().name());

            ssl.beginRead();
        }

        void onInput(Pt::Ssl::IOBuffer& ssl)
        {
            std::streamsize r = ssl.endRead();
            log_debug("server Received raw = " << _ios->buffer().in_avail());

            if(r < 0) 
            {
                log_debug("server *** The stream has been shutdown by the other peer ***");
                ssl.beginShutdown();
                return;
            }

            log_debug("server Received decoded = " << ssl.in_avail());

            char buf[512];
            std::istream is(&ssl);
            std::streamsize n = is.readsome(buf, 512);
            if(n <= 0) 
                return;

            std::cerr  << "######## SERVER RECEIVED: " << std::endl;
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
            while(ifs) 
            {
                ifs.read( rbuf, sizeof(rbuf) );
                lmsg.append( rbuf, static_cast<size_t>(ifs.gcount()) );
            }

            std::ostream os(&ssl);
            log_debug("server Sending response to the client ... body size = " << lmsg.length());
            os << "HTTP/1.1 200 OK\r\n"
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
            ssl.beginWrite();
        }

        void onOutput(Pt::Ssl::IOBuffer& ssl)
        {
            ssl.endWrite();
            log_debug("server Sent raw; remaining = " << _ios->buffer().out_avail());


            log_debug("server Done sending response to the client");
            // NOTE: If we uncomment this, the client will get the shutdown notification before receiving
            //       the full HTML body that will cause the client to never get the full HTML body.
            // ssl.beginShutdown();

            _client->detach();
            delete _client; _client = 0;
            delete _ios; _ios = 0;
            delete _ssl; _ssl = 0;

            log_debug("server *** Waiting connection from client ***");
        }

    private:
        Pt::Ssl::Context&    _sslContext;
        Pt::Ssl::IOBuffer*     _ssl;
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
