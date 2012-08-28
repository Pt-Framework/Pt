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
#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>

log_define("Pt.Ssl.HttpClientDemo")

void onReply(Pt::Http::Client& client)
{
    bool received = client.endReceive();

    while ( client.reply().rdbuf()->in_avail() )
    {
        char ch;
        client.reply().get(ch);
        std::cout << ch;
    }

    if( client.isEnd() )
    {
        client.loop()->exit();
        return;
    }
            
    client.beginReceive();
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
        client.request().setUrl("/index.html");
        client.request().header().setHeader("User-Agent", "Platinum");
        client.setContext(clientContext);
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
            while( client.reply().get(ch) )
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
