/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_API_WEBSOCKET_H
#define PT_HTTP_API_WEBSOCKET_H

/** @addtogroup Pt-Http-WebSocket

    @brief Upgrade an HTTP connection to a WebSocket.

    WebSocket is an HTTP upgrade. The HTTP exchange performs a
    handshake, and after the handshake the same connection carries
    messages instead of request and reply messages. The public unit is
    a text or binary payload. Framing, fragmentation, masking, and
    control frames stay inside the engine. The application does not
    read an opcode, a FIN bit, or a continuation.

    An HTTP exchange already separates the message from the connection.
    A %Client holds a %Request and a %Reply. A WebSocket needs the same
    split. The socket owns two %WebSocketMessage objects, %incoming()
    and %outgoing(). A send writes %outgoing(). A receive fills
    %incoming(). Those two messages may move at the same time. One
    begin is outstanding until the matching end. A second begin on the
    same direction while that begin is outstanding is an error.

    %beginSend() writes %outgoing(). Set the type, write the body, and
    call %beginSend(). %outputReady() reports that data bytes were
    sent. %endSend() returns %MessageProgress. If the send is not
    finished, %beginSend() continues the same message. The engine may
    split the body into several frames. The application does not see
    that split.

    %beginReceive() fills %incoming(). %inputReady() reports that data
    bytes were received. %endReceive() returns %MessageProgress.
    %header() means the message type is known. %body() means payload
    bytes were processed on this step. %finished() means the data
    message is complete. If the receive is not finished, the
    application discards the consumed body and calls %beginReceive()
    again. After %finished(), %clear() drops the body so the same
    object can carry the next message.

    Ping, pong, and close are socket operations, not messages. %ping()
    enqueues a ping. A received ping is answered by the engine. A
    received pong is consumed. Neither is delivered through
    %incoming(), and neither emits %inputReady() or %outputReady().
    %close() enqueues a close frame with a status code and a reason.
    A received close is answered by the engine. After a local close,
    no more data frames are sent. Receive continues until the peer
    close or the idle timeout. %closeCode() and %closeReason() report
    the handshake that ended the stream. %closed() ends an outstanding
    send or receive.

    On the client, construct %WebSocket with the %Client that performs
    the handshake. The socket stores a reference and does not own the
    client, so the client must outlive the socket. Host, port, event
    loop, timeout and TLS are settings of that client. %beginConnect()
    sends the handshake for a request path, or for a %ws:// URL whose
    host and port are the client's endpoint. A finished 101 becomes
    the %Stream this socket formats. A client masks every frame it
    writes.

    On the server, %WebSocketService is an HTTP %Service, mapped with
    a %Servlet like any other service. It is the factory and the
    endpoint policy. It does not keep the sessions. A
    %WebSocketServlet does, and it is constructed with the service
    and destroyed before it. That servlet is the release scope. The
    handshake responder answers a WebSocket Upgrade request with 101
    Switching Protocols, with 503 when the accepted-session limit is
    already reached, or with 404 when the request is not a WebSocket
    upgrade. After a successful upgrade the HTTP server keeps the
    connection and calls %Service::onUpgrade(). The WebSocket servlet
    asks the service for one %WebSocketSession, holds it, and passes
    the stream together with the loop that already serializes it. The
    session formats that stream, which accepts the upgrade. A server
    does not mask. A null session, or no registered WebSocket servlet,
    leaves the stream unbound, and the HTTP server closes it.

    %WebSocketSession exposes the same %incoming(), %outgoing(),
    %beginSend(), %endSend(), %beginReceive(), and %endReceive()
    operations directly. %onInput() runs when data bytes were
    received. %onOutput() runs when data bytes were sent. A ping or a
    pong does not run those callbacks. %onClose() is the last look at
    the session. Do not call %endReceive() or %endSend() from
    %onClose().

    %Stream is the upgraded channel, not the HTTP message body. A 101
    reply is the generic HTTP upgrade. %WebSocketService is the
    WebSocket case of %onUpgrade(). The application derives
    %WebSocketSession and keeps the state of that connection there.

    The example is a client handshake. The slot completes the connect
    and then writes a text message.

    @code
    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
        socket.outgoing().body() << "hello";
        socket.beginSend();
    }

    Pt::System::MainLoop loop;
    Pt::Net::Endpoint ep("localhost", 80);
    Pt::Http::Client client(loop, ep);
    Pt::Http::WebSocket socket(client);
    socket.connected() += Pt::slot(onConnected);
    socket.beginConnect("/ws");
    loop.run();
    @endcode

    The server example is one session type. The constructor starts
    the receive. %onInput() reads the payload from %incoming(). The
    service owns the session and releases it when the stream ends.

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
    Pt::Http::WebSocketServlet sockets(service);
    @endcode
*/

#endif
