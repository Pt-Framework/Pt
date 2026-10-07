/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSERVLET_H
#define PT_HTTP_WEBSOCKETSERVLET_H

#include <Pt/Http/Api.h>
#include <Pt/Connectable.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Http {

class Request;
class Reply;
class Stream;
class WebSocketService;
class WebSocketSession;

/** @brief Release scope of the sessions one WebSocket service creates.

    %WebSocketServlet holds the sessions a WebSocket service creates
    and releases them while that service is still fully constructed.
    It is not a server. The HTTP server already owns the connection
    and the stream. It is not a request-mapping servlet. This type
    does not map a URL. A URL mapping is still required so the
    handshake reaches the service. The service is the factory and the
    endpoint policy. This type asks the factory for a session, holds
    the session pointer, and releases it. It does not decide the
    session type, the allocator, or the domain teardown.

    The example registers the scope before the server starts taking
    upgrades. Destroy the servlet before the service.

    @code
    EchoService service;
    Pt::Http::WebSocketServlet sockets(service);
    Pt::Http::MapUrl mapUrl("/ws", service);
    server.addServlet(mapUrl);
    @endcode

    Construct it with the %WebSocketService and destroy it before
    that service. The constructor registers this servlet, so the
    next upgrade of that service is delivered here. The destructor
    releases every session it still holds while the derived service
    is still fully constructed, so the virtual release call and the
    allocator are still there. That order is required. The service
    destructor cannot release the sessions, because by then the
    derived object is already gone.

    One service has one registered servlet. A second servlet
    replaces the registration. The previous servlet keeps the
    sessions it already accepted and releases them itself. No
    registration, which is the state before the first servlet and
    after the last destructor, declines the next upgrade. The HTTP
    server closes a stream that has no bound session. When a stream
    ends, the session close callback runs and this servlet releases
    that session.

    @ingroup Pt-Http-WebSocket-Server
*/
class PT_HTTP_API WebSocketServlet : public Connectable
                                   , private NonCopyable
{
    friend class WebSocketService;
    friend class WebSocketSession;

    public:
        /** @brief Registers this servlet with @a service.
        */
        explicit WebSocketServlet(WebSocketService& service);

        /** @brief Releases every session this servlet still holds.
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

        /** @brief Returns the number of sessions this servlet holds.
        */
        std::size_t size() const
        { return _sessions.size(); }

    private:
        void onUpgrade(Stream& stream,
                       const Request& request,
                       const Reply& reply);

        void onSessionClosed(WebSocketSession& session);

        void releaseSession(WebSocketSession* session);

        WebSocketService* _service;
        std::vector<WebSocketSession*> _sessions;
};

} // namespace Http

} // namespace Pt

#endif
