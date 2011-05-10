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

namespace Pt {

namespace Ssl {

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////
log_define(PT_SSL_LOGGER_CATEGORY);
#define PT_SSL_LOG(CODE) PT_SSL_LOG_INFO("SSLContext  ", CODE)
////////////////////////////////////////////////////////////////////////////////////////////////////

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
: _protocol(protocol)
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
    _enabledCiphers = _availCiphers;

    // Set some options
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(_ctx, 1);
#endif
    SSL_CTX_set_options(_ctx, SSL_OP_SINGLE_DH_USE);
    SSL_CTX_set_mode(_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
  //SSL_CTX_set_read_ahead(_ctx, 1);

    // Set session ID
    if(sessionID) {
        SSL_CTX_set_session_id_context(
            _ctx,
            reinterpret_cast<const unsigned char*>(sessionID), strlen(sessionID)
        );
    }
}

SSLContext::~SSLContext()
{
    // Clear any loaded CA certificates
    // NOTE: If we do not do this, we will get segmentation fault.
    if(_ctx->extra_certs) {
        sk_X509_pop(_ctx->extra_certs);
        _ctx->extra_certs = 0;
    }

    // Free the context
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
    _enabledCiphers = _availCiphers;
}

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

void SSLContext::setTrustedCACertificate(const SSLCertificateList& trustedCert)
{
    // Clear the previous trusted CA certificates (if any)
    X509_STORE_free(_ctx->cert_store);
    _ctx->cert_store = X509_STORE_new();

    // Try to add the CA X509 certificates (if any)
    for(std::vector<X509*>::const_iterator it = trustedCert.impl().begin(); it != trustedCert.impl().end(); ++it) {
        if( ! X509_STORE_add_cert(_ctx->cert_store, *it) )
            throw SSLRuntimeError("Could not store the CA certificate as a trusted certificate!", PT_SOURCEINFO);
    }

    // Store a reference to the certificate list
    _trustedCACert = trustedCert;
}

void SSLContext::setCertificateChain(const SSLCertificateList& certChain)
{
    // Set the first certificate as this context's certificate
    std::vector<X509*>::const_iterator it = certChain.impl().begin();
    
    // Try to use the X509 certificate
    ERR_clear_error();
    if( ! SSL_CTX_use_certificate( _ctx, *it ) || ERR_peek_error() )
        throw SSLRuntimeError("Invalid/mismatched certificate!", PT_SOURCEINFO);

    // Clear any previous CA certificates
    if(_ctx->extra_certs) {
        sk_X509_pop_free(_ctx->extra_certs, X509_free);
        _ctx->extra_certs = 0;
    }

    // Try to add the CA X509 certificates (if any)
    // NOTE: OpenSSL do not copy the X509* data, so we must make sure that OpenSSL do not
    //       free the X509 certificate when the context is destroyed. Please check the code
    //       in ~SSLContext().
    for(; it != certChain.impl().end(); ++it) {
        if( ! SSL_CTX_add_extra_chain_cert( _ctx, *it ) )
            throw SSLRuntimeError("Could not add CA certificate!", PT_SOURCEINFO);
    }

    // Store a reference to the certificate chain
    _certChain = certChain;
}

void SSLContext::setPrivateKey(const SSLPrivateKey& privKey)
{
    // Try to use the private key
    if( ! SSL_CTX_use_PrivateKey( _ctx, privKey.impl() ) )
        throw SSLRuntimeError("Invalid private-key!", PT_SOURCEINFO);

    // Store a reference to the private key
    _privKey = privKey;
    
    // Check the private key (if needed)
    if(!_certChain.impl().empty()) {
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

} // namespace Pt
} // namespace Ssl
