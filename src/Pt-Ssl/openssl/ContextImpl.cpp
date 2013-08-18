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

#include "ContextImpl.h"
#include "OpenSsl.h"
#include "CertificateImpl.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Logger.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <cstdio>

log_define("Pt.Ssl.Context")

namespace Pt {

namespace Ssl {

static int ssl_init_counter = 0;

static Pt::System::Mutex* sslmtx = 0;

void pt_locking_callback_impl(int mode, int type, const char* file,  int line)
{
    //log_trace("thread: " << ((mode&CRYPTO_LOCK)?"l":"u") 
    //                     << ((type&CRYPTO_READ)?"r":"w")
    //                     << ' ' << file << ':' << line );
    
    if (mode & CRYPTO_LOCK)
    {
        sslmtx[type].lock();
    }
    else
    {
        sslmtx[type].unlock();
    }
}

void SSLInitImpl()
{
    if(0 == ssl_init_counter++) 
    {
        log_info("OpenSSL library initialization");

        SSL_library_init();
        SSL_load_error_strings();
        ERR_load_crypto_strings();

        int numLocks = CRYPTO_num_locks();
        sslmtx = new Pt::System::Mutex[numLocks];

	      //CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
	      CRYPTO_set_locking_callback(pt_locking_callback_impl);

        //OpenSSL_add_all_algorithms();
        EVP_add_cipher(EVP_des_ede3_cfb());
        EVP_add_cipher(EVP_des_ede3_cfb1());
        EVP_add_cipher(EVP_des_ede3_cfb8());
        EVP_add_cipher(EVP_des_ede3_ofb());

        EVP_add_cipher(EVP_aes_128_ecb());
        EVP_add_cipher(EVP_aes_128_cbc());
        EVP_add_cipher(EVP_aes_128_cfb());
        EVP_add_cipher(EVP_aes_128_cfb1());
        EVP_add_cipher(EVP_aes_128_cfb8());
        EVP_add_cipher(EVP_aes_128_ofb());
        EVP_add_cipher(EVP_aes_192_ecb());
        EVP_add_cipher(EVP_aes_192_cbc());
        EVP_add_cipher(EVP_aes_192_cfb());
        EVP_add_cipher(EVP_aes_192_cfb1());
        EVP_add_cipher(EVP_aes_192_cfb8());
        EVP_add_cipher(EVP_aes_192_ofb());
        EVP_add_cipher(EVP_aes_256_ecb());
        EVP_add_cipher(EVP_aes_256_cbc());
        EVP_add_cipher(EVP_aes_256_cfb());
        EVP_add_cipher(EVP_aes_256_cfb1());
        EVP_add_cipher(EVP_aes_256_cfb8());
        EVP_add_cipher(EVP_aes_256_ofb());
    }
}


void SSLExitImpl()
{
    if(0 == --ssl_init_counter) 
    {
        log_info("OpenSSL library shutdown");
        delete [] sslmtx;
        sslmtx = 0;
    }
}


ContextImpl::ContextImpl(Context::Protocol protocol)
: _protocol(protocol)
, _reserved(0)
, _verify(Context::VerifyPeer)
{
    // Create the context for the given protocol
    switch(_protocol) 
    {
        case Context::SSLv2    : _ctx = SSL_CTX_new( SSLv2_method () ); break;
        case Context::SSLv3or2 : _ctx = SSL_CTX_new( SSLv23_method() ); break;
        case Context::TLSv1    : _ctx = SSL_CTX_new( TLSv1_method () ); break;
        case Context::DefaultProtocol  : // Fall through
        case Context::SSLv3    : // Fall through
        default       :  _ctx = SSL_CTX_new( SSLv3_method () ); break;
    }

    // Set some options
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(_ctx, 1);
#endif
    
    SSL_CTX_set_options(_ctx, SSL_OP_SINGLE_DH_USE);
    SSL_CTX_set_mode(_ctx, SSL_MODE_NO_AUTO_CHAIN);
    SSL_CTX_set_mode(_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
    //SSL_CTX_set_read_ahead(_ctx, 1);

    SSL_CTX_set_session_cache_mode(_ctx, SSL_SESS_CACHE_OFF);
}


ContextImpl::~ContextImpl()
{
    SSL_CTX_free(_ctx);
}


Context::Protocol ContextImpl::protocol() const
{ 
    return _protocol; 
}


void ContextImpl::setProtocol(Context::Protocol protocol)
{
    bool v2 = false;
    int ret = 0;

    switch(protocol) 
    {
        case Context::SSLv2    : ret = SSL_CTX_set_ssl_version(_ctx, SSLv2_method());  v2 = true; break;
        case Context::SSLv3or2 : ret = SSL_CTX_set_ssl_version(_ctx, SSLv23_method()); v2 = true; break;
        case Context::TLSv1    : ret = SSL_CTX_set_ssl_version(_ctx, TLSv1_method()); break;
        case Context::SSLv3    : // Fall through
        case Context::DefaultProtocol  : // Fall through
        default       : ret = SSL_CTX_set_ssl_version(_ctx, SSLv3_method());
    }

    if( 0 == ret)
        throw std::logic_error("Unknown protocol");

    _protocol = protocol;

    const char* list = v2 ? "ALL:!aNULL:!eNULL" : "ALL:!aNULL:!eNULL:!SSLv2";
    ret = SSL_CTX_set_cipher_list(_ctx, list);
    if( 0 == ret )
        throw std::logic_error("Invalid default cipher list");
}


void ContextImpl::setVerifyDepth(int n)
{
    SSL_CTX_set_verify_depth(_ctx, n);
}


void ContextImpl::setVerifyMode(Context::VerifyMode m)
{
    _verify = m;

    int mode = 0;
    switch(_verify)
    {
        case Context::VerifyPeer:
            mode = SSL_VERIFY_PEER;
            break;
    
        case Context::VerifyPeerRequired:
            mode = SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
            break;

        default:
            mode = SSL_VERIFY_NONE;
    }

    SSL_CTX_set_verify(_ctx, mode, 0);
}


X509* addExtraCertImpl(SSL_CTX* ctx, X509* x)
{
    // NOTE: SSL_CTX_add_extra_chain_cert does not copy the X509 certificate, 
    // or increase the refcount, so we must "copy" it manually, because SSL_CTX
    // will take ownership

    X509* dupX509 = X509_dup(x);

    // // Convert the X509 certificate to raw binary data
    // unsigned char* buf = 0;
    // int len = i2d_X509(cert.getX509(), &buf);
    // if(len < 0)
    //     throw InvalidCertificate("Could not convert the CA certificate to raw binary data");
    // 
    // // Convert the raw binary data back to an X509 certificate
    // unsigned char** tbf = &buf;
    // X509* x509 = d2i_X509(0, tbf, len);
    // OPENSSL_free(buf);
    //
    //if( ! x509)
    //    throw InvalidCertificate("Could not convert the raw binary data back to a CA certificate");

    if( ! dupX509 )
        throw InvalidCertificate("Could not duplicate the CA certificate");

    if( ! SSL_CTX_add_extra_chain_cert( ctx, dupX509 ) )
        throw InvalidCertificate("Could not add CA certificate");

    return dupX509;
}


void ContextImpl::assign(const ContextImpl& ctx)
{
    setProtocol(ctx._protocol);
    setVerifyMode(ctx._verify);
    setCertificate(ctx._cert);

    // TODO: verify depth

    _caCerts.clear();
    X509_STOREAutoPtr cert_store( X509_STORE_new() );

    for(std::vector<Certificate>::const_iterator it = ctx._caCerts.begin(); it != ctx._caCerts.end(); ++it) 
    {
        if( ! X509_STORE_add_cert(cert_store.get(), it->impl()->getX509()) )
            throw InvalidCertificate("Could not store the CA certificate as a trusted certificate");

        _caCerts.push_back(*it);
    }

    // Set it to the context
    SSL_CTX_set_cert_store(_ctx, cert_store.get());
    cert_store.release();

    std::vector<X509*>::const_iterator it;
    for(it = ctx._extraCerts.begin(); it != ctx._extraCerts.end(); ++it)
    {
        X509* x509 = addExtraCertImpl(_ctx, *it);
        _extraCerts.push_back(x509);
    }
}


//void Context::setCACertificates(const CertificateList& caCerts)
//{
//    // certificates to validate certificate presented by peer
//    _caCerts = caCerts;
//
//    // Try to add the CA X509 certificates (if any)
//    X509_STOREAutoPtr cert_store( X509_STORE_new() );
//
//    for(CertificateList::Iterator it = _caCerts.begin(); it != _caCerts.end(); ++it) 
//    {
//        if( ! X509_STORE_add_cert(cert_store.get(), it->impl()->getX509()) )
//            throw InvalidCertificate("Could not store the CA certificate as a trusted certificate");
//    }
//
//    // Set it to the context
//    SSL_CTX_set_cert_store(_ctx, cert_store.get());
//    cert_store.release();
//}


void ContextImpl::setCACertificates(const std::vector<Certificate>& caCerts)
{
    // certificates to validate certificate presented by peer
    _caCerts.clear();

    // Try to add the CA X509 certificates (if any)
    X509_STOREAutoPtr cert_store( X509_STORE_new() );

    for(std::vector<Certificate>::const_iterator it = caCerts.begin(); it != caCerts.end(); ++it) 
    {
        if( ! X509_STORE_add_cert(cert_store.get(), it->impl()->getX509()) )
            throw InvalidCertificate("Could not store the CA certificate as a trusted certificate");

        _caCerts.push_back(*it);
    }

    // Set it to the context
    SSL_CTX_set_cert_store(_ctx, cert_store.get());
    cert_store.release();
}


void ContextImpl::addCACertificate(const Certificate& trustedCert)
{
    log_trace("adding CA certificate:" << trustedCert.subject());
    X509_STORE* store = SSL_CTX_get_cert_store(_ctx);

    if( ! store)
    {
        log_trace("creating new X509 store");
        store = X509_STORE_new();
    }

    if( ! X509_STORE_add_cert(store, trustedCert.impl()->getX509()) )
        throw InvalidCertificate("untrusted certificate");

    _caCerts.push_back(trustedCert);
}


void ContextImpl::setCertificate(const Certificate& cert)
{
    // certificate to present to peer
    _cert = cert;

    X509* x509 = _cert.impl()->getX509();

    if( ! SSL_CTX_use_certificate(_ctx, x509) )
    {
        throw InvalidCertificate("invalid certificate");
    }

    EVP_PKEY* pkey = _cert.impl()->evp();

    if( pkey )
    {
        if( ! SSL_CTX_use_PrivateKey( _ctx, pkey ) )
        {
            throw InvalidCertificate("invalid certificate");
        }
    
        // openssl will not check the private key of this context against the 
        // certifictate. TO do so call SSL_CTX_check_private_key(_ctx)
    }
}


//void Context::setCertificateChain(const CertificateList& certs)
//{
//    CertificateList::ConstIterator it = certs.begin();
//    if( it == certs.end() )
//        throw InvalidCertificate("certificate chain too short");
//
//    this->setCertificate(*it);
//    ++it;
//
//    for(; it != certs.end(); ++it)
//    {
//        X509* x509 = addExtraCert(_ctx, it->impl()->getX509());
//        _extraCerts.push_back(x509);
//    }
//}


void ContextImpl::loadPkcs12(const char* data, size_t len, const char* passwd)
{
    EVP_PKEY* pkey = NULL;
    X509* cert = NULL;
    STACK_OF(X509)* ca = NULL;

    BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );

    PKCS12* p12 = d2i_PKCS12_bio(in.get(), NULL);
    if( ! p12)
        throw InvalidCertificate("invalid PKCS12 data");

    int status = PKCS12_parse(p12, passwd, &pkey, &cert, &ca);
    PKCS12_free(p12);

    if( ! status )
        throw InvalidCertificate("invalid PKCS12 content");

    // TODO: use smart pointers later...

    if(cert) 
    {
        X509AutoPtr x509(cert);
        Certificate c( new CertificateImpl(x509.get(), pkey) );
        _certificates.push_back(c);
        x509.release();
    }
    
    if(ca)
    {
        for(int i = 0; i < sk_X509_num(ca); i++)
        {
            X509AutoPtr x509( sk_X509_pop(ca) );
            Certificate cert( new CertificateImpl(x509.get()) );
            _certificates.push_back(cert);
            x509.release();
        }

        sk_X509_pop_free(ca, X509_free);
        ca = NULL;
    }  
}


Certificate* ContextImpl::findCertificate(const std::string& subject)
{
    for(std::vector<Certificate>::const_iterator it = _certificates.begin(); it != _certificates.end(); ++it) 
    {
        if( it->subject().find(subject) != std::string::npos )
            return &(*it);
    }

    return 0;
}


ssl_ctx_st* ContextImpl::ctx() const
{ 
    return _ctx; 
}

} // namespace Ssl

} // namespace Pt
