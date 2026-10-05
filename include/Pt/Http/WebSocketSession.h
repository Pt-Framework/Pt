/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSESSION_H
#define PT_HTTP_WEBSOCKETSESSION_H

#include <Pt/Http/Api.h>
#include <Pt/Http/WebSocket.h>
#include <Pt/Connectable.h>
#include <iosfwd>
#include <cstddef>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Http {

class Stream;
class WebSocketServlet;
class WebSocketService;
class WebSocketConnection;;

/** @brief Server facade of one accepted WebSocket stream.

    %WebSocketSession is the application object of one upgraded
    stream. A %WebSocketServlet asks the %WebSocketService to create
    it when a handshake has finished, holds it, and releases it when
    the stream ends or when the servlet is destroyed. Derive from it
    to keep the state that must survive from one frame to the next: a
    subscription, a cursor, a user, or a reference into the
    application domain.

    The session formats frames on the stream the HTTP server already
    owns. It is not a %StreamSession. The bind belongs to the frame
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

    The frame operations are methods of this session. %body() is the
    payload stream. %beginSend() writes one frame from that body.
    %beginReceive() reads one frame into it. %frame() is the opcode.
    Ping, pong, and close stay control operations on this type. A
    server does not mask the frames it writes.

    %onInput(), %onOutput(), and %onClose() are the later events. They
    take no arguments. The session and the loop do not change between
    callbacks. %onInput() runs when one whole frame has been received.
    Call %endReceive(), read %body(), and start the next transfer.
    %onOutput() runs when a frame has left the stream buffer. Call
    %endSend() and start the next send when more data remains.
    %onClose() runs while this object is still alive, after the stream
    has ended. The %WebSocketServlet releases the session after
    %onClose() returns. Destroying that servlet closes a session that
    is still open and then releases it the same way. The service is
    still alive, because the servlet is destroyed first.

    %service() reaches state shared by every connection of this
    endpoint. %loop() is where a timer or posted work must run.
    Closing the session closes the stream. The session does not own
    the stream or the connection.

    The destructor closes the stream. A derived constructor that
    throws still runs this destructor, so a failed construction does
    not leave an accepted stream without an owner.

    The example echoes by receiving one frame and starting the next
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
                endReceive();
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

        /** @brief Returns the payload stream.

            Write payload here before %beginSend(). After
            %endReceive() this stream holds the received payload.
        */
        std::iostream& body();

        /** @brief Returns how many payload bytes can be read.
        */
        std::size_t available() const;

        /** @brief Returns how many payload bytes are waiting to be sent.
        */
        std::size_t pending() const;

        /** @brief Drops the buffered payload.
        */
        void discard();

        /** @brief Returns the opcode of the frame last received.
        */
        WebSocket::Frame frame() const;

        /** @brief Begins sending the payload in %body() as @a frame.
        */
        void beginSend(WebSocket::Frame frame);

        /** @brief Completes the send started by %beginSend().
        */
        void endSend();

        /** @brief Begins receiving one frame.
        */
        void beginReceive();

        /** @brief Completes the receive started by %beginReceive().
        */
        void endReceive();

        /** @brief Sends a ping frame.
        */
        void sendPing();

        /** @brief Sends a pong frame.
        */
        void sendPong();

        /** @brief Sends a close frame and closes the stream.
        */
        void close();

    protected:
        /** @brief Called when one frame has been received.
        */
        virtual void onInput() = 0;

        /** @brief Called when one frame has been sent.
        */
        virtual void onOutput() = 0;

        /** @brief Called when the stream has ended.

            This object is still alive. The servlet releases it after
            this call returns.
        */
        virtual void onClose() = 0;

    private:
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
