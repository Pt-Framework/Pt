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
#include <Pt/Ssl/Client.h>
#include <Pt/System/MainLoop.h>

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////
log_define(PT_SSL_LOGGER_CATEGORY);
#define PT_SSL_LOG_C(CODE) PT_SSL_LOG_INFO("@@ Client @@", CODE)
#define PT_SSL_LOG_M(CODE) PT_SSL_LOG_INFO("@@ main() @@", CODE)
////////////////////////////////////////////////////////////////////////////////////////////////////

class Client : public Pt::Connectable {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::Context& sslClientContext)
        : _sslContext(sslClientContext), _ssl(0), _loop(loop), _header(""), _result(""), _httpSize(0)
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
          //_ssl->beginHandshake(true);
            _ssl->beginHandshake(false);

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
            
            PT_SSL_LOG_C("Peer CN = " << _ssl->buffer().peerName());
            PT_SSL_LOG_C("Current cipher = \n" << _ssl->buffer().currentCipher().name());

            _ios.buffer().inputReady() += Pt::slot(*this, &Client::onInput);
            _ios.buffer().outputReady() += Pt::slot(*this, &Client::onOutput);

            PT_SSL_LOG_C("Sending request to the server ...");
            *_ssl <<
                "GET / HTTP/1.1\r\n"
                "Host: localhost:443\r\n"
                "User-Agent: Platinum\r\n"
                "Accept: text/html\r\n"
                "Accept-Language: en-us,en;q=0.5\r\n"
                "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                "Connection: close\r\n"
                "Cache-Control: max-age=0\r\n\r\n"
            << std::flush;
/*            
            *_ssl <<
                "GET /long_html.html HTTP/1.1\r\n"
                "Host: localhost:443\r\n"
                "User-Agent: Platinum\r\n"
                "Accept: text/html\r\n"
                "Accept-Language: en-us,en;q=0.5\r\n"
                "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                "Cache-Control: max-age=0\r\n\r\n"
            << std::flush;
*/

            _ios.buffer().beginWrite();
        }

        void onShutdownFinished(Pt::Ssl::Client& ssl)
        {
            _ssl->endShutdown();
            PT_SSL_LOG_C("*** SHUTDOWN FINISHED ***");
            _loop.exit();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            PT_SSL_LOG_C("Received raw = " << sb.in_avail());

            if( sb.device()->eof() )
            {
                PT_SSL_LOG_C("Received EOF ");
                return;
            }

            while(true)
            {
                std::streamsize avail = _ssl->buffer().import();

                if(avail == -1) {
                    PT_SSL_LOG_C("*** The stream has been shutdown by the other peer ***");
                    _ios.buffer().inputReady() -= Pt::slot(*this, &Client::onInput);
                    _ios.buffer().outputReady() -= Pt::slot(*this, &Client::onOutput);
                    std::cerr
                        << "############### CLIENT RECEIVED BEFORE SHUTDOWN: "
                        << std::endl << _result << std::endl;

                    _ssl->beginShutdown();
                    _ssl->shutdownFinished += Pt::slot(*this, &Client::onShutdownFinished);
                    return;
                }

                if( ! avail )
                    break;

                PT_SSL_LOG_C("Received decoded = " << _ssl->buffer().in_avail());

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
                    << "############### CLIENT RECEIVED HEADER: "
                    << std::endl << _header << std::endl;

                pos = _header.find("Content-Length:");
                if(pos != std::string::npos) {
                    size_t start = _header.find(" ", pos);
                    size_t end   = _header.find("\r\n", start);
                    _httpSize = atol(_header.substr(start + 1, end - start - 1).c_str());
                    PT_SSL_LOG_C("EXPECTED Content-Length = " << _httpSize);
                }
            }

            if(_httpSize && _result.length() < _httpSize) {
                PT_SSL_LOG_C("Message not complete; current size = " << _result.length());
                _ios.buffer().beginRead();
                return;
            }

            std::cerr
                << "############################################################################################# CLIENT RECEIVED CONTENT: "
                << std::endl << _result << std::endl;

            PT_SSL_LOG_C("Shutting down the stream");
            _ios.buffer().inputReady() -= Pt::slot(*this, &Client::onInput);
            _ios.buffer().outputReady() -= Pt::slot(*this, &Client::onOutput);
            _ssl->buffer().shutdown();
        }

        void onOutput(Pt::System::StreamBuffer& sb)
        {
            sb.endWrite();
            PT_SSL_LOG_C("Sent raw; remaining = " << sb.out_avail());

            if(sb.out_avail() > 0) {
                sb.beginWrite();
                return;
            }

            PT_SSL_LOG_C("Done sending request to the server");

            _ios.buffer().beginRead();
        }

    private:
        Pt::Ssl::Context&   _sslContext;
        Pt::Ssl::Client*    _ssl;
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
        Pt::System::Logger::getTarget("").setLogLevel(Pt::System::Trace);
        PT_SSL_LOG_M("OpenSSL HTTP test progam started");

        Pt::System::MainLoop loop;
      //std::string          addr("127.0.0.1");
        std::string          addr("www.pt-framework.org");
        unsigned short       port = 443;

        Pt::Ssl::CertificateList trustedCACert;
        Pt::Ssl::CertificateList clientCertChain;
        Pt::Ssl::PrivateKey      clientPrivKey("");
        Pt::Ssl::Context         clientContext(0, Pt::Ssl::Context::Default);
        trustedCACert  .fromPemFile           ("ca.pem");
        clientCertChain.fromPemFile           ("client.pem");
        clientPrivKey  .fromPemFile           ("client.key");
        clientContext  .setCACertificates(trustedCACert);
        clientContext  .setCertificateChain    (clientCertChain);
        clientContext  .setPrivateKey          (clientPrivKey);

        Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(30000);
        loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
        loop.run();

        PT_SSL_LOG_M("OpenSSL HTTP test progam ended");
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
