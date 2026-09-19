/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_HTTPSERVICE_H
#define PT_MCP_HTTPSERVICE_H

#include <Pt/Mcp/Api.h>
#include <Pt/Http/Service.h>
#include <Pt/Types.h>

namespace Pt {

namespace Remoting {
    class ServiceDefinition;
}

namespace Mcp {

class ToolDeclaration;

/** @brief HTTP service that serves declared MCP tools.

    %HttpService is the HTTP transport the HTTP group described. It is
    an %Pt::Http::Service: the server asks it for a responder, and
    that responder handles one POST as initialize, tools/list, or
    tools/call. Attach it to an %Pt::Http::Server with
    %Pt::Http::MapUrl or another servlet. It does not listen and it
    does not implement MCP on GET.

    The constructor stores the %Pt::Remoting::ServiceDefinition and
    the %ToolDeclaration. Neither is owned. Both must outlive this
    service, and this service must outlive any responder it has not
    yet released. Asynchronous tool procedures run on the server's
    %EventLoop.

    @ingroup Pt-Mcp-Http
*/
class PT_MCP_API HttpService : public Http::Service
{
  public:
    /** @brief Creates a service for @a serviceDef and @a decl.
    */
    HttpService(Remoting::ServiceDefinition& serviceDef,
                const ToolDeclaration& decl);

    /** @brief Destroys the service.
    */
    virtual ~HttpService();

  protected:
    // inheritdoc
    virtual Http::Responder* onGetResponder(const Http::Request& request);

    // inheritdoc
    virtual void onReleaseResponder(Http::Responder* resp);

  private:
    Remoting::ServiceDefinition* _serviceDef;
    const ToolDeclaration* _decl;
    Pt::varint_t _r1;
    Pt::varint_t _r2;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_HTTPSERVICE_H
