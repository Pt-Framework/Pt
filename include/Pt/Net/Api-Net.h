/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_NET_API_NET_H
#define PT_NET_API_NET_H

/** @defgroup Pt-Net Networking

    @brief TCP and UDP sockets.

    This module is the portable socket layer for TCP and UDP on IPv4
    and IPv6, so a caller listens, connects, and transfers bytes through
    the same types on every supported platform. IPv4 and IPv6 are
    address content rather than separate socket classes, and unicast,
    broadcast, and multicast UDP use one datagram socket type.

    %Endpoint is the address value both protocols use: it names a host
    and a service port, it does not open a socket, and listen, bind,
    connect, and send destinations all take endpoints. Name resolution
    happens when a socket uses the endpoint, not when the value is
    constructed.

    Sockets take part in the System I/O model. A server that waits for
    TCP connections is a %Selectable, which reports pending peers and
    does not transfer bytes, while TCP and UDP sockets that transfer
    bytes are %IODevice types and use the same operations as files and
    pipes: blocking %read() and %write(), or %beginRead() and
    %beginWrite() with an %EventLoop. The I/O device and event-loop
    chapters document those operations; this module adds the network
    types that use them.

    Synchronous and asynchronous work use the same socket object.
    Asynchronous work attaches with %setActive() so an %EventLoop can
    monitor the socket, but the loop does not own it: the code that
    creates the socket keeps it alive while it is attached, and
    destroys or closes it only when no operation is still waiting on
    the loop.

    When %listen() or %bind() finds the local address already occupied,
    the operation throws %AddressInUse, and when a remote host cannot
    be reached, %connect() and related operations throw %AccessFailed.
    Option values configure listen, connect, bind, and send, but they
    do not open a socket and they do not own one.

    TCP is a connected byte stream between two endpoints, in which a
    %TcpServer listens and a %TcpSocket is the stream, created by
    accepting a pending connection or by connecting to a remote
    endpoint.

    UDP is datagram I/O on a single %UdpSocket, which sends and
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
