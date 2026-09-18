/*
 * Copyright (C) 2013 by Marc Boris Duerner
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

#ifndef PT_SSL_API_SSL_H
#define PT_SSL_API_SSL_H

/** @defgroup Pt-Ssl Secure Communication

    @brief SSL/TLS streams, certificates and contexts.

    This module is the portable SSL/TLS layer, so a caller encrypts
    and decrypts bytes through the same types on every supported
    platform. It is not a socket API. The transport is an iostream
    the caller already has: a TCP connection, a string stream in a
    test, or any other stream that moves bytes to the peer. The SSL
    types wrap that stream, encrypt what is written to them, and
    decrypt what is read from them.

    Work proceeds in three steps. Certificates and private keys are
    loaded into a %CertificateStore. A %Context takes certificates
    from that store and holds the protocol, the identity presented
    to the peer, the trusted CAs, and the verification mode. An
    %IOStream or a %StreamBuffer then opens on a context and an
    underlying iostream, as a client (%Connect) or as a server
    (%Accept). One context is reused for many connections.

    The handshake, the encrypted I/O, and the shutdown are operations
    on the stream, not on the context. They can run as blocking
    iostream operations or as non-blocking steps that consume only
    the bytes already available in the underlying buffer, so the
    same types work with a %Pt::System::IOStream that fills
    asynchronously.

    HTTPS in the HTTP module is this layer behind %setSecure(): the
    client or server is given a %Context, and send and receive stay
    the HTTP API. Certificate loading, protocol, verification, and
    the handshake itself are documented here.

    A failure that is specific to SSL is an %SslError, which is an
    I/O error. A handshake that cannot complete is a
    %HandshakeFailed, and certificate data that cannot be used is an
    %InvalidCertificate.

    The rest of this chapter is the certificate store, then the
    context, then the secure stream.
*/

/** @defgroup Pt-Ssl-Certificates Certificates

    @ingroup Pt-Ssl
*/

/** @defgroup Pt-Ssl-Context SSL Context

    @ingroup Pt-Ssl
*/

/** @defgroup Pt-Ssl-Streams Secure Streams

    @ingroup Pt-Ssl
*/

#endif
