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
    is the factory and the endpoint policy. It does not keep the
    sessions it creates. A separate release scope does, and that
    scope is destroyed before the service so the factory is still
    fully constructed when the last session is released. Derive the
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
            EchoSession(Pt::Http::WebSocketServlet& servlet,
                        Pt::System::EventLoop& loop,
                        Pt::Http::Stream& stream)
            : Pt::Http::WebSocketSession(servlet, loop, stream)
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
    session. %setMaxSockets(), %setIdleTimeout() and
    %setMaxMessageSize() are endpoint policy. The handshake reads
    the session limit. The session base copies the idle timeout and
    the data-message limit onto the connection before the derived
    constructor runs.

    After a finished 101 the HTTP server keeps the connection and
    delivers the upgraded stream to this service. The service
    implements that delivery and does not pass it on. It forwards
    the stream to the registered %WebSocketServlet. The servlet
    asks the factory for a %WebSocketSession and passes the
    %EventLoop of the stream together with the stream. The session
    binds the stream in its constructor, which accepts the upgrade.
    A null session, or no registered servlet, leaves the stream
    unbound, and the HTTP server closes it. A server does not mask
    the frames it writes.

    %WebSocketSession exposes %incoming(), %outgoing(),
    %beginSend(), %endSend(), %beginReceive() and %endReceive()
    directly. %onInput() runs when data bytes were received.
    %onOutput() runs when data bytes were sent. A ping or a pong
    does not run those callbacks. %onClose() is the last look at
    the session. Do not call %endReceive() or %endSend() from
    %onClose(). Closing the session closes the stream. The session
    does not own the stream or the connection.

    %WebSocketServlet holds the session pointers. Construct it with
    the service and destroy it before the service. The constructor
    registers this servlet, so the next upgrade of that service is
    delivered here. The destructor releases every session it still
    holds while the service is still fully constructed. The derived
    service destructor has not run yet, so the virtual release call
    and the allocator are still there. That order is required. One
    service has one registered servlet. A second servlet replaces
    the registration. The previous servlet keeps the sessions it
    already accepted and releases them itself.

    %BasicWebSocketService is this factory for one session type.
    Derive %WebSocketService when the session type depends on the
    upgrade, or when the session constructor needs more than the
    servlet, the loop and the stream.

    @code
    Pt::Http::WebSocketSession* ChatService::onGetSession(
        Pt::Http::WebSocketServlet& servlet,
        Pt::System::EventLoop& loop,
        Pt::Http::Stream& stream)
    {
        return new ChatSession(servlet, loop, stream, _rooms);
    }

    void ChatService::onReleaseSession(Pt::Http::WebSocketSession* session)
    {
        delete session;
    }
    @endcode

    %onGetSession() creates the session and %onReleaseSession()
    destroys it. The two methods must use the same allocator, as
    %onGetResponder() and %onReleaseResponder() must match. A pool,
    or any other detach, lives in the derived service. Return null
    from %onGetSession() to decline the upgrade. The servlet
    releases a session through %onReleaseSession() after %onClose()
    returns, and from its destructor for every session it still
    holds.
*/

#endif
