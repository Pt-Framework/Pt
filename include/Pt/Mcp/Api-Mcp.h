/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_API_MCP_H
#define PT_MCP_API_MCP_H

/** @defgroup Pt-Mcp Model Context Protocol

    @brief MCP servers that expose C++ procedures as tools.

    This module is an MCP server, so a caller registers ordinary C++
    procedures and serves them as tools that a client can list and
    call. It is not an MCP client. The unit of work is a tool: a named
    procedure, a JSON Schema for its parameters, and a content type
    for its result. JSON-RPC is the envelope. %Pt::Remoting supplies
    the procedure table. This module adds the MCP methods, the tool
    schemas, and the transports.

    Work proceeds in four steps. A %Pt::Remoting::ServiceDefinition
    holds the C++ functions or methods. A %ToolDeclaration holds the
    matching tool names, descriptions, and parameter schemas. Each
    tool may name a %ContentType for its result; the default is text.
    A transport then serves that pair: %HttpService on an HTTP server,
    or %Service / %StdioService on standard streams.

    The tool name in the declaration must be the procedure name in the
    definition. Arguments and results must be serializable, as in the
    other remoting modules: a type that can be written to
    %SerializationInfo can be a tool parameter or a return value.
    Custom types are welcome when they have that support.

    The declaration and the definition are not owned by the transport.
    Both must outlive the %HttpService, %Service, or %StdioService that
    uses them. A %Type named by a tool parameter must outlive that
    tool. Primitive schema types from %integerType() and the other
    factories are process-wide singletons. Composed types
    (%ObjectType, %ArrayType, %EnumType, %NullableType) are owned by
    the caller. %ToolDeclaration owns the %Tool objects it creates.
    A %ContentType used with %Tool::setContent() must outlive the tool;
    %textContent() and %imageContent() are process-wide singletons.

    The protocol methods are initialize, tools/list, and tools/call.
    Initialize negotiates a protocol version and returns the server
    name and version from the declaration. Tools/list returns the
    declared tools and their input schemas. Tools/call looks up the
    tool, maps named arguments onto the procedure, and writes the
    result as MCP content. A request without an id is a notification
    and produces no JSON-RPC body.

    Supported protocol versions are 2025-11-25 and 2025-03-26.
    %ToolDeclaration::preferredVersion() returns the requested version
    when it is supported, otherwise the latest. A tool that throws
    %Pt::JsonRpc::Fault or %Pt::Remoting::Fault becomes an MCP error
    response. Other failures are reported as JSON-RPC errors by the
    transport.

    The example registers two procedures, declares the matching tools
    with schemas, and serves them over HTTP. The stdio path is the
    same declaration and definition with %StdioService.

    @code
    class DemoDefinition : public Pt::Remoting::ServiceDefinition
    {
      public:
        DemoDefinition()
        {
            registerProcedure("add", *this, &DemoDefinition::add);
            registerProcedure("echo", *this, &DemoDefinition::echo);
        }

      private:
        int add(int a, int b)
        { return a + b; }

        std::string echo(const std::string& msg)
        { return msg; }
    };

    DemoDefinition defn;

    Pt::Mcp::ToolDeclaration decl("pt-mcp-demo", "1.0.0");
    decl.addTool("add", "Add two integers")
        .addParam("a", Pt::Mcp::integerType(), "First operand")
        .addParam("b", Pt::Mcp::integerType(), "Second operand");
    decl.addTool("echo", "Echo a string")
        .addParam("message", Pt::Mcp::stringType(), "The message");

    Pt::System::MainLoop loop;
    Pt::Http::Server server(loop, Pt::Net::Endpoint::ip4Any(8079));
    Pt::Mcp::HttpService mcpService(defn, decl);
    Pt::Http::MapUrl servlet("/mcp", mcpService);
    server.addServlet(servlet);
    loop.run();
    @endcode

    The rest of this chapter is parameter schemas, then tool
    declarations, then result content, then the HTTP transport, then
    standard I/O.
*/

/** @defgroup Pt-Mcp-Types Parameter Schemas

    @ingroup Pt-Mcp
*/

/** @defgroup Pt-Mcp-Tools Tools and Server Declaration

    @ingroup Pt-Mcp
*/

/** @defgroup Pt-Mcp-Content Result Content

    @ingroup Pt-Mcp
*/

/** @defgroup Pt-Mcp-Http HTTP Transport

    @ingroup Pt-Mcp
*/

/** @defgroup Pt-Mcp-Stdio Standard I/O Transport

    @ingroup Pt-Mcp
*/

#endif
