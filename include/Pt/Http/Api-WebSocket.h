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
    an I/O device. The socket owns two %WebSocketFrame objects.
    %output() is the frame the next send writes. %input() is the frame
    the next receive fills. The opcode and FIN live on the frame. The
    payload is %WebSocketFrame::body(), an iostream, the same surface
    a %Message uses for its body. Write that stream and send it as one
    frame. A receive parses one frame and leaves the payload in
    %input().body(). %WebSocketFrame::available() is how many of those
    bytes can be read. The connection stream buffer is not the
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

    On the server, %WebSocketService is an HTTP %Service, mapped with
    a servlet like any other service. It is the factory and the
    endpoint policy. It does not keep the sessions. A %WebSocketServer
    does, and it is constructed with the service and destroyed before
    it. The handshake responder answers a WebSocket Upgrade request
    with 101 Switching Protocols, with 503 when the accepted-session
    limit is already reached, or with 404 when the request is not a
    WebSocket upgrade. After a successful upgrade the HTTP server
    keeps the connection and calls %Service::onUpgrade(). The
    WebSocket server asks the service for one %WebSocketSession,
    holds it, and passes the stream together with the loop that
    already serializes it. The session contains one %WebSocket and
    binds it, which accepts the upgrade. A null session, or no
    registered WebSocket server, leaves the stream unbound, and the
    HTTP server closes it. The socket does not own the connection,
    and the stream does not own the socket. When the stream ends,
    the session's %onClose() runs and the WebSocket server releases
    the session while the service is still alive.

    %Stream is the upgraded channel, not the HTTP message body.
    A 101 reply is the generic HTTP upgrade. %WebSocketService is the
    WebSocket case of %onUpgrade(). The application derives
    %WebSocketSession and keeps the state of that connection there.

    Ping and pong are control frames. %sendPing() writes a ping from
    %output(), and after a ping is received the session can write the
    matching pong with %sendPong(). The socket does not answer a ping
    by itself. Text and binary frames are the data payload.
    %WebSocketFrame::Continuation is a following fragment. A cleared
    frame is text with FIN set.

    The example is a client handshake. The slot completes the connect
    and then writes the output frame as text.

    @code
    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        Pt::Http::WebSocketFrame& out = socket.output();
        out.setType(Pt::Http::WebSocketFrame::Text);
        out.body() << "hello";
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
    the receive. %onInput() reads the payload from the member socket.
    The service owns the session and releases it when the stream ends.

    @code
    class EchoSession : public Pt::Http::WebSocketSession
    {
        public:
            EchoSession(Pt::Http::WebSocketServer& server,
                        Pt::System::EventLoop& loop,
                        Pt::Http::Stream& stream)
            : Pt::Http::WebSocketSession(server, loop, stream)
            {
                socket().beginReceive();
            }

        protected:
            virtual void onInput()
            {
                socket().endReceive();
                Pt::Http::WebSocketFrame& in = socket().input();
                std::string message;
                message.resize(in.available());
                if( ! message.empty() )
                    in.body().read(&message[0], message.size());
                socket().beginReceive();
            }

            virtual void onOutput()
            {
                socket().endSend();
            }

            virtual void onClose()
            {}
    };

    typedef Pt::Http::BasicWebSocketService<EchoSession> EchoService;

    EchoService service;
    Pt::Http::WebSocketServer sockets(service);
    @endcode
*/

#endif
