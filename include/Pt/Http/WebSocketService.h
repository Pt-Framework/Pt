/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef PT_HTTP_WEBSOCKETSERVICE_H
#define PT_HTTP_WEBSOCKETSERVICE_H

#include <Pt/Http/Api.h>
#include <Pt/Http/WebSocketResponder.h>
#include <Pt/Http/WebSocket.h>
#include <Pt/Http/Service.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Http {

/** @brief HTTP service that owns accepted WebSocket connections.

    The responder answers a WebSocket upgrade with 101, or with 503
    when %maxSockets() accepted sockets are already open. After the
    101 the server calls %onUpgrade(). This service constructs the
    %WebSocket, binds the stream, and emits %accepted(). The slot
    receives the socket this service owns. It does not construct one
    and does not free it.

    %closed() erases the socket from this service. The destructor
    deletes any socket still tracked. The stream and the socket only
    clear each other's pointer.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketService : public Pt::Http::Service
                                   , public Pt::Connectable
{
    public:
        /** @brief Default constructor.
        */
        WebSocketService();
        
        /** @brief Destructor.

            Deletes every socket this service still owns.
        */
        ~WebSocketService();

        /** @brief Returns the signal emitted when a socket is accepted.

            Emitted on the server thread after the socket has bound
            the stream. The socket stays valid until its owner deletes
            it.
        */
        Signal<WebSocket&>& accepted();

        /** @brief Returns the number of accepted sockets not yet closed.
        */
        std::size_t size() const;

        /** @brief Returns the maximum number of accepted sockets.
        */
        std::size_t maxSockets() const;

        /** @brief Sets the maximum number of accepted sockets.

            A handshake above this limit is answered with 503. Zero
            means no limit.
        */
        void setMaxSockets(std::size_t n);

        /** @brief Returns the idle timeout in milliseconds.
        */
        std::size_t idleTimeout() const;

        /** @brief Sets the idle timeout in milliseconds.

            An accepted socket is closed after @a ms without a finished
            transfer. A finished send or receive restarts the timeout.
            Zero disables it.
        */
        void setIdleTimeout(std::size_t ms);

        /** @brief Returns the maximum frame payload in bytes.
        */
        std::size_t maxMessageSize() const;

        /** @brief Sets the maximum frame payload in bytes.

            A larger frame closes the socket. Zero disables the limit.
        */
        void setMaxMessageSize(std::size_t n);

    protected:
        virtual Responder* onGetResponder(const Request&);

        virtual void onReleaseResponder(Responder* r);

        virtual void onUpgrade(Stream& stream);

    private:
        void onClosed(WebSocket& socket);

        std::vector<WebSocket*> _sockets;
        Signal<WebSocket&> _accepted;
        std::size_t _maxSockets;
        std::size_t _idleTimeout;
        std::size_t _maxMessageSize;
};

}}

#endif