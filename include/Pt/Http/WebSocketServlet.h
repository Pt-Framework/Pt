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

class Stream;
class WebSocketService;
class WebSocketSession;

/** @brief Release scope of the sessions one WebSocket service creates.

    %WebSocketServlet holds the sessions a %WebSocketService creates
    and releases them while that service is still fully constructed.
    It is not a server. The HTTP %Server already owns the connection
    and the stream. It is not a %Servlet. A %Servlet maps a request
    to a service and stays registered for that mapping. This type
    does not map a URL. A %MapUrl is still required so the handshake
    reaches the service.

    The service is the factory and the endpoint policy. This type
    asks the factory for a session, holds the session pointer, and
    releases it. It does not decide the session type, the allocator,
    or the domain teardown. Those stay in the derived service, the
    same way a responder's allocator stays in its service.

    Construct it with the service and destroy it before the service.
    The constructor registers this servlet, so the next upgrade of
    that service is delivered here. The destructor releases every
    session it still holds through
    %WebSocketService::onReleaseSession(). The derived service
    destructor has not run yet, so the virtual call and the allocator
    are still there. That order is required. The service destructor
    cannot release the sessions, because by then the derived object
    is already gone.

    One service has one registered servlet. A second servlet replaces
    the registration. The previous servlet keeps the sessions it
    already accepted and releases them itself. No registration, which
    is the state before the first servlet and after the last
    destructor, declines the next upgrade. The HTTP server closes a
    stream that has no bound session.

    When a stream ends, %WebSocketSession::onClose() runs and this
    servlet releases that session. The HTTP %Stream does not know
    this type. It only knows the frame session that was bound.

    The example registers the scope before the server starts taking
    upgrades. Destroy the servlet before the service.

    @code
    EchoService service;
    Pt::Http::WebSocketServlet sockets(service);
    Pt::Http::MapUrl mapUrl("/ws", service);
    server.addServlet(mapUrl);
    @endcode

    @ingroup Pt-Http-WebSocket
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
        void onUpgrade(Stream& stream);

        void onSessionClosed(WebSocketSession& session);

        void releaseSession(WebSocketSession* session);

        WebSocketService* _service;
        std::vector<WebSocketSession*> _sessions;
};

} // namespace Http

} // namespace Pt

#endif
