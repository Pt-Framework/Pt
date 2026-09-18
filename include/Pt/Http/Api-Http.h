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

#ifndef PT_HTTP_API_HTTP_H
#define PT_HTTP_API_HTTP_H

/** @defgroup Pt-Http HTTP Clients and Servers

    @brief HTTP messages, clients and servers.

    This module is the portable HTTP layer, so a caller sends and
    receives HTTP messages through the same types on every supported
    platform. The unit of work is a message, which is a header and an
    optional body. A %Request is the message a client sends and a
    server receives, and a %Reply is the message a server sends and a
    client receives. Both are %Message types that share the
    header-and-body model and differ in the start line.

    A %Client is an HTTP user agent that holds one request and one
    reply. There is no separate connect step, because the client opens
    a TCP connection to its %Endpoint when a send needs one. A
    persistent connection is a keep-alive header that the server also
    permits, and pipelining reuses that connection. After a 401 reply,
    %Authenticator prepares the request so it can be sent again.

    A %Server listens on an %Endpoint and does not implement resources
    itself. A %Servlet maps an incoming request to a %Service and may
    attach an %Authorizer. The service is a factory for %Responder
    objects, and each responder handles one request and writes one
    reply.

    HTTP I/O takes part in the System event-loop model. Asynchronous
    work attaches a client or a server with %setActive() so an
    %EventLoop can monitor the connection, but the loop does not own
    the HTTP object: the code that creates it keeps it alive while an
    operation is still waiting on the loop. The same objects also have
    blocking send and receive operations, though asynchronous
    operation is the usual path.

    Endpoints and TCP belong to %Pt::Net. This module names a host and
    a port with %Endpoint and listens or connects through that address.
    HTTPS is the same client and server API after %setSecure() has been
    given a %Pt::Ssl::Context; certificate and handshake details live
    in the SSL module.

    A protocol error that is specific to HTTP is an %HttpError, which
    is an I/O error. A local address that is already occupied still
    throws %AddressInUse from the listen that uses it.

    An HTTP connection can be upgraded. %WebSocket is the framed
    %IODevice that follows a WebSocket handshake, which the client
    opens and the server accepts through a %WebSocketService and an
    %IOStream taken from the upgraded connection.

    The rest of this chapter is the message model, then the client,
    then the server, then the WebSocket upgrade.
*/

/** @defgroup Pt-Http-Messages HTTP Messages

    @ingroup Pt-Http
*/

/** @defgroup Pt-Http-Clients HTTP Clients

    @ingroup Pt-Http
*/

/** @defgroup Pt-Http-Servers HTTP Servers

    @ingroup Pt-Http
*/

/** @defgroup Pt-Http-WebSocket WebSocket

    @ingroup Pt-Http
*/

#endif
