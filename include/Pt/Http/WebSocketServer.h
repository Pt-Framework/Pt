/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSERVER_H
#define PT_HTTP_WEBSOCKETSERVER_H

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

/** @brief Owner of the sessions of one WebSocket service.

    %WebSocketServer is the scope of the sessions a %WebSocketService
    creates. The service is the factory and the endpoint policy. This
    type asks the factory for a session, holds the session, and
    releases it. The service does not keep a list of sessions.

    Construct it with the service. The constructor registers this
    server, so the next upgrade of that service is delivered here.
    Destroy it before the service. The destructor closes every session
    it still holds and releases each one through
    %WebSocketService::onReleaseSession(), while the derived service
    is still fully constructed. That order is the lifetime guarantee.
    A session that outlives this server, or a server that outlives
    its service, is a use outside that order.

    One service has one registered server. A second server replaces
    the registration. The previous server keeps the sessions it
    already accepted and releases them itself. Registering nothing,
    which is the state before the first server and after the last
    destructor, declines the next upgrade.

    When a stream ends, %WebSocketSession::onClose() runs and this
    server releases that session. The HTTP %Stream does not know this
    type. It only knows the %WebSocket that the session bound.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketServer : public Connectable
                                  , private NonCopyable
{
    friend class WebSocketService;
    friend class WebSocketSession;

    public:
        /** @brief Registers this server with @a service.
        */
        explicit WebSocketServer(WebSocketService& service);

        /** @brief Releases every session this server still holds.
        */
        ~WebSocketServer();

        /** @brief Returns the service this server was constructed with.
        */
        WebSocketService& service()
        { return *_service; }

        /** @brief Returns the service this server was constructed with.
        */
        const WebSocketService& service() const
        { return *_service; }

        /** @brief Returns the number of sessions this server holds.
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
