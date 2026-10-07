/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSERVLET_H
#define PT_HTTP_WEBSOCKETSERVLET_H

#include <Pt/Http/Api.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Http {

class WebSocketService;

/** @brief Release scope of the sessions one WebSocket service creates.

    %WebSocketServlet establishes the sole active release scope of a
    %WebSocketService. The service owns its live sessions and keeps a
    non-owning pointer to this scope. Destroy this scope before the
    service, so the service can release every remaining session while
    its derived type, virtual release call, and allocator are still
    available. It is not a server and does not map a URL. A URL
    mapping is still required so the handshake reaches the service.

    The example registers the scope before the server starts taking
    upgrades. Destroy the servlet before the service.

    @code
    EchoService service;
    Pt::Http::WebSocketServlet sockets(service);
    Pt::Http::MapUrl mapUrl("/ws", service);
    server.addServlet(mapUrl);
    @endcode

    Construct it with the %WebSocketService and destroy it before
    that service. The constructor activates the scope, so the service
    accepts upgrades and creates sessions through %onGetSession().
    The destructor ends the scope and asks the service to release its
    remaining sessions. That order is required. The service destructor
    cannot release sessions, because by then the derived object is
    already gone.

    One service has one active scope. Constructing a second
    %WebSocketServlet for the same service throws %std::logic_error.
    Before the first scope and after the last scope, the service
    declines the next upgrade by leaving its stream unbound. The HTTP
    server closes that stream. When a stream ends, %onClose() runs
    while the session is alive and the service then releases it.

    @ingroup Pt-Http-WebSocket-Server
*/
class PT_HTTP_API WebSocketServlet : private NonCopyable
{
    public:
        /** @brief Activates this scope for @a service.

            @throw %std::logic_error if @a service already has an
            active scope.
        */
        explicit WebSocketServlet(WebSocketService& service);

        /** @brief Ends this scope and releases the service's remaining sessions.
        */
        ~WebSocketServlet();

        /** @brief Returns the service this servlet was constructed with.
        */
        WebSocketService& service()
        { return *_service; }

        /** @brief Returns the service this servlet was constructed with.
        */
        const WebSocketService& service() const
        { return *_service; }

    private:
        WebSocketService* _service;
};

} // namespace Http

} // namespace Pt

#endif
