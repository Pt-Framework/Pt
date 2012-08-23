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

class HelloResponder : public Pt::Http::Responder
{
    public:
        HelloResponder(Pt::Http::Service& service)
        : Pt::Http::Responder(service)
        {}

        void readRequest(std::istream& is, Pt::Http::Reply& reply)
        {
            is.ignore( is.rdbuf()->in_avail() );
        }
        
        void writeReply(Pt::Http::RequestHeader& request, Pt::Http::Reply& reply)
        {
            reply.body() << "Hello World!";
            reply.finish();
        }
};

typedef Pt::Http::CachedService<HelloResponder> HelloService;

class ChunkedResponder : public Pt::Http::Responder
{
    public:
        ChunkedResponder(Pt::Http::Service& service)
        : Pt::Http::Responder(service)
        , _chunks(5)
        {}

        void beginRequest(std::istream& in, Pt::Http::RequestHeader& request)
        { 
            _chunks = 5;
            //for (Pt::Http::RequestHeader::const_iterator it = request.begin();
            //    it != request.end(); ++it)
            //{
            //    std::cerr << it->first << ": " << it->second << "\r\n";
            //}
        }

        void readRequest(std::istream& is, Pt::Http::Reply& reply)
        {
            is.ignore( is.rdbuf()->in_avail() );
            //while ( is.rdbuf()->in_avail() )
            //    std::cerr << char( is.get() );
        }
        
        void writeReply(Pt::Http::RequestHeader& request, Pt::Http::Reply& reply)
        {
            reply.body() << "Chunk" << _chunks--;

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
            Pt::System::Logger::setLogLevel("Pt.Http", Pt::System::Error);

            this->registerMethod( "NotFound", *this, &ServerTest::NotFound);
#ifdef PT_HTTP_WITH_SSL
            this->registerMethod( "NotFoundHttps", *this, &ServerTest::NotFoundHttps);
#endif

            this->registerMethod( "ReplyWithBody", *this, &ServerTest::ReplyWithBody);
            this->registerMethod( "ChunkedReply", *this, &ServerTest::ChunkedReply);
        }

        void setUp()
        {
            _reply.clear();

            _chunks.clear();
            _chunks.push_back("ChunkA");
            _chunks.push_back("ChunkB");
            _chunks.push_back("ChunkC");
            _chunks.push_back("ChunkD");
            _chunks.push_back("ChunkE");

            loop = new Pt::System::MainLoop();
            loop->setIdleTimeout(5000);
            loop->timeout() += Pt::slot(*loop, &Pt::System::MainLoop::exit);
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
            client.replyReceived() += Pt::slot(*this, &ServerTest::onNotFoundReceived);
            client.request().url("/index.html");
            client.request().header().setHeader("foo", "bar");
            client.beginReceive();

            loop->run();
        }

        void onNotFoundReceived(Pt::Http::Client& client)
        {
            bool received = client.endReceive();

            if(received)
            {
                PT_UNIT_ASSERT_EQUALS(client.replyHeader().httpReturnCode(), 404);
            }

            while ( client.reply().rdbuf()->in_avail() )
                _reply += client.reply().get();

            if( client.isEnd() )
            {
                PT_UNIT_ASSERT_EQUALS(client.replyHeader().httpReturnCode(), 404);
                loop->exit();
                return;
            }

            client.beginReceive();
        }

#ifdef PT_HTTP_WITH_SSL
        void NotFoundHttps()
        {
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
            client.replyReceived() += Pt::slot(*this, &ServerTest::onNotFoundReceived);
            client.request().url("/index.html");
            client.request().header().setHeader("foo", "bar");
            client.beginReceive();

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

        void ReplyWithBody()
        {
            HelloService service;
            Pt::Http::Server server(*loop, "127.0.0.1", 8001);
            server.addService("/test", service);

            Pt::Http::Client client("127.0.0.1", 8001);
            client.setActive(*loop);
            client.requestSent() += Pt::slot(*this, &ServerTest::onHelloSent);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onHelloReceived);
            client.request().url("/test");
            client.request().header().setHeader("foo", "bar");
            client.beginSend();

            loop->run();
            PT_UNIT_ASSERT_EQUALS(client.replyHeader().httpReturnCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Hello World!");
        }

        void onHelloSent(Pt::Http::Client& client)
        {
            client.endSend();
            client.beginReceive();
        }

        void onHelloReceived(Pt::Http::Client& client)
        {
            bool received = client.endReceive();

            if( received )
            {
                PT_UNIT_ASSERT_EQUALS(client.replyHeader().httpReturnCode(), 200);
            }
            
            while ( client.reply().rdbuf()->in_avail() )
                _reply += client.reply().get();

            if( client.isEnd() )
            {
                loop->exit();
                return;
            }

            client.beginReceive();
        }

        void ChunkedReply()
        {
            ChunkedService service;
            Pt::Http::Server server(*loop, "127.0.0.1", 8001);
            server.addService("/test", service);

            Pt::Http::Client client("127.0.0.1", 8001);
            client.setActive(*loop);
            client.requestSent() += Pt::slot(*this, &ServerTest::onChunkedSent);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onChunkedReceived);
            client.request().url("/test");
            
            client.request().body() << _chunks.front();
            _chunks.erase( _chunks.begin() );
            client.beginSend(false);

            loop->run();

            PT_UNIT_ASSERT_EQUALS(client.replyHeader().httpReturnCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Chunk5Chunk4Chunk3Chunk2Chunk1");
        }

        void onChunkedSent(Pt::Http::Client& client)
        {
            client.endSend();

            if( ! _chunks.empty() )
            {
              client.request().body() << _chunks.front();
              _chunks.erase( _chunks.begin() );
              client.beginSend();
              return;
            }

            client.beginReceive();
        }

        void onChunkedReceived(Pt::Http::Client& client)
        {
            bool received = client.endReceive();

            if(received)
            {
                PT_UNIT_ASSERT_EQUALS(client.replyHeader().httpReturnCode(), 200);
            }

            while ( client.reply().rdbuf()->in_avail() )
                _reply += client.reply().get();

            if( client.isEnd() )
            {
                loop->exit();
                return;
            }
            
            client.beginReceive();
        }

    private:
        Pt::System::MainLoop* loop;
        std::string _reply;
        std::vector<std::string> _chunks;
};

Pt::Unit::RegisterTest<ServerTest> register_HttpServerTest;
