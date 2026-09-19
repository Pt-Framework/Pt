/* Copyright (C) 2013 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
