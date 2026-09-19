/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_NET_API_UDP_H
#define PT_NET_API_UDP_H

/** @addtogroup Pt-Net-Udp

    @brief Bind, send and receive UDP datagrams.

    UDP is datagram I/O on a single %UdpSocket type, which sends and
    receives unicast, broadcast, and multicast datagrams. It is an
    %IODevice: after a local bind or a send destination is set, %read()
    and %write() are the inherited device operations, blocking or
    asynchronous, except that each %write() sends one datagram and each
    %read() receives one datagram rather than a TCP-style byte stream.

    Three operations set where datagrams go and where they come from.
    %bind() sets the local endpoint that receives datagrams, %connect()
    associates a remote peer so writes go to that peer and reads come
    from it, and %setTarget() sets a send destination without
    associating the socket, which is the path broadcast and multicast
    sends use. A socket may bind and still set a target, which is how a
    receiver that also sends to a group is set up.

    A socket joins a multicast group to receive that group's datagrams,
    which means bind first, then join. %UdpSocketOptions enable
    broadcast and set the hop limit; those values configure an
    operation, but they do not open a socket.
*/

#endif
