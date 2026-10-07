/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_API_WEBSOCKETSERVER_H
#define PT_HTTP_API_WEBSOCKETSERVER_H

/** @addtogroup Pt-Http-WebSocket-Server

    @brief Accept an upgrade and keep application state on the
    session.

    A server WebSocket is an HTTP service that answers the handshake
    and a session object that lasts for the life of the upgraded
    stream. The HTTP server still owns the connection. The service
    is the factory, endpoint policy, and owner of its live sessions.
    %WebSocketServlet provides the sole active release scope and, on
    destruction, asks the service to release remaining sessions while
    the factory is still fully constructed. Derive the
    session to keep the state that must survive from one message to
    the next: a subscription, a cursor, a user, or a reference into
    the application domain.

    The example is one session type. The constructor starts the
    receive. The input callback reads the payload. The mapping
    servlet and the release scope are different objects.

    @code
    class EchoSession : public Pt::Http::WebSocketSession
    {
        public:
            EchoSession(Pt::Http::WebSocketService& service,
                        Pt::System::EventLoop& loop,
                        Pt::Http::Stream& stream,
                        const Pt::Http::Reply& reply)
            : Pt::Http::WebSocketSession(service, loop, stream, reply)
            {
                beginReceive();
            }

        protected:
            virtual void onInput()
            {
                Pt::Http::MessageProgress progress = endReceive();
                if( ! progress.finished() )
                {
                    incoming().discard();
                    beginReceive();
                    return;
                }

                incoming().clear();
                beginReceive();
            }

            virtual void onOutput()
            {
                endSend();
            }

            virtual void onClose()
            {}
    };

    typedef Pt::Http::BasicWebSocketService<EchoSession> EchoService;

    EchoService service;
    service.setMaxSockets(100);
    service.setIdleTimeout(60000);
    service.setMaxMessageSize(1 << 20);
    Pt::Http::WebSocketServlet sockets(service);
    Pt::Http::MapUrl mapUrl("/ws", service);
    server.addServlet(mapUrl);
    @endcode

    %WebSocketService is an HTTP %Service, mapped with a %Servlet
    like any other service. Its handshake responder answers a
    WebSocket Upgrade request with 101 Switching Protocols, with
    503 when the accepted-session limit is already reached, or with
    404 when the request is not a WebSocket upgrade. The responder
    is released before the server opens the stream. It is not the
    session. %addProtocol() names one Sec-WebSocket-Protocol value
    this endpoint accepts. Several calls name several values, in
    preference order. An empty list accepts the upgrade and selects
    no name. A non-empty list selects the first offered name that
    is also accepted, and a request that offers none of them is
    answered with 400. %setMaxSockets(), %setIdleTimeout() and
    %setMaxMessageSize() are endpoint policy. The handshake reads
    the session limit. The session base copies the idle timeout and
    the data-message limit onto the connection before the derived
    constructor runs. It also copies the single
    Sec-WebSocket-Protocol name from the opening reply, or leaves
    that name empty when the reply selected none.

    After a finished 101 the HTTP server keeps the connection and
    delivers the upgraded stream to this service. The service accepts
    it only while a %WebSocketServlet scope is active. It calls
    %onGetSession() with the %EventLoop of the stream, the stream, the
    opening request, and the opening reply, and retains a non-null
    result as a live session. Both messages are valid for that call.
    The session binds the stream in its constructor, which accepts the
    upgrade. It reads the selected subprotocol from the reply and does
    not store the reply. A null session, or no active scope, leaves
    the stream unbound, and the HTTP server closes it. A server does
    not mask the frames it writes.

    %WebSocketSession exposes %incoming(), %outgoing(),
    %beginSend(), %endSend(), %beginReceive() and %endReceive()
    directly. %onInput() runs when data bytes were received.
    %onOutput() runs when data bytes were sent. A ping or a pong
    does not run those callbacks. %onClose() is the last look at
    the session. Do not call %endReceive() or %endSend() from
    %onClose(). Closing the session closes the stream. The session
    does not own the stream or the connection.

    %WebSocketServlet establishes the service's one active release
    scope. Construct it with the service and destroy it before the
    service. The destructor asks the service to release every session
    it still owns while the derived service is fully constructed. The
    derived service destructor has not run yet, so the virtual release
    call and the allocator are still there. Constructing a second
    scope for the same service throws %std::logic_error.

    %BasicWebSocketService is this factory for one session type.
    Derive %WebSocketService when the session type depends on the
    upgrade, or when the session constructor needs more than the
    servlet, the loop, the stream, and the opening reply.

    @code
    Pt::Http::WebSocketSession* ChatService::onGetSession(
        Pt::System::EventLoop& loop,
        Pt::Http::Stream& stream,
        const Pt::Http::Request& request,
        const Pt::Http::Reply& reply)
    {
        const char* user = request.header().get("X-User");
        return new ChatSession(*this, loop, stream, reply, _rooms,
                               user ? user : "");
    }

    void ChatService::onReleaseSession(Pt::Http::WebSocketSession* session)
    {
        delete session;
    }
    @endcode

    %onGetSession() creates the session and %onReleaseSession()
    destroys it. The request argument is the opening request. The
    reply argument is the opening reply. Both are valid for that
    call. Copy any header the session must keep. The session does
    not store the request or the reply. The two methods must use the
    same allocator, as %onGetResponder() and %onReleaseResponder()
    must match. A pool, or any other detach, lives in the derived
    service. Return null from %onGetSession() to decline the upgrade.
        The service releases a session through %onReleaseSession() after
    %onClose() returns, and when its release scope ends for every
    remaining session it owns.
*/

#endif
