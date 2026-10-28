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

    Pt::System::MainLoop loop;
    Pt::Net::Endpoint ep("localhost", 80);
    Pt::Http::Client client(loop, ep);
    client.setTimeout(10000);
    Pt::Http::WebSocket socket(client);
    socket.connected() += Pt::slot(onConnected);
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

    %close() releases the stream and writes no frame. The destructor
    calls it. The application learns that the stream has ended from
    the outstanding operation: an outstanding receive completes with
    an error, and an outstanding shutdown completes through
    %shutdownReady(). Without either, a peer close is not seen.
    Destruction of the stream detaches the channel. It ends an
    outstanding send or receive. The owner deletes this socket.
    The handshake uses the timeout of the client. After the upgrade
    that timeout is stopped. %setIdleTimeout() is the bound on
    silence and on a close handshake the peer never answers.
*/

#endif
