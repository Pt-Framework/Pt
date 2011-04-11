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

#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Ssl/SSLServer.h>
#include <Pt/System/MainLoop.h>
#include <fstream>

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////
log_define(PT_SSL_LOGGER_CATEGORY);
#define PT_SSL_LOG_S(CODE) PT_SSL_LOG_INFO("@@ Server @@", CODE)
#define PT_SSL_LOG_M(CODE) PT_SSL_LOG_INFO("@@ main() @@", CODE)
////////////////////////////////////////////////////////////////////////////////////////////////////

class Server : public Pt::Connectable {
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslServerContext)
        : _sslContext(sslServerContext), _ssl(0), _ios(0), _loop(loop), _client(0)
        {
            PT_SSL_LOG_S("*** Waiting connection from client ***");

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

            _ios = new Pt::System::IOStream;
            _ios->attachDevice(*_client);

            PT_SSL_LOG_S("Starting handshake");
            _ssl = new Pt::Ssl::SSLServer(*_ios, _sslContext, 0);
            _ssl->beginHandshake(true, false);
          //_ssl->beginHandshake(true, true);

            _ssl->handshakeFinished += Pt::slot(*this, &Server::onSSLHandshakeFinished);
        }

        void onSSLHandshakeFinished(Pt::Ssl::SSLServer& ssl)
        {
            try {
                ssl.endHandshake();
            }
            catch(...) {
                PT_SSL_LOG_S("*** HANDSHAKE FAILED ***");
                _loop.remove(*_client);
                delete _client; _client = 0;
                delete _ios; _ios = 0;
                delete _ssl; _ssl = 0;

                PT_SSL_LOG_S("*** Waiting connection from client ***");
                return;
            }

            PT_SSL_LOG_S("Peer CN = " << _ssl->buffer().getPeerCN());
            PT_SSL_LOG_S("Current cipher = \n" << _ssl->buffer().currentCipher().dump());

            _ios->buffer().inputReady += Pt::slot(*this, &Server::onInput);
            _ios->buffer().outputReady += Pt::slot(*this, &Server::onOutput);

            _ios->buffer().beginRead();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            PT_SSL_LOG_S("Received raw = " << sb.in_avail());

            if(_ssl->buffer().import() == -1) {
                PT_SSL_LOG_S("*** The stream has been shutdown by the other peer ***");
                _ios->buffer().inputReady -= Pt::slot(*this, &Server::onInput);
                _ios->buffer().outputReady -= Pt::slot(*this, &Server::onOutput);
                return;
            }

            PT_SSL_LOG_S("Received decoded = " << _ssl->buffer().in_avail());

            char buf[512];
            unsigned n =_ssl->readsome(buf, 512);
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
                lmsg += std::string( rbuf, ifs.gcount() );
            }

            PT_SSL_LOG_S("Sending response to the client ... body size = " << lmsg.length());
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

            PT_SSL_LOG_S("Sending response to the client ... out_avail = " << _ios->buffer().out_avail());
            _ios->buffer().beginWrite();
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            PT_SSL_LOG_S("Sent raw; remaining = " << sb.out_avail());

            if(sb.out_avail() > 0) {
                sb.beginWrite();
                return;
            }

            PT_SSL_LOG_S("Done sending response to the client");

            _ios->buffer().inputReady -= Pt::slot(*this, &Server::onInput);
            _ios->buffer().outputReady -= Pt::slot(*this, &Server::onOutput);

            // NOTE: If we uncomment this, the client will get the shutdown notification before receiving
            //       the full HTML body that will cause the client to never get the full HTML body.
            // _ssl->buffer().shutdown();

            _loop.remove(*_client);
            delete _client; _client = 0;
            delete _ios; _ios = 0;
            delete _ssl; _ssl = 0;

            PT_SSL_LOG_S("*** Waiting connection from client ***");
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
        PT_SSL_LOG_M("OpenSSL test progam started");

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 8000;

        Pt::Ssl::SSLCertificateList trustedCACert;
        Pt::Ssl::SSLCertificateList serverCertChain;
        Pt::Ssl::SSLPrivateKeyPtr   serverPrivKey(Pt::Ssl::SSLPrivateKey::newPrivateKey("password"));
        Pt::Ssl::SSLContext         serverContext(0, Pt::Ssl::SSLContext::DefaultProtocol);
        trustedCACert  .loadFromFile           ("ca.pem");
        serverCertChain.loadFromFile           ("server.pem");
        serverPrivKey ->loadFromFile           ("server.key");
        serverContext  .setTrustedCACertificate(trustedCACert);
        serverContext  .setCertificateChain    (serverCertChain);
        serverContext  .setPrivateKey          (serverPrivKey);

        Server server(loop, addr, port, serverContext);

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
