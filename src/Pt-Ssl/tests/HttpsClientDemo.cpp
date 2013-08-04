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
#include <Pt/Ssl/Context.h>
#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>

log_define("Pt.Ssl.HttpClientDemo")

void onReply(Pt::Http::Client& client)
{
    Pt::Http::MessageProgress progress  = client.endReceive();

    if( progress.body() )
    {
        while ( client.reply().body().rdbuf()->in_avail() )
        {
            char ch;
            client.reply().body().get(ch);
            std::cout << ch;
        }
    }

    if( progress.finished() )
    {
        client.loop()->exit();
        return;
    }
            
    client.beginReceive();
}

void httpsGet(Pt::Ssl::Context& sslctx)
{
    try
    {
        Pt::System::MainLoop loop;

        //std::string addr("127.0.0.1");
        std::string  addr("www.pt-framework.org");
        unsigned short port = 443;
        
        Pt::Http::Client client(loop, addr, port);
        client.setSecure(sslctx);
        client.request().setUrl("/index.html");
        client.request().header().set("User-Agent", "Platinum");
        client.replyReceived() += Pt::slot(&onReply);
 
        bool noblock = true;
        if(noblock)
        {
            log_debug("excuting non-blocking HTTPS request");
            client.beginReceive();

            //loop.setIdleTimeout(60000);
            loop.timeout() += Pt::slot(loop, &Pt::System::EventLoop::exit);
            loop.run();
        }
        else
        {
            log_debug("excuting blocking HTTPS request");
            client.send();
            client.receive();

            char ch = ' ';
            while( client.reply().body().get(ch) )
                std::cout << ch;
        }

        log_debug("OpenSSL HTTP test progam ended");
    }
    catch(const std::exception& ex)
    {
        log_debug("Error: " << ex.what());
    }
    catch(const char* ex)
    {
        log_debug("Error: " << ex);
    }
}

int main(int argc, char** argv)
{
    try 
    {
        Pt::System::Logger::setLogLevel("", Pt::System::Trace);
        log_debug("OpenSSL HTTP test progam started");
        
        Pt::Ssl::Context sslctx;  

        Pt::Ssl::CertificateList trustedCACert;
        trustedCACert.fromPem(caPemData, sizeof(caPemData));

        Pt::Ssl::CertificateList clientCertChain;
        clientCertChain.fromPem(clientCertPemData, sizeof(clientCertPemData));

        Pt::Ssl::PrivateKey clientPrivKey("");
        clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));
        
        sslctx.setCACertificates(trustedCACert);
        sslctx.setCertificateChain(clientCertChain);
        sslctx.setPrivateKey(clientPrivKey);
        sslctx.setVerifyMode(Pt::Ssl::Context::VerifyNone);

        Pt::Net::TcpSocket socket;

        log_debug("connecting tcp socket");
        socket.connect("www.pt-framework.org", 443);
        log_debug("connected");

        std::stringstream ss;
        Pt::Ssl::Connection conn( sslctx, *ss.rdbuf() );
        conn.setConnecting();

        while( ! conn.connected() )
        {
            log_debug("---write handshake---");
            conn.writeHandshake();
            std::string data = ss.str();

            log_debug("write: " << data.size());
            while(data.size() > 0)
            {
                size_t written = socket.write(data.c_str(), data.size());
                log_debug("wrote: " << written);

                data.erase(0, written);
            }

            ss.str("");

            log_debug("---read handshake---");
            
            while( conn.readHandshake() )
            {
                char buf[4096];
                size_t read = socket.read(buf, sizeof(buf));
                log_debug("read: " << read);
                ss.str( std::string(buf, read) );
            }

            ss.str("");
        }

        log_debug("---DEMO FINISHED SUCCESSFULLY---");

        const char request[] = "GET / HTTP/1.1\r\n"
                                "Host: www.pt-framework.org\r\n"
                                "Connection: close\r\n"
                                //"User-Agent: Web-sniffer/1.0.46 (+http://web-sniffer.net/)\r\n"
                                //"Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7\r\n"
                                //"Cache-Control: no-cache\r\n"
                                //"Accept-Language: de,en;q=0.7,en-us;q=0.3\r\n"
                                //"Referer: http://web-sniffer.net/\r\n"
                                "\r\n";

        log_debug("--- HTTP request---");
        std::streamsize written = conn.write( request, sizeof(request) );
        log_debug("encoded: " << written);

        std::string data = ss.str();

        log_debug("write: " << data.size() << " bytes");
        while(data.size() > 0)
        {
            size_t written = socket.write(data.c_str(), data.size());
            log_debug("wrote: " << written << " bytes");

            data.erase(0, written);
        }

        ss.str("");
        
        log_debug("--- HTTP reply---");

        std::streamsize replySize = 0;

        while(replySize < 1000)
        {
            char buf[4096];
            size_t read = socket.read(buf, sizeof(buf));
            log_debug("read: " << read << " bytes from socket");
            ss.str( std::string(buf, read) );

            while( ss.rdbuf()->in_avail() )
            {
                std::streamsize readDecoded = conn.read( buf, sizeof(buf), ss.rdbuf()->in_avail() );
                log_debug("decoded: " << readDecoded << " bytes");

                replySize += readDecoded;
                std::clog.write(buf, readDecoded);
            }
        }
        
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
