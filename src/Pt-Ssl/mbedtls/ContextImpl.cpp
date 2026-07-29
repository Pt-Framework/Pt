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

#include "ContextImpl.h"
#include "CertificateImpl.h"
#include "MbedTls.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <mbedtls/error.h>
#include <cstring>

PT_LOG_DEFINE("Pt.Ssl.Context")

namespace Pt {

namespace Ssl {

namespace {

// mbedtls has no cert/key "dup"; deep-copy via a DER parse round-trip instead.
mbedtls_x509_crt* copyCertificate(const mbedtls_x509_crt* src)
{
    X509CrtAutoPtr copyPtr( new mbedtls_x509_crt() );
    mbedtls_x509_crt_init( copyPtr.get() );

    if( mbedtls_x509_crt_parse_der(copyPtr.get(), src->raw.p, src->raw.len) != 0 )
        throw InvalidCertificate("invalid certificate");

    return copyPtr.release();
}


mbedtls_pk_context* copyPrivateKey(const mbedtls_pk_context* src, mbedtls_ctr_drbg_context* drbg)
{
    unsigned char buf[4096];
    int n = mbedtls_pk_write_key_der( const_cast<mbedtls_pk_context*>(src), buf, sizeof(buf) );
    if(n < 0)
        throw InvalidCertificate("invalid certificate key");

    // mbedtls_pk_write_key_der() writes right-aligned into buf
    const unsigned char* start = buf + sizeof(buf) - n;

    PkAutoPtr copyPtr( new mbedtls_pk_context() );
    mbedtls_pk_init( copyPtr.get() );

    int ret = mbedtls_pk_parse_key( copyPtr.get(), start, static_cast<std::size_t>(n),
                                     0, 0, mbedtls_ctr_drbg_random, drbg );
    if(ret != 0)
        throw InvalidCertificate("invalid certificate key");

    return copyPtr.release();
}


void freeCertChain(mbedtls_x509_crt* head)
{
    while(head)
    {
        mbedtls_x509_crt* next = head->next;
        head->next = 0;
        mbedtls_x509_crt_free(head);
        delete head;
        head = next;
    }
}

} // namespace

ContextImpl::ContextImpl(Protocol protocol)
: _protocol(protocol)
, _verify(TryVerify)
, _verifyDepth(1)
, _identityCert(0)
, _identityKey(0)
, _ownCertRegistered(false)
, _caChain(0)
{
    mbedtls_ssl_config_init(&_config);
    mbedtls_entropy_init(&_entropy);
    mbedtls_ctr_drbg_init(&_drbg);

    static const char pers[] = "Pt.Ssl.Context";
    if( mbedtls_ctr_drbg_seed(&_drbg, mbedtls_entropy_func, &_entropy,
                              reinterpret_cast<const unsigned char*>(pers),
                              sizeof(pers) - 1) != 0 )
        throw SslError("failed to seed RNG");

    if( mbedtls_ssl_config_defaults(&_config, MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT) != 0 )
        throw SslError("failed to initialize SSL configuration");

    mbedtls_ssl_conf_rng(&_config, mbedtls_ctr_drbg_random, &_drbg);

    setProtocol(protocol);
    setVerifyMode(_verify);
}


ContextImpl::~ContextImpl()
{
    if(_identityKey)
    {
        mbedtls_pk_free(_identityKey);
        delete _identityKey;
    }

    freeCertChain(_identityCert);
    freeCertChain(_caChain);

    mbedtls_ssl_config_free(&_config);
    mbedtls_ctr_drbg_free(&_drbg);
    mbedtls_entropy_free(&_entropy);
}


Protocol ContextImpl::protocol() const
{
    return _protocol;
}


void ContextImpl::setProtocol(Protocol protocol)
{
    switch(protocol)
    {
        case TLS: // negotiate the highest version mbedtls supports
            mbedtls_ssl_conf_min_tls_version(&_config, MBEDTLS_SSL_VERSION_TLS1_2);
            mbedtls_ssl_conf_max_tls_version(&_config, MBEDTLS_SSL_VERSION_TLS1_3);
            break;

        case TLSv1_2:
            mbedtls_ssl_conf_min_tls_version(&_config, MBEDTLS_SSL_VERSION_TLS1_2);
            mbedtls_ssl_conf_max_tls_version(&_config, MBEDTLS_SSL_VERSION_TLS1_2);
            break;

        default:
            // mbedtls 3.x no longer implements SSLv2/SSLv3/TLSv1.0/TLSv1.1
            throw SslError("unsupported protocol");
    }

    _protocol = protocol;
}


void ContextImpl::setVerifyDepth(int n)
{
    _verifyDepth = n;
}


VerifyMode ContextImpl::verifyMode() const
{
    return _verify;
}


void ContextImpl::setVerifyMode(VerifyMode m)
{
    int mode = MBEDTLS_SSL_VERIFY_NONE;
    switch(m)
    {
        case NoVerify:     mode = MBEDTLS_SSL_VERIFY_NONE;     break;
        case TryVerify:    mode = MBEDTLS_SSL_VERIFY_OPTIONAL; break;
        case AlwaysVerify: mode = MBEDTLS_SSL_VERIFY_REQUIRED; break;
    }

    mbedtls_ssl_conf_authmode(&_config, mode);
    _verify = m;
}


void ContextImpl::assign(const ContextImpl& ctx)
{
    setProtocol(ctx._protocol);
    setVerifyMode(ctx._verify);
}


void ContextImpl::addCACertificate(const Certificate& trustedCert)
{
    mbedtls_x509_crt* copy = copyCertificate( trustedCert.impl()->crt() );

    if( ! _caChain )
    {
        _caChain = copy;
    }
    else
    {
        mbedtls_x509_crt* tail = _caChain;
        while(tail->next)
            tail = tail->next;
        tail->next = copy;
    }

    mbedtls_ssl_conf_ca_chain(&_config, _caChain, 0);
}


void ContextImpl::setIdentity(const Certificate& cert)
{
    if( ! cert.impl()->pk() )
        throw InvalidCertificate("certificate has no private key");

    mbedtls_x509_crt* leaf = copyCertificate( cert.impl()->crt() );
    mbedtls_pk_context* key = copyPrivateKey( cert.impl()->pk(), &_drbg );

    // splice any chain certs already added via addCertificate() before the identity was known
    leaf->next = _identityCert;
    _identityCert = leaf;
    _identityKey = key;

    maybeRegisterOwnCert();
}


void ContextImpl::addCertificate(const Certificate& certificate)
{
    mbedtls_x509_crt* copy = copyCertificate( certificate.impl()->crt() );

    if( ! _identityCert )
    {
        _identityCert = copy;
    }
    else
    {
        mbedtls_x509_crt* tail = _identityCert;
        while(tail->next)
            tail = tail->next;
        tail->next = copy;
    }

    maybeRegisterOwnCert();
}


void ContextImpl::maybeRegisterOwnCert()
{
    if(_ownCertRegistered || ! _identityCert || ! _identityKey)
        return;

    if( mbedtls_ssl_conf_own_cert(&_config, _identityCert, _identityKey) != 0 )
        throw SslError("failed to set identity certificate");

    _ownCertRegistered = true;
}

} // namespace Ssl

} // namespace Pt
