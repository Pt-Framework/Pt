/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_API_STDIO_H
#define PT_MCP_API_STDIO_H

/** @addtogroup Pt-Mcp-Stdio

    @brief Serve MCP tools over standard streams.

    Standard I/O is the local transport. Messages are JSON-RPC bodies
    framed with a Content-Length header and a blank line, as MCP
    clients use on stdin and stdout. %readMessage() consumes one
    framed message and returns the JSON body, or an empty string on
    EOF. %writeMessage() writes the header and the body.
    %dispatch() runs initialize, tools/list, or tools/call on that
    body and returns the response JSON, or an empty string for a
    notification.

    %Service is the synchronous form. It takes a
    %Pt::Remoting::ServiceDefinition and a %ToolDeclaration.
    %dispatch() runs the procedure on the calling thread. Use it when
    every tool is a synchronous procedure.

    %StdioService is the same framing and the same methods, with an
    %EventLoop. Asynchronous service procedures need that loop:
    %dispatch() starts the procedure and runs the loop until the
    result is ready. The loop is not owned. Keep it alive for as long
    as the service is used.

    Neither type owns the definition or the declaration. A typical
    stdio server reads from %std::cin and writes to %std::cout in a
    loop until %readMessage() returns empty.

    @code
    Pt::System::MainLoop loop;
    Pt::Mcp::StdioService mcp(serviceDef, decl, loop);

    for(;;)
    {
        std::string msg = mcp.readMessage(std::cin);
        if(msg.empty())
            break;

        std::string response = mcp.dispatch(msg);
        if( ! response.empty())
            mcp.writeMessage(std::cout, response);
    }
    @endcode
*/

#endif
