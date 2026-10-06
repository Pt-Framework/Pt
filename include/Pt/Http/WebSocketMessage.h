/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETMESSAGE_H
#define PT_HTTP_WEBSOCKETMESSAGE_H

#include <Pt/Http/Api.h>
#include <Pt/NonCopyable.h>
#include <iostream>
#include <cstddef>

namespace Pt {

namespace Http {

class WebSocketConnection;

/** @brief Payload of one WebSocket text or binary message.

    %WebSocketMessage is the payload object of a WebSocket. It is the
    peer of %Message, not of %Request or %Reply. A socket holds two of
    them for the life of the stream: %incoming() is filled by a
    receive, and %outgoing() is what a send writes. Callers do not
    construct a message. There is no header and no URL. The only type
    it exposes is the data type of the payload.

    %Type is %Unknown until a data type is set. %setType() is for the
    outgoing message and accepts %Text or %Binary. The engine sets the
    type of the incoming message when it has parsed the first data
    frame of that message. A text message is UTF-8 over the whole
    message. Binary is an uninterpreted payload.

    %body() is the iostream, the same surface %Message uses. Write it
    before %WebSocket::beginSend(). After a receive step, read the
    bytes that step delivered. %available() is how many payload bytes
    can be read. %pending() is how many payload bytes are waiting to
    be sent.

    %discard() drops the buffered body and leaves the type. That is
    the call during a message that is not finished, after the
    application has consumed what %body() holds. %clear() drops the
    body and sets the type to %Unknown, so the same object can carry
    the next message. After a receive that reports finished, the
    application calls %clear().

    The example writes a text payload on the outgoing message and
    reads an incoming payload after a receive step.

    @code
    socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
    socket.outgoing().body() << "hello";
    socket.beginSend();

    Pt::Http::MessageProgress progress = socket.endReceive();
    if(progress.body())
    {
        std::string text;
        text.resize( socket.incoming().available() );
        if( ! text.empty() )
            socket.incoming().body().read(&text[0], text.size());
    }
    if( ! progress.finished() )
        socket.incoming().discard();
    else
        socket.incoming().clear();
    @endcode

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketMessage : private NonCopyable
{
    friend class WebSocketConnection;

    public:
        /** @brief Data type of a WebSocket message.
        */
        enum Type
        {
            Unknown, ///< No data type yet
            Text,    ///< UTF-8 text
            Binary   ///< Binary payload
        };

        /** @brief Returns the data type of this message.
        */
        Type type() const
        { return _type; }

        /** @brief Sets the data type of an outgoing message.

            @throw %std::invalid_argument if @a type is %Unknown.
        */
        void setType(Type type);

        /** @brief Returns the payload stream.
        */
        std::iostream& body()
        { return _body; }

        /** @brief Returns how many payload bytes can be read.
        */
        std::size_t available() const;

        /** @brief Returns how many payload bytes are waiting to be sent.
        */
        std::size_t pending() const;

        /** @brief Drops the buffered body and leaves the type.
        */
        void discard();

        /** @brief Drops the buffered body and sets the type to %Unknown.
        */
        void clear();

    private:
        class PayloadBuffer;

        WebSocketMessage();

        ~WebSocketMessage();

        void setTypeFromEngine(Type type);

        void append(const char* data, std::size_t n);

        void prepareRead();

        const char* sendData() const;

        std::size_t sendSize() const;

        void consume(std::size_t n);

    private:
        Type           _type;
        PayloadBuffer* _buffer;
        std::iostream  _body;
};

} // namespace Http

} // namespace Pt

#endif
