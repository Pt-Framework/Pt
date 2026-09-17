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

    This module is the portable socket layer for TCP and UDP.

    Sockets take part in the System I/O model. A server that waits for
    TCP connections is a %Selectable.

    TCP and UDP sockets that transfer bytes are %IODevice types. They
    use the same read and write operations as files and pipes.

    Asynchronous work attaches with setActive so an %EventLoop can
    monitor the socket. The loop does not own the socket. The code that
    creates it keeps it alive while it is attached.

    %Endpoint is the address value both protocols use. It names a host
    and a service port for IPv4 and IPv6.

    When listen or bind finds the local address already occupied, the
    operation throws %AddressInUse.

    TCP is a connected byte stream between two endpoints.

    UDP is datagram I/O on a single socket type.
*/

/** @defgroup Pt-Net-Tcp TCP Sockets

    @ingroup Pt-Net
*/

/** @defgroup Pt-Net-Udp UDP Sockets

    @ingroup Pt-Net
*/

#endif
