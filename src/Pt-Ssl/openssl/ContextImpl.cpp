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
#include "CertificateImpl.h"
#include "OpenSsl.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Logger.h>

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


X509* copyX509(X509* from)
{   
    return X509_dup(from);
}


EVP_PKEY* copyPrivateKey(EVP_PKEY* from)
{
    BIO* b = BIO_new( BIO_s_mem() );

    BioAutoPtr bio(b);

    if (PEM_write_bio_PKCS8PrivateKey(bio.get(), from, 0, 0, 0, 0, 0) <= 0) 
    {
        throw InvalidCertificate("invalid certificate");
    }

    EVP_PKEY *target = 0;
    if (PEM_read_bio_PrivateKey(bio.get(), &target, 0, 0) == 0) 
    {
        throw InvalidCertificate("invalid certificate");
    }
    
    return target;
}


ContextImpl::ContextImpl(Protocol protocol)
: _protocol(protocol)
, _verify(TryVerify)
, _verifyDepth(1)
, _x509(0)
, _pkey(0)
{
    // Create the context for the given protocol
    switch(_protocol) 
    {
        case SSLv2: 
            // SSLv2_method is not available everywhere (check OPENSSL_NO_SSL2)
            _ctx = SSL_CTX_new( SSLv23_method () ); 
            break;
        
        case SSLv3or2: 
            _ctx = SSL_CTX_new( SSLv23_method() ); 
            break;

        default:
        case SSLv3: 
            _ctx = SSL_CTX_new( SSLv3_method () ); 
            break;
        
        case TLSv1: 
            _ctx = SSL_CTX_new( TLSv1_method () ); 
            break;
    }

    // Set some options
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(_ctx, _verifyDepth);
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

    if(_pkey)
        EVP_PKEY_free(_pkey);
    
    if(_x509)
        X509_free(_x509);

    for(std::vector<X509*>::iterator it = _caCerts.begin(); it != _caCerts.end(); ++it)
    {
        X509_free(*it);
    }
}


Protocol ContextImpl::protocol() const
{ 
    return _protocol; 
}


void ContextImpl::setProtocol(Protocol protocol)
{
    bool v2 = false;

    switch(protocol) 
    {
        case SSLv2: 
            // SSLv2_method is not available everywhere (check OPENSSL_NO_SSL2)
            SSL_CTX_set_ssl_version(_ctx, SSLv23_method() );
            v2 = true;
            break;
        
        case SSLv3or2: 
            SSL_CTX_set_ssl_version( _ctx, SSLv23_method() ); 
            v2 = true;
            break;

        default:
        case SSLv3: 
            SSL_CTX_set_ssl_version( _ctx, SSLv3_method() ); 
            break;
        
        case TLSv1: 
            SSL_CTX_set_ssl_version( _ctx, TLSv1_method() ); 
            break;
    }

    _protocol = protocol;

    const char* ciphers = v2 ? "ALL:!aNULL:!eNULL" : "ALL:!aNULL:!eNULL:!SSLv2";
    SSL_CTX_set_cipher_list(_ctx, ciphers);
}


void ContextImpl::setVerifyDepth(int n)
{
    SSL_CTX_set_verify_depth(_ctx, n);
    _verifyDepth = n;
}


VerifyMode ContextImpl::verifyMode() const
{ 
    return _verify; 
}


void ContextImpl::setVerifyMode(VerifyMode m)
{
    int mode = SSL_VERIFY_NONE;
    switch(m)
    {
        case TryVerify:
            mode = SSL_VERIFY_PEER;
            break;
    
        case AlwaysVerify:
            mode = SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
            break;

        default:
        case NoVerify:
            mode = SSL_VERIFY_NONE;
    }

    SSL_CTX_set_verify(_ctx, mode, 0);

    _verify = m;
}


