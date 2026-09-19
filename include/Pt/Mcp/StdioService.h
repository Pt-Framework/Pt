/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_STDIOSERVICE_H
#define PT_MCP_STDIOSERVICE_H

#include <Pt/Mcp/Api.h>
#include <Pt/Mcp/ToolDeclaration.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <Pt/System/Api.h>
#include <string>
#include <iosfwd>

namespace Pt {

namespace Mcp {

/** @brief MCP service over standard streams with an event loop.

    %StdioService is the stdio transport that can run asynchronous
    tool procedures. Framing and dispatch are the same as %Service:
    %readMessage(), %writeMessage(), and %dispatch() on Content-Length
    framed JSON-RPC. The extra argument is an %EventLoop.
    %dispatch() starts the procedure and, when the procedure is
    asynchronous, runs that loop until the result is ready.

    Use this type when a tool is an asynchronous remoting procedure.
    Use %Service when every tool completes on the calling thread. The
    loop is not owned; it must outlive this service. The
    %Pt::Remoting::ServiceDefinition and the %ToolDeclaration are not
    owned either.

    A typical server reads from stdin and writes to stdout until
    %readMessage() returns empty. %dispatch() returns an empty string
    for a notification, and that string is not written.

    @ingroup Pt-Mcp-Stdio
*/
class PT_MCP_API StdioService
{
  public:
    /** @brief Creates a service for @a serviceDef, @a decl and @a loop.

        @a loop drives asynchronous service procedures.
    */
    StdioService(Remoting::ServiceDefinition& serviceDef,
                 const ToolDeclaration& decl,
                 System::EventLoop& loop);

    /** @brief Destroys the service.
    */
    ~StdioService();

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
    System::EventLoop& _loop;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_STDIOSERVICE_H
