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
#include "Pt/Http/Service.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Logger.h"
#include <string>

#ifdef PT_HTTP_WITH_SSL
#include <Pt/Ssl/Context.h>
#include "../../Pt-Ssl/tests/PemData.h"
#endif

class HelloResponder : public Pt::Http::Responder
{
    public:
        HelloResponder(Pt::Http::Service& s)
        : Pt::Http::Responder(s)
        {}
        
        virtual void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply)
        {}
        
        virtual void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply)
        {}

        virtual void onBeginReply(Pt::Http::Request& request, Pt::Http::Reply& reply)
        { onWriteReply(request, reply); }

        virtual void onWriteReply(Pt::Http::Request& request, Pt::Http::Reply& reply)
        {
            reply.body() << "Hello World!";
            reply.finish();
            reply.beginSend();
        }
};

typedef Pt::Http::BasicService<HelloResponder> HelloService;

class ChunkedResponder : public Pt::Http::Responder
{
    public:
        ChunkedResponder(Pt::Http::Service& s)
        : Pt::Http::Responder(s)
        , _chunks(5)
        {}
      
        virtual void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply)
        { _chunks = 5; }
        
        virtual void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply)
        {}

        virtual void onBeginReply(Pt::Http::Request& request, Pt::Http::Reply& reply)
        { onWriteReply(request, reply); }

        virtual void onWriteReply(Pt::Http::Request& request, Pt::Http::Reply& reply)
        {
            reply.body() << "Chunk" << _chunks--;

            if(_chunks == 0)
                reply.finish();

            reply.beginSend();
        }

    private:
        unsigned _chunks;
};

typedef Pt::Http::BasicService<ChunkedResponder> ChunkedService;


class ServerTest : public Pt::Unit::TestSuite
{
    public:
        ServerTest()
        : Pt::Unit::TestSuite("ServerTest")
        , _authent("test-realm")
        , _author("testo", "testpwd")
        {
            Pt::System::Logger::setLogLevel("Pt.Http", Pt::System::Error);

            _authent.setUser("testo", "testpwd");

            this->registerMethod( "NotFound", *this, &ServerTest::NotFound);
#ifdef PT_HTTP_WITH_SSL
            this->registerMethod( "NotFoundHttps", *this, &ServerTest::NotFoundHttps);
#endif

            this->registerMethod( "BasicAuthentication", *this, &ServerTest::BasicAuthentication);
            this->registerMethod( "ReplyWithBody", *this, &ServerTest::ReplyWithBody);
            this->registerMethod( "ChunkedReply", *this, &ServerTest::ChunkedReply);
            this->registerMethod( "PipelinedRequests", *this, &ServerTest::PipelinedRequests);
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

        void PipelinedRequests()
        {
            HelloService service;
            Pt::Http::Server server(*loop, "127.0.0.1", 8001);
            server.addService(Pt::Http::MapUrl("/test"), service);

            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.requestSent() += Pt::slot(*this, &ServerTest::onPipelinedSent);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onPipelinedReceived);
            client.request().setUrl("/test");
            client.request().header().setHeader("foo", "bar");
            PT_UNIT_ASSERT(client.request().header().hasHeader("foo") );
            client.request().finish();
            client.beginSend();

            loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().header().httpReturnCode(), 200);

            PT_UNIT_ASSERT_EQUALS(_reply, "Hello World!Hello World!");
        }

        void onPipelinedSent(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endSend();
            if( ! progress.finished() )
            {
                client.beginSend();
                return;
            }

            if( ! client.request().header().hasHeader("foo2") )
            {
                client.request().header().setHeader("foo2", "bar2");
                client.request().finish();
                client.beginSend();
                return;
            }

            client.beginReceive();
        }

        void onPipelinedReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().header().httpReturnCode(), 200);
            }
            
            if(progress.body())
            {
                while ( client.reply().body().rdbuf()->in_avail() )
                {
                    _reply += client.reply().body().get();
                }
            }

            if( progress.finished() )
            {
                if( _reply == "Hello World!Hello World!")
                {
                    loop->exit();
                    return;
                }
            }

            client.beginReceive();
        }

        void NotFound()
        {
            loop->timeout() += Pt::slot(*loop, &Pt::System::MainLoop::exit);

            Pt::Http::Server server(*loop, "127.0.0.1", 8001);

            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onNotFoundReceived);
            client.request().setUrl("/index.html");
            client.request().header().setHeader("foo", "bar");
            client.beginReceive();

            loop->run();
        }

        void onNotFoundReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if(progress.header())
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().header().httpReturnCode(), 404);
            }

            if(progress.body())
                while ( client.reply().body().rdbuf()->in_avail() )
                    _reply += client.reply().body().get();

            if( progress.finished() )
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().header().httpReturnCode(), 404);
                loop->exit();
                return;
            }

            client.beginReceive();
        }

