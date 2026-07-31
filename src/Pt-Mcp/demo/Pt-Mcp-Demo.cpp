/*
 * MCP (Model Context Protocol) Demo Server
 *
 * A unified MCP server that can run over HTTP or stdio.
 * Exposes two tools: "add" (adds two integers) and "echo" (returns a string).
 *
 * Default: HTTP server listening on http://localhost:8079/mcp
 * With --stdio: JSON-RPC 2.0 over stdio with Content-Length framing.
 *
 * Usage:
 *   Pt-Mcp-Demo.exe              (HTTP mode)
 *   Pt-Mcp-Demo.exe --stdio      (stdio mode)
 */

#include <Pt/Mcp/HttpService.h>
#include <Pt/Mcp/StdioService.h>
#include <Pt/Mcp/ToolDeclaration.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <Pt/Http/Server.h>
#include <Pt/Http/Servlet.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>
#include <Pt/Arg.h>
#include <iostream>
#include <string>


class McpDemoDefinition : public Pt::Remoting::ServiceDefinition
{
    public:
        McpDemoDefinition()
        {
            registerProcedure("add", *this, &McpDemoDefinition::add);
            registerProcedure("echo", *this, &McpDemoDefinition::echo);
        }

    private:
        int add(int a, int b)
        {
            return a + b;
        }

        std::string echo(const std::string& msg)
        {
            return msg;
        }
};


void runHttpMode(Pt::Remoting::ServiceDefinition& serviceDef,
                 Pt::Mcp::ToolDeclaration& decl)
{
    const std::size_t timeoutMs = 300000;

    Pt::System::MainLoop loop;

    Pt::Net::Endpoint ep = Pt::Net::Endpoint::ip4Any(8079);
    Pt::Http::Server server(loop, ep);
    server.setTimeout(timeoutMs);
    server.setKeepAliveTimeout(timeoutMs);

    Pt::Mcp::HttpService mcpService(serviceDef, decl);
    Pt::Http::MapUrl servlet("/mcp", mcpService);
    server.addServlet(servlet);

    std::cout << "MCP HTTP server listening on http://localhost:8079/mcp\n";
    std::cout << "Press Ctrl-C to stop.\n";

    loop.run();
}


void runStdioMode(Pt::Remoting::ServiceDefinition& serviceDef,
                  Pt::Mcp::ToolDeclaration& decl)
{
    Pt::System::MainLoop loop;
    Pt::Mcp::StdioService mcp(serviceDef, decl, loop);

    while(true)
    {
        std::string msg = mcp.readMessage(std::cin);
        if(msg.empty())
            break;

        std::string response = mcp.dispatch(msg);

        if( ! response.empty())
            mcp.writeMessage(std::cout, response);
    }
}


int main(int argc, char* argv[])
{
    try
    {
        Pt::System::Logger::setLogLevel("Pt", Pt::System::Info);

        McpDemoDefinition defn;

        Pt::Mcp::ToolDeclaration decl("pt-mcp-demo", "1.0.0");
        decl.addTool("add", "Add two integers")
            .addParam("a", Pt::Mcp::integerType(), "First operand")
            .addParam("b", Pt::Mcp::integerType(), "Second operand");
        decl.addTool("echo", "Echo a string")
            .addParam("message", Pt::Mcp::stringType(), "The message to echo");

        Pt::Arg<bool> useStdio(argc, argv, "--stdio");

        if(useStdio)
        {
            runStdioMode(defn, decl);
        }
        else
        {
            runHttpMode(defn, decl);
        }

        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 1;
}
