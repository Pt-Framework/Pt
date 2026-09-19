/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_NET_API_TCP_H
#define PT_NET_API_TCP_H

/** @addtogroup Pt-Net-Tcp

    @brief Listen, accept and connect TCP streams.

    TCP is a connected byte stream between two endpoints, implemented
    by two types: %TcpServer listens on a local endpoint and reports
    pending peers, and %TcpSocket is the stream that transfers bytes,
    whether it is the client side of a %connect() or the accepted side
    of a pending server connection.

    A server %listen() binds the local endpoint and waits for peers.
    %TcpSocket::accept() takes one pending connection and makes that
    socket the accepted stream, while %TcpSocket::connect() reaches a
    remote endpoint and makes that socket the client stream. After the
    stream is up, %read() and %write() are the inherited I/O-device
    operations, blocking or asynchronous.

    %TcpServer is a %Selectable, not an %IODevice, so it does not read
    or write, whereas %TcpSocket is an %IODevice. Listen and connection
    settings live in %TcpServerOptions and %TcpSocketOptions, which
    configure an operation but do not open a socket.

    The example is the two-type model on one thread: %listen() queues
    incoming connections, %connect() completes against that queue,
    %accept() takes the pending stream, and the bytes then move through
    the inherited device operations.

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
