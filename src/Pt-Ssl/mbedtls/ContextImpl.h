/*
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

#ifndef PT_SSL_CONTEXTIMPL_H
#define PT_SSL_CONTEXTIMPL_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/Context.h>
#include <Pt/Ssl/Certificate.h>
#include <Pt/NonCopyable.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>

namespace Pt {

namespace Ssl {

inline void SSLInitImpl()
{}

inline void SSLExitImpl()
{}

class ContextImpl
{
    public:
        ContextImpl(Protocol protocol);

        ~ContextImpl();

        void assign(const ContextImpl& ctx);

        Protocol protocol() const;

        void setProtocol(Protocol protocol);

        void setVerifyDepth(int n);

        VerifyMode verifyMode() const;

        void setVerifyMode(VerifyMode mode);

        void addCACertificate(const Certificate& trustedCert);

        void setIdentity(const Certificate& cert);

        void addCertificate(const Certificate& cert);

        //! @internal used by Connection to set up a client-role mbedtls SSL session
        mbedtls_ssl_config* clientConfig()
        { return &_clientConfig; }

        //! @internal used by Connection to set up a server-role mbedtls SSL session
        mbedtls_ssl_config* serverConfig()
        { return &_serverConfig; }

    private:
        void maybeRegisterOwnCert();

    private:
        Protocol                 _protocol;
        VerifyMode               _verify;
        int                      _verifyDepth;
        mbedtls_ssl_config       _clientConfig;
        mbedtls_ssl_config       _serverConfig;
        mbedtls_entropy_context  _entropy;
        mbedtls_ctr_drbg_context _drbg;
        mbedtls_x509_crt*        _identityCert;
        mbedtls_pk_context*      _identityKey;
        bool                     _ownCertRegistered;
        mbedtls_x509_crt*        _caChain;
};

} // namespace Ssl

} // namespace Pt

#endif
