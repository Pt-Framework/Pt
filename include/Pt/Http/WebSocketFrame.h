/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETFRAME_H
#define PT_HTTP_WEBSOCKETFRAME_H

#include <Pt/Http/Api.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>

namespace Pt {

namespace Http {

class WebSocket;

/** @brief One WebSocket frame: opcode, FIN, and a payload stream.

    %WebSocketFrame is the message a %WebSocket sends or receives. It
    is not a session, not a stream, and not an HTTP %Message. %Message
    is bound to an HTTP connection, header fields, chunked transfer,
    and keep-alive. A frame has a 2- to 14-byte header, an opcode, a
    FIN bit, and a payload. The iostream body is the shared surface.
    Inheritance is not. A frame that derived %Message would pull HTTP
    into the session.

    The socket owns the frame. The caller does not construct one and
    does not pass one into %WebSocket::beginSend() or
    %WebSocket::beginReceive(). %WebSocket::output() is the frame the
    next send writes. %WebSocket::input() is the frame the next
    receive fills. The two buffers are distinct, so a receive does not
    overwrite the outbound payload and a send does not consume the
    inbound payload.

    A cleared frame is %Text with FIN set and an empty body, the same
    way a new %Request is GET and a new %Reply is 200. The caller sets
    the opcode and FIN, writes %body(), and the socket sends that
    frame. After a receive, %type() is the opcode the peer sent and
    %body() holds the payload. %available() is how many of those bytes
    can be read. %pending() is how many payload bytes are waiting to
    be sent. %discard() drops the buffered payload and leaves the
    opcode and FIN. %clear() resets opcode, FIN, close fields, and
    payload so the same frame can be filled again. Either call is an
    error while a transfer of that frame is in flight.

    @code
    Pt::Http::WebSocketFrame& out = socket.output();
    out.setType(Pt::Http::WebSocketFrame::Text);
    out.setFin(true);
    out.body() << "hello";
    socket.beginSend();
    @endcode

    %Text and %Binary are data. %Continuation is a following fragment
    of a data message. A data message that fits in one frame has FIN
    set. A fragmented message clears FIN on every frame but the last
    and uses %Continuation after the first. The socket delivers each
    of those frames. It does not concatenate them. %Close, %Ping, and
    %Pong are control frames. Control frames are always final.
    %setFin(false) on a control type is an error when the socket sends
    that frame.

    %closeCode() and %closeReason() are meaningful when %type() is
    %Close. On send, the socket writes the code and reason into the
    payload. The default code is 1000. On receive, the socket parses
    them out of the payload and leaves the raw payload in %body() as
    well. For any other opcode the close accessors are empty, and
    setting them does not change the frame the socket writes.

    The frame does not expose the mask, the RSV bits, or the header
    length. Masking stays inside %WebSocket. RSV stays internal until
    an extension exists.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketFrame
{
    friend class WebSocket;

    public:
        /** @brief WebSocket frame opcode.
        */
        enum Type
        {
            Continuation, ///< Following fragment of a data message
            Text,         ///< Text data frame
            Binary,       ///< Binary data frame
            Close,        ///< Close frame
            Ping,         ///< Ping frame
            Pong          ///< Pong frame
        };

        /** @brief Creates a text frame with FIN set and an empty body.
        */
        WebSocketFrame();

        /** @brief Destructor.
        */
        ~WebSocketFrame();

        /** @brief Returns the opcode.
        */
        Type type() const
        { return _type; }

        /** @brief Sets the opcode.

            A frame the caller has cleared is %Text. A received frame
            has the opcode the peer sent.
        */
        void setType(Type type);

        /** @brief Returns the FIN bit.

            A data message that fits in one frame has FIN set. Control
            frames are always final.
        */
        bool fin() const
        { return _fin; }

        /** @brief Sets the FIN bit.

            %setFin(false) on a control type is an error when the
            socket sends that frame.
        */
        void setFin(bool fin);

        /** @brief Returns the payload stream.

            Write payload here before %WebSocket::beginSend(). After
            %WebSocket::endReceive() this stream holds the received
            payload.
        */
        std::iostream& body()
        { return _body; }

        /** @brief Returns how many payload bytes can be read.
        */
        std::size_t available() const;

        /** @brief Returns how many payload bytes are waiting to be sent.
        */
        std::size_t pending() const;

        /** @brief Drops the buffered payload.

            The opcode and FIN stay as they are.

            @throw %std::logic_error if a transfer of this frame is
            in flight.
        */
        void discard();

        /** @brief Resets opcode, FIN, close fields, and payload.

            The frame becomes %Text with FIN set and an empty body.

            @throw %std::logic_error if a transfer of this frame is
            in flight.
        */
        void clear();

        /** @brief Returns the close code when %type() is %Close.

            Empty for any other opcode. The default close code is
            1000.
        */
        unsigned short closeCode() const;

        /** @brief Sets the close code.

            Used when the socket sends a %Close frame. Ignored for
            any other opcode.
        */
        void setCloseCode(unsigned short code);

        /** @brief Returns the close reason when %type() is %Close.

            Empty for any other opcode.
        */
        const std::string& closeReason() const;

        /** @brief Sets the close reason.

            Used when the socket sends a %Close frame. Ignored for
            any other opcode.
        */
        void setCloseReason(const std::string& reason);

    private:
        void setBusy(bool busy);

        bool isBusy() const
        { return _busy; }

        void composeClosePayload();

        void assign(Type type, bool fin, const char* data, std::size_t n);

        const char* data() const;

        std::size_t size() const
        { return _payload.size(); }

        void ensureIdle() const;

    private:
        class PayloadBuffer;

        Type _type;
        bool _fin;
        bool _busy;
        bool _closeSet;
        unsigned short _closeCode;
        std::string _closeReason;
        std::vector<char> _payload;
        PayloadBuffer* _buffer;
        std::iostream _body;
};

} // namespace Http

} // namespace Pt

#endif
