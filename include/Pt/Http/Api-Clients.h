/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_API_CLIENTS_H
#define PT_HTTP_API_CLIENTS_H

/** @addtogroup Pt-Http-Clients

    @brief Send requests and receive replies.

    %Client is the HTTP user agent. It holds one %Request and one
    %Reply, so the usual work is to fill the request, send it, and
    read the reply. The client opens a TCP connection to its host when
    a send needs one; there is no separate connect method.

    The host is an %Endpoint passed to a constructor or to %setHost().
    Asynchronous work needs an %EventLoop, passed to a constructor or
    to %setActive(), but the loop does not own the client. HTTPS is
    the same send and receive path after %setSecure() has been given a
    %Pt::Ssl::Context.

    The request is %request(). Set the URL, the method, query
    parameters and header fields before the send starts. The default
    method is GET. Write the request body with %request().body(). The
    reply is %reply() after a receive step has made it available.

    Asynchronous receive is %beginReceive() and %endReceive(), and
    %replyReceived() is emitted when a step has completed.
    %endReceive() returns %MessageProgress; if the reply is not
    finished, call %beginReceive() again. Asynchronous send is
    %beginSend() and %endSend(), with %requestSent() as the matching
    signal. A single %beginReceive() is enough when the request is
    already complete and the caller only waits for the reply. Send
    first when the request has a body, when the body is chunked, or
    when several requests are pipelined.

    A keep-alive header on the request asks for a persistent
    connection, which the server may still close. Pipelining needs
    that persistent connection, because several requests are sent
    before the matching replies are received. %close() ends the
    connection. Leave it open only while the next request will reuse
    it; a later send on a closed or timed-out connection opens a new
    one.

    %send() and %receive() are the blocking forms of the same
    exchange, and they complete the request or the reply on the
    calling thread.

    A finished reply with status 101 switches the connection to a
    %Stream. %upgrade() returns that stream. The client no longer
    sends requests on it. Retain the stream to keep it.

    When a reply is 401, %Authenticator complements the request from
    realm credentials so the client can send it again. Basic
    authentication is built in, and other %Authentication methods can
    be added.
*/

#endif
