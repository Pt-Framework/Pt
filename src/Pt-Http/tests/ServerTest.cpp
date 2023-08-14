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
#include "Pt/System/Timer.h"
#include "Pt/System/Logger.h"
#include <string>
#include <fstream>

#include <Pt/Ssl/Context.h>
#include <Pt/Ssl/CertificateStore.h>
#include "../../Pt-Ssl/tests/PemData.h"

class EchoQueryResponder : public Pt::Http::Responder
{
    public:
        EchoQueryResponder(Pt::Http::Service& s)
        : Pt::Http::Responder(s)
        {}
        
        void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, 
                            Pt::System::EventLoop& loop)
        { 
            setReady(false); 
        }
        
        void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, 
                             Pt::System::EventLoop& loop)
        { 
            setReady(false); 
        }

        void onBeginReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                            Pt::System::EventLoop& loop)
        { 
          return onWriteReply(request, reply, loop); 
        }

        void onWriteReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                            Pt::System::EventLoop& loop)
        {
            reply.body() << request.qparams();
            setReady(true); 
        }
};

typedef Pt::Http::BasicService<EchoQueryResponder> EchoQueryService;

/*
class WebSocketResponder : public Pt::Http::Responder
{
    public:
        WebSocketResponder(WebSocketService& s)
            : Pt::Http::Responder(s)
        {}

        virtual void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {
        }

        virtual void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {
        }

        virtual void onBeginReply(const Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {
            onWriteReply(request, reply, loop);
        }

        virtual void onWriteReply(const Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {
            reply.setStatus(101, "Switching Protocols");
            reply.header().setUpgrade();
            reply.header().set("Upgrade", "websocket");
            reply.header().set("Connection", "Upgrade");
            reply.header().set("Sec-WebSocket-Accept", "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
            reply.beginSend(true);
        }

        virtual void onUpgrade(const Pt::Http::Request& request, Pt::Http::IOStream* stream)
        {
            //TODO send event to WebSocketService
            // return true to destroy responder
        }
};

class WebSocketService : public  Pt::Http::BasicService<WebSocketService>
{
    public:
        WebSocketService()
        {}

        void onUpgrade(Pt::Http::IOStream* stream)
        {
            auto s = new WebSocket(stream);

            _signal.send(s);
        }
};
*/

class Counter
{
    public:
        Counter()
        : _n(1)
        { }

        int count()
        {
            return _n++;
        }

    private:
        int _n;
};

class HelloResponder : public Pt::Http::Responder
                     , public Pt::Connectable
{
    public:
        HelloResponder(Pt::Http::Service& s, Counter& c)
        : Pt::Http::Responder(s)
        , _counter(c)
        {
        }
        
        virtual void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply,
                                    Pt::System::EventLoop& loop)
        {
          _request = &request;
          _reply = &reply;
          _loop = &loop;

          return onReadRequest(request, reply, loop);
        }
        
        virtual void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply,
                                   Pt::System::EventLoop& loop)
        {
          _timer.setActive(loop);
          _timer.timeout() += Pt::slot(*this, &HelloResponder::onTimeout);
          _timer.start(200);

          //onTimeout();
        }

        void onTimeout()
        {
          _timer.stop();

          char body[64] = {};
          _request->body().readsome( body, sizeof(body) );

          setReady(false);
        }

        virtual void onBeginReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                                  Pt::System::EventLoop& loop)
        { 
          return onWriteReply(request, reply, loop); 
        }

        virtual void onWriteReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                                  Pt::System::EventLoop& loop)
        {
            _timer.timeout().disconnect();
            _timer.timeout() += Pt::slot(*this, &HelloResponder::onTimeout2);
            _timer.start(200);

            //onTimeout2();
        }

        void onTimeout2()
        {
          _timer.stop();
          _reply->body() << "Hello World #" << _counter.count();
          
          setReady(true);
        }

    private:
        Counter&                _counter;
        Pt::System::Timer       _timer;
        Pt::Http::Request*      _request;
        Pt::Http::Reply*        _reply;
        Pt::System::EventLoop*  _loop;
};


class HelloService : public Pt::Http::Service
{
    public:
        HelloService()
        {}

        virtual Pt::Http::Responder* onGetResponder(const Pt::Http::Request&)
        {
            return new HelloResponder(*this, _counter);
        }
        
        virtual void onReleaseResponder(Pt::Http::Responder* r)
        {
            delete r;
        }

    private:
        Counter _counter;
};


class ChunkedResponder : public Pt::Http::Responder
{
    public:
        ChunkedResponder(Pt::Http::Service& s)
        : Pt::Http::Responder(s)
        , _chunks(5)
        {}
      
        virtual void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, 
                                    Pt::System::EventLoop& loop)
        { 
            _chunks = 5;
            setReady(false);
        }
        
        virtual void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, 
                                   Pt::System::EventLoop& loop)
        { 
            setReady(false); 
        }

        virtual void onBeginReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                                  Pt::System::EventLoop& loop)
        {
            return onWriteReply(request, reply, loop); 
        }

        virtual void onWriteReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                                  Pt::System::EventLoop& loop)
        {
            reply.body() << "Chunk" << _chunks--;

            bool isDone = _chunks == 0;
            setReady(isDone);
        }

    private:
        unsigned _chunks;
};

