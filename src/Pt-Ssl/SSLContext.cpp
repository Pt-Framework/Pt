/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

#include <Pt/Ssl/SSLContext.h>
#include <cstdio>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "Utils.h"

namespace Pt {

namespace Ssl {

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////
log_define(PT_SSL_LOGGER_CATEGORY);
#define PT_SSL_LOG(CODE) PT_SSL_LOG_INFO("SSLContext  ", CODE)
////////////////////////////////////////////////////////////////////////////////////////////////////

#define COPY_EXTRA_CERT

static int ssl_init_counter = 0;

SSLInit::SSLInit()
{
    if(0 == ssl_init_counter++) {
        SSL_library_init();
        SSL_load_error_strings();
        ERR_load_crypto_strings();

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

        Pt::System::LogTarget& target = Pt::System::LogTarget::get(PT_SSL_LOGGER_CATEGORY);
        target.setLogLevel(Pt::System::Info);
        target.setChannel("console://");
    }
}

SSLInit::~SSLInit()
{
    if(0 == --ssl_init_counter) {
        log_info(Pt::System::endlog);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

SSLContext::SSLContext(const char* sessionID, Protocol    protocol)
: _protocol(protocol), _certChainExist(false)
{
    // Create the context
    switch(_protocol) {
        case SSLv2           : _ctx = SSL_CTX_new( SSLv2_method () ); break;
        case SSLv3or2        : _ctx = SSL_CTX_new( SSLv23_method() ); break;
        case TLSv1           : _ctx = SSL_CTX_new( TLSv1_method () ); break;
        case DefaultProtocol : /* Fall through */
        case SSLv3           : /* Fall through */
        default              :  _ctx = SSL_CTX_new( SSLv3_method () ); break;
    }

    getAvailableCiphers();

    // Set some options
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(_ctx, 1);
#endif
    SSL_CTX_set_options(_ctx, SSL_OP_SINGLE_DH_USE);
    SSL_CTX_set_mode(_ctx, SSL_MODE_NO_AUTO_CHAIN);
    SSL_CTX_set_mode(_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
  //SSL_CTX_set_read_ahead(_ctx, 1);

    // Set session ID
    if(sessionID) {
        SSL_CTX_set_session_id_context(
            _ctx,
            reinterpret_cast<const unsigned char*>(sessionID), strlen(sessionID)
        );
    }

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

SSLContext::~SSLContext()
{
#ifndef COPY_EXTRA_CERT
    if(_ctx->extra_certs) {
        sk_X509_pop(_ctx->extra_certs);
        _ctx->extra_certs = 0;
    }
#endif

    SSL_CTX_free(_ctx);
}

void SSLContext::setProtocol(Protocol protocol)
{
    int  ret = 0;
    bool v2  = false;

    switch(_protocol) {
        case SSLv2           : ret = SSL_CTX_set_ssl_version( _ctx, SSLv2_method () ); v2 = true; break;
        case SSLv3or2        : ret = SSL_CTX_set_ssl_version( _ctx, SSLv23_method() ); v2 = true; break;
        case TLSv1           : ret = SSL_CTX_set_ssl_version( _ctx, TLSv1_method () );            break;
        case SSLv3           : /* Fall through */
        case DefaultProtocol : /* Fall through */
        default              : ret = SSL_CTX_set_ssl_version( _ctx, SSLv3_method () );
    }

    if(!ret) throw SSLRuntimeError("Failed setting the SSL protocol!", PT_SOURCEINFO);

    if(!SSL_CTX_set_cipher_list(_ctx, v2 ? "ALL:!aNULL:!eNULL" : "ALL:!aNULL:!eNULL:!SSLv2"))
        throw SSLRuntimeError("Failed selecting the default SSL ciphers!", PT_SOURCEINFO);

    getAvailableCiphers();
}

/*
void SSLContext::setEnabledCiphers(const std::vector<SSLCipherInfo>& ciphers)
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

void SSLContext::setTrustedCACertificate(const SSLCertificateList& trustedCert)
{
    // Try to add the CA X509 certificates (if any)
    X509_STOREAutoPtr cert_store(X509_STORE_new());
    for(std::vector<X509*>::const_iterator it = trustedCert.impl().begin(); it != trustedCert.impl().end(); ++it) {
        if( ! X509_STORE_add_cert(cert_store.get(), *it) )
            throw SSLRuntimeError("Could not store the CA certificate as a trusted certificate!", PT_SOURCEINFO);
    }

    // Set it to the context
    SSL_CTX_set_cert_store(_ctx, cert_store.get());
    cert_store.release();

    // Store a reference to the certificate list
    _trustedCACert = trustedCert;
}

void SSLContext::setCertificateChain(const SSLCertificateList& certList)
{
    setCertificate(certList);
    if(certList.impl().size() > 1) addCertificateChain(certList, true);
}

void SSLContext::setCertificate(const SSLCertificateList& certList)
{
    ERR_clear_error();
    
    if( ! SSL_CTX_use_certificate( _ctx, *certList.impl().begin() ) || ERR_peek_error() )
        throw SSLRuntimeError("Invalid/mismatched certificate!", PT_SOURCEINFO);
}

void SSLContext::addCertificateChain(const SSLCertificateList& certList, bool skipFirstCert)
{
    // Skip the first certificate (if needed)
    std::vector<X509*>::const_iterator it = certList.impl().begin();
    if(skipFirstCert) ++it;

    // Check if we do not have any CA certificates
    if(it == certList.impl().end())
        throw SSLRuntimeError("Could not find any CA certificate in the certificate list!", PT_SOURCEINFO);

    // Add the CA X509 certificates
    for(; it != certList.impl().end(); ++it) {
        // Convert the X509 certificate to raw binary data
        // NOTE: OpenSSL do not copy the X509 certificate, so we must "copy" it manually
        unsigned char* buf = 0;
        int            len = i2d_X509(*it, &buf);
        if(len < 0)
            throw SSLRuntimeError("Could not convert the CA certificate to raw binary data!", PT_SOURCEINFO);
        // Convert the raw binary data back to an X509 certificate
        const unsigned char* tbf  = buf;
        X509*                x509 = d2i_X509(0, &tbf, len);
        OPENSSL_free(buf);
        if(!x509)
            throw SSLRuntimeError("Could not convert the raw binary data back to a CA certificate!", PT_SOURCEINFO);
        // Add the certificate
        if( ! SSL_CTX_add_extra_chain_cert( _ctx, x509 ) )
            throw SSLRuntimeError("Could not add CA certificate!", PT_SOURCEINFO);
        //if( ! SSL_CTX_add_extra_chain_cert( _ctx, *it ) )
        //    throw SSLRuntimeError("Could not add CA certificate!", PT_SOURCEINFO);
    }

    // Set flag
    _certChainExist = true;
}

void SSLContext::setPrivateKey(const SSLPrivateKey& privKey)
{
    // Try to use the private key
    if( ! SSL_CTX_use_PrivateKey( _ctx, privKey.impl() ) )
        throw SSLRuntimeError("Invalid private-key!", PT_SOURCEINFO);

    // Store a reference to the private key
    _privKey = privKey;
    
    // Check the private key (if needed)
    if(!_certChainExist) {
        if(!SSL_CTX_check_private_key(_ctx))
            throw SSLRuntimeError(
                "The private key does not agree with the corresponding public key in the certificate!",
                PT_SOURCEINFO );
    }
}

void SSLContext::getAvailableCiphers()
{
    // Clear the list
    _availCiphers.clear();

    // Get the available ciphers
    SSL*                  ssl = SSL_new(_ctx);
    STACK_OF(SSL_CIPHER)* chp = SSL_get_ciphers(ssl);

    // Walk trough the ciphers
    for(int i = 0; i < sk_SSL_CIPHER_num(chp); ++i)
    {
        // Skip if not valid
        const SSL_CIPHER* c = sk_SSL_CIPHER_value(chp, i);
        if( ! c->valid )
            continue;

        // Get the ID and split it
        const unsigned long id  = c->id;
        const int           id0 = (int) (  id >> 24);
        const int           id1 = (int) ( (id >> 16) & 0xFFL );
        const int           id2 = (int) ( (id >>  8) & 0xFFL );
        const int           id3 = (int) (  id        & 0xFFL );

        // Convert the ID to a readable string
        char strid[64];
        if((id & 0xFF000000L) == 0x02000000L)
            sprintf(strid, "0x%02X,0x%02X,0x%02X", id1, id2, id3);
        else if((id & 0xFF000000L) == 0x03000000L)
            sprintf(strid, "0x%02X,0x%02X", id2, id3);
        else
            sprintf(strid, "0x%02X,0x%02X,0x%02X,0x%02X", id0, id1, id2, id3);

        // Get some information
        char desc[512];
        SSL_CIPHER_description(c, desc, sizeof(desc));
        const int dlen = strlen(desc);
        if(desc[dlen - 1] == '\n')
            desc[dlen - 1] = 0;

        // Store the chiper information
        int np;
        _availCiphers.push_back(SSLCipherInfo(
            id,
            strid,
            SSL_CIPHER_get_name(c),
            SSL_CIPHER_get_bits(c, &np),
            SSL_CIPHER_get_version(c),
            desc
        ));
        _availCiphers.back().usedBits = np;
    }

    // Clear all
    SSL_free(ssl);
}

} // namespace Ssl
} // namespace Pt

