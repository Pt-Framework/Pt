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
#include <Pt/Ssl/Certificate.h>
#include <Pt/NonCopyable.h>
#include <string>

namespace Pt {

namespace Ssl {

class ContextImpl;

//! @internal Library initialization.
static struct PT_SSL_API SSLInit 
{
    SSLInit();
    ~SSLInit();
} ssl_init;

class PT_SSL_API CertificateStore
{
    public:
        CertificateStore();

        ~CertificateStore();

        void loadPkcs12(std::istream& is, const char* passwd);

        void loadPkcs12(const char* data, size_t len, const char* passwd);

        const Certificate* findCertificate(const std::string& subject);

    private:
        class CertificateStoreImpl* _impl;
};

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

    public:
        //! @brief Construct with session id and protocol. 
        Context(Protocol protocol = DefaultProtocol);

        //! @brief Destructor.
        ~Context();

        //! @brief Assigns the certificates, keys, validation mode and protocol.
        void assign(const Context& ctx);

        //! @brief Returns the current protocol. 
        Protocol protocol() const;

        //! @brief Sets the current protocol. 
        void setProtocol(Protocol protocol);

        //! @brief Limits the number of certificates checked in the peer's certificate chain.
        void setVerifyDepth(int n);

        VerifyMode verification() const;

        //! @brief Sets the current validation mode.
        void setVerifyMode(VerifyMode mode);

        /** @brief Add a certificate to the  trusted CA certificates.
            
            Trusted CA certificates are needed to check, if the peer's 
            certificate is signed by a trusted Certificate Authority. 
            Add the certificates of all trusted CAs.
         */
        void addCACertificate(const Certificate& trustedCert);

        /** @brief Set the main certificate of this context.

            Setting a main certificate is mandatory for a server context. For
            a client context, it is only needed for client authentication.
         */
        void setCertificate(const Certificate& cert);

        /** @brief Builds certificate chain.

            Adds the certificate to the certificate chain presented to the
            peer together with the main certificate.
        */
        void addCertificate(const Certificate& cert);

        ContextImpl* impl();

        const ContextImpl* impl() const;

    private:
        ContextImpl* _impl;
};

} // namespace Ssl

} // namespace Pt

#endif
