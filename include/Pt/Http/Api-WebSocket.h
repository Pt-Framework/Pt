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
    formats those frames into the stream buffer. It is not an I/O
    device. The caller writes payload into %buffer() and sends that
    buffer as one frame. A receive parses one frame and leaves the
    payload in the same buffer. %frame() is the opcode.

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
    WebSocket Upgrade request with 101 Switching Protocols, or with
    404 when the request is not a WebSocket upgrade. After a
    successful upgrade the server keeps the connection and emits
    %Service::upgradeRequested() with a %Stream.
    %WebSocket::accept() retains that stream and formats it. The
    socket does not own the connection.

    %Stream is the upgraded channel, not the HTTP message body.
    A 101 reply is the generic HTTP upgrade. %WebSocketService is the
    WebSocket case: %Service::upgradeRequested() runs on the server
    thread, and %WebSocket::accept() keeps the stream.

    Ping and pong are control frames. %sendPing() writes a ping, and
    after a ping is received %sendPong() writes the matching pong.
    Text and binary frames are the data payload. Unknown is the unset
    frame type.

    The example is a client handshake. The slot completes the connect
    and then writes the payload buffer as a text frame.

    @code
    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        socket.buffer().sputn("hello", 5);
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
*/

#endif
