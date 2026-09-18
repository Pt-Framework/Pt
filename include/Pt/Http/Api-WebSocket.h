/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_HTTP_API_WEBSOCKET_H
#define PT_HTTP_API_WEBSOCKET_H

/** @addtogroup Pt-Http-WebSocket

    @brief Upgrade an HTTP connection to a WebSocket.

    WebSocket is an HTTP upgrade. The HTTP exchange performs a
    handshake, and after the handshake the same TCP connection carries
    framed messages instead of request and reply messages. %WebSocket
    is the %IODevice for that framed stream: %read() and %write()
    transfer payload bytes, while the frame type is separate.
    %setSendFrame() selects the opcode for the next send, and
    %receiveFrame() reports the opcode of the frame that was last
    received.

    On the client, %beginConnect() sends the handshake to a %ws://
    URL. %connected() is emitted when the attempt finishes, and
    %endConnect() completes it and throws if the handshake failed. The
    socket must be attached to an event loop before the connect
    starts.

    On the server, %WebSocketService is an HTTP %Service, mapped with
    a servlet like any other service. Its responder answers a
    WebSocket Upgrade request with 101 Switching Protocols, or with
    404 when the request is not a WebSocket upgrade. After a
    successful upgrade the connection is an %IOStream, and
    %WebSocket::accept() takes that stream and becomes the framed
    device.

    %IOStream is the upgraded connection, not the HTTP message body.
    %Service::upgradeRequested() reports that stream and the Upgrade
    header value to a service that handles upgrades itself.

    Ping and pong are control frames. %sendPingFrame() writes a ping,
    and after a ping is received %sendPongFrame() writes the matching
    pong. Text and binary frames are the data payload. Unknown is the
    unset frame type.

    The example is a client handshake. The slot completes the connect
    and then writes through the inherited device operation.

    @code
    void onConnected(Pt::Http::WebSocket& socket)
    {
        socket.endConnect();
        socket.setSendFrame(Pt::Http::WebSocket::Text);
        socket.beginWrite("hello", 5);
    }

    Pt::System::MainLoop loop;
    Pt::Http::WebSocket socket;
    socket.setActive(loop);
    socket.connected() += Pt::slot(onConnected);
    socket.beginConnect("ws://localhost/ws");
    loop.run();
    @endcode
*/

#endif
