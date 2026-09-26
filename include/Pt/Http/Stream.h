/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_STREAM_H
#define PT_HTTP_STREAM_H

#include <Pt/Http/Api.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/System/IOBuffer.h>
#include <string>
#include <cstddef>
#include <iosfwd>

namespace Pt {

namespace Http {

class Connection;
class ServerImpl;

struct StreamState
{
    Connection* connection;
    std::string protocol;
    bool retained;
    std::size_t handles;

    StreamState(Connection* conn, const std::string& proto)
    : connection(conn)
    , protocol(proto)
    , retained(false)
    , handles(0)
    { }
};

/** @brief One HTTP stream on a server-owned connection.

    %Stream is one bidirectional channel of a connection the server
    still owns. A finished reply with status 101 does not hand the TCP
    connection to the service. The server keeps that connection and
    emits %Service::upgradeRequested() with a %Stream on the server
    thread. HTTP/1 has one stream, and it fills the connection. A
    later multiplexed protocol can issue more than one stream for the
    same connection. WebSocket is one protocol that formats frames
    into the stream buffer.

    The signal argument is the server's stream. A slot that needs the
    handle after the signal returns copies it. Copying shares the
    connection and does not retain it, and destroying a copy does not
    close the stream. Lifetime follows %retain() and %close().

    %retain() tells the server to keep the connection. The server
    deletes a connection whose stream was not retained, which is how
    an upgrade is declined. %close() ends this stream. While it is the
    only stream of the connection, that also closes and deletes the
    connection. A later protocol that multiplexes streams closes the
    connection when its last stream ends.

    %buffer() is the stream buffer of the connection. A protocol
    formats into that buffer and extracts from it. The stream does not
    take a caller buffer, and it does not expose the socket.
    %beginInput() and %beginOutput() start a transfer of that buffer.
    %inputReady() and %outputReady() report that a transfer finished,
    and %endInput() and %endOutput() complete it. One read and one
    write may run at a time. Copies share that transfer. The copy that
    started it receives the ready signal.

    %protocol() is the value of the request's Upgrade header.
    %setTimeout() replaces the HTTP read and write timeout for this
    stream. The server has already stopped that timeout when it
    opened the stream.

    The example retains the stream and starts a read. Without
    %retain() the server closes the connection when the signal
    returns.

    @code
    void onUpgrade(Pt::Http::Stream& stream)
    {
        stream.retain();
        stream.inputReady() += Pt::slot(onInput);
        stream.beginInput();
    }
    @endcode

    @ingroup Pt-Http-Servers
*/
class PT_HTTP_API Stream : public Connectable
{
    friend class Connection;

    public:
        /** @brief Creates an empty stream.
        */
        Stream();

        /** @brief Copies @a other without retaining the connection.
        */
        Stream(const Stream& other);

        /** @brief Destructor.

            Does not close the stream.
        */
        ~Stream();

        /** @brief Shares the connection of @a other.

            Does not retain the connection and does not close the
            previous stream.
        */
        Stream& operator=(const Stream& other);

        /** @brief Returns true when this handle has a connection.
        */
        bool isValid() const
        { return _state && _state->connection; }

        /** @brief Returns the Upgrade header value.
        */
        const std::string& protocol() const;

        /** @brief Keeps the connection after the upgrade signal returns.
        */
        void retain();

        /** @brief Returns true when %retain() was called.
        */
        bool isRetained() const;

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

        /** @brief Cancels a pending transfer.
        */
        void cancel();

        /** @brief Sets the stream timeout in milliseconds.
        */
        void setTimeout(std::size_t ms);

        /** @brief Returns the signal emitted when input is ready.
        */
        Signal<>& inputReady();

        /** @brief Returns the signal emitted when output is ready.
        */
        Signal<>& outputReady();

        /** @brief Returns the signal emitted when the stream is closed.
        */
        Signal<>& closed();

    private:
        explicit Stream(StreamState* state);

    private:
        void onInput(System::IOBuffer&);

        void onOutput(System::IOBuffer&);

        void attach();

        void detach();

        void release();

    private:
        StreamState* _state;
        Signal<> _inputReady;
        Signal<> _outputReady;
        Signal<> _closed;
};

} // namespace Http

} // namespace Pt

#endif // PT_HTTP_STREAM_H
