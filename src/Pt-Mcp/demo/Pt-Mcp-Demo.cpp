/*
 * MCP (Model Context Protocol) Demo Server
 *
 * A minimal MCP server over stdio using Pt::Mcp::StdioService.
 * Exposes two tools: "add" (adds two integers) and "echo" (returns a string).
 *
 * Protocol: JSON-RPC 2.0 over stdio with Content-Length framing.
 */

#include <Pt/Mcp/StdioService.h>
#include <Pt/Mcp/ToolDeclaration.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <iostream>
#include <string>

// --- Tool implementations ---

int add(int a, int b)
{
    return a + b;
}

std::string echo(const std::string& msg)
{
    return msg;
}

int main()
{
    // Register procedures
    Pt::Remoting::ServiceDefinition serviceDef;
    serviceDef.registerProcedure("add", &add);
    serviceDef.registerProcedure("echo", &echo);

    // Declare tools with parameter schemas
    Pt::Mcp::ToolDeclaration decl("pt-mcp-demo", "1.0.0");

    decl.addTool("add", "Add two integers")
        .addParam("a", Pt::Mcp::integerType(), "First operand")
        .addParam("b", Pt::Mcp::integerType(), "Second operand");

    decl.addTool("echo", "Echo a string")
        .addParam("message", Pt::Mcp::stringType(), "The message to echo");

    // Create MCP service and run message loop
    Pt::Mcp::StdioService mcp(serviceDef, decl);

    while(true)
    {
        std::string msg = mcp.readMessage(std::cin);
        if(msg.empty())
            break;

        std::string response = mcp.dispatch(msg);

        if( ! response.empty())
            mcp.writeMessage(std::cout, response);
    }

    return 0;
}
