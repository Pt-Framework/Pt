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

#include "OpenSsl.h"
#include <Pt/Ssl/Context.h>
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Logger.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <cstdio>

namespace Pt {

namespace Ssl {

log_define("Pt.Ssl.Context")

#define COPY_EXTRA_CERT

static int ssl_init_counter = 0;

static Pt::System::Mutex* sslmtx = 0;

void pt_locking_callback(int mode, int type, const char* file,  int line)
{
    log_trace("thread: " << ((mode&CRYPTO_LOCK)?"l":"u") 
                         << ((type&CRYPTO_READ)?"r":"w")
                         << ' ' << file << ':' << line );
    
    if (mode & CRYPTO_LOCK)
    {
        sslmtx[type].lock();
    }
    else
    {
        sslmtx[type].unlock();
    }
}

SSLInit::SSLInit()
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
	      CRYPTO_set_locking_callback(pt_locking_callback);

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


SSLInit::~SSLInit()
{
    if(0 == --ssl_init_counter) 
    {
        log_info("OpenSSL library shutdown");
        delete [] sslmtx;
        sslmtx = 0;
    }
}


Context::Context(Protocol protocol)
: _protocol(protocol)
, _certChainExist(false)
, _reserved(0)
{
    // Create the context for the given protocol
    switch(_protocol) 
    {
        case SSLv2    : _ctx = SSL_CTX_new( SSLv2_method () ); break;
        case SSLv3or2 : _ctx = SSL_CTX_new( SSLv23_method() ); break;
        case TLSv1    : _ctx = SSL_CTX_new( TLSv1_method () ); break;
        case DefaultProtocol  : // Fall through
        case SSLv3    : // Fall through
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


Context::~Context()
{
#ifndef COPY_EXTRA_CERT
    if(_ctx->extra_certs) 
    {
        sk_X509_pop(_ctx->extra_certs);
        _ctx->extra_certs = 0;
    }
#endif

    SSL_CTX_free(_ctx);
}


void Context::enableSessions(const char* id, unsigned long timeout, unsigned long cacheSize)
{
    SSL_CTX_set_session_cache_mode(_ctx, SSL_SESS_CACHE_SERVER);

    SSL_CTX_set_session_id_context(_ctx,
                                    reinterpret_cast<const unsigned char*>(id),
                                    strlen(id));

    if(timeout == DefaultTimeout)
        SSL_CTX_set_timeout(_ctx, 300);
    else
        SSL_CTX_set_timeout( _ctx, static_cast<long>(timeout) );

    if(cacheSize == DefaultCacheSize)
        SSL_CTX_sess_set_cache_size(_ctx, SSL_SESSION_CACHE_MAX_SIZE_DEFAULT);
    else
        SSL_CTX_sess_set_cache_size( _ctx, static_cast<long>(cacheSize) );
    
    /* Possible functions that will allow us to store/retrieve session data to/from disk.

      long SSL_CTX_set_session_cache_mode(SSL_CTX ctx, long mode);
          # Enables/disables session caching by setting the operational mode for ctx to <mode>.
          # Returns the previously set cache mode.
          + SSL_SESS_CACHE_OFF
              No session caching for client or server takes place.
          + SSL_SESS_CACHE_CLIENT
              Client sessions are added to the session cache. As there is no reliable way for the OpenSSL library to know whether
              a session should be reused or which session to choose (due to the abstract BIO layer the SSL engine does not have
              details about the connection), the application must select the session to be reused by using the ssl_set_session(3)
              function. This option is not activated by default.
          + SSL_SESS_CACHE_SERVER
              Server sessions are added to the session cache. When a client proposes a session to be reused, the server looks for
              the corresponding session in (first) the internal session cache (unless SSL_SESS_CACHE_NO_INTERNAL_LOOKUP is set),
              then (second) in the external cache if available. If the session is found, the server will try to reuse the session.
              This is the default.
          + SSL_SESS_CACHE_BOTH
              Enable both SSL_SESS_CACHE_CLIENT and SSL_SESS_CACHE_SERVER at the same time.
          + SSL_SESS_CACHE_NO_AUTO_CLEAR
              Normally the session cache is checked for expired sessions every 255 connections using the ssl_ctx_flush_sessions(3)
              function. Since this may lead to a delay which cannot be controlled, the automatic flushing may be disabled and
              ssl_ctx_flush_sessions(3) can be called explicitly by the application.
          + SSL_SESS_CACHE_NO_INTERNAL_LOOKUP
              By setting this flag, session-resume operations in an SSL/TLS server will not automatically look up sessions in
              the internal cache, even if sessions are automatically stored there. If external session caching callbacks are
              in use, this flag guarantees that all lookups are directed to the external cache. As automatic lookup only applies
              for SSL/TLS servers, the flag has no effect on clients.
          + SSL_SESS_CACHE_NO_INTERNAL_STORE
              Depending on the presence of SSL_SESS_CACHE_CLIENT and/or SSL_SESS_CACHE_SERVER, sessions negotiated in an SSL/TLS
              handshake may be cached for possible reuse. Normally a new session is added to the internal cache as well as any
              external session caching (callback) that is configured for the SSL_CTX . This flag will prevent sessions being stored
              in the internal cache (though the application can add them manually using ssl_ctx_add_session(3)). Note: in any
              SSL/TLS servers where external caching is configured, any successful session lookups in the external cache
              (ie. for session-resume requests) would normally be copied into the local cache before processing continues -
              this flag prevents these additions to the internal cache as well.
          + SSL_SESS_CACHE_NO_INTERNAL
              Enable both SSL_SESS_CACHE_NO_INTERNAL_LOOKUP and SSL_SESS_CACHE_NO_INTERNAL_STORE at the same time.

      long SSL_CTX_sess_set_cache_size(SSL_CTX *ctx, long t);
          # The internal session cache size is SSL_SESSION_CACHE_MAX_SIZE_DEFAULT, currently 1024*20, so that up to 20000 sessions
            can be held.
          # A special case is the size 0, which is used for unlimited size.
          # When the maximum number of sessions is reached, no more new sessions are added to the cache. New space may be added
              by calling SSL_CTX_flush_sessions(3) to remove expired sessions.
          # Returns the previously valid size.

      void SSL_CTX_flush_sessions(SSL_CTX *ctx, long tm);
          # The parameter tm specifies the time which should be used for the expiration test, in most cases the actual time given
            by time(0) will be used.
          # SSL_CTX_flush_sessions() will only check sessions stored in the internal cache. When a session is found and removed,
              the remove_session_cb is however called to synchronize with the external cache (see ssl_ctx_sess_set_get_cb(3)).

      int SSL_CTX_add_session(SSL_CTX *ctx, SSL_SESSION *c);
          # Adds the session c to the context ctx. The reference count for session c is incremented by 1. If a session with
            the same session id already exists, the old session is removed by calling SSL_SESSION_free(3).
          # If a server SSL_CTX is configured with the SSL_SESS_CACHE_NO_INTERNAL_STORE flag then the internal cache will not
            be populated automatically by new sessions negotiated by the SSL/TLS implementation, even though the internal cache
            will be searched automatically for session-resume requests (the latter can be surpressed by
            SSL_SESS_CACHE_NO_INTERNAL_LOOKUP). So the application can use SSL_CTX_add_session() directly to have full control
            over the sessions that can be resumed if desired.
          # Return 0 if the operation failed. In case of the add operation, it was tried to add
            the same (identical) session twice.

      int SSL_CTX_remove_session(SSL_CTX *ctx, SSL_SESSION *c);
          # Removes the session c from the context ctx. SSL_SESSION_free(3) is called once for c.
          # Return 0 if the operation failed. In case of the add operation, it was tried to add
            the same (identical) session twice. In case of the remove operation, the
            session was not found in the cache.

      void SSL_CTX_sess_set_new_cb(SSL_CTX *ctx, int (*new_session_cb)(SSL *, SSL_SESSION *));
          # Callback: int (*new_session_cb)(struct ssl_st *ssl, SSL_SESSION *sess);
          # The callback is called, whenever a new session has been negotiated and session caching is enabled.
          # The callback is passed the ssl connection and the ssl session sess.
          # If the callback returns 0, the session will be immediately removed again.

      void SSL_CTX_sess_set_remove_cb(SSL_CTX *ctx, void (*remove_session_cb)(SSL_CTX *ctx, SSL_SESSION *));
          # void (*remove_session_cb)(struct ssl_ctx_st *ctx, SSL_SESSION *sess);
          # The callback is called, whenever the SSL engine removes a session from the internal cache.
          # This happens when the session is removed because it is expired or when a connection was not shutdown cleanly.
            It also happens for all sessions in the internal session cache when ssl_ctx_free(3) is called.
          # The callback is passed the ctx and the ssl session sess.

      void SSL_CTX_sess_set_get_cb(SSL_CTX *ctx, SSL_SESSION (*get_session_cb)(SSL *, unsigned char *, int, int *));
          # SSL_SESSION *(*get_session_cb)(struct ssl_st *ssl, unsigned char *data, int len, int *copy);
          # The callback is only called on SSL/TLS servers with the session id proposed by the client.
          # The callback is always called, also when session caching was disabled.
          # The callback is passed the ssl connection, the session id of length len at the memory location data.
          # With the parameter copy the callback can require the SSL engine to increment the reference count of the SSL_SESSION object.
            Normally the reference count is not incremented and therefore the session must not be explicitly freed with ssl_session_free(3).

      SSL_SESSION *d2i_SSL_SESSION(SSL_SESSION **a, const unsigned char **pp, long length);
          # Transforms the external ASN1 representation of an SSL/TLS session, stored as binary data at location pp with length length,
            into an SSL_SESSION object.
          # In case of failure the NULL-pointer is returned and the error message can be retrieved from the error stack.

      int i2d_SSL_SESSION(SSL_SESSION *in, unsigned char **pp);
          # Transforms the SSL_SESSION object in into the ASN1 representation and stores it into the memory location pointed to by pp.
          # The length of the resulting ASN1 representation is returned.
          # If pp is the NULL pointer, only the length is calculated and returned.
          # When the session is not valid, 0 is returned and no operation is performed.
          # There is no known limit on the size of the created ASN1 representation, so the necessary amount of space should be obtained
            by first calling i2d_SSL_SESSION() with pp=NULL, and obtain the size needed, then allocate the memory and call
            i2d_SSL_SESSION() again.
  */
}


Context::Protocol Context::protocol() const
{ 
    return _protocol; 
}


void Context::setProtocol(Protocol protocol)
{
    bool v2 = false;
    int ret = 0;

    switch(protocol) 
    {
        case SSLv2    : ret = SSL_CTX_set_ssl_version(_ctx, SSLv2_method());  v2 = true; break;
        case SSLv3or2 : ret = SSL_CTX_set_ssl_version(_ctx, SSLv23_method()); v2 = true; break;
        case TLSv1    : ret = SSL_CTX_set_ssl_version(_ctx, TLSv1_method()); break;
        case SSLv3    : // Fall through
        case DefaultProtocol  : // Fall through
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


void Context::setVerifyMode(VerifyMode m)
{
    int mode = 0;
    switch(m)
    {
        case VerifyPeer:
            mode = SSL_VERIFY_PEER;
            break;
    
        case VerifyPeerRequired:
            mode = SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
            break;

        default:
            mode = SSL_VERIFY_NONE;
    }

    SSL_CTX_set_verify(_ctx, mode, 0);
}


/*
std::vector<SSLCipherInfo> SSLStreamBuf::availableCiphers() const
{
    NOTE: we could keep a ssl_st here to determine available ciphers, if a
    new SSLStreamBuf is constricted we pass that ssl_st and create a new one
    Also would need to recreate the sst_st if we change protocol.
}
*/
/*
void Context::setEnabledCiphers(const std::vector<SSLCipherInfo>& ciphers)
{
    std::string str;
    for(size_t i = 0; i < ciphers.size(); ++i) {
        if(!str.empty()) str += ":";
        str += ciphers[i].name;
    }

    if(!SSL_CTX_set_cipher_list(_ctx, str.c_str()))
        throw SSLRuntimeError("Failed selecting SSL ciphers!", PT_SOURCEINFO);

    _enabledCiphers = ciphers;
}
*/

// certificates to validate certificate presented by peer
void Context::setCACertificates(const CertificateList& trustedCert)
{
    // Try to add the CA X509 certificates (if any)
    X509_STOREAutoPtr cert_store( X509_STORE_new() );

    for(CertificateList::Iterator it = trustedCert.begin(); it != trustedCert.end(); ++it) 
    {
        if( ! X509_STORE_add_cert(cert_store.get(), it->getX509()) )
            throw InvalidCertificate("Could not store the CA certificate as a trusted certificate");
    }

    // Set it to the context
    SSL_CTX_set_cert_store(_ctx, cert_store.get());
    cert_store.release();
}

// certificate to present to peer
void Context::setCertificate(const Certificate& cert)
{
    ERR_clear_error();
    
    X509* x509 = cert.getX509();

    if( ! SSL_CTX_use_certificate(_ctx, x509) || ERR_peek_error() )
    {
        throw InvalidCertificate("Invalid/mismatched certificate");
    }
}


// intermediate certificates to present to peer
void Context::addCertificate(const Certificate& cert)
{
#ifdef COPY_EXTRA_CERT
    // NOTE: SSL_CTX_add_extra_chain_cert does not copy the X509 certificate, 
    // or increase the refcount, so we must "copy" it manually, because SSL_CTX
    // will take ownership

    X509* x509 = X509_dup( cert.getX509() );

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

    if( ! x509 )
        throw InvalidCertificate("Could not duplicate the CA certificate");

    if( ! SSL_CTX_add_extra_chain_cert( _ctx, x509 ) )
        throw InvalidCertificate("Could not add CA certificate");
#else        
    if( ! SSL_CTX_add_extra_chain_cert( _ctx, cert.getX509() ) )
        throw InvalidCertificate("Could not add CA certificate");
#endif

    _certChainExist = true;
}


void Context::setCertificateChain(const CertificateList& certs)
{
    CertificateList::Iterator it = certs.begin();
    if( it == certs.end() )
        throw InvalidCertificate("certificate list too short");

    this->setCertificate(*it);
    ++it;

    for(; it != certs.end(); ++it)
    {
        this->addCertificate(*it);
    }
}


void Context::setPrivateKey(const PrivateKey& privKey)
{
    // Try to use the private key
    if( ! SSL_CTX_use_PrivateKey( _ctx, privKey.impl() ) )
        throw InvalidKey("Invalid private-key!");

    // Store a reference to the private key
    _privKey = privKey;
    
    // Check the private key (if needed)
    if( ! _certChainExist) 
    {
        if( ! SSL_CTX_check_private_key(_ctx) )
            throw InvalidKey("The private key does not agree with the corresponding public key in the certificate!");
    }
}


const PrivateKey& Context::privateKey() const
{
    return _privKey;
}


ssl_ctx_st* Context::impl() const
{ 
    return _ctx; 
}

} // namespace Ssl

} // namespace Pt
