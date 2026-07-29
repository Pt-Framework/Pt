/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/Unit/TestMain.h>
#include <Pt/Mcp/HttpService.h>
#include <Pt/Mcp/ToolDeclaration.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <Pt/Http/Server.h>
#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Servlet.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>
#include <Pt/SerializationInfo.h>
#include <string>
#include <sstream>


namespace Pt {

namespace Mcp {

namespace {

    /** @brief Minimal test payload that decomposes to a single Binary node. */
    struct PngImage
    {
        std::string bytes;
    };

    void operator<<=(Pt::SerializationInfo& si, const PngImage& img)
    {
        si.setBinary(img.bytes.data(), img.bytes.size());
    }

} // anonymous namespace

class HttpTest : public Pt::Unit::TestSuite
              , public Pt::Connectable
{
  private:
    Pt::System::Timer _exitTimer;
    Pt::System::MainLoop* _loop;
    Pt::Http::Server* _server;
    std::string _reply;

  public:
    HttpTest()
    : Pt::Unit::TestSuite("Pt::Mcp::HttpTest")
    {
        Pt::System::Logger::setLogLevel("Pt", Pt::System::Error);

        registerMethod("Initialize", *this, &HttpTest::Initialize);
        registerMethod("ToolsList", *this, &HttpTest::ToolsList);
        registerMethod("ToolsCall", *this, &HttpTest::ToolsCall);
        registerMethod("ToolsCallArgumentsFirst", *this, &HttpTest::ToolsCallArgumentsFirst);
        registerMethod("ToolsCallUnknownArgument", *this, &HttpTest::ToolsCallUnknownArgument);
        registerMethod("Ping", *this, &HttpTest::Ping);
        registerMethod("Notification", *this, &HttpTest::Notification);
        registerMethod("GetRequest", *this, &HttpTest::GetRequest);
        registerMethod("InvalidOrigin", *this, &HttpTest::InvalidOrigin);
        registerMethod("VersionNegotiation", *this, &HttpTest::VersionNegotiation);
        registerMethod("InvalidMcpVersionHeader", *this, &HttpTest::InvalidMcpVersionHeader);
        registerMethod("ImageContent", *this, &HttpTest::ImageContent);
    }

    void failTest()
    {
        throw Pt::Unit::Assertion("test timed out", PT_SOURCEINFO);
    }

    void setUp()
    {
        _loop = new Pt::System::MainLoop();
        _reply.clear();

        _exitTimer.setActive(*_loop);
        _exitTimer.start(10000);
        _exitTimer.timeout() += Pt::slot(*this, &HttpTest::failTest);
        _exitTimer.timeout() += Pt::slot(*_loop, &Pt::System::MainLoop::exit);

        Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Any(8079);
        _server = new Pt::Http::Server(*_loop, ep);
    }

    void tearDown()
    {
        delete _server;
        _server = 0;
        delete _loop;
        _loop = 0;
    }

    ////////////////////////////////////////////////////////////
    // Initialize
    //
    void Initialize()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        serviceDef.registerProcedure("add", *this, &HttpTest::addInt);

        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        decl.addTool("add", "Add two integers")
            .addParam("a", Pt::Mcp::integerType())
            .addParam("b", Pt::Mcp::integerType());

        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"initialize\",\"id\":1,\"params\":{}}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 200);
        PT_UNIT_ASSERT(_reply.find("\"protocolVersion\"") != std::string::npos);
        PT_UNIT_ASSERT(_reply.find("\"test-server\"") != std::string::npos);

        delete _server;
        _server = 0;
    }

    ////////////////////////////////////////////////////////////
    // ToolsList
    //
    void ToolsList()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        serviceDef.registerProcedure("add", *this, &HttpTest::addInt);

        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        decl.addTool("add", "Add two integers")
            .addParam("a", Pt::Mcp::integerType())
            .addParam("b", Pt::Mcp::integerType());

        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"tools/list\",\"id\":2,\"params\":{}}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 200);
        PT_UNIT_ASSERT(_reply.find("\"add\"") != std::string::npos);
        PT_UNIT_ASSERT(_reply.find("\"Add two integers\"") != std::string::npos);

        delete _server;
        _server = 0;
    }

    ////////////////////////////////////////////////////////////
    // ToolsCall
    //
    void ToolsCall()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        serviceDef.registerProcedure("add", *this, &HttpTest::addInt);

        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        decl.addTool("add", "Add two integers")
            .addParam("a", Pt::Mcp::integerType())
            .addParam("b", Pt::Mcp::integerType());

        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"tools/call\",\"id\":3,"
               "\"params\":{\"name\":\"add\",\"arguments\":{\"a\":6,\"b\":7}}}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 200);
        PT_UNIT_ASSERT(_reply.find("\"isError\":false") != std::string::npos);
        PT_UNIT_ASSERT(_reply.find("13") != std::string::npos);

