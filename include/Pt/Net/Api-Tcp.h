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

#ifndef PT_NET_API_TCP_H
#define PT_NET_API_TCP_H

/** @addtogroup Pt-Net-Tcp

    @brief Listen, accept and connect TCP streams.

    TCP is a connected byte stream between two endpoints. Two types
    implement that model. %TcpServer listens on a local endpoint and
    reports pending peers. %TcpSocket is the stream that transfers
    bytes. The same socket type is the client side of a %connect() and
    the accepted side of a pending server connection.

    A server %listen() binds the local endpoint and waits for peers.
    %TcpSocket::accept() takes one pending connection and makes that
    socket the accepted stream. %TcpSocket::connect() reaches a remote
    endpoint and makes that socket the client stream. After the stream
    is up, %read() and %write() are the inherited I/O-device operations,
    blocking or asynchronous.

    %TcpServer is a %Selectable, not an %IODevice. It does not read or
    write. %TcpSocket is an %IODevice. Listen and connection settings
    live in %TcpServerOptions and %TcpSocketOptions. Those values
    configure an operation; they do not open a socket.

    The example is the two-type model on one thread. %listen() queues
    incoming connections. %connect() completes against that queue.
    %accept() takes the pending stream. The bytes then move through the
    inherited device operations.

    @code
    Pt::Net::TcpServer server;
    server.listen(Pt::Net::Endpoint::ip4Any(9000));

    Pt::Net::TcpSocket client;
    client.connect(Pt::Net::Endpoint::ip4Loopback(9000));

    Pt::Net::TcpSocket peer;
    peer.accept(server);

    peer.write("Hello", 5);
    char buf[5];
    client.read(buf, 5);
    @endcode
*/

#endif
