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

    %open() and %close() are protected because the derived type owns the
    handshake and the shutdown frame. %shutdown() writes that frame.
    %close() releases the stream and does not write it. A public close
    on this base would skip the frame.

    The destructor unbinds and closes the stream. Destroying the stream
    unbinds this channel and calls %onClose(). The channel object stays, and
    %stream() is null. The two sides call each other through the peer pointer.
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

    protected:
        /** @brief Creates a channel with no stream.
        */
        Channel();

        /** @brief Creates a channel bound to @a stream.

            @throw %std::logic_error if @a stream already has a channel.
        */
        explicit Channel(Stream& stream);

        /** @brief Unbinds and closes the stream.
        */
        ~Channel();

        /** @brief Binds @a stream.

            @throw %std::logic_error if this channel is already open,
            or if @a stream already has a channel.
        */
        void open(Stream& stream);

        /** @brief Unbinds and closes the stream.

            Does nothing when no stream is bound. The stream clears
            its channel pointer before it ends, so a close that runs
            from this destructor does not re-enter the stream.
        */
        void close();

        /** @brief Returns the bound stream, or null.
        */
        Stream* stream() const
        { return _stream; }

    protected:
        void closeStream(Stream& stream);

        /** @brief Called when the stream ends.

            The stream pointer is already null.
        */
        virtual void onCloseStream(Stream& stream);

    private:
        Stream* _stream;
};

} // namespace Http

} // namespace Pt

#endif // PT_HTTP_CHANNEL_H