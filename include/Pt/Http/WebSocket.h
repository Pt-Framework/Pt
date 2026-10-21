/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKET_H
#define PT_HTTP_WEBSOCKET_H

#include <Pt/Http/Api.h>
#include <Pt/Http/WebSocketMessage.h>
#include <Pt/Http/Message.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Http {

class Client;
class WebSocketChannel;


/** @brief Client handshake and messages on an upgraded stream.

    %WebSocket is the client side of a WebSocket upgrade. It performs
    the HTTP handshake through an HTTP user agent and then sends and
    receives payloads on the stream that handshake produces. Host,
    port, event loop, timeout and TLS are settings of that agent. The
    socket stores a reference and does not own it, so the agent must
    outlive the socket, including a handshake that is still waiting
    on the loop. After a finished 101 the socket formats the stream
    the client already owns. The client still owns the connection.
    Closing the socket closes the stream. The socket does not own the
    stream. A client masks every frame it writes.

    The example completes the handshake and then writes a text
    message.

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
        if( ! progress.finished() )
        {
            socket.incoming().discard();
            socket.beginReceive();
            return;
        }

        socket.incoming().clear();
        socket.beginReceive();
    }

    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
        socket.outgoing().body() << "hello";
        socket.beginSend();
        socket.beginReceive();
    }

    void onClosed(Pt::Http::WebSocket& socket)
    {
    }

    Pt::System::MainLoop loop;
    Pt::Net::Endpoint ep("localhost", 80);
    Pt::Http::Client client(loop, ep);
    Pt::Http::WebSocket socket(client);
    socket.connected() += Pt::slot(onConnected);
    socket.outputReady() += Pt::slot(onOutputReady);
    socket.inputReady() += Pt::slot(onInputReady);
    socket.closed() += Pt::slot(onClosed);
    socket.beginConnect("/ws");
    loop.run();
    @endcode

    Construct the socket with the %Client that performs the
    handshake. Connect %connected() and call %beginConnect() with a
    request path, or with a %ws:// URL whose host and port are
    already the client's endpoint. The path and query of that URL
    become the request URL; the client does not take the host from
    the URL. The optional second argument is the Origin header.
    %endConnect() completes the handshake and throws if it failed.
    %addProtocol() offers one Sec-WebSocket-Protocol name before
    %beginConnect(). Several calls offer several names, in order.
    After a finished handshake %protocol() is the single name the
    server echoed, or empty when the server selected none. An echo
    that was not offered fails the handshake. After that success
    the socket no longer uses the client for messages.

    The socket owns two %WebSocketMessage objects, %incoming() and
    %outgoing(), the way a client owns a request and a reply. Set
    the type, write the body, and call %beginSend(). %outputReady()
    reports that data bytes were sent. %endSend() returns
    %MessageProgress. If the send is not finished, %beginSend()
    continues the same message. %beginReceive() fills %incoming().
    %inputReady() reports that data bytes were received.
    %endReceive() returns progress. After %finished(), %clear()
    drops the body so the same object can carry the next message.
    %beginReceive() starts frame processing. Until then, incoming
    frames stay in the stream. Control frames are processed while a
    receive is outstanding. A local %beginShutdown() also keeps input
    running until the peer replies with its close frame. The two are
    independent: a receive started before or during a shutdown still
    delivers what the peer sent before that reply.

    Ping, pong, shutdown and close are socket operations, not
    messages.

    @code
    socket.beginPing();
    socket.beginShutdown(1000, "done");
    socket.close();
    @endcode

    %beginPing() enqueues a ping. %pingReady() reports that its frame
    was written; call %endPing() from that signal to observe a write
    error. A received ping is answered by the engine. After a close
    frame is queued nothing further is written, so %beginPing() throws
    and a received ping is no longer answered. A received pong
    is reported by %pong(). Neither is delivered through %incoming().
    %beginShutdown() enqueues a close frame with a status code and a
    reason, then keeps reading until the peer replies. %shutdownReady()
    reports the completed handshake before %closed(); call
    %endShutdown() from that signal to observe an error. A peer that
    never starts a receive never replies, so %setIdleTimeout() is the
    only bound on that wait. It is zero on this type until the
    application sets it. A received
    close is answered by the engine. %close() releases the stream and writes no frame. It
    does not block. The destructor calls it. %beginSend() and
    %beginReceive() then throw. %closed() is not emitted from the
    destructor. %closeCode() and %closeReason() report the handshake
    that ended the stream. No close frame leaves the code at 1006.
    %closed() is emitted while this socket is still alive. The
    stream has already cleared its channel pointer. Peer close, an
    I/O end with no close frame, a close frame, a protocol violation,
    and an idle timeout emit it. An I/O error does not: it is reported
    to every outstanding operation, the stream stays open, and the
    application calls %close(). Destruction of the stream detaches the
    channel and does not emit it. It ends an outstanding send or
    receive. The owner deletes this socket.

    On the server the application does not construct this type. The
    server facade is %WebSocketSession, which exposes the same
    message operations without a client handshake.

    @ingroup Pt-Http-WebSocket-Client
*/
class PT_HTTP_API WebSocket : public Pt::Connectable
{
    public:
        /** @brief Creates a WebSocket that handshakes through @a client.

            Stores a reference to @a client. The client must outlive
            this socket.
        */
        explicit WebSocket(Client& client);

