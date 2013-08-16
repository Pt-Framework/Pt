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
#include "Pt/Http/Authorizer.h"
#include "Pt/Http/Authenticator.h"
#include "Pt/Http/Request.h"
#include "Pt/Http/Reply.h"
#include "Pt/Http/Service.h"
#include "Pt/Http/Servlet.h"
#include "Pt/Http/Responder.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Logger.h"
#include <string>
#include <fstream>

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
        
        virtual void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {}
        
        virtual void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {}

        virtual void onBeginReply(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        { onWriteReply(request, reply, loop); }

        virtual void onWriteReply(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {
            reply.body() << "Hello World!";
            reply.beginSend(true);
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
      
        virtual void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        { _chunks = 5; }
        
        virtual void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {}

        virtual void onBeginReply(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        { onWriteReply(request, reply, loop); }

        virtual void onWriteReply(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {
            reply.body() << "Chunk" << _chunks--;

            if(_chunks == 0)
                reply.beginSend(true);
            else
                reply.beginSend(false);
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
        {
            Pt::System::Logger::setLogLevel("Pt", Pt::System::Error);

            _authent.setUser( Pt::Http::Credentials("testo", "testpwd") );

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
            Pt::Http::MapUrl mapurl("/test", service);

            Pt::Http::Server server(*loop, "127.0.0.1", 8001);
            server.addServlet(mapurl);

            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.requestSent() += Pt::slot(*this, &ServerTest::onPipelinedSent);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onPipelinedReceived);
            client.request().setUrl("/test");
            client.request().header().set("foo", "bar");
            PT_UNIT_ASSERT(client.request().header().has("foo") );
            client.beginSend(true);

            loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);

            PT_UNIT_ASSERT_EQUALS(_reply, "Hello World!Hello World!");
        }

        void onPipelinedSent(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endSend();
            if( ! progress.finished() )
            {
                client.beginSend(true);
                return;
            }

            if( ! client.request().header().has("foo2") )
            {
                client.request().header().set("foo2", "bar2");
                client.beginSend(true);
                return;
            }

            client.beginReceive();
        }

        void onPipelinedReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
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
            client.request().header().set("foo", "bar");
            client.beginReceive();

            loop->run();
        }

        void onNotFoundReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 404);
            }

            if( progress.body() )
                while ( client.reply().body().rdbuf()->in_avail() )
                    _reply += client.reply().body().get();

            if( progress.finished() )
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 404);
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
            Pt::Http::Server::Options options;
            options.setSecure(serverCtx);
            
            Pt::Http::Server server(*loop);
            server.listen("127.0.0.1", 8001, options);

            Pt::Ssl::Context clientContext;
            setupSslClientContext(clientContext);
            
            // start HTTP client
            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.setSecure(clientContext);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onNotFoundReceived);
            client.request().setUrl("/index.html");
            client.request().header().set("foo", "bar");
            client.beginReceive();

            loop->run();

            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 404);
        }

        static void setupSslClientContext(Pt::Ssl::Context& ctx)
        {
            #ifdef _WIN32
                std::ifstream ifs("src\\Pt-Ssl\\tests\\cert\\client-with-password.p12", std::ios::binary);
            #else
                std::ifstream ifs("src/Pt-Ssl/tests/cert/client-with-password.p12", std::ios::binary);
            #endif

            #ifdef _WIN32
                std::ifstream ifs_ca("src\\Pt-Ssl\\tests\\cert\\ca-with-password.p12", std::ios::binary);
            #else
                std::ifstream ifs_ca("src/Pt-Ssl/tests/cert/ca-with-password.p12", std::ios::binary);
            #endif
            
            ctx.setVerifyMode(Pt::Ssl::Context::VerifyPeer);

            ctx.loadPkcs12(ifs, "123");
            ctx.loadPkcs12(ifs_ca, "123");

            Pt::Ssl::Certificate clientCert = ctx.findCertificate("Atlantis Mainframe");
            PT_UNIT_ASSERT( clientCert.isValid() );
            ctx.setCertificate( clientCert );

            Pt::Ssl::Certificate clientCA = ctx.findCertificate("SGC Certificate Authority");
            PT_UNIT_ASSERT( clientCA.isValid() );
            ctx.addCACertificate(clientCA);


            //Pt::Ssl::CertificateStore clientCerts;
            //clientCerts.loadPkcs12(ifs, "123");
            //PT_UNIT_ASSERT( ! clientCerts.empty() );

            //Pt::Ssl::CertificateStore caStore;
            //caStore.loadPkcs12(ifs_ca, "123");
            //PT_UNIT_ASSERT( ! caStore.empty() );

            //ctx.setCertificate( *clientCerts.begin() );
            //ctx.setCACertificates( caStore );
            //ctx.setVerifyMode(Pt::Ssl::Context::VerifyPeer);
        }

        static void setupSslServerContext(Pt::Ssl::Context& ctx)
        {
            #ifdef _WIN32
                std::ifstream ifs_ca("src\\Pt-Ssl\\tests\\cert\\ca-with-password.p12", std::ios::binary);
            #else
                std::ifstream ifs_ca("src/Pt-Ssl/tests/cert/ca-with-password.p12", std::ios::binary);
            #endif

            #ifdef _WIN32
                std::ifstream server_ifs("src\\Pt-Ssl\\tests\\cert\\server-with-password.p12", std::ios::binary);
            #else
                std::ifstream server_ifs("src/Pt-Ssl/tests/cert/server-with-password.p12", std::ios::binary);
            #endif

            ctx.setVerifyMode(Pt::Ssl::Context::VerifyPeerRequired);

            ctx.loadPkcs12(server_ifs, "123");
            ctx.loadPkcs12(ifs_ca, "123");

            Pt::Ssl::Certificate servCert = ctx.findCertificate("SGC Mainframe");
            PT_UNIT_ASSERT( servCert.isValid() );
            ctx.setCertificate( servCert );

            Pt::Ssl::Certificate servCA = ctx.findCertificate("SGC Certificate Authority");
            PT_UNIT_ASSERT( servCA.isValid() );
            ctx.addCACertificate(servCA);

            //Pt::Ssl::CertificateStore caStore;
            //caStore.loadPkcs12(ifs_ca, "123");
            //PT_UNIT_ASSERT( ! caStore.empty() );

            //Pt::Ssl::CertificateStore serverCerts;
            //serverCerts.loadPkcs12(server_ifs, "123");
            //PT_UNIT_ASSERT( ! serverCerts.empty() );

            //ctx.setCertificate( *serverCerts.begin() );
            //ctx.setCACertificates( caStore );
            //ctx.setVerifyMode(Pt::Ssl::Context::VerifyPeerRequired);
        }
