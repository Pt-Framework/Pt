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
#include <cstddef>

namespace Pt {

namespace Http {

class Client;
class WebSocketConnection;


/** @brief Client handshake and messages on an upgraded stream.

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
    handshake the socket no longer uses the client for messages. The
    client still owns the connection and the stream.

    The socket owns two messages, %incoming() and %outgoing(), the way
    a %Client owns a request and a reply. A send writes %outgoing().
    Set the type, write the body, and call %beginSend().
    %outputReady() reports that data bytes were sent. %endSend()
    returns %MessageProgress. If the send is not finished,
    %beginSend() continues the same message. A receive fills
    %incoming(). %inputReady() reports that data bytes were received.
    %endReceive() returns progress. Body bytes are readable from
    %incoming().body(). After %finished(), %clear() drops the body
    so the same object can carry the next message.

    Ping, pong, and close are socket operations, not messages. %ping()
    enqueues a ping. A received ping is answered by the engine. A
    received pong is consumed. Neither is delivered through
    %incoming(). %close() enqueues a close frame with a status code
    and a reason. A received close is answered by the engine.
    %closeCode() and %closeReason() report the handshake that ended
    the stream. %closed() ends an outstanding send or receive.

    On the server the application does not construct a %WebSocket.
    %WebSocketSession is the server facade. It exposes the same
    message operations without a client handshake.

    A client masks every frame it writes. A server does not. That
    mode is fixed when the upgraded stream opens.

    The example completes the handshake and then writes a text
    message.

    @code
    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
        socket.outgoing().body() << "hello";
        socket.beginSend();
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

        /** @brief Returns the incoming message.
        */
        WebSocketMessage& incoming();

        /** @brief Returns the outgoing message.
        */
        WebSocketMessage& outgoing();

        /** @brief Begins sending %outgoing().

            @throw %std::logic_error if a send is outstanding, the
            handshake is not finished, or a close is queued.
            @throw %std::invalid_argument if the type is %Unknown, the
            body is larger than %setMaxMessageSize(), or a text body
            is not valid UTF-8.
        */
        void beginSend();

        /** @brief Completes the send started by %beginSend().

            @return Progress of this send step.
        */
        MessageProgress endSend();

        /** @brief Begins receiving into %incoming().

            @throw %std::logic_error if a receive is outstanding, the
            handshake is not finished, or the stream has ended.
        */
        void beginReceive();

        /** @brief Completes the receive started by %beginReceive().

            @return Progress of this receive step.
        */
        MessageProgress endReceive();

        /** @brief Enqueues a ping with an optional payload of at most 125 bytes.

            @throw %std::invalid_argument if @a n is greater than 125.
            @throw %std::logic_error if the handshake is not finished.
        */
        void ping(const char* payload = 0, std::size_t n = 0);

        /** @brief Enqueues a close frame and ends the stream after the close handshake.

            @throw %std::invalid_argument if @a code is 1005, 1006 or
            1015, if @a reason is longer than 123 bytes, or if @a reason
            is not valid UTF-8.
            @throw %std::logic_error if the handshake is not finished
            or a close is already queued.
        */
        void close(unsigned code = 1000,
                   const std::string& reason = std::string());

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

        /** @brief Returns the signal emitted when the stream ends.

            Emitted while this socket is still alive. The stream has
            already cleared its session pointer. Peer close, an I/O
            error, a close frame and destruction of the stream all
            emit it. The owner deletes this socket. Ends an outstanding
            send or receive.
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

        /** @brief Closes the stream after @a ms without a finished transfer.

            Zero disables the idle timeout. A finished send or receive
            restarts it. A received ping or pong restarts it.
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
