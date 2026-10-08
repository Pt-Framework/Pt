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

    Ping, pong and shutdown are socket operations, not messages.

    @code
    socket.ping();
    socket.shutdown(1000, "done");
    @endcode

    %ping() enqueues a ping. A received ping is answered by the
    engine. A received pong is consumed. Neither is delivered
    through %incoming(). %shutdown() enqueues a close frame with a
    status code and a reason. A received close is answered by the
    engine. %closeCode() and %closeReason() report the handshake
    that ended the stream. %closed() is emitted while this socket
    is still alive. The stream has already cleared its session
    pointer. Peer close, an I/O error, a close frame and
    destruction of the stream all emit it. It ends an outstanding
    send or receive. The owner deletes this socket.

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
        void shutdown(unsigned code = 1000,
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
            already cleared its channel pointer. Peer close, an I/O
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
        WebSocketChannel* _channel;
        std::string _path;
        std::string _protocol;
        std::vector<std::string> _protocols;
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
