/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSERVICE_H
#define PT_HTTP_WEBSOCKETSERVICE_H

#include <Pt/Http/Api.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/WebSocketSession.h>
#include <Pt/Allocator.h>
#include <Pt/Connectable.h>
#include <cstddef>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Http {

class Request;
class Responder;
class Stream;
class WebSocketServer;

/** @brief Factory for WebSocket sessions.

    %WebSocketService is the server-side factory and the endpoint
    policy. Map it with a servlet like any other %Service. Its
    handshake responder answers a WebSocket upgrade with 101, with
    503 when %maxSockets() sessions are already open, or with 404
    when the request is not a WebSocket upgrade.

    This service does not keep the sessions it creates. A
    %WebSocketServer does. The server is constructed with this
    service and destroyed before it, so the factory is still fully
    constructed when the last session is released. One service can
    feed several servers. Each server releases only the sessions it
    accepted.

    After a finished 101 the HTTP server keeps the connection and
    calls %Service::onUpgrade(). This service implements that call.
    It forwards the stream to the %WebSocketServer registered for
    that upgrade. The server asks %onGetSession() for a
    %WebSocketSession and passes the %EventLoop of the stream
    together with the stream. The session binds the stream in its
    constructor, which accepts the upgrade. A null return, or no
    registered server, leaves the stream unbound, and the HTTP server
    closes it.

    %onGetSession() creates the session and %onReleaseSession()
    destroys it. The two methods must use the same allocator. The
    %WebSocketServer calls the second while this service is still
    alive.

    %maxSockets(), %idleTimeout(), and %maxMessageSize() are endpoint
    policy. The handshake reads the session limit. The session base
    copies the idle timeout and the frame limit onto the socket
    before the derived constructor runs. A session may lower those
    socket limits. It does not raise them past the service limit.

    %BasicWebSocketService is this factory for one session type.
    Derive %WebSocketService when the session type depends on the
    upgrade, or when the session constructor needs more than the
    service and the loop.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketService : public Service
                                   , public Connectable
{
    friend class WebSocketResponder;

    public:
        /** @brief Default constructor.
        */
        WebSocketService();

        /** @brief Destructor.

            Does not release sessions. Each %WebSocketServer of this
            service does that, and it is destroyed first.
        */
        ~WebSocketService();

        /** @brief Returns the maximum number of live sessions.

            Zero means no limit.
        */
        std::size_t maxSockets() const;

        /** @brief Sets the maximum number of live sessions.

            A handshake above this limit is answered with 503. Zero
            means no limit.
        */
        void setMaxSockets(std::size_t n);

        /** @brief Returns the idle timeout in milliseconds.
        */
        std::size_t idleTimeout() const;

        /** @brief Sets the idle timeout in milliseconds.

            An accepted socket is closed after @a ms without a finished
            transfer. A finished send or receive restarts the timeout.
            Zero disables it.
        */
        void setIdleTimeout(std::size_t ms);

        /** @brief Returns the maximum frame payload in bytes.
        */
        std::size_t maxMessageSize() const;

        /** @brief Sets the maximum frame payload in bytes.

            A larger frame closes the socket. Zero disables the limit.
        */
        void setMaxMessageSize(std::size_t n);

    protected:
        /** @brief Creates the handshake responder.
        */
        virtual Responder* onGetResponder(const Request&);

        /** @brief Destroys the handshake responder.
        */
        virtual void onReleaseResponder(Responder* responder);

        /** @brief Creates the session for @a stream.

            @a server owns the returned session. @a loop serializes
            @a stream. Return null to decline the upgrade.
        */
        virtual WebSocketSession* onGetSession(WebSocketServer& server,
                                               System::EventLoop& loop,
                                               Stream& stream) = 0;

        /** @brief Destroys a session created by %onGetSession().

            Called by the %WebSocketServer that owns the session,
            while this service is still fully constructed.
        */
        virtual void onReleaseSession(WebSocketSession* session) = 0;

        /** @brief Accepts or declines the upgraded stream.

            Implemented by this class. A derived service uses
            %onGetSession() instead.
        */
        virtual void onUpgrade(Stream& stream) final;

    private:
        friend class WebSocketServer;

        /** @internal Live sessions of the registered server.
        */
        std::size_t sessionCount() const;

        void registerServer(WebSocketServer& server);

        void unregisterServer(WebSocketServer& server);

        WebSocketServer* _server;
        std::size_t _maxSockets;
        std::size_t _idleTimeout;
        std::size_t _maxMessageSize;
};

/** @brief WebSocket service for one session type.

    @ingroup Pt-Http-WebSocket
*/
template <typename S, typename Alloc = Allocator>
class BasicWebSocketService : public WebSocketService
{
    public:
        /** @brief Default constructor.
        */
        BasicWebSocketService()
        { }

        /** @brief Destructor.
        */
        ~BasicWebSocketService()
        { }

    protected:
        virtual WebSocketSession* onGetSession(WebSocketServer& server,
                                               System::EventLoop& loop,
                                               Stream& stream)
        {
            void* memory = _alloc.allocate(sizeof(S));
            return new(memory) S(server, loop, stream);
        }

        virtual void onReleaseSession(WebSocketSession* session)
        {
            session->~WebSocketSession();
            _alloc.deallocate(session, sizeof(S));
        }

    private:
        Alloc _alloc;
};

} // namespace Http

} // namespace Pt

#endif
