/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_CHANNEL_H
#define PT_HTTP_CHANNEL_H

#include <Pt/Http/Api.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Http {

class Stream;

/** @brief Binds one channel to one HTTP stream.

    %Channel is the external peer of one %Stream. The server or the
    client owns the connection, and the connection owns the stream. A channel
    does not own either. It stores a pointer to the stream, and the stream
    stores a pointer back. One stream has one channel. A second %open() throws
    %std::logic_error.

    Binding is how an upgrade is accepted. On the server,
    %Service::onUpgrade() receives the stream the server already owns,
    with the request and the reply of the finished 101. A
    derived channel constructs itself with that stream, or calls %open() from
    its own accept method. After %onUpgrade() returns, a stream with no channel
    is declined and the server closes it. On the client, %Client::upgrade()
    returns the stream the client already owns, and the derived channel opens
    that stream after the 101 reply.

    This base does not transfer bytes. The derived type uses the stream it
    holds: %Stream::buffer(), %beginInput(), %beginOutput(), and the ready
    signals of that stream. A framed protocol formats into the stream
    buffer and connects its own slots to the stream. Application code
    does not derive this base for WebSocket. The client uses %WebSocket,
    and the server uses %WebSocketSession.

    %open() is protected because the derived type owns the handshake.
    %shutdown() on that type writes the shutdown frame. %close() is
    public. It closes the bound stream and does not write a frame.
    The connection deletes that stream. The stream destructor
    releases this channel through %detachStream(). That call runs
    %onClose(), drops the stream pointer, and calls %onRelease().
    It does not close the stream. Neither call emits a protocol
    signal.

    The destructor calls %close(). Destroying the stream calls
    %detachStream(). The channel object stays, and %stream() is null.
    %isOpen() is false after either side ends.

    @ingroup Pt-Http-Servers
    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API Channel : private NonCopyable
{
    friend class Stream;

    public:
        /** @brief Returns true when a stream is bound.
        */
        bool isOpen() const
        { return _stream != 0; }

        /** @brief Closes the bound stream.

            Does nothing when no stream is bound. Does not write a
            protocol frame. The stream ends the bind.
        */
        void close();

    protected:
        /** @brief Creates a channel with no stream.
        */
        Channel();

        /** @brief Creates a channel bound to @a stream.

            @throw %std::logic_error if @a stream already has a channel.
        */
        explicit Channel(Stream& stream);

        /** @brief Releases the bound stream.
        */
        ~Channel();

        /** @brief Binds @a stream.

            @throw %std::logic_error if this channel is already open,
            or if @a stream already has a channel.
        */
        void open(Stream& stream);

        /** @brief Returns the bound stream, or null.
        */
        Stream* stream() const
        { return _stream; }

        /** @brief Cleans up before the stream pointer is dropped.

            Called by %detachStream() while this channel still holds
            the stream pointer. Not called when no stream is bound.
            The stream is still attached to its connection.
        */
        virtual void onClose();

        /** @brief Notifies that the stream has ended the bind.

            Called by %detachStream() after the stream pointer has
            been dropped. Not called when no stream was bound. Does
            not close the stream.
        */
        virtual void onRelease();

    private:
        /** @brief Drops the stream after the stream detaches.

            Does nothing when no stream is bound. Runs %onClose(),
            drops the stream pointer, and runs %onRelease(). Does
            not close the stream.
        */
        void detachStream(Stream& stream);

    private:
        Stream* _stream;
};

} // namespace Http

} // namespace Pt

#endif // PT_HTTP_CHANNEL_H