        /** @brief Calls %close().
        */
        ~WebSocket();

        /** @brief Begins a client handshake for @a url.

            @a url is a request path, or a %ws:// URL whose host and
            port are the client's endpoint. @a origin is the Origin
            header. Host, port and TLS come from the client.
        */
        void beginConnect(const std::string& url,
                          const std::string& origin = std::string());

        /** @brief Offers one Sec-WebSocket-Protocol name.

            Several calls offer several names, in order. The offer is
            sent by the next %beginConnect().

            @throw %std::invalid_argument if @a name is empty or is
            not a single protocol token.
        */
        void addProtocol(const std::string& name);

        /** @brief Returns the protocol name echoed by the server.

            Empty before a finished handshake, and empty when the
            server selected no name.
        */
        const std::string& protocol() const
        { return _protocol; }

        /** @brief Returns the signal emitted when the handshake finishes.
        */
        Pt::Signal<WebSocket&>& connected()
        { return _connected; }

        /** @brief Completes the client handshake.

            @throw %std::exception if the handshake failed.
        */
        void endConnect();

        /** @brief Returns the incoming message.
        */
        WebSocketMessage& incoming();

        /** @brief Returns the outgoing message.
        */
        WebSocketMessage& outgoing();

        /** @brief Begins sending %outgoing().

            @throw %std::logic_error if a send is outstanding, the
            handshake is not finished, or a shutdown is queued.
            @throw %std::invalid_argument if the type is %Unknown, the
            body is larger than %setMaxMessageSize(), or a text body
            is not valid UTF-8.
            @throw %std::exception if a previous write failed.
        */
        void beginSend();

        /** @brief Completes the send started by %beginSend().

            @return Progress of this send step.

            @throw %std::logic_error if the send has not completed or
            the stream has ended.
        */
        MessageProgress endSend();

        /** @brief Begins receiving into %incoming().

            Starts processing incoming frames. Frames received before
            this call stay in the stream.

            @throw %std::logic_error if a receive is outstanding,
            the handshake is not finished, or the stream has ended.
            @throw %std::exception if a protocol violation ended the
            frame stream, or a previous read failed.
        */
        void beginReceive();

        /** @brief Completes the receive started by %beginReceive().

            @return Progress of this receive step.

            @throw %std::logic_error if the receive has not completed
            or the stream has ended.
        */
        MessageProgress endReceive();

        /** @brief Begins writing a ping with an optional payload of at most 125 bytes.

            @throw %std::invalid_argument if @a n is greater than 125,
            or if @a payload is null and @a n is nonzero.
            @throw %std::logic_error if a ping is outstanding, a close is
            queued, or the handshake is not finished.
            @throw %std::exception if a previous write failed.
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
            the handshake is not finished, or a close is already queued.
            @throw %std::exception if a previous write failed.
        */
        void beginShutdown(unsigned code = 1000,
                           const std::string& reason = std::string());

