/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_STREAM_H
#define PT_HTTP_STREAM_H

#include <Pt/Http/Api.h>
#include <Pt/Connectable.h>
#include <Pt/NonCopyable.h>
#include <Pt/Signal.h>
#include <string>
#include <cstddef>
#include <iosfwd>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Http {

class Connection;
class Channel;

/** @brief One HTTP stream on a connection.

    %Stream is one bidirectional channel of a connection the server or
    the client still owns. A finished reply with status 101 does not
    hand the TCP connection to the service. The owner keeps that
    connection and keeps this stream. HTTP/1 has one stream, and it
    fills the connection. A later multiplexed protocol can issue more
    than one stream for the same connection.

    The connection creates the stream after the 101 reply has been
    written, and it deletes the stream. This type is not copyable and
    has no public constructor. The server reports it through
    %Service::onUpgrade(). The client reports it through
    %Client::upgrade(). Both return the stream the owner already holds.

    A %Channel is the external peer. The stream stores one channel
    pointer, and the channel stores one stream pointer. Binding that
    peer accepts the upgrade. A second bind throws %std::logic_error.
    On the server, a stream that still has no channel after
    %onUpgrade() returns is declined, and the server closes it.
    A WebSocket formats frames into %buffer(). The bind is internal
    to that protocol. Application code uses %WebSocket on the client
    and %WebSocketSession on the server.

    %close() ends this stream. It clears the channel pointer first
    and then tells the channel that the stream ended, so a close
    that runs from the channel destructor does not re-enter this
    stream. While this stream is the only stream of the connection,
    closing it also closes the connection. The server deletes that
    connection on its event loop, after the close that requested it
    has returned. Destroying the stream tells the channel the same
    way. The channel object stays, and its stream pointer is null.

    %buffer() is the stream buffer of the connection. A channel
    formats into that buffer and extracts from it. The stream does not
    take a caller buffer, and it does not expose the socket.
    %beginInput() and %beginOutput() start a transfer of that buffer.
    %inputReady() and %outputReady() report that a transfer finished,
    and %endInput() and %endOutput() complete it. One read and one
    write may run at a time. %cancel() stops the transfer of this
    stream.

    %protocol() is the value of the request's Upgrade header.
    %setTimeout() replaces the HTTP read and write timeout for this
    stream. The owner has already stopped that timeout when it opened
    the stream.

    The example accepts the upgrade by opening a channel on
    the stream. A service that does not bind a channel declines it.

    @code
    void onUpgrade(Pt::Http::Stream& stream,
                   const Pt::Http::Request&,
                   const Pt::Http::Reply&)
    {
        _channel.open(stream);
        stream.inputReady() += Pt::slot(onInput);
        stream.beginInput();
    }
    @endcode

    @ingroup Pt-Http-Servers
*/
class PT_HTTP_API Stream : public Connectable
                         , private NonCopyable
{
    friend class Connection;
    friend class Channel;

    public:
        /** @brief Destructor.

            Unbinds the channel and notifies it. Does not close the
            connection. The connection deletes this stream.
        */
        ~Stream();

        /** @brief Returns true when the connection still owns this stream.
        */
        bool isValid() const
        { return _connection != 0; }

        /** @brief Returns the bound channel, or null.
        */
        Channel* channel() const
        { return _channel; }

        /** @brief Returns the Upgrade header value.
        */
        const std::string& protocol() const
        { return _protocolName; }

        /** @internal Name selected on the opening reply.

            Set before the session constructor runs. Empty when the
            reply selected none.
        */
        const std::string& selectedProtocol() const
        { return _selectedProtocol; }

        /** @internal Stores the name selected on the opening reply.
        */
        void setSelectedProtocol(const std::string& name)
        { _selectedProtocol = name; }

        /** @brief Ends this stream.

            Closes the connection while this stream is its only stream.
        */
        void close();

        /** @brief Returns the stream buffer of the connection.
        */
        std::streambuf* buffer();

        /** @brief Begins a read into the stream buffer.
        */
        void beginInput();

        /** @brief Completes the read started by %beginInput().

            @return The number of bytes available in %buffer().
        */
        std::size_t endInput();

        /** @brief Begins a write of the bytes buffered in %buffer().
        */
        void beginOutput();

        /** @brief Completes the write started by %beginOutput().
        */
        std::size_t endOutput();

        /** @brief Cancels a pending transfer of this stream.
        */
        void cancel();

        /** @brief Sets the stream timeout in milliseconds.
        */
        void setTimeout(std::size_t ms);

        /** @brief Returns the event loop of the owning connection.

            Returns null after the stream has been closed.
        */
        System::EventLoop* loop() const;

        /** @brief Returns the signal emitted when input is ready.
        */
        Signal<>& inputReady()
        { return _inputReady; }

        /** @brief Returns the signal emitted when output is ready.
        */
        Signal<>& outputReady()
        { return _outputReady; }

    protected:
        Stream(Connection& connection, const std::string& protocol);

        void openChannel(Channel& channel);

        void closeChannel(Channel& channel);

    private:
        Connection* _connection;
        Channel* _channel;
        std::string _protocolName;
        std::string _selectedProtocol;
        Signal<> _inputReady;
        Signal<> _outputReady;
};

} // namespace Http

} // namespace Pt

#endif // PT_HTTP_STREAM_H