void ContextImpl::assign(const ContextImpl& ctx)
{
    // TODO: consider to create a new SSL_CTX if required

    setProtocol(ctx._protocol);
    setVerifyMode(ctx._verify);
    setVerifyDepth(ctx._verifyDepth);

    // copy certificates presented to peer

    if(_pkey)
        EVP_PKEY_free(_pkey);
    _pkey = 0;
    
    if(_x509)
        X509_free(_x509);
    _x509 = 0;

    if( ctx._x509 )
    {
        _pkey = copyPrivateKey( ctx._pkey );  
        _x509 = copyX509( ctx._x509 );
        
        if( ! SSL_CTX_use_certificate(_ctx, _x509) )
        {
            throw InvalidCertificate("invalid certificate");
        }

        if( ! SSL_CTX_use_PrivateKey( _ctx, _pkey ) )
        {
            throw InvalidCertificate("invalid certificate");
        }
    }

    _extraCerts.clear();
    _extraCerts.reserve( ctx._extraCerts.size() );

    for(std::vector<X509*>::const_iterator it = ctx._extraCerts.begin(); it != ctx._extraCerts.end(); ++it)
    {
        // NOTE: SSL_CTX_add_extra_chain_cert does not copy the X509 certificate, 
        // or increase the refcount. We must copy it, because the SSL_CTX will
        // free it

        X509* extraX509 = copyX509(*it);
        X509AutoPtr x509Ptr(extraX509);

        if( ! SSL_CTX_add_extra_chain_cert( _ctx, extraX509 ) )
            throw InvalidCertificate("invalid extra certificate");

        _extraCerts.push_back(extraX509);
        x509Ptr.release();
    }

    // copy trusted CA certificates
    for(std::vector<X509*>::iterator it = _caCerts.begin(); it != _caCerts.end(); ++it)
    {
        X509_free(*it);
    }
    
    _caCerts.clear();
    _caCerts.reserve( ctx._caCerts.size() );

    X509_STORE* store = X509_STORE_new();
    X509StoreAutoPtr storePtr(store);

    for(std::vector<X509*>::const_iterator it = ctx._caCerts.begin(); it != ctx._caCerts.end(); ++it)
    {
        X509* x509 = copyX509(*it);
        X509AutoPtr x509Ptr(x509);

        if( ! X509_STORE_add_cert(store, x509) )
            throw InvalidCertificate("untrusted certificate");

        _caCerts.push_back(x509);
        x509Ptr.release();
    }

    SSL_CTX_set_cert_store( _ctx, store );
    storePtr.release();
}


void ContextImpl::setIdentity(const Certificate& cert)
{
    if( ! cert.impl()->pkey() )
        throw InvalidCertificate("invalid certificate");

    if(_pkey)
        EVP_PKEY_free(_pkey);
    _pkey = 0;

    if(_x509)
        X509_free(_x509);
    _x509 = 0;

    _x509 = copyX509( cert.impl()->x509() );
    _pkey = copyPrivateKey( cert.impl()->pkey() );

    if( ! SSL_CTX_use_certificate(_ctx, _x509) )
    {
        throw InvalidCertificate("invalid certificate");
    }

    if( ! SSL_CTX_use_PrivateKey( _ctx, _pkey ) )
    {
        throw InvalidCertificate("invalid certificate");
    }
    
    // openssl will not check the private key of this context against the 
    // certifictate. TO do so call SSL_CTX_check_private_key(_ctx)
}


void ContextImpl::addCertificate(const Certificate& certificate)
{
    // NOTE: SSL_CTX_add_extra_chain_cert does not copy the X509 certificate, 
    // or increase the refcount. We must copy it, because the SSL_CTX will
    // free it

    _extraCerts.reserve(_extraCerts.size() + 1);

    X509* extraX509 = copyX509( certificate.impl()->x509() );
    X509AutoPtr x509Ptr(extraX509);

    if( ! SSL_CTX_add_extra_chain_cert(_ctx, extraX509) )
    {
        throw InvalidCertificate("invalid extra certificate");
    }

    _extraCerts.push_back(extraX509);
    x509Ptr.release();
}


void ContextImpl::addCACertificate(const Certificate& trustedCert)
{
    log_trace("adding CA certificate:" << trustedCert.subject());
    
    _caCerts.reserve(_caCerts.size() + 1);

    X509* x509 = copyX509( trustedCert.impl()->x509() );
    X509AutoPtr x509Ptr(x509);

    X509_STORE* store = SSL_CTX_get_cert_store(_ctx);
    if( ! store)
    {
        log_trace("creating new X509 store");
        store = X509_STORE_new();
    }

    if( ! X509_STORE_add_cert(store, x509) )
    {
        throw InvalidCertificate("invalid CA certificate");
    }
    
    _caCerts.push_back(x509);
    x509Ptr.release();
}


SSL_CTX* ContextImpl::ctx() const
{ 
    return _ctx; 
}

} // namespace Ssl

} // namespace Pt
