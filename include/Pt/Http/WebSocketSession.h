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
class WebSocketServlet;
class WebSocketService;
class WebSocketConnection;

/** @brief Server facade of one accepted WebSocket stream.

    %WebSocketSession is the application object of one upgraded
    stream. A %WebSocketServlet asks the %WebSocketService to create
    it when a handshake has finished, holds it, and releases it when
    the stream ends or when the servlet is destroyed. Derive from it
    to keep the state that must survive from one message to the next:
    a subscription, a cursor, a user, or a reference into the
    application domain.

    The session formats messages on the stream the HTTP server already
    owns. It is not a %StreamSession. The bind belongs to the message
    connection inside the session, and a second bind would throw. It
    is not a %Responder. The handshake responder has already been
    released when the session begins. Application code does not see
    that connection, the same way an HTTP caller sees %Client and not
    the connection behind it.

    The servlet constructs the session with itself, with the loop
    that serializes this stream, and with the stream of this upgrade.
    The base constructor binds the stream, copies the service limits
    onto the connection, and stores the loop. The derived constructor
    runs after that. Its members are initialized, the stream is open,
    and %loop() is the loop of this stream. Start the first
    %beginReceive() or %beginSend() there. There is no separate
    accept callback. By the time the derived constructor body runs,
    the base has already bound the stream.

    The message operations are methods of this session. %incoming()
    and %outgoing() are the two payloads. %beginSend() writes
    %outgoing(). %beginReceive() reads into %incoming(). Ping and
    close stay control operations on this type. A server does not
    mask the frames it writes.

    %onInput(), %onOutput(), and %onClose() are the later events. They
    take no arguments. The session and the loop do not change between
    callbacks. %onInput() runs when data bytes were received. Call
    %endReceive(), read %incoming() when progress reports body bytes,
    and start the next receive if the message is not finished. A ping
    or a pong does not run %onInput(). %onOutput() runs when data
    bytes were sent. Call %endSend() and continue the send when
    progress is not finished. %onClose() runs while this object is
    still alive, after the stream has ended. Do not call %endReceive()
    or %endSend() from %onClose(). The %WebSocketServlet releases the
    session after %onClose() returns.

    %service() reaches state shared by every connection of this
    endpoint. %loop() is where a timer or posted work must run.
    Closing the session closes the stream. The session does not own
    the stream or the connection.

    The destructor closes the stream. A derived constructor that
    throws still runs this destructor, so a failed construction does
    not leave an accepted stream without an owner.

    The example echoes by receiving one message and starting the next
    receive. Domain state belongs in the derived session.

    @code
    class EchoSession : public Pt::Http::WebSocketSession
    {
        public:
            EchoSession(Pt::Http::WebSocketServlet& servlet,
                        Pt::System::EventLoop& loop,
                        Pt::Http::Stream& stream)
            : Pt::Http::WebSocketSession(servlet, loop, stream)
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

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketSession : public Connectable
{
    friend class WebSocketServlet;

    public:
        /** @brief Binds @a stream on @a loop.

            @a servlet holds this session. @a loop must be the loop of
            @a stream.

            @throw %std::logic_error if @a loop is not the loop of
            @a stream.
        */
        WebSocketSession(WebSocketServlet& servlet,
                         System::EventLoop& loop,
                         Stream& stream);

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

            This object is still alive. The servlet releases it after
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
        WebSocketServlet*    _servlet;
        System::EventLoop*   _loop;
        WebSocketConnection* _connection;
};

} // namespace Http

} // namespace Pt

#endif