#endif

        void BasicAuthentication()
        {
            Pt::Http::Server server(*loop, "127.0.0.1", 8001);

            HelloService service;
            Pt::Http::MapUrl mapurl("/test", service, _authent);
            server.addServlet(mapurl);

            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onBasicAuthenticationReceived);
            client.request().setUrl("/test");
            client.beginReceive();

            loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Authorization Required Hello World!");
        }

        void onBasicAuthenticationReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                if( client.reply().statusCode() == 401)
                {
                    PT_UNIT_ASSERT(client.reply().header().has("WWW-Authenticate"));

                    Pt::Http::Authenticator auth;
                    auth.setCredentials("test-realm", Pt::Http::Credentials("testo", "testpwd"));
                    bool authOk = auth.authenticate(client.request(), client.reply());
                    PT_UNIT_ASSERT(authOk);

                    _reply += client.reply().statusText();
                    _reply += ' ';
                }               
            }
            
            if( progress.body() )
                while ( client.reply().body().rdbuf()->in_avail() )
                    _reply += client.reply().body().get();

            if( progress.finished() )
            {
                if( client.reply().statusCode() == 200 )
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

            Pt::Http::MapUrl mapurl("/test", service);
            server.addServlet(mapurl);

            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onHelloReceived);
            client.request().setUrl("/test");
            client.request().header().set("foo", "bar");
            client.beginReceive();

            loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Hello World!");
        }

        void onHelloReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
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
            Pt::Http::MapUrl servlet("/test", service);
            server.addServlet(servlet);

            Pt::Http::Client client(*loop, "127.0.0.1", 8001);
            client.requestSent() += Pt::slot(*this, &ServerTest::onChunkedSent);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onChunkedReceived);
            client.request().setUrl("/test");
            
            client.request().body() << _chunks.front();
            _chunks.erase( _chunks.begin() );
            client.beginSend(false);

            loop->run();

            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Chunk5Chunk4Chunk3Chunk2Chunk1");
        }

        void onChunkedSent(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endSend();
            if( ! progress.finished() )
            {
                client.beginSend(false);
                return;
            }

            if( ! _chunks.empty() )
            {
                client.request().body() << _chunks.front();
                _chunks.erase( _chunks.begin() );
                client.beginSend(false);
                return;
            }

            client.beginReceive();
        }

        void onChunkedReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
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
        Pt::Http::BasicUserListAuthorizer _authent;
};

Pt::Unit::RegisterTest<ServerTest> register_HttpServerTest;
