/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKET_H
#define PT_HTTP_WEBSOCKET_H

#include <Pt/Http/Api.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <string>
#include <iostream>
#include <cstddef>

namespace Pt {

namespace Http {

class Client;
class WebSocketConnection;


/** @brief Client handshake and framed messages on an upgraded stream.

    %WebSocket is the client side of a WebSocket upgrade. Construct it
    with the %Client that performs the handshake. The socket stores a
    reference and does not own that client, so the client must outlive
    the socket, including a handshake that is still waiting on the
    loop. Host, port, event loop, timeout and TLS are settings of that
    client.

    Connect %connected() and call %beginConnect() with the request
    path, or with a %ws:// URL whose host and port are the client's
    endpoint. The handshake is an HTTP request and reply on that
    client. %endConnect() completes it and throws if it failed. A
    finished 101 becomes the stream this socket formats. After the
    handshake the socket no longer uses the client for frames. The
    client still owns the connection and the stream.

    The payload is %body(), an iostream, the same surface a %Message
    uses for its body. Write that stream and send it as one frame. A
    receive parses one frame and leaves the payload in %body(). The
    connection stream buffer stays inside the socket. %WebSocket is
    not an I/O device, and it is not a %StreamSession that application
    code binds itself.

    On the server the application does not construct a %WebSocket.
    %WebSocketSession is the server facade. It formats the stream the
    HTTP server already owns and exposes the same frame operations
    without a client handshake.

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

    A client masks every frame it writes. A server does not. That
    mode is fixed when the upgraded stream opens, so application code
    does not choose a mask.

    Ping and pong are control frames. %sendPing() and %sendPong()
    write those frames through the connection stream buffer. Text and
    binary are the data payload. Unknown is the unset frame type.

    The example completes the handshake and then writes a text frame.

    @code
    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        socket.body() << "hello";
        socket.beginSend(Pt::Http::WebSocket::Text);
    }

    Pt::System::MainLoop loop;
    Pt::Net::Endpoint ep("localhost", 80);
    Pt::Http::Client client(loop, ep);
    Pt::Http::WebSocket socket(client);
    socket.connected() += Pt::slot(onConnected);
    socket.beginConnect("/ws");
    loop.run();
    @endcode

    %closed() is emitted while this socket is still alive. The stream
    has already cleared its session pointer. Peer close, an I/O
    error, a close frame and destruction of the stream all emit it.
    The owner deletes this socket. Closing the socket closes the
    stream. The socket does not own the stream or the connection.

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
            Pong,    ///< Pong frame
            Close    ///< Close frame
        };

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
        Frame frame() const;

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

        void finishHandshake(bool failed);

        void onRequestSent(Client& client);

        void onReply(Client& client);

        void onInputReady();

        void onOutputReady();

        void onClosed();

    private:
        Client* _client;
        WebSocketConnection* _connection;
        std::string _path;
        bool _error;
        bool _connecting;
        Pt::Signal<WebSocket&> _connected;
        Pt::Signal<WebSocket&> _inputReady;
        Pt::Signal<WebSocket&> _outputReady;
        Pt::Signal<WebSocket&> _closed;
};

} // namespace Http

} // namespace Pt

#endif