        /** @brief Completes the shutdown started by %beginShutdown().

            @throw %std::logic_error if the shutdown is not ready.
            @throw %std::exception if the close handshake failed.
        */
        void endShutdown();

        /** @brief Releases the stream and writes no frame.

            Does not block and emits nothing. The destructor calls
            this. %beginSend() and %beginReceive() then throw. No
            close frame leaves %closeCode() at 1006.
        */
        void close();

        /** @brief Returns the close status code of the handshake that ended the stream.
        */
        unsigned closeCode() const;

        /** @brief Returns the close reason of the handshake that ended the stream.
        */
        const std::string& closeReason() const;

        /** @brief Returns the signal emitted when data bytes were received.
        */
        Pt::Signal<WebSocket&>& inputReady()
        { return _inputReady; }

        /** @brief Returns the signal emitted when data bytes were sent.
        */
        Pt::Signal<WebSocket&>& outputReady()
        { return _outputReady; }

        /** @brief Returns the signal emitted when a ping operation completes.

            Call %endPing() from the signal to observe a write error.
        */
        Pt::Signal<WebSocket&>& pingReady()
        { return _pingReady; }

        /** @brief Returns the signal emitted when a shutdown operation completes.

            The stream is detached. Call %endShutdown() from the signal
            to observe a close-handshake error. This signal is emitted
            before %closed().
        */
        Pt::Signal<WebSocket&>& shutdownReady()
        { return _shutdownReady; }

        /** @brief Returns the signal emitted for a received pong.

            The payload contains arbitrary bytes and is valid for the
            duration of the signal call. A pong does not change
            %incoming() or emit %inputReady().
        */
        Pt::Signal<WebSocket&, const std::string&>& pong()
        { return _pong; }

        /** @brief Returns the signal emitted when the stream ends.

            Emitted while this socket is still alive, after an end
            this socket did not start. Peer close, an idle timeout,
            a read that returns no bytes, a protocol violation, and a
            finished shutdown emit it. A finished shutdown emits
            %shutdownReady() first. An I/O error does not emit it. It
            runs %inputReady() or %outputReady() instead, the matching
            end throws, and the stream stays open until the
            application calls %close(). %close() and destruction do
            not emit it.
        */
        Pt::Signal<WebSocket&>& closed()
        { return _closed; }

        /** @brief Sets the stream timeout in milliseconds.

            The handshake timeout is %Client::setTimeout().
        */
        void setTimeout(std::size_t timeout);

        /** @brief Closes the stream when a data message exceeds @a maxSize.

            Zero disables the limit. The count is the declared payload
            of one data message from the first data opcode to FIN.
        */
        void setMaxMessageSize(std::size_t maxSize);

        /** @brief Closes the stream after @a ms without a transfer.

            Zero disables the idle timeout. Transferred data bytes, a
            received ping, and a received pong restart it. It is also
            the only bound on a close handshake the peer never answers.
        */
        void setIdleTimeout(std::size_t ms);

    private:
        void parseUrl(const std::string& url, const std::string& origin);

        static std::string createKey();

        void finishHandshake(bool failed);

        void onRequestSent(Client& client);

        void onReply(Client& client);

        void onInputReady();

        void onOutputReady();

        void onPingReady();

        void onShutdownReady();

        void onPong(const std::string& payload);

        void onClosed();

    private:
        Client* _client;
        WebSocketChannel* _channel;
        std::string _path;
        std::string _protocol;
        std::vector<std::string> _protocols;
        bool _error;
        bool _connecting;
        Pt::Signal<WebSocket&> _connected;
        Pt::Signal<WebSocket&> _inputReady;
        Pt::Signal<WebSocket&> _outputReady;
        Pt::Signal<WebSocket&> _pingReady;
        Pt::Signal<WebSocket&> _shutdownReady;
        Pt::Signal<WebSocket&, const std::string&> _pong;
        Pt::Signal<WebSocket&> _closed;
};

} // namespace Http

} // namespace Pt

#endif
