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

    On the server, %WebSocketService is an HTTP %Service, mapped with
    a servlet like any other service. Its responder answers a
    WebSocket Upgrade request with 101 Switching Protocols, with 503
    when the accepted-socket limit is already reached, or with 404
    when the request is not a WebSocket upgrade. After a successful
    upgrade the server keeps the connection and calls
    %Service::onUpgrade(). %WebSocketService constructs the %WebSocket,
    binds the stream, and emits %accepted(). The service owns that
    socket. The socket does not own the connection, and the stream does
    not own the socket. Each side clears its pointer to the other.
    %closed() reports that the stream has ended. The service erases
    the socket there and deletes it when the service is destroyed.

    %Stream is the upgraded channel, not the HTTP message body.
    A 101 reply is the generic HTTP upgrade. %WebSocketService is the
    WebSocket case: %onUpgrade() runs on the server thread and binds
    the stream before %accepted() is emitted.

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

    The server example receives the socket the service already owns.
    The slot starts the receive and does not free the socket. When the
    frame is complete, the payload is read from %body().

    @code
    void onInput(Pt::Http::WebSocket& socket)
    {
        socket.endReceive();
        std::string message;
        message.resize(socket.available());
        if( ! message.empty() )
            socket.body().read(&message[0], message.size());
    }

    void onAccepted(Pt::Http::WebSocket& socket)
    {
        socket.inputReady() += Pt::slot(onInput);
        socket.beginReceive();
    }

    Pt::Http::WebSocketService service;
    service.accepted() += Pt::slot(onAccepted);
    @endcode
*/

#endif
