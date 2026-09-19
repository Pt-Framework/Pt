/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_SERVICE_H
#define PT_MCP_SERVICE_H

#include <Pt/Mcp/Api.h>
#include <Pt/Mcp/ToolDeclaration.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <string>
#include <iosfwd>

namespace Pt {

namespace Mcp {

/** @brief Synchronous MCP service over standard streams.

    %Service is the synchronous stdio transport. It reads and writes
    Content-Length framed JSON-RPC messages and dispatches initialize,
    tools/list, and tools/call on the calling thread. Use it when every
    tool is a synchronous procedure. Asynchronous procedures need
    %StdioService and an %EventLoop.

    %readMessage() consumes one framed message from a stream and
    returns the JSON body, or an empty string on EOF.
    %writeMessage() writes a Content-Length header and the JSON body.
    %dispatch() runs the request and returns the response JSON, or an
    empty string for a notification.

    The %Pt::Remoting::ServiceDefinition and the %ToolDeclaration are
    not owned. Both must outlive this service.

    @ingroup Pt-Mcp-Stdio
*/
class PT_MCP_API Service
{
  public:
    /** @brief Creates a service for @a serviceDef and @a decl.
    */
    Service(Remoting::ServiceDefinition& serviceDef,
            const ToolDeclaration& decl);

    /** @brief Destroys the service.
    */
    ~Service();

    /** @brief Reads one Content-Length framed message from @a is.

        Returns the JSON body, or an empty string on EOF.
    */
    std::string readMessage(std::istream& is);

    /** @brief Writes a Content-Length framed message @a json to @a os.
    */
    void writeMessage(std::ostream& os, const std::string& json);

    /** @brief Dispatches an MCP request and returns the response JSON.

        Returns an empty string for notifications (no id).
    */
    std::string dispatch(const std::string& json);

  private:
    std::string dispatchToolsCall(const std::string& json);
    std::string dispatchInitialize(const std::string& json);
    std::string dispatchToolsList(const std::string& json);

    static std::string extractMethod(const std::string& json);
    static Pt::int64_t extractId(const std::string& json);

    Remoting::ServiceDefinition* _serviceDef;
    const ToolDeclaration* _decl;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_SERVICE_H
