/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_API_WEBSOCKET_H
#define PT_HTTP_API_WEBSOCKET_H

/** @addtogroup Pt-Http-WebSocket

    @brief Upgrade an HTTP connection to a WebSocket.

    WebSocket is an HTTP upgrade. The HTTP exchange performs a
    handshake, and after the handshake the same TCP connection carries
    framed messages instead of request and reply messages. %WebSocket
    is the %IODevice for that framed stream: %read() and %write()
    transfer payload bytes, while the frame type is separate.
    %setSendFrame() selects the opcode for the next send, and
    %receiveFrame() reports the opcode of the frame that was last
    received.

    On the client, %beginConnect() sends the handshake to a %ws://
    URL. %connected() is emitted when the attempt finishes, and
    %endConnect() completes it and throws if the handshake failed. The
    socket must be attached to an event loop before the connect
    starts.

    On the server, %WebSocketService is an HTTP %Service, mapped with
    a servlet like any other service. Its responder answers a
    WebSocket Upgrade request with 101 Switching Protocols, or with
    404 when the request is not a WebSocket upgrade. After a
    successful upgrade the connection is an %IOStream, and
    %WebSocket::accept() takes that stream and becomes the framed
    device.

    %IOStream is the upgraded connection, not the HTTP message body.
    A 101 reply is the generic HTTP upgrade. %WebSocketService is the
    WebSocket case: %Service::onAcceptUpgrade() runs on the server
    thread, and %Service::upgradeRequested() reports that stream and
    the Upgrade header value.

    Ping and pong are control frames. %sendPingFrame() writes a ping,
    and after a ping is received %sendPongFrame() writes the matching
    pong. Text and binary frames are the data payload. Unknown is the
    unset frame type.

    The example is a client handshake. The slot completes the connect
    and then writes through the inherited device operation.

    @code
    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        socket.setSendFrame(Pt::Http::WebSocket::Text);
        socket.beginWrite("hello", 5);
    }

    Pt::System::MainLoop loop;
    Pt::Http::WebSocket socket;
    socket.setActive(loop);
    socket.connected() += Pt::slot(onConnected);
    socket.beginConnect("ws://localhost/ws");
    loop.run();
    @endcode
*/

#endif
