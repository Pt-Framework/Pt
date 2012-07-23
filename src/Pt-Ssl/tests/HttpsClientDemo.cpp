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
#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>

log_define("Pt.Ssl.HttpClientDemo")

class Client : public Pt::Connectable {
    public:
        Client(Pt::System::EventLoop& loop, Pt::Ssl::Context& sslClientContext,
               const std::string& addr, unsigned short port)
        : _sslContext(sslClientContext)
        , _ssl(0)
        , _loop(loop)
        , _header("")
        , _result("")
        , _httpSize(0)
        {
            log_debug("client Connecting to server");

            _socket.setActive(_loop);
            _socket.connected() += Pt::slot(*this, &Client::onConnect);
            _socket.beginConnect(addr, port);
        }

        ~Client()
        { delete _ssl; }

    private:
        void onConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();
            _ios.attach(socket);

            log_debug("client Starting handshake");
            _ssl = new Pt::Ssl::IOBuffer(_sslContext, _ios.buffer());
            _ssl->beginConnect();
            _ssl->handshakeFinished() += Pt::slot(*this, &Client::onHandshake);
            _ssl->shutdownFinished() += Pt::slot(*this, &Client::onShutdown);
            _ssl->inputReady() += Pt::slot(*this, &Client::onInput);
            _ssl->outputReady() += Pt::slot(*this, &Client::onOutput);
        }

        void onHandshake(Pt::Ssl::IOBuffer& ssl)
        {
            try 
            {
                ssl.endHandshake();
            }
            catch(...) 
            {
                log_debug("client *** HANDSHAKE FAILED ***");
                _loop.exit();
                return;
            }
            
            log_debug("client Peer CN = " << _ssl->peerName());
            log_debug("client Current cipher = \n" << _ssl->currentCipher().name());

            std::ostream os(&ssl);
            log_debug("client Sending request to the server ...");
            os <<
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
            os <<
                "GET /long_html.html HTTP/1.1\r\n"
                "Host: localhost:443\r\n"
                "User-Agent: Platinum\r\n"
                "Accept: text/html\r\n"
                "Accept-Language: en-us,en;q=0.5\r\n"
                "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
                "Cache-Control: max-age=0\r\n\r\n"
            << std::flush;
*/

            ssl.beginWrite();
        }

        void onShutdown(Pt::Ssl::IOBuffer& ssl)
        {
            ssl.endShutdown();
            log_debug("client *** SHUTDOWN FINISHED ***");
            _loop.exit();
        }

        void onInput(Pt::Ssl::IOBuffer& ssl)
        {
            std::streamsize r = ssl.endRead();
            log_debug("client ends read, shutdown: " << r);            
            
            if( _ios.buffer().device()->eof() )
            {
                log_debug("client received EOF ");
                return;
            }

            if(r < 0) 
            {                   
                log_info("received shutdown from server");
                ssl.beginShutdown();
                return;
            }
 
            std::iostream ios(&ssl);
            std::string msg;
            char buf[512];
            while( ios.readsome(buf, 512) > 0 ) 
            {
                size_t n = static_cast<size_t>( ios.gcount() );
                log_debug("client received: " << n);
                msg.append(buf, n);
            }

            log_debug("client received: " << msg);

            if( _header.empty() ) 
            {
                size_t pos = _result.find("\r\n\r\n");
                if(pos != std::string::npos) 
                {
                    _header = _result.substr(0, pos);
                    _result = _result.substr(pos);
                }

                pos = _header.find("Content-Length:");
                if(pos != std::string::npos) 
                {
                    size_t start = _header.find(" ", pos);
                    size_t end   = _header.find("\r\n", start);
                    _httpSize = atol(_header.substr(start + 1, end - start - 1).c_str());
                    log_debug("client EXPECTED Content-Length = " << _httpSize);
                }
            }

            if(_httpSize == 0 || _result.length() < _httpSize) 
            {
                log_debug("client Message not complete; current size = " << _result.length());
                ssl.beginRead();
                return;
            }

            log_debug("client Shutting down the stream");
            ssl.beginShutdown();
        }

        void onOutput(Pt::Ssl::IOBuffer& ssl)
        {
            ssl.endWrite();
            log_debug("client Sent raw; remaining = " << _ios.buffer().out_avail());

            log_debug("client Done sending request to the server");
            ssl.beginRead();
        }

    private:
        Pt::Ssl::Context&      _sslContext;
        Pt::Ssl::IOBuffer*     _ssl;
        Pt::System::IOStream   _ios;
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpSocket     _socket;
        std::string            _header;
        std::string            _result;
        size_t                 _httpSize;
};


