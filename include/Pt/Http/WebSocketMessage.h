/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETMESSAGE_H
#define PT_HTTP_WEBSOCKETMESSAGE_H

#include <Pt/Http/Api.h>
#include <Pt/NonCopyable.h>
#include <iostream>
#include <streambuf>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Http {

class WebSocketChannel;


/** @internal
    @brief Output buffer for WebSocket messages.
*/
class WebSocketMessageBuffer : public std::streambuf
{
    public:
        //! @brief Constructs an empty buffer.
        WebSocketMessageBuffer();

        //! @brief Destructor.
        ~WebSocketMessageBuffer();

        //! @brief Discards the buffered data.
        void discard();

        //! @brief Discards up to @a n buffered bytes.
        void discard(std::size_t n);

        //! @brief Returns the size of the buffered data.
        std::size_t size() const;

        //! @brief Returns a pointer to the buffered data.
        const char* data() const;

    protected:
        virtual int_type overflow(int_type ch);

        virtual std::streamsize xsputn(const char* data, std::streamsize n);

        virtual int_type underflow();

    private:
        std::vector<char> _payload;
};


/** @brief Progress of one WebSocket send or receive step.

    %WebSocketProgress is the result of one begin/end step on a
    %WebSocketMessage. %WebSocket::endSend(), %WebSocket::endReceive(),
    and the same methods on %WebSocketSession return it. Ping, pong,
    and shutdown do not; those operations complete in one step.

    The flags are independent and can be combined. %header() means this
    step processed the opening text or binary opcode, so
    %WebSocketMessage::type() is no longer %Unknown. That is not an
    HTTP header and not the frame header on the wire. An empty first
    frame reports %header() without %body(). %continuation() means this
    step processed a later data frame of the same message. %body()
    means payload bytes were processed on this step. %finished() means
    the data message is complete, so the caller must not begin the
    same send or receive again.

    After %finished(), the next begin starts a new message and the
    flags start over. A later I/O step of the first frame still
    reports %header(). A later frame reports %continuation(), not
    %header(). The type of the message stays on %WebSocketMessage.

    @ingroup Pt-Http-WebSocket
*/
class WebSocketProgress
{
    private:
        //! @internal
        enum Result
        {
            Header       = 1,
            Body         = 2,
            Finished     = 4,
            Continuation = 8
        };

    public:
        /** @brief Default Constructor.
        */
        WebSocketProgress()
        : _result(0)
        {}

        /** @brief Returns true if this step processed the opening data opcode.
        */
        bool header() const
        { return (_result & Header) == Header; }

        /** @brief Returns true if this step processed a continuation frame.
        */
        bool continuation() const
        { return (_result & Continuation) == Continuation; }

        /** @brief Returns true if payload bytes were processed on this step.
        */
        bool body() const
        { return (_result & Body) == Body; }

        /** @brief Returns true if the data message is complete.
        */
        bool finished() const
        { return (_result & Finished) == Finished; }

        //! @internal
        void setFinished()
        { _result |= Finished; }

        //! @internal
        void setHeader()
        { _result |= Header; }

        //! @internal
        void setContinuation()
        { _result |= Continuation; }

        //! @internal
        void setBody()
        { _result |= Body; }

    private:
        unsigned long _result;
};


/** @brief Payload of one text or binary WebSocket message.

    %WebSocketMessage is the payload of one WebSocket data message. It
    is the peer of an HTTP message, not of a request or a reply. There
    is no header and no URL. The only type it exposes is the data type
    of the payload: UTF-8 text, or an uninterpreted binary body.
    Callers do not construct this object. The socket holds two of them
    for the life of the stream, one that a receive fills and one that
    a send writes.

    The example writes a text payload on the outgoing message and
    reads an incoming payload after a receive step.

    @code
    socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
    socket.outgoing().body() << "hello";
    socket.beginSend();

    Pt::Http::WebSocketProgress progress = socket.endReceive();
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

    %type() is %Unknown until a data type is set. %setType() is for
    the outgoing message and accepts %Text or %Binary. The engine
    sets the type of the incoming message when it has parsed the
    first data frame of that message. A text body must be valid
    UTF-8 over the whole message. %body() is the iostream. Write it
    before the send begins. After a receive step, read the bytes
    that step delivered. %available() is how many payload bytes can
    be read. %pending() is how many payload bytes are waiting to be
    sent.

    %discard() drops the buffered body and leaves the type. That is
    the call during a message that is not finished, after the
    application has consumed what %body() holds. %clear() drops the
    body and sets the type to %Unknown, so the same object can carry
    the next message. After a receive that reports finished, the
    application calls %clear().

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketMessage : private NonCopyable
{
    friend class WebSocketChannel;

    public:
        /** @brief Creates an empty message.
        */
        WebSocketMessage();

        /** @brief Destroys the message.
        */
        ~WebSocketMessage();

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

        /** @brief Drops up to @a n buffered body bytes and leaves the type.

            Discards up to @a n readable input bytes and up to @a n pending
            output bytes.
        */
        void discard(std::size_t n);

        /** @brief Drops the buffered body and sets the type to %Unknown.
        */
        void clear();

        //! @internal
        WebSocketMessageBuffer& buffer()
        { return _buf; }

    private:
        void setTypeFromEngine(Type type);

        //! @internal
        void setBuffer(std::streambuf& sb);

    private:
        Type                   _type;
        WebSocketMessageBuffer _buf;
        std::iostream          _body;
};

} // namespace Http

} // namespace Pt

#endif
