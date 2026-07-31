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

/** @brief MCP protocol service over stdio.

    Handles the full MCP protocol lifecycle: initialize, tools/list,
    and tools/call. Reads/writes Content-Length framed messages.
*/
class PT_MCP_API StdioService
{
  public:
    /** @brief Construct with a service definition, tool declaration and
        the event loop used to drive asynchronous service procedures.
    */
    StdioService(Remoting::ServiceDefinition& serviceDef,
                 const ToolDeclaration& decl,
                 System::EventLoop& loop);

    /** @brief Destructor.
    */
    ~StdioService();

    /** @brief Read a Content-Length framed message from the stream.

        Returns the JSON body, or an empty string on EOF.
    */
    std::string readMessage(std::istream& is);

    /** @brief Write a Content-Length framed message to the stream.
    */
    void writeMessage(std::ostream& os, const std::string& json);

    /** @brief Dispatch an MCP request and return the response JSON.

        Routes to initialize, tools/list, or tools/call handlers.
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
