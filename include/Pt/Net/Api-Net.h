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

#ifndef PT_NET_API_NET_H
#define PT_NET_API_NET_H

/** @defgroup Pt-Net Networking

    @brief TCP and UDP sockets.

    This module is the portable socket layer for TCP and UDP on IPv4
    and IPv6. A caller listens, connects, and transfers bytes through
    the same types on every supported platform. IPv4 and IPv6 are
    address content, not separate socket classes. Unicast, broadcast,
    and multicast UDP use one datagram socket type.

    %Endpoint is the address value both protocols use. It names a host
    and a service port. It does not open a socket. Listen, bind,
    connect, and send destinations all take endpoints. Name resolution
    happens when a socket uses the endpoint, not when the value is
    constructed.

    Sockets take part in the System I/O model. A server that waits for
    TCP connections is a %Selectable: it reports pending peers and does
    not transfer bytes. TCP and UDP sockets that transfer bytes are
    %IODevice types. They use the same operations as files and pipes:
    blocking %read() and %write(), or %beginRead() and %beginWrite()
    with an %EventLoop. The I/O device and event-loop chapters document
    those operations. This module adds the network types that use them.

    Synchronous and asynchronous work use the same socket object.
    Asynchronous work attaches with %setActive() so an %EventLoop can
    monitor the socket. The loop does not own the socket. The code that
    creates it keeps the socket alive while it is attached, and
    destroys or closes it only when no operation is still waiting on
    the loop.

    When %listen() or %bind() finds the local address already occupied,
    the operation throws %AddressInUse. When a remote host cannot be
    reached, %connect() and related operations throw %AccessFailed.
    Option values configure listen, connect, bind, and send. They do
    not open a socket and they do not own one.

    TCP is a connected byte stream between two endpoints. A %TcpServer
    listens. A %TcpSocket is the stream, created by accepting a pending
    connection or by connecting to a remote endpoint.

    UDP is datagram I/O on a single %UdpSocket. The same type sends and
    receives unicast, broadcast, and multicast datagrams.

    The rest of this chapter is the address value, then the TCP stream
    model, then the UDP datagram model.
*/

/** @defgroup Pt-Net-Tcp TCP Sockets

    @ingroup Pt-Net
*/

/** @defgroup Pt-Net-Udp UDP Sockets

    @ingroup Pt-Net
*/

#endif
