/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_API_WEBSOCKET_H
#define PT_HTTP_API_WEBSOCKET_H

/** @addtogroup Pt-Http-WebSocket

    @brief Upgrade an HTTP connection to a bidirectional message stream.

    WebSocket is a protocol that begins as HTTP and then uses the same
    connection as a bidirectional message channel. HTTP is a sequence
    of request and reply exchanges, and the client always starts the
    next exchange. After a WebSocket handshake the two sides send
    complete payloads independently, for as long as the connection
    remains open. That is the reason to upgrade: a live feed, a
    remote editor, or any conversation that cannot wait for the next
    request.

    The handshake is still HTTP. The client sends a GET whose
    Connection field contains Upgrade, whose Upgrade field is
    websocket, and whose Sec-WebSocket-Key is sixteen random bytes
    encoded as Base64. Sec-WebSocket-Version is 13, the version of
    RFC 6455. An Origin field is common when a browser is the client.
    The server answers with status 101 Switching Protocols, repeats
    the Upgrade, and returns Sec-WebSocket-Accept, which is the
    Base64 encoding of the SHA-1 of that key concatenated with the
    RFC 6455 GUID. After that 101 the bytes on the connection are
    WebSocket frames, not HTTP messages. A later HTTP request on the
    same connection would be a protocol error.

    On the wire a frame has an opcode, a FIN bit, a payload length,
    and, on every frame a client writes, a four-byte masking key.
    Opcode 1 is a text message, opcode 2 is a binary message, and
    opcode 0 continues the data message that is already open. Opcode
    8 is close, 9 is ping, and 10 is pong. FIN false means more
    frames of the same message follow. The application unit is the
    message: every frame from the first data opcode through the frame
    that has FIN set. The engine may split one payload into several
    frames. The application does not read an opcode, a FIN bit, or a
    continuation.

    Masking is a client-to-server rule of the protocol. Every frame a
    client writes is XOR-masked with a 32-bit key so that a
    misbehaving client cannot inject a crafted frame that looks like
    an HTTP request to an intervening proxy. A server writes unmasked
    frames. The mode is fixed when the upgraded stream opens. The
    engine applies it. The application does not supply a mask.

    A text message is UTF-8 over the whole message, not per frame. A
    binary message is an uninterpreted payload. Ping, pong, and close
    are control frames. They are not messages. A ping asks the peer
    to answer with a pong of the same payload, at most 125 bytes. A
    close frame carries a status code and an optional UTF-8 reason,
    at most 123 bytes of reason text, and starts the close handshake.
    After a local close, no more data frames are sent. Receive
    continues until the peer close arrives or the idle timeout fires.

    This API keeps the HTTP split between the message and the
    connection. The HTTP owner keeps the TCP connection and the
    upgraded stream. The framing layer writes frames into that
    stream. Callers never construct a payload object. Each side of
    the conversation holds two payloads for the life of the stream:
    one that a receive fills, and one that a send writes. Those two
    payloads may move at the same time. One send or one receive is
    outstanding until it completes. Starting a second operation on
    the same direction while the first is still outstanding is an
    error.

    The example writes a text payload, completes a send step, and
    reads an incoming payload after a receive step.

    @code
    void onOutputReady(Pt::Http::WebSocket& socket)
    {
        Pt::Http::MessageProgress progress = socket.endSend();
        if( ! progress.finished() )
            socket.beginSend();
    }

    void onInputReady(Pt::Http::WebSocket& socket)
    {
        Pt::Http::MessageProgress progress = socket.endReceive();
        if(progress.body())
        {
            std::string text;
            text.resize( socket.incoming().available() );
            if( ! text.empty() )
                socket.incoming().body().read(&text[0], text.size());
        }

        if( ! progress.finished() )
        {
            socket.incoming().discard();
            socket.beginReceive();
            return;
        }

        socket.incoming().clear();
        socket.beginReceive();
    }

    socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
    socket.outgoing().body() << "hello";
    socket.beginSend();
    socket.beginReceive();
    @endcode

    The outgoing payload is %outgoing() and the incoming payload is
    %incoming(). Both are %WebSocketMessage objects. Set the type to
    %Text or %Binary, write the body, and call %beginSend().
    %outputReady() reports that data bytes were sent. %endSend()
    returns %MessageProgress. If the send is not finished,
    %beginSend() continues the same message. %beginReceive() fills
    %incoming(). %inputReady() reports that data bytes were received.
    %endReceive() returns progress. %header() means the data type is
    known, which is the first data opcode of that message. %body()
    means payload bytes were processed on this step. %finished()
    means the data message is complete. If the receive is not
    finished, discard the consumed body and call %beginReceive()
    again. After %finished(), %clear() drops the body so the same
    object can carry the next message. The server session exposes
    the same begin and end methods on the session object; the data
    callbacks there take the place of the ready signals.

    Heartbeats and close are socket operations, not payloads.

    @code
    socket.ping("are you there", 13);
    socket.close(1000, "done");
    @endcode

    %ping() enqueues a ping frame. A received ping is answered by
    the engine. A received pong is consumed. Neither is delivered
    through %incoming(), and neither reports data-ready. %close()
    enqueues a close frame with a status code and a reason. Codes
    1005, 1006 and 1015 cannot be sent; they are reserved for "no
    status received", abnormal closure, and a failed TLS handshake.
    A received close is answered by the engine. After a local close,
    no more data frames are sent. %closeCode() and %closeReason()
    report the handshake that ended the stream. A data message
    larger than the configured limit, or an idle period without a
    finished transfer, closes the stream. Zero disables each limit.
    The size count is the declared payload from the first data
    opcode to FIN. A finished send or receive restarts the idle
    timeout, and a received ping or pong restarts it as well.

    The HTTP owner still owns the stream. Opening a client socket
    uses the HTTP user agent of the client chapter. Accepting a
    server session uses the service mapping of the server chapter.
    The client and server sections of this chapter take those two
    reader tasks from the handshake onward.
*/

#endif