        delete _server;
        _server = 0;
    }

    void ToolsCallArgumentsFirst()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        serviceDef.registerProcedure("add", *this, &HttpTest::addInt);

        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        decl.addTool("add", "Add two integers")
            .addParam("a", Pt::Mcp::integerType())
            .addParam("b", Pt::Mcp::integerType());

        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"tools/call\",\"id\":10,"
               "\"params\":{\"arguments\":{\"a\":6,\"b\":7},\"name\":\"add\"}}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 200);
        PT_UNIT_ASSERT(_reply.find("\"isError\":false") != std::string::npos);
        PT_UNIT_ASSERT(_reply.find("13") != std::string::npos);

        delete _server;
        _server = 0;
    }

    void ToolsCallUnknownArgument()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        serviceDef.registerProcedure("add", *this, &HttpTest::addInt);

        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        decl.addTool("add", "Add two integers")
            .addParam("a", Pt::Mcp::integerType())
            .addParam("b", Pt::Mcp::integerType());

        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"tools/call\",\"id\":11,"
               "\"params\":{\"name\":\"add\",\"arguments\":{\"a\":6,\"c\":7}}}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 200);
        PT_UNIT_ASSERT(_reply.find("\"error\"") != std::string::npos);
        PT_UNIT_ASSERT(_reply.find("unknown argument") != std::string::npos);

        delete _server;
        _server = 0;
    }

    ////////////////////////////////////////////////////////////
    // Ping
    //
    void Ping()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        serviceDef.registerProcedure("add", *this, &HttpTest::addInt);

        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        decl.addTool("add", "Add two integers")
            .addParam("a", Pt::Mcp::integerType())
            .addParam("b", Pt::Mcp::integerType());

        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"ping\",\"id\":4}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 200);
        PT_UNIT_ASSERT(_reply.find("\"result\":{}") != std::string::npos);

        delete _server;
        _server = 0;
    }

    ////////////////////////////////////////////////////////////
    // Notification
    //
    void Notification()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        serviceDef.registerProcedure("add", *this, &HttpTest::addInt);

        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        decl.addTool("add", "Add two integers")
            .addParam("a", Pt::Mcp::integerType())
            .addParam("b", Pt::Mcp::integerType());

        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"notifications/initialized\"}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 202);
        PT_UNIT_ASSERT(_reply.empty());

        delete _server;
        _server = 0;
    }

    ////////////////////////////////////////////////////////////
    // GetRequest — GET must return 405 Method Not Allowed (no SSE support)
    //
    void GetRequest()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("GET");
        client.request().setUrl("/mcp");
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 405);

        delete _server;
        _server = 0;
    }

    ////////////////////////////////////////////////////////////
    // InvalidOrigin — mismatched Origin header must return 403 Forbidden
    //
    void InvalidOrigin()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().header().set("Origin", "http://evil.example.com");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"initialize\",\"id\":1,\"params\":{}}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 403);

        delete _server;
        _server = 0;
    }

    ////////////////////////////////////////////////////////////
    // VersionNegotiation — server echoes back a supported requested version
    //
    void VersionNegotiation()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"initialize\",\"id\":1,"
               "\"params\":{\"protocolVersion\":\"2025-03-26\","
               "\"capabilities\":{},\"clientInfo\":{\"name\":\"test\",\"version\":\"1.0\"}}}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 200);
        PT_UNIT_ASSERT(_reply.find("\"protocolVersion\":\"2025-03-26\"") != std::string::npos);

        delete _server;
        _server = 0;
    }

    ////////////////////////////////////////////////////////////
    // InvalidMcpVersionHeader — unsupported MCP-Protocol-Version must return 400
    //
    void InvalidMcpVersionHeader()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        serviceDef.registerProcedure("add", *this, &HttpTest::addInt);

        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        decl.addTool("add", "Add two integers")
            .addParam("a", Pt::Mcp::integerType())
            .addParam("b", Pt::Mcp::integerType());

        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().header().set("MCP-Protocol-Version", "1.0.0-invalid");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"tools/list\",\"id\":2,\"params\":{}}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 400);

        delete _server;
        _server = 0;
    }

    ////////////////////////////////////////////////////////////
    // ImageContent
    //
    void ImageContent()
    {
        Pt::Remoting::ServiceDefinition serviceDef;
        serviceDef.registerProcedure("getImage", *this, &HttpTest::getImage);

        Pt::Mcp::ToolDeclaration decl("test-server", "1.0.0");
        decl.addTool("getImage", "Return a test image")
            .setContent(Pt::Mcp::imageContent());

        Pt::Mcp::HttpService mcpService(serviceDef, decl);
        Pt::Http::MapUrl servlet("/mcp", mcpService);
        _server->addServlet(servlet);

        Pt::Http::Client client(*_loop);
        client.setHost( Pt::Net::Endpoint::ip4Loopback(8079) );
        client.replyReceived() += Pt::slot(*this, &HttpTest::onReplyReceived);
        client.request().setMethod("POST");
        client.request().setUrl("/mcp");
        client.request().body()
            << "{\"jsonrpc\":\"2.0\",\"method\":\"tools/call\",\"id\":20,"
               "\"params\":{\"name\":\"getImage\",\"arguments\":{}}}";
        client.beginReceive();

        _loop->run();

        PT_UNIT_ASSERT(client.reply().statusCode() == 200);
        PT_UNIT_ASSERT(_reply.find("\"isError\":false") != std::string::npos);
        PT_UNIT_ASSERT(_reply.find("\"type\":\"image\"") != std::string::npos);
        PT_UNIT_ASSERT(_reply.find("\"mimeType\":\"image/png\"") != std::string::npos);
        PT_UNIT_ASSERT(_reply.find("\"data\":\"UE5HREFUQQ==\"") != std::string::npos);

        delete _server;
        _server = 0;
    }

  private:
    void onReplyReceived(Pt::Http::Client& client)
    {
        Pt::Http::MessageProgress progress = client.endReceive();

        if( progress.body() )
        {
            while( client.reply().body().rdbuf()->in_avail() )
                _reply += static_cast<char>( client.reply().body().get() );
        }

        if( progress.finished() )
        {
            _loop->exit();
            return;
        }

        client.beginReceive();
    }

    int addInt(int a, int b)
    {
        return a + b;
    }

    PngImage getImage()
    {
        PngImage img;
        img.bytes = "PNGDATA";
        return img;
    }
};

Pt::Unit::RegisterTest<HttpTest> register_McpHttpTest;

} // namespace Mcp

} // namespace Pt
