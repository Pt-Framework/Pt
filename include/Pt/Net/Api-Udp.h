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

    A UDP socket sends and receives datagrams. The same type covers
    unicast, broadcast and multicast.

    Bind sets the local endpoint that receives datagrams.

    Connect associates a remote peer. Writes go to that peer. Reads
    come from it.

    A send destination can be set without associating the socket.
    Broadcast and multicast sends use that path.

    A socket joins a multicast group to receive that group's datagrams.
    Bind first.

    Option values enable broadcast and set the hop limit.
*/

#endif
