/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSERVLET_H
#define PT_HTTP_WEBSOCKETSERVLET_H

#include <Pt/Http/Api.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <cstddef>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Http {

class Request;
class Reply;
class Stream;
class WebSocketService;
class WebSocketSession;

/** @brief Owner of the sessions one WebSocket service creates.

    %WebSocketServlet owns the live sessions of one %WebSocketService.
    The service owns the endpoint policy and the allocator, and stays
    the mapped HTTP service and the factory. Destroy this servlet
    before the service, so each remaining session is released while
    the derived service, its virtual release call, and its allocator
    are still available. It is not a server and does not map a URL.
    A URL mapping is still required so the handshake reaches the
    service.

    The example registers the servlet before the server starts taking
    upgrades. The three setters are endpoint policy of the service.
    They apply before this servlet is constructed. Destroy the servlet
    before the service.

    @code
    EchoService service;
    service.setMaxSockets(100);
    service.setIdleTimeout(60000);
    service.setMaxMessageSize(1 << 20);
    Pt::Http::WebSocketServlet sockets(service);
    Pt::Http::MapUrl mapUrl("/ws", service);
    server.addServlet(mapUrl);
    @endcode

    Construct it with the %WebSocketService and destroy it before
    that service. The constructor attaches this servlet, so the
    handshake can accept upgrades. The destructor detaches it and
    releases every remaining session through %releaseSession().
    That order is required. The service destructor cannot release
    sessions, because by then the derived object is already gone.

    One service has one active servlet. Constructing a second
    %WebSocketServlet for the same service throws %std::logic_error.
    With no servlet, the handshake answers the upgrade with 503 and
    opens no stream. The service owns %setMaxSockets(),
    %setIdleTimeout() and %setMaxMessageSize(). The handshake reads
    the session limit and %socketCount() from the service.
    %socketCount() is the number of sessions %getSession() returned
    and %releaseSession() has not yet released. The session base
    copies the idle timeout and the data-message limit from the
    service onto the connection before the derived constructor runs.

    @ingroup Pt-Http-WebSocket-Server
*/
class PT_HTTP_API WebSocketServlet : private NonCopyable
{
    friend class WebSocketService;

    public:
        /** @brief Attaches this servlet to @a service.

            @throw %std::logic_error if @a service already has an
            active servlet.
        */
        explicit WebSocketServlet(WebSocketService& service);

        /** @brief Detaches this servlet and releases its remaining sessions.
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

        /** @brief Returns the number of live sessions.
        */
        std::size_t size() const
        { return _sessions.size(); }

    private:
        void accept(Stream& stream, const Request& request, const Reply& reply);

        void close(WebSocketSession& session);

    private:
        WebSocketService* _service;
        std::vector<WebSocketSession*> _sessions;
};

} // namespace Http

} // namespace Pt

#endif
