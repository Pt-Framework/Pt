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

namespace Http {

class Connection;
class StreamSession;

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
    %Service::upgradeRequested(). The client reports it through
    %Client::upgrade(). Both return the stream the owner already holds.

    A %StreamSession is the external peer. The stream stores one session
    pointer, and the session stores one stream pointer. Binding that
    peer accepts the upgrade. A second bind throws %std::logic_error.
    On the server, a stream that still has no session after
    %upgradeRequested() returns is declined, and the server closes it.
    %WebSocket is one session type that formats frames into %buffer().

    %close() ends this stream. It clears the session pointer first
    and then tells the session that the stream ended, so a close
    that runs from the session destructor does not re-enter this
    stream. While this stream is the only stream of the connection,
    closing it also closes the connection. The server deletes that
    connection on its event loop, after the close that requested it
    has returned. Destroying the stream tells the session the same
    way. The session object stays, and its stream pointer is null.

    %buffer() is the stream buffer of the connection. A session
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

    The example accepts the upgrade by constructing a %WebSocket on
    the stream. A slot that does not bind a session declines it.

    @code
    void onUpgrade(Pt::Http::Stream& stream)
    {
        _socket.accept(stream);
        _socket.inputReady() += Pt::slot(onInput);
        _socket.beginReceive();
    }
    @endcode

    @ingroup Pt-Http-Servers
*/
class PT_HTTP_API Stream : public Connectable
                         , private NonCopyable
{
    friend class Connection;
    friend class StreamSession;

    public:
        /** @brief Destructor.

            Unbinds the session and notifies it. Does not close the
            connection. The connection deletes this stream.
        */
        ~Stream();

        /** @brief Returns true when the connection still owns this stream.
        */
        bool isValid() const
        { return _connection != 0; }

        /** @brief Returns the bound stream session, or null.
        */
        StreamSession* session() const
        { return _session; }

        /** @brief Returns the Upgrade header value.
        */
        const std::string& protocol() const
        { return _protocolName; }

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

        void openSession(StreamSession& session);

        void closeSession(StreamSession& session);

    private:
        Connection* _connection;
        StreamSession* _session;
        std::string _protocolName;
        Signal<> _inputReady;
        Signal<> _outputReady;
};

} // namespace Http

} // namespace Pt

#endif // PT_HTTP_STREAM_H
