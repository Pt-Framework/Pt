/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKET_H
#define PT_HTTP_WEBSOCKET_H

#include <Pt/Http/Api.h>
#include <Pt/Http/StreamSession.h>
#include <Pt/Http/WebSocketFrame.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/System/Timer.h>
#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Http {

class Client;
class WebSocketSession;


/** @brief Framed messages on an HTTP stream.

    %WebSocket formats WebSocket frames into the stream buffer of an
    upgraded HTTP connection and parses frames out of it. It is not an
    I/O device. The HTTP core does not know frames. The handshake
    remains an HTTP exchange. The frame exists only after the server
    has written the upgrade reply and opened the stream.

    The socket owns two %WebSocketFrame objects for its life.
    %output() is the frame the next %beginSend() writes. %input() is
    the frame the next %beginReceive() fills. The caller does not
    pass a frame in, and does not construct one. The two frames are
    distinct buffers. A receive does not overwrite the outbound
    payload, and a send does not consume the inbound payload. The
    caller may fill %output() while a receive is in progress, and may
    read %input() while a send is in progress.

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
    On the server the application does not construct a %WebSocket.
    %WebSocketSession contains one and binds it to the stream of the
    upgrade. Binding accepts the upgrade. The server keeps the
    connection. The socket formats that stream and does not own it.

    A send is fill, begin, ready, end. Set the opcode and FIN on
    %output(), write %output().body(), and call %beginSend().
    %outputReady() reports that the frame has left the connection
    stream buffer, and %endSend() completes the send. The output
    frame still holds what was sent. Nothing in %endSend() discards
    the body. The caller clears it or overwrites it before the next
    send.

    @code
    Pt::Http::WebSocketFrame& out = socket.output();
    out.setType(Pt::Http::WebSocketFrame::Text);
    out.setFin(true);
    out.body() << "hello";
    socket.beginSend();
    @endcode

    A receive is begin, ready, end, read. %beginReceive() reads one
    frame. %inputReady() reports that the frame is complete.
    %endReceive() completes the read. %input().type() is the opcode,
    and %input().body() holds the payload. A short read stays inside
    the socket until the frame is complete, so the ready signal means
    one whole frame, not one reassembled message.

    One transfer per direction still holds. A second %beginSend()
    before %endSend() is an error. A second %beginReceive() before
    %endReceive() is an error. %WebSocketFrame::discard() and
    %WebSocketFrame::clear() on a frame that has a transfer in flight
    are an error.

    Ping, pong, and close are frames. The caller can fill %output()
    with %WebSocketFrame::Ping, %WebSocketFrame::Pong, or
    %WebSocketFrame::Close and send it. %sendPing() and %sendPong()
    are the helpers for the common case. They set the output opcode,
    leave the payload empty unless the caller has already written
    %output().body(), and send that frame. They are valid only when
    no output transfer is active. %close() sets the output type to
    %Close, writes the close code and reason the caller set on
    %output(), sends that frame, and then closes the stream. The
    default code is 1000. A received close frame arrives on %input()
    like any other frame. The socket then ends the stream. A received
    ping is also an input frame. The socket does not answer it.

    Masking stays inside the socket. The client masks every frame it
    sends. The server rejects a masked frame it did not expect and
    unmasks a frame the client sent. RSV stays internal until an
    extension exists.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocket : public StreamSession
                            , public Pt::Connectable
{
    friend class WebSocketSession;

    public:
        /** @brief Creates a WebSocket that handshakes through @a client.

            Stores a reference to @a client. The client must outlive
            this socket.
        */
        explicit WebSocket(Client& client);

        /** @brief Destructor.
        */
        ~WebSocket();

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

        /** @brief Returns the client used for the handshake.

            Null after a server accept, and after a finished handshake.
        */
        Client* client()
        { return _client; }

        /** @brief Returns the frame the next receive fills.
        */
        WebSocketFrame& input()
        { return _input; }

        /** @brief Returns the frame the next receive fills.
        */
        const WebSocketFrame& input() const
        { return _input; }

        /** @brief Returns the frame the next send writes.
        */
        WebSocketFrame& output()
        { return _output; }

        /** @brief Returns the frame the next send writes.
        */
        const WebSocketFrame& output() const
        { return _output; }

        /** @brief Begins sending %output().

            The opcode and FIN are already on %output(). The payload
            is what %output().body() holds.

            @throw %std::logic_error if no stream is bound, if an
            output transfer is already active, or if a control frame
            is not final.
        */
        void beginSend();

        /** @brief Completes the send started by %beginSend().

            The output frame still holds what was sent.
        */
        void endSend();

        /** @brief Begins receiving one frame into %input().

            @throw %std::logic_error if no stream is bound, or if an
            input transfer is already active.
        */
        void beginReceive();

        /** @brief Completes the receive started by %beginReceive().

            %input().type() is the opcode and %input().body() holds
            the payload.
        */
        void endReceive();

        /** @brief Sends %output() as a ping frame.

            Leaves the payload empty unless the caller has already
            written %output().body(). Valid only when no output
            transfer is active.
        */
        void sendPing();

        /** @brief Sends %output() as a pong frame.

            Leaves the payload empty unless the caller has already
            written %output().body(). Valid only when no output
            transfer is active.
        */
        void sendPong();

        /** @brief Sends a close frame and closes the stream.

            Uses the close code and reason set on %output(). The
            default code is 1000.

            @throw %std::logic_error if an output transfer is active.
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

            Zero disables the limit. The limit is one frame, not a
            reassembled message.
        */
        void setMaxMessageSize(std::size_t maxSize);

        /** @brief Closes the stream after @a ms without a finished transfer.

            Zero disables the idle timeout. A finished send or receive
            restarts it.
        */
        void setIdleTimeout(std::size_t ms);

    protected:
        /** @brief Creates a server socket with no stream.
        */
        WebSocket();

        /** @brief Accepts an upgraded @a stream.

            Binds @a stream. The server keeps the connection.
        */
        void accept(Stream& stream);

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

        void writeFrame(WebSocketFrame::Type type, bool fin,
                        const char* payload, std::size_t n);

        void beginFrameRead();

        bool parseAvailable();

        void failStream();

        void onIdleTimeout();

        void shutdown(bool sendClose);

    private:
        enum State
        {
            Idle,
            Connecting,
            Handshake,
            ReceiveHeader,
            ReceiveLength,
            ReceiveMask,
            ReceivePayload
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
        bool _outputActive;
        bool _inputActive;
        bool _peerClose;
        State _state;
        bool _fin;
        WebSocketFrame::Type _frame;
        bool _masked;
        Pt::uint32_t _mask;
        std::size_t _payloadSize;
        std::size_t _payloadGot;
        std::size_t _headerNeed;
        std::vector<char> _header;
        std::vector<char> _payload;
        WebSocketFrame _input;
        WebSocketFrame _output;
};

} // namespace Http

} // namespace Pt

#endif
