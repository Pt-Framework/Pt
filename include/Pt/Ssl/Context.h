/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 * Copyright (C) 2010-2013 by Marc Duerner
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

#ifndef PT_SSL_CONTEXT_H
#define PT_SSL_CONTEXT_H

#include <Pt/Ssl/Api.h>
#include <Pt/NonCopyable.h>
#include <string>

namespace Pt {

namespace Ssl {

class Certificate;
class ContextImpl;

//! @internal Library initialization.
static struct PT_SSL_API SSLInit 
{
    SSLInit();
    ~SSLInit();
} ssl_init;

/** @brief Open mode for an SSL stream.

    @ingroup Pt-Ssl-Streams
*/
enum OpenMode
{
    Connect = 1, //!< Client handshake
    Accept = 2   //!< Server handshake
};

/** @brief SSL/TLS protocol version.

    @ingroup Pt-Ssl-Context
*/
enum Protocol 
{
    SSLv2     = 0,   //!< SSL version 2
    SSLv3or2  = 1,   //!< Highest available protocol version
    SSLv3     = 2,   //!< SSL version 3
    TLS = SSLv3or2,  //!< Highest available TLS protocol version
    TLSv1     = 3,   //!< TLS version 1.0
    TLSv1_1   = 4,   //!< TLS version 1.1
    TLSv1_2   = 5    //!< TLS version 1.2
};

/** @brief How the peer certificate is verified.

    @ingroup Pt-Ssl-Context
*/
enum VerifyMode
{
    NoVerify = 0,    //!< Do not verify the peer
    TryVerify = 1,   //!< Verify a certificate when the peer presents one
    AlwaysVerify = 2 //!< Require a peer certificate
};

/** @brief Shared configuration for SSL connections.

    %Context is the configuration the group described: protocol,
    verification, identity, chain, and trusted CAs, used by every
    stream opened with it. It is not a connected stream. Keep the
    context alive while a stream that opened with it is still open.

    The default constructor selects %TLS, the highest available
    protocol, and %TryVerify. The protocol constructor selects that
    protocol and the same default verify mode. %setProtocol() and
    %setVerifyMode() change them later. %setVerifyDepth() limits
    how many certificates in the peer's chain are checked.

    %setIdentity() is required on a server context and optional on
    a client context. %addCertificate() extends the chain presented
    with that identity. %addCACertificate() extends the trust store
    used to verify the peer. Each of those calls copies what it
    needs from the %Certificate argument, which must be valid for
    the call. Unusable material throws %InvalidCertificate.

    The object is not copyable. %assign() copies protocol, verify
    settings, and certificates from another context into this one.

    @ingroup Pt-Ssl-Context
*/
class PT_SSL_API Context : public NonCopyable
{
    public:
        /** @brief Creates a context for the highest available TLS protocol.
        */
        Context();

        /** @brief Creates a context for @a protocol.
        */
        Context(Protocol protocol);

        /** @brief Destructor.
        */
        ~Context();

        /** @brief Copies protocol, verify settings and certificates from @a ctx.
        */
        void assign(const Context& ctx);

        /** @brief Returns the protocol.
        */
        Protocol protocol() const;

        /** @brief Sets the protocol.
        */
        void setProtocol(Protocol protocol);

        /** @brief Limits how many certificates are checked in the peer chain.
        */
        void setVerifyDepth(int n);

        /** @brief Returns the verification mode.
        */
        VerifyMode verifyMode() const;

        /** @brief Sets the verification mode.
        */
        void setVerifyMode(VerifyMode mode);

        /** @brief Adds @a trustedCert to the trusted CA certificates.

            @throw %InvalidCertificate if @a trustedCert cannot be used.
        */
        void addCACertificate(const Certificate& trustedCert);

        /** @brief Sets the certificate presented to the peer.

            Required for a server context. For a client context, needed
            only for client authentication.

            @throw %InvalidCertificate if @a cert cannot be used as identity.
        */
        void setIdentity(const Certificate& cert);

        /** @brief Adds @a cert to the chain presented with the identity.

            @throw %InvalidCertificate if @a cert cannot be used.
        */
        void addCertificate(const Certificate& cert);

        //! @internal
        ContextImpl* impl();

        //! @internal
        const ContextImpl* impl() const;

    private:
        ContextImpl* _impl;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_CONTEXT_H
