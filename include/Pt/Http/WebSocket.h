/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKET_H
#define PT_HTTP_WEBSOCKET_H

#include <Pt/Http/Api.h>
#include <Pt/Http/Stream.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Http {

class Client;



/** @brief Framed messages on an HTTP stream.

    %WebSocket formats WebSocket frames into the stream buffer of an
    upgraded HTTP connection. It is not an I/O device. The caller
    writes payload into %buffer() and sends that buffer as one frame.
    A receive parses one frame from the stream and leaves the payload
    in the same buffer.

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
    On the server, construct the socket with the %Stream from
    %Service::upgradeRequested(). %accept() retains that stream on a
    socket that already exists. The server keeps the connection. The
    socket formats that stream and does not own it.

    %beginSend() writes one frame. The opcode is the argument. The
    payload is what was written to %buffer() since the previous send.
    %outputReady() reports that the frame has left the stream buffer,
    and %endSend() completes the send.

    %beginReceive() reads one frame. %inputReady() reports that the
    frame is complete. %endReceive() completes the read. %frame() is
    the opcode, and %buffer() then holds the payload. A short read
    stays inside the socket until the frame is complete, so the ready
    signal means one whole frame.

    Ping and pong are control frames. %sendPing() and %sendPong()
    write those frames through the same stream buffer. Text and binary
    are the data payload. Unknown is the unset frame type.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocket : public Pt::Connectable
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
            Pong     ///< Pong frame
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

            Retains @a stream. The server keeps the connection.
        */
        void accept(Stream& stream);

        /** @brief Begins a client handshake for @a url.

            @a url is a request path, or a %ws:// URL whose host and
            port are the client's endpoint. @a origin is the Origin
            header. Host, port and TLS come from the client.

            @throw %std::logic_error if this socket was constructed
            from a stream.
        */
        void beginConnect(const std::string& url, const std::string& origin = std::string());

        /** @brief Returns the signal emitted when the handshake finishes.
        */
        Pt::Signal<WebSocket&>& connected()
        { return _connected; }

        /** @brief Completes the client handshake.

            @throw %std::exception if the handshake failed.
        */
        void endConnect();

        /** @brief Returns the payload buffer.

            Write payload here before %beginSend(). After
            %endReceive() this buffer holds the received payload.
        */
        std::streambuf& buffer();

        /** @brief Returns the opcode of the frame last received.
        */
        Frame frame() const
        { return _frame; }

        /** @brief Begins sending the payload in %buffer() as @a frame.
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

        /** @brief Returns the signal emitted when a frame was received.
        */
        Pt::Signal<WebSocket&>& inputReady()
        { return _inputReady; }

        /** @brief Returns the signal emitted when a frame was sent.
        */
        Pt::Signal<WebSocket&>& outputReady()
        { return _outputReady; }

        /** @brief Sets the stream timeout in milliseconds.

            The handshake timeout is %Client::setTimeout().
        */
        void setTimeout(std::size_t timeout);

        /** @brief Closes the WebSocket.
        */
        void close();

    private:
        void parseUrl(const std::string& url, const std::string& origin);

        static std::string createKey();

        Pt::uint32_t createMask();

        void finishHandshake(bool failed);

        void onRequestSent(Client& client);

        void onReply(Client& client);

        void onInput();

        void onOutput();

        void writeFrame(Frame frame, const char* payload, std::size_t n);

        void beginFrameRead();

        bool parseAvailable();

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
        Stream _stream;
        bool _isClient;
        std::string _path;
        Pt::Signal<WebSocket&> _connected;
        Pt::Signal<WebSocket&> _inputReady;
        Pt::Signal<WebSocket&> _outputReady;
        std::size_t _timeout;
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
};

}}

#endif
