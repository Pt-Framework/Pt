/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKET_H
#define PT_HTTP_WEBSOCKET_H

#include <Pt/Http/Api.h>
#include <Pt/Http/StreamSession.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/System/Timer.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>

namespace Pt {

namespace Http {

class Client;



/** @brief Framed messages on an HTTP stream.

    %WebSocket formats WebSocket frames into the stream buffer of an
    upgraded HTTP connection. It is not an I/O device. The payload is
    %body(), an iostream, the same surface a %Message uses for its
    body. Write that stream and send it as one frame. A receive parses
    one frame from the connection stream and leaves the payload in
    %body(). The connection stream buffer stays inside the socket.

    On the client, construct the socket with the %Client that will
    perform the handshake. The socket stores a reference and does not
    own that client, so the client must outlive the socket, including
    a handshake that is still waiting on the loop. Host, port, event
    loop, timeout and TLS are settings of that client. Connect
    %connected() and call %beginConnect() with the request path, or
    with a %ws:// URL whose host and port are the client's endpoint.
    The handshake is an HTTP request and reply on that client.
    %endConnect() completes it and throws if it failed. The 101 reply
    becomes the %Stream this socket formats. After the handshake the
    socket no longer uses the client.
    On the server, %WebSocketService constructs the socket with the
    %Stream from %Service::onUpgrade() and emits %accepted(). Binding
    accepts the upgrade. The server keeps the connection. The socket
    formats that stream and does not own it.

    %beginSend() writes one frame. The opcode is the argument. The
    payload is what was written to %body() since the previous send.
    %outputReady() reports that the frame has left the connection
    stream buffer, and %endSend() completes the send.

    %beginReceive() reads one frame. %inputReady() reports that the
    frame is complete. %endReceive() completes the read. %frame() is
    the opcode, and %body() then holds the payload. %available() is
    how many of those bytes can be read. A short read stays inside
    the socket until the frame is complete, so the ready signal means
    one whole frame.

    Ping and pong are control frames. %sendPing() and %sendPong()
    write those frames through the connection stream buffer. Text and
    binary are the data payload. Unknown is the unset frame type.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocket : public StreamSession
                            , public Pt::Connectable
{
    public:
        /** @brief WebSocket frame opcode.
        */
        enum Frame
        {
            Unknown, ///< Unset frame type
            Text,    ///< Text data frame
            Binary,  ///< Binary data frame
            Ping,    ///< Ping frame
            Pong,    ///< Pong frame
            Close    ///< Close frame
        };

        /** @brief Creates a WebSocket that handshakes through @a client.

            Stores a reference to @a client. The client must outlive
            this socket.
        */
        explicit WebSocket(Client& client);

        /** @brief Creates a WebSocket that accepts @a stream.
        */
        WebSocket(Stream& stream);

        /** @brief Destructor.
        */
        ~WebSocket();

        /** @brief Accepts an upgraded @a stream.

            Binds @a stream. The server keeps the connection.
        */
        void accept(Stream& stream);

        /** @brief Begins a client handshake for @a url.

            @a url is a request path, or a %ws:// URL whose host and
            port are the client's endpoint. @a origin is the Origin
            header. Host, port and TLS come from the client.

            @throw %std::logic_error if this socket was constructed
            from a stream.
        */
        void beginConnect(const std::string& url,
                          const std::string& origin = std::string());

        /** @brief Returns the signal emitted when the handshake finishes.
        */
        Pt::Signal<WebSocket&>& connected()
        { return _connected; }

        /** @brief Completes the client handshake.

            @throw %std::exception if the handshake failed.
        */
        void endConnect();

        /** @brief Returns the payload stream.

            Write payload here before %beginSend(). After
            %endReceive() this stream holds the received payload.
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
        */
        void discard();

        /** @brief Returns the opcode of the frame last received.
        */
        Frame frame() const
        { return _frame; }

        /** @brief Begins sending the payload in %body() as @a frame.
        */
        void beginSend(Frame frame);

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

        /** @brief Returns the signal emitted when a frame was received.
        */
        Pt::Signal<WebSocket&>& inputReady()
        { return _inputReady; }

        /** @brief Returns the signal emitted when a frame was sent.
        */
        Pt::Signal<WebSocket&>& outputReady()
        { return _outputReady; }

        /** @brief Returns the signal emitted when the stream ends.

            Emitted while this socket is still alive. The stream has
            already cleared its session pointer. Peer close, an I/O
            error, a close frame and destruction of the stream all
            emit it. The owner deletes this socket.
        */
        Pt::Signal<WebSocket&>& closed()
        { return _closed; }

        /** @brief Sets the stream timeout in milliseconds.

            The handshake timeout is %Client::setTimeout().
        */
        void setTimeout(std::size_t timeout);

        /** @brief Closes the stream when a frame exceeds @a maxSize.

            Zero disables the limit.
        */
        void setMaxMessageSize(std::size_t maxSize);

        /** @brief Closes the stream after @a ms without a finished transfer.

            Zero disables the idle timeout. A finished send or receive
            restarts it.
        */
        void setIdleTimeout(std::size_t ms);

    private:
        void parseUrl(const std::string& url, const std::string& origin);

        static std::string createKey();

        Pt::uint32_t createMask();

        void finishHandshake(bool failed);

        void onRequestSent(Client& client);

        void onReply(Client& client);

        virtual void onCloseStream(Stream&) override;

        void onInput();

        void onOutput();

        void writeFrame(Frame frame, const char* payload, std::size_t n);

        void beginFrameRead();

        bool parseAvailable();

        void failStream();

        void onIdleTimeout();

    private:
        enum State
        {
            Idle,
            Connecting,
            Handshake,
            ReceiveHeader,
            ReceiveLength,
            ReceiveMask,
            ReceivePayload,
            Sending
        };

        Client* _client;
        bool _isClient;
        std::string _path;
        Pt::Signal<WebSocket&> _connected;
        Pt::Signal<WebSocket&> _inputReady;
        Pt::Signal<WebSocket&> _outputReady;
        Pt::Signal<WebSocket&> _closed;
        std::size_t _timeout;
        std::size_t _maxMessageSize;
        std::size_t _idleTimeout;
        System::Timer _idleTimer;
        bool _error;
        State _state;
        Frame _frame;
        bool _masked;
        Pt::uint32_t _mask;
        std::size_t _payloadSize;
        std::size_t _payloadGot;
        std::size_t _headerNeed;
        std::vector<char> _header;
        std::vector<char> _payload;
        class PayloadBuffer;
        PayloadBuffer* _payloadBuffer;
        std::iostream _body;
};

} // namespace Http

} // namespace Pt

#endif
