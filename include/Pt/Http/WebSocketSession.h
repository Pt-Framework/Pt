/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSESSION_H
#define PT_HTTP_WEBSOCKETSESSION_H

#include <Pt/Http/Api.h>
#include <Pt/Http/WebSocketMessage.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <string>
#include <cstddef>

namespace Pt {

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

    The example connects the ready signals in the constructor and
    starts a receive. Domain state belongs in the derived session.

    @code
    class EchoSession : public Pt::Http::WebSocketSession
    {
        public:
            EchoSession(Pt::Http::WebSocketService& service,
                        Pt::Http::Stream& stream,
                        const Pt::Http::Reply& reply)
            : Pt::Http::WebSocketSession(service, stream, reply)
            {
                inputReady() += Pt::slot(*this, &EchoSession::onInputReady);
                outputReady() += Pt::slot(*this, &EchoSession::onOutputReady);
                beginReceive();
            }

        private:
            void onInputReady(Pt::Http::WebSocketSession&)
            {
                try
                {
                    Pt::Http::WebSocketProgress progress = endReceive();
                    if( ! progress.finished() )
                    {
                        incoming().discard();
                        beginReceive();
                        return;
                    }

                    incoming().clear();
                    beginReceive();
                }
                catch(const std::exception&)
                {
                    close();
                }
            }

            void onOutputReady(Pt::Http::WebSocketSession&)
            {
                try
                {
                    endSend();
                }
                catch(const std::exception&)
                {
                    close();
                }
            }
    };
    @endcode

    The service factory constructs the session with the service, with
    the stream of this upgrade, and with the opening reply. The reply
    is valid for the constructor call. The base constructor reads the
    selected subprotocol from that reply, binds the stream, and copies
    the service idle, ping-interval, ping and close timeouts and the
    data-message limit onto the connection. It does not store the
    reply. The derived constructor
    runs after that. Its members are initialized and the stream is
    open. Connect ready slots there, then start the first
    %beginReceive() or %beginSend().
    There is no separate accept callback. By the time the derived
    constructor body runs, the base has already bound the stream.

    %incoming() and %outgoing() are the two payloads. %beginSend()
    writes %outgoing(). Call %beginSend(false) while more body will
    be written. %beginReceive() reads into %incoming().
    %inputReady() runs when data bytes were received. Call
    %endReceive(), read %incoming() when progress reports body
    bytes, and start the next receive if the message is not
    finished. A ping does not emit %inputReady(). A pong runs
    %pong() and does not change %incoming(). %outputReady() runs
    when data bytes were sent. Call %endSend() and continue the
    send when progress is not finished. %closed() runs when the
    engine has ended the protocol and no local shutdown is
    outstanding. The stream stays bound for that call. After
    %closed() returns, the servlet releases the session. The
    destructor unbinds the stream. A call to %close() from
    %closed() does not unbind or release. A session that connects
    no %closed() slot is still released. Call %close() from
    %shutdownReady() after %endShutdown(), or from an I/O slot
    after a thrown end, to abort and release. The servlet also
    releases remaining sessions when it is destroyed.

    %beginReceive() starts frame processing. Until then, incoming
    frames stay in the stream. Control frames are processed while a
    receive is outstanding. A local %beginShutdown() also keeps input
    running until the peer replies with its close frame. The two are
    independent: a receive started before or during a shutdown still
    delivers what the peer sent before that reply.

    %protocol() is the single name the responder wrote as
    Sec-WebSocket-Protocol on the 101, or empty when none was
    selected. The base copies it from the opening reply before the
    derived constructor body runs. It is not the list the client
    offered. A reply that echoes more than one token, or a value
    that is not one protocol token, fails construction. The server
    then closes the stream.
    %service() reaches state shared by every connection of this
    endpoint. The idle, ping-interval, ping and close timeouts and
    the data-message limit are copied from the %WebSocketService
    before the derived constructor runs.
    Closing the session closes the stream. The session does not own
    the stream or the connection. The destructor unbinds the stream
    and does not release the session again. A derived constructor
    that throws still runs this destructor, so a failed construction
    does not leave an accepted stream without an owner. %close()
    during construction unbinds the stream. The servlet does not
    list that session and releases it after construction returns.

    Ping, shutdown and close stay control operations on this type.

    @code
    beginPing();
    beginShutdown(1000, "done");
    close();
    @endcode

    %beginPing() enqueues a ping. %pingReady() reports that its frame
    was written; call %endPing() from that signal to observe a write
    error. A received ping is answered by the engine. After a close
    frame is queued nothing further is written, so %beginPing() throws
    and a received ping is no longer answered. %beginShutdown() enqueues
    a close frame and keeps reading until
    the peer replies. A peer that never starts a receive never replies,
    so the idle timeout copied from the %WebSocketService is the only
    bound on that wait. %shutdownReady() reports the completed handshake;
    call %endShutdown() from that signal to observe an error, then
    %close(). %close() unbinds the stream and writes no frame. The
    servlet then releases this session. The destructor unbinds the
    stream. %beginSend() and %beginReceive() then throw. No close
    frame leaves the code at 1006. %closeCode() and %closeReason()
    report the handshake that ended the stream.

    @ingroup Pt-Http-WebSocket-Server
*/
class PT_HTTP_API WebSocketSession : public Connectable
{
    friend class WebSocketService;

