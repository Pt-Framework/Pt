/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSESSION_H
#define PT_HTTP_WEBSOCKETSESSION_H

#include <Pt/Http/Api.h>
#include <Pt/Http/WebSocketMessage.h>
#include <Pt/Http/Message.h>
#include <Pt/Connectable.h>
#include <string>
#include <cstddef>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Http {

class Stream;
class WebSocketService;
class WebSocketChannel;

/** @brief Server facade of one accepted WebSocket stream.

    %WebSocketSession is the application object of one upgraded
    stream. Derive from it to keep the state that must survive from
    one message to the next: a subscription, a cursor, a user, or a
    reference into the application domain. The HTTP server already
    owns the connection and the stream. This session formats messages
    on that stream. It is not a channel bind the application
    performs, and it is not a request responder. The handshake
    responder has already been released when the session begins. A
    server does not mask the frames it writes.

    The example starts a receive in the constructor and reads each
    payload from the input callback. Domain state belongs in the
    derived session.

    @code
    class EchoSession : public Pt::Http::WebSocketSession
    {
        public:
            EchoSession(Pt::Http::WebSocketService& service,
                        Pt::System::EventLoop& loop,
                        Pt::Http::Stream& stream,
                        const Pt::Http::Reply& reply)
            : Pt::Http::WebSocketSession(service, loop, stream, reply)
            {
                beginReceive();
            }

        protected:
            virtual void onInput()
            {
                Pt::Http::MessageProgress progress = endReceive();
                if( ! progress.finished() )
                {
                    incoming().discard();
                    beginReceive();
                    return;
                }

                incoming().clear();
                beginReceive();
            }

            virtual void onOutput()
            {
                endSend();
            }

            virtual void onClose()
            {}
    };
    @endcode

    The service factory constructs the session with the service, with
    the loop that serializes this stream, with the stream of this
    upgrade, and with the opening reply. The loop must be the loop of
    the stream. The reply is valid for the constructor call. The base
    constructor reads the selected subprotocol from that reply, binds
    the stream, copies the service limits onto the connection, and
    stores the loop. It does not store the reply. The derived
    constructor runs after that.
    Its members are initialized, the stream is open, and %loop() is
    the loop of this stream. Start the first %beginReceive() or
    %beginSend() there. There is no separate accept callback. By the
    time the derived constructor body runs, the base has already
    bound the stream.

    %incoming() and %outgoing() are the two payloads. %beginSend()
    writes %outgoing(). %beginReceive() reads into %incoming().
    %onInput() runs when data bytes were received. Call
    %endReceive(), read %incoming() when progress reports body
    bytes, and start the next receive if the message is not
    finished. A ping or a pong does not run %onInput(). %onOutput()
    runs when data bytes were sent. Call %endSend() and continue
    the send when progress is not finished. %onClose() runs while
    this object is still alive, after the stream has ended. Do not
    call %endReceive() or %endSend() from %onClose(). The service
    releases this session after %onClose() returns.

    %protocol() is the single name the responder wrote as
    Sec-WebSocket-Protocol on the 101, or empty when none was
    selected. The base copies it from the opening reply before the
    derived constructor body runs. It is not the list the client
    offered. A reply that echoes more than one token, or a value
    that is not one protocol token, fails construction. The server
    then closes the stream.
    %service() reaches state shared by every connection of this
    endpoint. %loop() is where a timer or posted work must run.
    Closing the session closes the stream. The session does not own
    the stream or the connection. The destructor closes the stream.
    A derived constructor that throws still runs this destructor, so
    a failed construction does not leave an accepted stream without
    an owner.

    Ping and close stay control operations on this type.

    @code
    ping();
    close(1000, "done");
    @endcode

    %ping() enqueues a ping. %close() enqueues a close frame.
    %closeCode() and %closeReason() report the handshake that ended
    the stream.

    @ingroup Pt-Http-WebSocket-Server
*/
class PT_HTTP_API WebSocketSession : public Connectable
{
    friend class WebSocketService;

    public:
        /** @brief Binds @a stream on @a loop for @a service.

            @a service owns this session. @a loop must be the loop of
            @a stream. @a reply is the opening handshake reply and is
            valid for this call. The selected subprotocol is copied
            from it. The reply is not stored.

            @throw %std::logic_error if @a loop is not the loop of
            @a stream.
            @throw %std::runtime_error if Sec-WebSocket-Protocol is
            not one protocol token.
        */
        WebSocketSession(WebSocketService& service,
                         System::EventLoop& loop,
                         Stream& stream,
                         const Reply& reply);

        /** @brief Closes the stream.
        */
        ~WebSocketSession();

        /** @brief Returns the service that created this session.
        */
        WebSocketService& service()
        { return *_service; }

        /** @brief Returns the service that created this session.
        */
        const WebSocketService& service() const
        { return *_service; }

        /** @brief Returns the loop that serializes this stream.
        */
        System::EventLoop& loop()
        { return *_loop; }

        /** @brief Returns the loop that serializes this stream.
        */
        const System::EventLoop& loop() const
        { return *_loop; }

        /** @brief Returns the protocol name selected on the 101.

            Empty when the opening reply selected no name.
        */
        const std::string& protocol() const
        { return _protocol; }

        /** @brief Returns the incoming message.
        */
        WebSocketMessage& incoming();

        /** @brief Returns the outgoing message.
        */
        WebSocketMessage& outgoing();

        /** @brief Begins sending %outgoing().
        */
        void beginSend();

        /** @brief Completes the send started by %beginSend().

            @return Progress of this send step.
        */
        MessageProgress endSend();

        /** @brief Begins receiving into %incoming().
        */
        void beginReceive();

        /** @brief Completes the receive started by %beginReceive().

            @return Progress of this receive step.
        */
        MessageProgress endReceive();

        /** @brief Enqueues a ping with an optional payload of at most 125 bytes.
        */
        void ping(const char* payload = 0, std::size_t n = 0);

        /** @brief Enqueues a close frame and ends the stream after the close handshake.
        */
        void close(unsigned code = 1000,
                   const std::string& reason = std::string());

        /** @brief Returns the close status code of the handshake that ended the stream.
        */
        unsigned closeCode() const;

        /** @brief Returns the close reason of the handshake that ended the stream.
        */
        const std::string& closeReason() const;

    protected:
        /** @brief Called when data bytes were received.
        */
        virtual void onInput() = 0;

        /** @brief Called when data bytes were sent.
        */
        virtual void onOutput() = 0;

        /** @brief Called when the stream has ended.

            This object is still alive. The service releases it after
            this call returns. Do not call %endReceive() or %endSend().
        */
        virtual void onClose() = 0;

    private:
        void shutdown();

        void onInputReady();

        void onOutputReady();

        void onClosed();

    private:
        WebSocketService*    _service;
        System::EventLoop*   _loop;
        WebSocketChannel* _channel;
        std::string _protocol;
        bool _ended;
};

} // namespace Http

} // namespace Pt

#endif
