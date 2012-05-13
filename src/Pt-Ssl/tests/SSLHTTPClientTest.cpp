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
#include <Pt/Ssl/Client.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>

log_define("Pt.Ssl.HttpClientTest")

class Client : public Pt::Connectable {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::Context& sslClientContext)
        : _sslContext(sslClientContext), _ssl(0), _loop(loop), _header(""), _result(""), _httpSize(0)
        {
            log_debug("client Connecting to server");

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

            log_debug("client Starting handshake");
            _ssl = new Pt::Ssl::Client(_sslContext, _ios, 0);
          //_ssl->beginHandshake(true);
            _ssl->beginConnectHandshake(false);

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
            
            log_debug("client Peer CN = " << _ssl->buffer().peerName());
            log_debug("client Current cipher = \n" << _ssl->buffer().currentCipher().name());

            _ios.buffer().inputReady() += Pt::slot(*this, &Client::onInput);
            _ios.buffer().outputReady() += Pt::slot(*this, &Client::onOutput);

            log_debug("client Sending request to the server ...");
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
            log_debug("client *** SHUTDOWN FINISHED ***");
            _loop.exit();
        }

        void onInput(Pt::System::StreamBuffer& sb)
        {
            sb.endRead();
            log_debug("client Received raw = " << sb.in_avail());

            if( sb.device()->eof() )
            {
                log_debug("client Received EOF ");
                return;
            }

            while(true)
            {
                std::streamsize avail = _ssl->buffer().import();

                if(avail == -1) {
                    log_debug("client *** The stream has been shutdown by the other peer ***");
                    _ios.buffer().inputReady() -= Pt::slot(*this, &Client::onInput);
                    _ios.buffer().outputReady() -= Pt::slot(*this, &Client::onOutput);
                    std::cerr
                        << "############### CLIENT RECEIVED BEFORE SHUTDOWN: "
                        << std::endl << _result << std::endl;

                    _ssl->beginShutdown();
                    _ssl->shutdownFinished() += Pt::slot(*this, &Client::onShutdownFinished);
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
                    << "############### CLIENT RECEIVED HEADER: "
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

            log_debug("client Shutting down the stream");
            _ios.buffer().inputReady() -= Pt::slot(*this, &Client::onInput);
            _ios.buffer().outputReady() -= Pt::slot(*this, &Client::onOutput);
            _ssl->buffer().shutdown();
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
        log_debug("OpenSSL HTTP test progam started");

        Pt::System::MainLoop loop;
      //std::string          addr("127.0.0.1");
        std::string          addr("www.pt-framework.org");
        unsigned short       port = 443;

        Pt::Ssl::CertificateList trustedCACert;
        Pt::Ssl::CertificateList clientCertChain;
        Pt::Ssl::PrivateKey      clientPrivKey("");
        Pt::Ssl::Context         clientContext(Pt::Ssl::Context::Default);
        trustedCACert.fromPem(caPemData, sizeof(caPemData));
        clientCertChain.fromPem(clientCertPemData, sizeof(clientCertPemData));
        clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));
        clientContext  .setCACertificates(trustedCACert);
        clientContext  .setCertificateChain    (clientCertChain);
        clientContext  .setPrivateKey          (clientPrivKey);

        Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(30000);
        loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
        loop.run();

        log_debug("OpenSSL HTTP test progam ended");
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