    public:
        /** @brief Binds @a stream for @a service.

            The active %WebSocketServlet owns this session. @a reply
            is the opening handshake reply and is valid for this
            call. The selected subprotocol is copied from it. The
            reply is not stored.

            The idle, ping-interval, ping and close timeouts and the
            data-message limit are copied from @a service.

            @throw %std::runtime_error if Sec-WebSocket-Protocol is
            not one protocol token.
        */
        WebSocketSession(WebSocketService& service,
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

            The completion flag is true when this is the last part of
            the message. Call %beginSend(false) while more body will
            be written.

            @param finished True if this is the last part of the message.
        */
        void beginSend(bool finished = true);

        /** @brief Completes the send started by %beginSend().

            @return Progress of this send step.

            @throw %std::logic_error if the send has not completed or
            the stream has ended.
        */
        WebSocketProgress endSend();

        /** @brief Begins receiving into %incoming().

            Starts processing incoming frames. Frames received before
            this call stay in the stream.

            @throw %std::logic_error if a receive is outstanding,
            or the stream has ended.
            @throw %std::exception if a protocol violation ended the
            frame stream, or a previous read failed.
        */
        void beginReceive();

        /** @brief Completes the receive started by %beginReceive().

            @return Progress of this receive step.

            @throw %std::logic_error if the receive has not completed.
            @throw %std::exception if a protocol violation ended the
            frame stream, or a previous read failed.
        */
        WebSocketProgress endReceive();

        /** @brief Begins writing a ping with an optional payload of at most 125 bytes.

            @throw %std::invalid_argument if @a n is greater than 125,
            or if @a payload is null and @a n is nonzero.
            @throw %std::logic_error if a ping is outstanding, a close is
            queued, or the stream has ended.
        */
        void beginPing(const char* payload = 0, std::size_t n = 0);

        /** @brief Completes the ping started by %beginPing().

            @throw %std::logic_error if the ping is not ready.
            @throw %std::exception if writing the ping failed.
        */
        void endPing();

        /** @brief Begins a close handshake.

            @throw %std::invalid_argument if @a code is 1005, 1006 or
            1015, if @a reason is longer than 123 bytes, or if @a reason
            is not valid UTF-8.
            @throw %std::logic_error if a shutdown is outstanding,
            or a close is already queued.
        */
        void beginShutdown(unsigned code = 1000,
                   const std::string& reason = std::string());

        /** @brief Completes the shutdown started by %beginShutdown().

            @throw %std::logic_error if the shutdown is not ready.
            @throw %std::exception if the close handshake failed.
        */
        void endShutdown();

        /** @brief Releases the stream and writes no frame.

            Does not block and emits nothing. Unbinds the stream.
            The servlet then releases this session. A call from
            %closed() does not unbind or release. %beginSend()
            and %beginReceive() then throw. No close frame leaves
            %closeCode() at 1006.
        */
        void close();

        /** @brief Returns the close status code of the handshake that ended the stream.
        */
        unsigned closeCode() const;

        /** @brief Returns the close reason of the handshake that ended the stream.
        */
        const std::string& closeReason() const;

        /** @brief Returns true when a stream is bound.

            False after %close(). True during %closed().
        */
        bool isOpen() const;

        /** @brief Returns the signal emitted when data bytes were received.
        */
        Pt::Signal<WebSocketSession&>& inputReady()
        { return _inputReady; }

        /** @brief Returns the signal emitted when data bytes were sent.
        */
        Pt::Signal<WebSocketSession&>& outputReady()
        { return _outputReady; }

        /** @brief Returns the signal emitted when a ping operation completes.

            Call %endPing() from the signal to observe a write error.
        */
        Pt::Signal<WebSocketSession&>& pingReady()
        { return _pingReady; }

        /** @brief Returns the signal emitted when a shutdown operation completes.

            The stream stays bound. Call %endShutdown() from the signal
            to observe a close-handshake error, then %close().
        */
        Pt::Signal<WebSocketSession&>& shutdownReady()
        { return _shutdownReady; }

        /** @brief Returns the signal emitted when the engine has ended the protocol.

            No local shutdown is outstanding. The stream stays bound
            for this call. After it returns, the servlet releases the
            session. The destructor unbinds the stream. A call to
            %close() from this signal does not unbind or release.
        */
        Pt::Signal<WebSocketSession&>& closed()
        { return _closed; }

        /** @brief Returns the signal emitted for a received pong.

            The payload contains arbitrary bytes and is valid for the
            duration of the signal call. A pong does not change
            %incoming() or emit %inputReady().
        */
        Pt::Signal<WebSocketSession&, const std::string&>& pong()
        { return _pong; }

    private:
        void onInputReady();

        void onOutputReady();

        void onPingReady();

        void onShutdownReady();

        void onClosed();

        void onPong(const std::string& payload);

    private:
        WebSocketService* _service;
        WebSocketChannel* _channel;
        std::string _protocol;
        bool _inClosed;
        Pt::Signal<WebSocketSession&> _inputReady;
        Pt::Signal<WebSocketSession&> _outputReady;
        Pt::Signal<WebSocketSession&> _pingReady;
        Pt::Signal<WebSocketSession&> _shutdownReady;
        Pt::Signal<WebSocketSession&> _closed;
        Pt::Signal<WebSocketSession&, const std::string&> _pong;
};

} // namespace Http

} // namespace Pt

#endif
