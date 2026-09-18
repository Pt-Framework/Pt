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
