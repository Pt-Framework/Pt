/* Copyright (C) 2013 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SSL_API_CONTEXT_H
#define PT_SSL_API_CONTEXT_H

/** @addtogroup Pt-Ssl-Context

    @brief Configure protocol, identity and peer verification.

    %Context is the shared configuration for SSL/TLS connections. It
    is not a connection and it does not wrap a stream. Many
    %IOStream and %StreamBuffer objects can open with the same
    context and then run their own handshakes.

    The context holds the protocol, the verification mode, how deep
    a peer certificate chain is checked, the identity presented to
    the peer, extra certificates that complete that identity's
    chain, and the trusted CA certificates used to verify the peer.
    A default-constructed context uses the highest available TLS
    protocol and %TryVerify. %setProtocol() selects an explicit
    version when the peer requires one. %TLS is the highest
    available protocol; %TLSv1, %TLSv1_1 and %TLSv1_2 pin a version.

    %setIdentity() sets the certificate presented to the peer. A
    server context must have an identity. A client context needs one
    only when the server asks for client authentication. Because a
    private key stays attached to its certificate in the store,
    setting the identity selects that key as well.
    %addCertificate() adds a certificate to the chain presented
    together with the identity. %addCACertificate() adds a trusted
    CA used to verify the peer; it does not change the identity.

    %setVerifyMode() chooses whether the peer must authenticate.
    %NoVerify does not check a peer certificate. %TryVerify checks
    a certificate when the peer presents one and continues if none
    is presented. %AlwaysVerify requires a peer certificate and
    fails the handshake without one. %setVerifyDepth() limits how
    many certificates in the peer chain are checked.

    The context copies the certificate data it needs when identity,
    chain, or CA certificates are set, so the store does not have to
    remain after those calls. The %Certificate argument of each call
    must still be valid for the call. Unusable certificate material
    throws %InvalidCertificate.

    %Context is not copyable. %assign() copies protocol, verify
    settings, and certificates into another context.

    The example prepares a server context from certificates already
    in a store. Opening a stream with that context is the next
    chapter.

    @code
    Pt::Ssl::Context ctx(Pt::Ssl::TLSv1_2);
    ctx.setVerifyMode(Pt::Ssl::AlwaysVerify);
    ctx.setIdentity(store.getCertificate("Example Server"));
    ctx.addCACertificate(store.getCertificate("Example CA"));
    @endcode
*/

#endif
