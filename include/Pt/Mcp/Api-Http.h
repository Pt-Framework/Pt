/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_API_HTTP_H
#define PT_MCP_API_HTTP_H

/** @addtogroup Pt-Mcp-Http

    @brief Serve MCP tools over HTTP.

    %HttpService is an %Pt::Http::Service that makes one
    %ToolDeclaration and one %Pt::Remoting::ServiceDefinition
    available as an HTTP resource. It does not listen. An
    %Pt::Http::Server listens, and an %Pt::Http::MapUrl (or another
    servlet) maps a path to this service. Each request gets a
    responder that runs initialize, tools/list, or tools/call.

    Only POST is accepted. A different method is 405. When an Origin
    header is present, its host must match the Host header; a mismatch
    is 403. After initialize, a request may send MCP-Protocol-Version;
    an unsupported value is 400. A notification (a JSON-RPC request
    without an id) is answered with 202 and no body.

    The service does not own the definition or the declaration. Both
    must outlive the service, and the service must outlive every
    responder it still has in flight, as any %Pt::Http::Service must.
    The HTTP server is asynchronous, so it needs an %EventLoop. Tool
    procedures that are asynchronous run on that loop.

    The example is the mapping. The server, the service, and the
    declaration come from the module chapter.

    @code
    Pt::Mcp::HttpService mcpService(serviceDef, decl);
    Pt::Http::MapUrl servlet("/mcp", mcpService);
    server.addServlet(servlet);
    @endcode
*/

#endif
