/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 * Copyright (C) 2010-2012 by Marc Duerner
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
#include <Pt/Ssl/PrivateKey.h>
#include <Pt/Ssl/CertificateList.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <cstddef>

namespace Pt {

namespace Ssl {

//! @internal Library initialization.
static struct PT_SSL_API SSLInit 
{
    SSLInit();
    ~SSLInit();
} ssl_init;

//! @brief Context for SSL connections.
class PT_SSL_API Context : public NonCopyable
{
    public:
        //! @brief Verification mode.
        enum VerifyMode
        {
            VerifyNone = 0,
            VerifyPeer = 1,
            VerifyPeerRequired = 2
        };

        //! @brief Communication protocol.
        enum Protocol 
        {
            DefaultProtocol,  //!< not less than SSL version 3.
            TLSv1,    //!< the latest standard for secure TCP communication.
            SSLv3,    //!< recommended for modern systems.
            SSLv3or2, //!< recommended for the most compatibility.
            SSLv2     //!< unsecure, not recommended.
        };

        static const unsigned long DefaultTimeout = static_cast<unsigned long>(-1);
        static const unsigned long DefaultCacheSize = static_cast<unsigned long>(-1);
        static const unsigned long UnlimitedCacheSize = 0;

    public:
        //! @brief Construct with session id and protocol. 
        Context(Protocol protocol = DefaultProtocol);

        //! @brief Destructor.
        ~Context();

        /** @brief Enables session caching on the server side

            The @cacheId is a application specific ID for the session cache,
            so that two application domains do not mix up their caches. The
            @timeout is in maximum lifetime of a session in seconds. The size
            of the session cache can be limited with @cacheSize.
        */
        void enableSessions(const char* cacheId, 
                            unsigned long timeout = DefaultTimeout, 
                            unsigned long cacheSize = DefaultCacheSize);

        //! @brief Returns the current protocol. 
        Protocol protocol() const;

        //! @brief Sets the current protocol. 
        void setProtocol(Protocol protocol);

        void setVerifyMode(VerifyMode mode);

        /** @brief Set the list of trusted CA certificates for this context.
            
            Setting the list of trusted CA certificates is needed if you 
            would like to check if the other peer's certificate is signed
            by a trusted Certificate Authority. In this case \a trustedCert
            must contain the certificates of all CAs that you trust.
         */
        void setCACertificates(const CertificateList& trustedCert);

        /** @brief Set the main certificate of this context.

            Setting a main certificate is mandatory for a server context. 
            Setting a main certificate for a client context is only needed 
            for certificate-based client authentication.
         */
        void setCertificate(const Certificate& cert);

        /** @brief Add certificate to chain of this context.

            Additionally to the main certificate of this context, more certificates
            of the intermediate CAs can be added to the certificate chain. Setting 
            a main certificate and certificate chain for a client context is only 
            needed for  certificate-based client authentication. In this case the
            main certificate must be the client certificate. The remaining certificates 
            are the certificates of the intermediate CAs.
        */
        void addCertificate(const Certificate& cert);

        /** @brief Sets main certificate and certificate chain.

            The first certificate in the given list of certificates will
            be used as the main certificate, the remaining certificates
            will be added to the intermediate CA certificate chain. For
            a server context the main certificate should be the server
            certificate.
        */
        void setCertificateChain(const CertificateList& certs);

        /** @brief Set the private key for this context.

            Setting a private key is mandatory for a server context. Setting
            a private key is for a client context is only needed for 
            certificate-based client authentication.
        */
        void setPrivateKey(const PrivateKey& privKey);
        
        //! @internal
        ssl_ctx_st* impl() const;

    private:
        ssl_ctx_st* _ctx;
        Protocol    _protocol;
        bool        _certChainExist;
        void*       _reserved0;
        std::size_t _reserved1;
};

} // namespace Ssl

} // namespace Pt

#endif
