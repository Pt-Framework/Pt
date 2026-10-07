/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_API_WEBSOCKETCLIENT_H
#define PT_HTTP_API_WEBSOCKETCLIENT_H

/** @addtogroup Pt-Http-WebSocket-Client

    @brief Handshake through an HTTP client and send messages on the
    upgraded stream.

    A client WebSocket is the handshake plus the message surface on
    the stream that handshake produces. Host, port, event loop,
    timeout and TLS stay settings of the HTTP user agent. The socket
    stores a reference to that agent and does not own it, so the
    agent must outlive the socket, including a handshake that is
    still waiting on the loop. After a finished 101 the socket
    formats the stream the client already owns. The client still
    owns the connection. Closing the socket closes the stream. The
    socket does not own the stream.

    The example completes the handshake and then writes a text
    message.

    @code
    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
        socket.outgoing().body() << "hello";
        socket.beginSend();
        socket.beginReceive();
    }

    void onClosed(Pt::Http::WebSocket& /*socket*/)
    {
    }

    Pt::System::MainLoop loop;
    Pt::Net::Endpoint ep("localhost", 80);
    Pt::Http::Client client(loop, ep);
    Pt::Http::WebSocket socket(client);
    socket.connected() += Pt::slot(onConnected);
    socket.closed() += Pt::slot(onClosed);
    socket.setTimeout(10000);
    socket.beginConnect("/ws");
    loop.run();
    @endcode

    Construct %WebSocket with the %Client that performs the
    handshake. Connect %connected() and call %beginConnect() with a
    request path, or with a %ws:// URL whose host and port are
    already the client's endpoint. The path and query of that URL
    become the request URL; the client does not take the host from
    the URL. The optional second argument is the Origin header.
    %beginConnect() fills a GET with Connection, Upgrade,
    Sec-WebSocket-Version 13 and Sec-WebSocket-Key, and sends it
    through the client. %addProtocol() offers one
    Sec-WebSocket-Protocol name before that send. Several calls
    offer several names, in order. A finished 101 becomes the stream this
    socket formats. %endConnect() completes the handshake and throws
    if it failed. %protocol() is the single name the server echoed,
    or empty when the server selected none. An echo that was not
    offered fails the handshake. After that success the socket no
    longer uses the client for messages. A client masks every frame
    it writes.

    %closed() is emitted while this socket is still alive. The
    stream has already cleared its channel pointer. Peer close, an
    I/O error, a close frame and destruction of the stream all emit
    it. It ends an outstanding send or receive. The owner deletes
    this socket. %setTimeout() bounds I/O after the upgrade. The
    handshake still uses the timeout of the client.
*/

#endif
