/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_API_TOOLS_H
#define PT_MCP_API_TOOLS_H

/** @addtogroup Pt-Mcp-Tools

    @brief Declare an MCP server and its tools.

    %ToolDeclaration is the server-facing catalog. It holds the server
    name and version that initialize returns, and the tools that
    tools/list returns. %Tool is one entry in that catalog: a name, a
    description, a list of parameters, and a content type for the
    result.

    %addTool() creates a %Tool owned by the declaration and returns it
    so parameters can be chained. The tool name must match a procedure
    already registered, or later registered, on the
    %Pt::Remoting::ServiceDefinition the transport will use. The
    declaration does not look up that procedure; a missing or
    mismatched name fails at tools/call.

    %addParam() appends a parameter. The name is the JSON member the
    client sends and the name used to find the positional argument of
    the procedure. The %Type is the schema. The description is copied
    into that schema. Parameters are required until %setOptional() is
    called with the same name. %setContent() selects how the result is
    written; the default is %textContent().

    %toInitializeResult() writes the initialize payload, including the
    negotiated protocol version. %toToolsList() writes the tools/list
    payload from the declared tools. Transports call both. Supported
    versions are 2025-11-25 and 2025-03-26.
    %preferredVersion() returns the requested version when it is in
    that set, otherwise the latest. %isSupportedVersion() tests a
    version string.

    The declaration owns every %Tool it creates and deletes them in
    its destructor. It does not own the %Type objects or the
    %ContentType a tool refers to. The declaration is not copyable.
*/

#endif