void onReply(Pt::Http::Client& client, std::istream& is)
{
    while ( is.rdbuf()->in_avail() )
    {
        char ch;
        is.get(ch);
        std::cout << ch;
    }
}

void onReplyFinished(Pt::Http::Client& client)
{
    if( client.loop() )
        client.loop()->exit();
}


int main(int argc, char** argv)
{
    try 
    {
        Pt::System::Logger::setLogLevel("", Pt::System::Trace);
        log_debug("OpenSSL HTTP test progam started");

        Pt::System::MainLoop loop;

        Pt::Ssl::CertificateList trustedCACert;
        trustedCACert.fromPem(caPemData, sizeof(caPemData));

        Pt::Ssl::CertificateList clientCertChain;
        clientCertChain.fromPem(clientCertPemData, sizeof(clientCertPemData));

        Pt::Ssl::PrivateKey clientPrivKey("");
        clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));
        
        Pt::Ssl::Context clientContext;        
        clientContext.setCACertificates(trustedCACert);
        clientContext.setCertificateChain(clientCertChain);
        clientContext.setPrivateKey(clientPrivKey);
        clientContext.setVerifyMode(Pt::Ssl::Context::VerifyNone);

        //std::string addr("127.0.0.1");
        std::string  addr("www.pt-framework.org");
        unsigned short port = 443;
        Pt::Http::Client client(loop, addr, port, true);
        client.setContext(clientContext);

        //client.headerReceived += Pt::slot(&onReplyHeader);
        client.bodyAvailable() += Pt::slot(&onReply);
        client.replyFinished() += Pt::slot(&onReplyFinished);

        //"GET / HTTP/1.1\r\n"
        //"Host: localhost:443\r\n"
        //"User-Agent: Platinum\r\n"
        //"Accept: text/html\r\n"
        //"Accept-Language: en-us,en;q=0.5\r\n"
        //"Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
        //"Connection: close\r\n"
        //"Cache-Control: max-age=0\r\n\r\n"
 
        Pt::Http::Request request("/index.html");
        request.setHeader("User-Agent", "Platinum");
        
        bool noblock = true;
        if(noblock)
        {
            log_debug("excuting non-blocking HTTPS request");
            client.beginExecute(request);

            //loop.setIdleTimeout(60000);
            loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
            loop.run();
        }
        else
        {
            log_debug("excuting blocking HTTPS request");
            client.execute(request);

            char ch = ' ';
            while( client.body().get(ch) )
                std::cout << ch;
        }

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

//int main(int argc, char** argv)
//{
//    try 
//    {
//        Pt::System::Logger::setLogLevel("", Pt::System::Trace);
//        log_debug("OpenSSL HTTP test progam started");
//
//        Pt::System::MainLoop loop;
//        //std::string          addr("127.0.0.1");
//        std::string          addr("www.pt-framework.org");
//        unsigned short       port = 443;
//
//        Pt::Ssl::CertificateList trustedCACert;
//        Pt::Ssl::CertificateList clientCertChain;
//        Pt::Ssl::PrivateKey clientPrivKey("");
//        Pt::Ssl::Context clientContext(Pt::Ssl::Context::Default);
//        trustedCACert.fromPem(caPemData, sizeof(caPemData));
//        clientCertChain.fromPem(clientCertPemData, sizeof(clientCertPemData));
//        clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));
//        clientContext.setCACertificates(trustedCACert);
//        clientContext.setCertificateChain(clientCertChain);
//        clientContext.setPrivateKey(clientPrivKey);
//
//        Client client(loop, clientContext, addr, port);
//
//        //loop.setIdleTimeout(60000);
//        loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
//        loop.run();
//
//        log_debug("OpenSSL HTTP test progam ended");
//        return 0;
//    }
//    catch(const std::exception& ex)
//    {
//        log_debug("Error: " << ex.what());
//    }
//    catch(const char* ex)
//    {
//        log_debug("Error: " << ex);
//    }
//    return 1;
//}