typedef Pt::Http::BasicService<ChunkedResponder> ChunkedService;


class ServerTest : public Pt::Unit::TestSuite
                 , public Pt::Connectable
{
    public:
        ServerTest()
        : Pt::Unit::TestSuite("ServerTest")
        , _authent("test-realm")
        {
            Pt::System::Logger::setLogLevel("Pt", Pt::System::Error);

            _authent.setUser( Pt::Http::Credential("testo", "testpwd") );

            registerMethod( "NotFound", *this, &ServerTest::NotFound);
#ifdef PT_HTTP_WITH_SSL
            registerMethod( "NotFoundHttps", *this, &ServerTest::NotFoundHttps);
#endif
            registerMethod( "BasicAuthentication", *this, &ServerTest::BasicAuthentication);
            registerMethod( "ReplyWithBody", *this, &ServerTest::ReplyWithBody);
            registerMethod( "ChunkedReply", *this, &ServerTest::ChunkedReply);
            registerMethod( "PipelinedRequests", *this,  &ServerTest::PipelinedRequests);
            registerMethod( "MaxRequestSize", *this, &ServerTest::MaxRequestSize);
            registerMethod( "QueryString", *this, &ServerTest::QueryString);
            
            //registerMethod("Upgrade", *this, &ServerTest::Upgrade);
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

            _loop = new Pt::System::MainLoop();

            _exitTimer.setActive(*_loop);
//            _exitTimer.start(10000);
            _exitTimer.timeout() += Pt::slot(*_loop, &Pt::System::EventLoop::exit);
        }

        void tearDown()
        {
            delete _loop;
        }

        void MaxRequestSize()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8001);
            HelloService service;
            
            Pt::Http::Server server(*_loop, ep);
            server.setMaxRequestSize(5);

            Pt::Http::MapUrl mapurl("/test", service);
            server.addServlet(mapurl);

            Pt::Http::Client client(*_loop);
            client.setHost(ep);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onMaxRequestSizeReply);
            client.request().setUrl("/test");
            client.request().body() << "Hello World";
            client.beginReceive();

            _loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 400);
        }

        void onMaxRequestSizeReply(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.finished() )
            {
                _loop->exit();
                return;
            }

            client.beginReceive();
        }

        void PipelinedRequests()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8001);

            HelloService service;
            Pt::Http::MapUrl mapurl("/test", service);

            Pt::Http::Server server(*_loop, ep);
            server.addServlet(mapurl);
            server.setKeepAliveTimeout(999999);
            server.setTimeout(999999);


            Pt::Http::Client client(*_loop);
            client.setHost(ep);
            client.requestSent() += Pt::slot(*this, &ServerTest::onPipelinedSent);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onPipelinedReceived);
            client.request().header().setKeepAlive();
            client.request().setUrl("/test");
            client.request().header().set("foo", "bar");
            PT_UNIT_ASSERT(client.request().header().has("foo") );
            client.beginSend(true);

            _loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Hello World #1Hello World #2");
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
                if( _reply == "Hello World #1Hello World #2")
                {
                    _loop->exit();
                    return;
                }
            }

            client.beginReceive();
        }

        void NotFound()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8001);
            
            Pt::Http::Server server(*_loop, ep);

            Pt::Http::Client client(*_loop);
            client.setHost(ep);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onNotFoundReceived);
            client.request().setUrl("/index.html");
            client.request().header().set("foo", "bar");
            client.beginReceive();

            _loop->run();
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
                _loop->exit();
                return;
            }

            client.beginReceive();
        }

        void NotFoundHttps()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8001);

            Pt::Ssl::Context serverCtx;
            setupSslServerContext(serverCtx);
            
            // NOTE: enable this to cause a server side handshake failure
            //serverCtx.setProtocol(Pt::Ssl::TLSv1);
            
            // start HTTP server          
            Pt::Http::Server server(*_loop);
            server.setSecure(serverCtx);
            server.listen(ep);

            Pt::Ssl::Context clientContext;
            setupSslClientContext(clientContext);
            
            // start HTTP client
            Pt::Http::Client client(*_loop);
            client.setHost(ep);
            client.setSecure(clientContext);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onNotFoundReceived);
            client.request().setUrl("/index.html");
            client.request().header().set("foo", "bar");
            client.beginReceive();

            _loop->run();

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

            Pt::Ssl::CertificateStore store;
            store.loadPkcs12(ifs, "123");
            store.loadPkcs12(ifs_ca, "123");

            ctx.setVerifyMode(Pt::Ssl::TryVerify);

            const Pt::Ssl::Certificate* clientCert = store.findCertificate("Atlantis Mainframe");
            PT_UNIT_ASSERT( clientCert);
            ctx.setIdentity( *clientCert );

            const Pt::Ssl::Certificate* clientCA = store.findCertificate("SGC Certificate Authority");
            PT_UNIT_ASSERT( clientCA);
            ctx.addCACertificate(*clientCA);
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

            Pt::Ssl::CertificateStore store;
            store.loadPkcs12(server_ifs, "123");
            store.loadPkcs12(ifs_ca, "123");

            ctx.setVerifyMode(Pt::Ssl::AlwaysVerify);

            const Pt::Ssl::Certificate* servCert = store.findCertificate("SGC Mainframe");
            PT_UNIT_ASSERT( servCert );
            ctx.setIdentity( *servCert );

            const Pt::Ssl::Certificate* servCA = store.findCertificate("SGC Certificate Authority");
            PT_UNIT_ASSERT( servCA );
            ctx.addCACertificate(*servCA);
        }

        void BasicAuthentication()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8001);

            Pt::Http::Server server(*_loop, ep);

            HelloService service;
            Pt::Http::MapUrl mapurl("/test", service, _authent);
            server.addServlet(mapurl);

            Pt::Http::Client client(*_loop);
            client.setHost(ep);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onBasicAuthenticationReceived);
            client.request().setUrl("/test");
            client.beginReceive();

            _loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Authorization Required Hello World #1");
        }

        void onBasicAuthenticationReceived(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();

            if( progress.header() )
            {
                if( client.reply().statusCode() == Pt::Http::Reply::Unauthorized)
                {
                    PT_UNIT_ASSERT(client.reply().header().has("WWW-Authenticate"));

                    Pt::Http::Authenticator auth;
                    auth.setCredential("test-realm", Pt::Http::Credential("testo", "testpwd"));
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
                    _loop->exit();
                    return;
                }

                PT_UNIT_ASSERT_EQUALS(_reply, "Authorization Required ");
            }

            client.beginReceive();
        }

        void ReplyWithBody()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8001);

            HelloService service;
            Pt::Http::Server server(*_loop, ep);

            Pt::Http::MapUrl mapurl("/test", service);
            server.addServlet(mapurl);

            Pt::Http::Client client(*_loop);
            client.setHost(ep);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onHelloReceived);
            client.request().setUrl("/test");
            client.request().header().set("foo", "bar");
            client.request().body() << "Hello";
            client.beginReceive();

            _loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "Hello World #1");
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
                _loop->exit();
                return;
            }

            client.beginReceive();
        }

        void QueryString()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8001);
            
            EchoQueryService service;

            Pt::Http::Server server(*_loop, ep);

            Pt::Http::MapUrl mapurl("/test", service);
            server.addServlet(mapurl);

            Pt::Http::Client client(*_loop);
            client.setHost(ep);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onQueryStringReceived);
            client.request().setUrl("/test");
            client.request().setQParams("a=4&b=Hello");
            client.beginReceive();

            _loop->run();
            PT_UNIT_ASSERT_EQUALS(client.reply().statusCode(), 200);
            PT_UNIT_ASSERT_EQUALS(_reply, "a=4&b=Hello");
        }

        void Upgrade()
        {
            /*
            Pt::Net::Endpoint ep("127.0.0.1", 80);

            WebSocketService service;

            Pt::Http::Server server(*_loop, ep);

            Pt::Http::MapUrl mapurl("/WebSocket", service);
            server.addServlet(mapurl);

            server.upgradeRequested() += Pt::slot(*this, &ServerTest::onUpgrade);

            _loop->run();
            */
        }

        Pt::Http::IOStream* _ioStream;

        void onUpgradeInput()
        {
            size_t s = _ioStream->endInput();
            char buffer[1024];
            _ioStream->read(buffer, s);

        }
        void onUpgrade(Pt::Http::IOStream* stream)
        {
            _ioStream = stream;
            _ioStream->inputReady() += Pt::slot(*this, &ServerTest::onUpgradeInput);
            _ioStream->beginInput();
            
        }

        void onQueryStringReceived(Pt::Http::Client& client)
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
                _loop->exit();
                return;
            }

            client.beginReceive();
        }

        void ChunkedReply()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8001);

            Pt::Http::Server server(*_loop, ep);

            ChunkedService service;
            Pt::Http::MapUrl servlet("/test", service);
            server.addServlet(servlet);

            Pt::Http::Client client(*_loop);
            client.setHost(ep);
            client.requestSent() += Pt::slot(*this, &ServerTest::onChunkedSent);
            client.replyReceived() += Pt::slot(*this, &ServerTest::onChunkedReceived);
            client.request().setUrl("/test");
            
            client.request().body() << _chunks.front();
            _chunks.erase( _chunks.begin() );
            client.beginSend(false);

            _loop->run();

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
                _loop->exit();
                return;
            }
            
            client.beginReceive();
        }

    private:
        Pt::System::Timer _exitTimer;
        Pt::System::MainLoop* _loop;
        std::string _reply;
        std::vector<std::string> _chunks;
        Pt::Http::BasicUserListAuthorizer _authent;
};

Pt::Unit::RegisterTest<ServerTest> register_HttpServerTest;