#ifdef PT_HTTP_WITH_SSL
        void NotFoundHttps()
        {
            Pt::Ssl::Context serverCtx;
            setupSslServerContext(serverCtx);
            
            // start HTTP server
            Pt::Http::Server server(*loop);
            server.setSecure(serverCtx);
            server.listen("127.0.0.1", 8001, true);

            Pt::Ssl::Context clientContext;
            setupSslClientContext(serverCtx);
            
            // start HTTP client
            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.setSecure(clientContext);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onNotFoundReceived);
            client.request().setUrl("/index.html");
            client.request().header().setHeader("foo", "bar");
            client.beginReceive();

            loop->run();
        }

        static void setupSslClientContext(Pt::Ssl::Context& ctx)
        {
            // SSL configuration
            Pt::Ssl::CertificateList caCert;
            caCert.fromPem(caPemData, sizeof(caPemData));

            // client-side SSL context
            Pt::Ssl::CertificateList clientCert;
            clientCert.fromPem(clientCertPemData, sizeof(clientCertPemData));

            Pt::Ssl::PrivateKey clientPrivKey("");
            clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));

            ctx.setCACertificates(caCert);
            ctx.setCertificateChain(clientCert);
            ctx.setPrivateKey(clientPrivKey);
            ctx.setVerifyMode(Pt::Ssl::Context::VerifyPeer);
        }

        static void setupSslServerContext(Pt::Ssl::Context& ctx)
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

        void BasicAuthentication()
        {
            HelloService service;
            Pt::Http::Server server(*loop, "127.0.0.1", 8001);
            server.addService(Pt::Http::MapUrl("/test"), service, _authent);

            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onBasicAuthenticationReceived);
            client.request().setUrl("/test");
            client.beginReceive();

            loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().header().httpReturnCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Authorization Required Hello World!");
        }

        void onBasicAuthenticationReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                if( client.reply().header().httpReturnCode() == 401)
                {
                    PT_UNIT_ASSERT(client.reply().header().hasHeader("WWW-Authenticate"));
                    client.setAuthorization(_author);
                    _reply += client.reply().header().httpReturnText();
                    _reply += ' ';
                }               
            }
            
            if( progress.body() )
                while ( client.reply().body().rdbuf()->in_avail() )
                    _reply += client.reply().body().get();

            if( progress.finished() )
            {
                if( client.reply().header().httpReturnCode() == 200 )
                {
                    loop->exit();
                    return;
                }

                PT_UNIT_ASSERT_EQUALS(_reply, "Authorization Required ");
            }

            client.beginReceive();
        }

        void ReplyWithBody()
        {
            HelloService service;
            Pt::Http::Server server(*loop, "127.0.0.1", 8001);
            server.addService(Pt::Http::MapUrl("/test"), service);

            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.requestSent() += Pt::slot(*this, &ServerTest::onHelloSent);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onHelloReceived);
            client.request().setUrl("/test");
            client.request().header().setHeader("foo", "bar");
            client.beginSend();

            loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().header().httpReturnCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Hello World!");
        }

        void onHelloSent(Pt::Http::Client& client)
        {
            client.endSend();
            client.beginReceive();
        }

        void onHelloReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().header().httpReturnCode(), 200);
            }
            
            if( progress.body() )
                while ( client.reply().body().rdbuf()->in_avail() )
                    _reply += client.reply().body().get();

            if( progress.finished() )
            {
                loop->exit();
                return;
            }

            client.beginReceive();
        }

        void ChunkedReply()
        {
            Pt::Http::Server server(*loop, "127.0.0.1", 8001);

            ChunkedService service;
            server.addService(Pt::Http::MapUrl("/test"), service);

            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.requestSent() += Pt::slot(*this, &ServerTest::onChunkedSent);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onChunkedReceived);
            client.request().setUrl("/test");
            
            client.request().body() << _chunks.front();
            _chunks.erase( _chunks.begin() );
            client.beginSend();

            loop->run();

            PT_UNIT_ASSERT_EQUALS(client.reply().header().httpReturnCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Chunk5Chunk4Chunk3Chunk2Chunk1");
        }

        void onChunkedSent(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endSend();
            if( ! progress.finished() )
            {
                client.beginSend();
                return;
            }

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
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().header().httpReturnCode(), 200);
            }

            if( progress.body() )
                while ( client.reply().body().rdbuf()->in_avail() )
                    _reply += client.reply().body().get();

            if( progress.finished() )
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
        Pt::Http::BasicAuthentication _authent;
        Pt::Http::BasicAuthorization _author;
};

Pt::Unit::RegisterTest<ServerTest> register_HttpServerTest;
