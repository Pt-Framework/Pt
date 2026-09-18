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
