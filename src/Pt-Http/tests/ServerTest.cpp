/*
 * Copyright (C) 2006 - 2009 by Marc Boris Duerner
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

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Http/Server.h"
#include "Pt/Http/Client.h"
#include "Pt/Http/ReplyHeader.h"
#include "Pt/Http/Request.h"
#include "Pt/Http/Reply.h"
#include "Pt/Http/Responder.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Logger.h"
#include <string>

#include <Pt/Ssl/Context.h>
#include "../../Pt-Ssl/tests/PemData.h"

class ChunkedResponder : public Pt::Http::Responder
{
    public:
        ChunkedResponder(Pt::Http::Service& service)
        : Pt::Http::Responder(service)
        , _chunks(5)
        {}

        void beginRequest(std::istream& in, Pt::Http::RequestHeader& request)
        { _chunks = 5; }

        void readBody(std::istream& is, Pt::Http::Reply& reply)
        {
            is.ignore( is.rdbuf()->in_avail() );
        }

        void beginReply(std::ostream& os, Pt::Http::RequestHeader& request, Pt::Http::Reply& reply)
        {
            os << "Chunk" << _chunks--;

            if(_chunks == 0)
                reply.finish();
            else
                reply.advance();
        }

    private:
        unsigned _chunks;
};

typedef Pt::Http::CachedService<ChunkedResponder> ChunkedService;


class ServerTest : public Pt::Unit::TestSuite
{
    public:
        ServerTest()
        : Pt::Unit::TestSuite("ServerTest")
        {
            //Pt::System::Logger::setLogLevel("Pt.Http.Server", Pt::System::Trace);

            this->registerMethod( "NotFound", *this, &ServerTest::NotFound);
#ifdef PT_HTTP_WITH_SSL
            this->registerMethod( "NotFoundHttps", *this, &ServerTest::NotFoundHttps);
#endif

            this->registerMethod( "ChunkedReply", *this, &ServerTest::ChunkedReply);
        }

        void setUp()
        {
            _reply.clear();

            loop = new Pt::System::MainLoop();
            loop->setIdleTimeout(5000);
        }

        void tearDown()
        {
            delete loop;
        }

        void NotFound()
        {
            loop->timeout() += Pt::slot(*loop, &Pt::System::MainLoop::exit);

            Pt::Http::Server server(*loop, "127.0.0.1", 8001);

            Pt::Http::Client client("127.0.0.1", 8001);
            client.setActive(*loop);
            connect(client.headerReceived(), *this, &ServerTest::onNotFoundHeader);
            connect(client.bodyAvailable(), *this, &ServerTest::onNotFound);
            connect(client.replyFinished(), *this, &ServerTest::onNotFoundFinished);

            Pt::Http::Request request("/index.html");
            request.setHeader("foo", "bar");
            client.beginExecute(request);

            loop->run();
        }

#ifdef PT_HTTP_WITH_SSL
        void NotFoundHttps()
        {
            loop->timeout() += Pt::slot(*loop, &Pt::System::MainLoop::exit);

            // SSL configuration
            Pt::Ssl::CertificateList caCert;
            caCert.fromPem(caPemData, sizeof(caPemData));

            // client-side SSL context
            Pt::Ssl::CertificateList clientCert;
            clientCert.fromPem(clientCertPemData, sizeof(clientCertPemData));

            Pt::Ssl::PrivateKey clientPrivKey("");
            clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));

            Pt::Ssl::Context clientContext;
            clientContext.setCACertificates(caCert);
            clientContext.setCertificateChain(clientCert);
            clientContext.setPrivateKey(clientPrivKey);
            clientContext.setVerifyMode(Pt::Ssl::Context::VerifyPeer);

            // start HTTP server
            Pt::Http::Server server(*loop);
            server.sslConfigured += Pt::slot(&ServerTest::loadSslContext);
            server.listen("127.0.0.1", 8001, true);

            // start HTTP client
            Pt::Http::Client client("127.0.0.1", 8001, true);
            client.setContext(clientContext);
            client.setActive(*loop);
            connect(client.headerReceived(), *this, &ServerTest::onNotFoundHeader);
            connect(client.bodyAvailable(), *this, &ServerTest::onNotFound);
            connect(client.replyFinished(), *this, &ServerTest::onNotFoundFinished);

            Pt::Http::Request request("/index.html");
            request.setHeader("foo", "bar");
            client.beginExecute(request);

            loop->run();
        }

        static void loadSslContext(Pt::Ssl::Context& ctx)
        {
            // SSL configuration
            Pt::Ssl::CertificateList caCert;
            caCert.fromPem(caPemData, sizeof(caPemData));

            // server-side SSL context
            Pt::Ssl::CertificateList cert;
            cert.fromPem(serverCertPemData, sizeof(serverCertPemData));

            Pt::Ssl::PrivateKey privKey("abc123");
            privKey.fromPem(serverKeyData, sizeof(serverKeyData));

            ctx.setCACertificates(caCert);
            ctx.setCertificateChain(cert);
            ctx.setPrivateKey(privKey);
            ctx.setVerifyMode(Pt::Ssl::Context::VerifyPeerRequired);
        }
#endif

        void onNotFoundHeader(Pt::Http::Client& client)
        {
        }

        std::size_t onNotFound(Pt::Http::Client& client)
        {
            std::size_t ret = 0;
            while ( client.body().rdbuf()->in_avail() )
            {
                char ch;
                client.body().get(ch);
                ++ret;
                std::cout << ch;
            }

            return ret;
        }

        void onNotFoundFinished(Pt::Http::Client& client)
        {
            PT_UNIT_ASSERT_EQUALS(client.header().httpReturnCode(), 404);
            loop->exit();
        }

        void ChunkedReply()
        {
            loop->timeout() += Pt::slot(*loop, &Pt::System::MainLoop::exit);

            ChunkedService service;
            Pt::Http::Server server(*loop, "127.0.0.1", 8001);
            server.addService("/test", service);

            Pt::Http::Client client("127.0.0.1", 8001);
            client.setActive(*loop);
            connect(client.headerReceived(), *this, &ServerTest::onChunkedHeader);
            connect(client.bodyAvailable(), *this, &ServerTest::onChunkedReply);
            connect(client.replyFinished(), *this, &ServerTest::onChunkedReplyFinished);

            Pt::Http::Request request("/test");
            client.beginExecute(request);

            loop->run();
        }

        void onChunkedHeader(Pt::Http::Client& client)
        {
        }

        void onChunkedReply(Pt::Http::Client& client)
        {
            std::size_t ret = 0;
            while ( client.body().rdbuf()->in_avail() )
            {
                char ch;
                client.body().get(ch);
                ++ret;
                _reply += ch;
            }
        }

        void onChunkedReplyFinished(Pt::Http::Client& client)
        {
            PT_UNIT_ASSERT_EQUALS(client.header().httpReturnCode(), 200);
            //std::clog << _reply << std::endl;
            loop->exit();
        }

    private:
        Pt::System::MainLoop* loop;
        std::string _reply;
};

Pt::Unit::RegisterTest<ServerTest> register_HttpServerTest;
