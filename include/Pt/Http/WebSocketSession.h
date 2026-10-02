/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSESSION_H
#define PT_HTTP_WEBSOCKETSESSION_H

#include <Pt/Http/Api.h>
#include <Pt/Http/WebSocket.h>
#include <Pt/Connectable.h>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Http {

class WebSocketServer;
class WebSocketService;

/** @brief Application context of one accepted WebSocket stream.

    %WebSocketSession is the server-side peer of one upgraded stream.
    A %WebSocketServer asks the %WebSocketService to create it when a
    handshake has finished, holds it, and releases it when the stream
    ends or when the server is destroyed. Derive from it to keep the
    state that must survive from one frame to the next: a
    subscription, a cursor, a user, or a reference into the
    application domain.

    The session contains one %WebSocket. That socket is the frame
    device. This type is not a %StreamSession, because the socket
    already binds the stream, and a second bind would throw. It is not
    a %Responder. The handshake responder has already been released
    when the session begins.

    The server constructs the session with itself, with the loop
    that serializes this stream, and with the stream of this upgrade.
    The base constructor binds %socket() to that stream, copies the
    service limits onto the socket, and stores the loop. The derived
    constructor runs after that. Its members are initialized,
    %socket() is open, and %loop() is the loop of this stream. Start
    the first %WebSocket::beginReceive() or %WebSocket::beginSend()
    there.

    %onInput(), %onOutput(), and %onClose() are the later events. They
    take no arguments. The session, the socket, and the loop do not
    change between callbacks. %onInput() runs when one whole frame has
    been received. %onOutput() runs when a frame has left the stream
    buffer. %onClose() runs while this object is still alive, after
    the stream has ended. The %WebSocketServer releases the session
    after %onClose() returns. Destroying that server closes a session
    that is still open and then releases it the same way. The service
    is still alive, because the server is destroyed first.

    %service() reaches state shared by every connection of this
    endpoint. %loop() is where a timer or posted work must run.
    %socket() is the frame device. Closing the socket closes the
    stream. The session does not own the stream or the connection.

    The destructor closes the socket. A derived constructor that
    throws still runs this destructor, so a failed construction does
    not leave an accepted stream without an owner.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketSession : public Connectable
{
    friend class WebSocketServer;

    public:
        /** @brief Binds @a stream on @a loop.

            @a server owns this session. @a loop must be the loop of
            @a stream.

            @throw %std::logic_error if @a loop is not the loop of
            @a stream.
        */
        WebSocketSession(WebSocketServer& server,
                         System::EventLoop& loop,
                         Stream& stream);

        /** @brief Closes the socket.
        */
        ~WebSocketSession();

        /** @brief Returns the service that created this session.
        */
        WebSocketService& service()
        { return *_service; }

        /** @brief Returns the service that created this session.
        */
        const WebSocketService& service() const
        { return *_service; }

        /** @brief Returns the loop that serializes this stream.
        */
        System::EventLoop& loop()
        { return *_loop; }

        /** @brief Returns the loop that serializes this stream.
        */
        const System::EventLoop& loop() const
        { return *_loop; }

        /** @brief Returns the frame device of this stream.
        */
        WebSocket& socket()
        { return _socket; }

        /** @brief Returns the frame device of this stream.
        */
        const WebSocket& socket() const
        { return _socket; }

    protected:
        /** @brief Called when one frame has been received.
        */
        virtual void onInput() = 0;

        /** @brief Called when one frame has been sent.
        */
        virtual void onOutput() = 0;

        /** @brief Called when the stream has ended.

            This object is still alive. The service releases it after
            this call returns.
        */
        virtual void onClose() = 0;

    private:
        void onSocketInput(WebSocket& socket);

        void onSocketOutput(WebSocket& socket);

        void onSocketClosed(WebSocket& socket);

        WebSocketService* _service;
        WebSocketServer* _server;
        System::EventLoop* _loop;
        WebSocket _socket;
};

} // namespace Http

} // namespace Pt

#endif
