/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_API_WEBSOCKET_H
#define PT_HTTP_API_WEBSOCKET_H

/** @addtogroup Pt-Http-WebSocket

    @brief Upgrade an HTTP connection to a WebSocket.

    WebSocket is an HTTP upgrade. The HTTP exchange performs a
    handshake, and after the handshake the same connection carries
    framed messages instead of request and reply messages. %WebSocket
    formats those frames into the connection stream buffer. It is not
    an I/O device. The payload is %body(), an iostream, the same
    surface a %Message uses for its body. Write that stream and send
    it as one frame. A receive parses one frame and leaves the payload
    in %body(). %available() is how many of those bytes can be read.
    %frame() is the opcode. The connection stream buffer is not the
    payload.

    On the client, construct %WebSocket with the %Client that performs
    the handshake. The socket stores a reference and does not own the
    client, so the client must outlive the socket. Host, port, event
    loop, timeout and TLS are settings of that client. %beginConnect()
    sends the handshake for a request path, or for a %ws:// URL whose
    host and port are the client's endpoint. The handshake is an HTTP
    request and reply. A finished 101 becomes the %Stream this socket
    formats. %connected() is emitted when the attempt finishes, and
    %endConnect() completes it and throws if the handshake failed.
    A client masks every frame it writes.

    On the server, %WebSocketService is an HTTP %Service, mapped with
    a %Servlet like any other service. It is the factory and the
    endpoint policy. It does not keep the sessions. A
    %WebSocketServlet does, and it is constructed with the service
    and destroyed before it. That servlet is the release scope. It
    is not the %Servlet that maps the URL, and it is not the HTTP
    server. The handshake responder answers a WebSocket Upgrade
    request with 101 Switching Protocols, with 503 when the
    accepted-session limit is already reached, or with 404 when the
    request is not a WebSocket upgrade. After a successful upgrade
    the HTTP server keeps the connection and calls
    %Service::onUpgrade(). The WebSocket servlet asks the service for
    one %WebSocketSession, holds it, and passes the stream together
    with the loop that already serializes it. The session formats
    that stream, which accepts the upgrade. A server does not mask.
    A null session, or no registered WebSocket servlet, leaves the
    stream unbound, and the HTTP server closes it. The session does
    not own the connection, and the stream does not own the session.
    When the stream ends, the session's %onClose() runs and the
    WebSocket servlet releases the session while the service is still
    alive.

    %Stream is the upgraded channel, not the HTTP message body.
    A 101 reply is the generic HTTP upgrade. %WebSocketService is the
    WebSocket case of %onUpgrade(). The application derives
    %WebSocketSession and keeps the state of that connection there.

    Ping and pong are control frames. %sendPing() writes a ping, and
    after a ping is received %sendPong() writes the matching pong.
    Text and binary frames are the data payload. Unknown is the unset
    frame type.

    The example is a client handshake. The slot completes the connect
    and then writes the payload body as a text frame.

    @code
    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        socket.body() << "hello";
        socket.beginSend(Pt::Http::WebSocket::Text);
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
    the receive. %onInput() reads the payload from the member socket.
    The service owns the session and releases it when the stream ends.

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
                endReceive();
                std::string message;
                message.resize(available());
                if( ! message.empty() )
                    body().read(&message[0], message.size());
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
